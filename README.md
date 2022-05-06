# Pipeline Benchmark

A set of benchmarks to compare the impact from different approaches to how data
is passed along between pipelined processing stages.

### Methods for passing data between stages

#### By reference

I.e. all the processing stages modify the same buffer. This is usually the
fastest, but the downside is that it is not easy to parallelize and stages that
need to aggregate data state may need to buffer it internally and copy data
around more than strictly necessary.

#### By static

I.e. processing stages return a read-only reference to some internal buffer
that is valid until its next call. This has the same downsides as by reference,
but now with even more internal buffering and copying. It does however makes
for a more safe implementation where processing stages are less likely to
interfere with each other.

#### By copy

I.e. every step in the way returns a new copy. This is by far the least
efficient approach, but it easy to work with and it is trivial to parallelize.
Reallocation of recently freed memory is not that expensive anyways, so for
most applications were the focus is not on performance it is usually good
enough.

#### By move

I.e. every processing stage gets ownership of its input and transfers ownership
of its output. This is the functional approach, so it is safer to work with and
easier to parallelize than the reference approaches. The performance depends on
how cheap move operations are (and move of heap buffers e.g. `std::vector` is
usually pretty efficient). It limits the amount of reallocations and copying
needed because it lets the pipeline as a whole reuse buffers as needed. The
downside is that it may complicate the code to deal with ownership transfer
and/or make sure unnecessary copies are avoided.

### Benchmark results
```
$ ./build_and_run.sh
[...]
0.019 cpp_O3/bench_input_only_static_raw
0.022 cpp_Os/bench_input_only_static_raw
0.175 cpp_O3/bench_input_only_static
0.185 cpp_O3/bench_input_only_moved
0.191 cpp_O3/bench_input_only_ref
0.271 cpp_O3/bench_input_append_ref
0.320 cpp_Os/bench_input_only_static
0.331 cpp_O3/bench_input_only_copy
0.331 cpp_Os/bench_input_only_moved
0.337 cpp_Os/bench_input_only_ref
0.359 cpp_O3/bench_input_append_moved
0.389 cpp_O3/bench_input_append_static
0.395 cpp_O3/bench_input_join_static
0.403 cpp_O3/bench_input_prepend_ref
0.428 cpp_Os/bench_input_append_ref
0.434 cpp_O3/bench_input_prepend_static
0.474 cpp_Os/bench_input_only_copy
0.503 cpp_O3/bench_input_join_ref
0.568 cpp_Os/bench_input_prepend_ref
0.578 cpp_O3/bench_input_prepend_moved
0.581 cpp_Os/bench_input_join_static
0.605 cpp_Os/bench_input_append_moved
0.662 cpp_Os/bench_input_prepend_static
0.694 cpp_Os/bench_input_prepend_moved
0.738 cpp_Os/bench_input_append_static
0.753 cpp_Os/bench_input_join_ref
0.806 cpp_O3/bench_input_join_moved
0.867 cpp_O3/bench_pipeline_ref
0.971 cpp_Os/bench_input_join_moved
0.995 cpp_O3/bench_pipeline_static
1.132 cpp_Os/bench_pipeline_ref
1.185 cpp_O3/bench_input_join_copy
1.323 cpp_O3/bench_pipeline_moved
1.383 cpp_Os/bench_input_join_copy
1.402 cpp_O3/bench_input_append_copy
1.418 cpp_O3/bench_input_prepend_copy
1.431 cpp_Os/bench_pipeline_static
1.595 cpp_Os/bench_input_append_copy
1.598 cpp_Os/bench_pipeline_moved
1.603 cpp_Os/bench_input_prepend_copy
3.906 cpp_O3/bench_pipeline_copy
4.330 cpp_Os/bench_pipeline_copy
```
The benchmark ran on a DELL XPS 13 (9350) laptop using one Intel(R) Core(TM)
i7-6500U CPU @ 2.50GHz core and 1866MHz DDR3 RAM.

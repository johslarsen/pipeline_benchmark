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
0.018 OK cpp_O3/bench_input_only_static_raw
0.023 OK cpp_Os/bench_input_only_static_raw
0.175 OK cpp_O3/bench_input_only_ref
0.175 OK cpp_O3/bench_input_only_static
0.215 OK cpp_O3/bench_input_only_moved
0.226 OK cpp_O3/bench_input_append_ref
0.316 OK cpp_Os/bench_input_only_moved
0.319 OK cpp_Os/bench_input_only_static
0.335 OK cpp_Os/bench_input_only_ref
0.338 OK cpp_O3/bench_input_only_copy
0.370 OK cpp_O3/bench_input_append_moved
0.390 OK cpp_O3/bench_input_join_static
0.421 OK cpp_O3/bench_input_append_static
0.421 OK cpp_O3/bench_input_prepend_static
0.426 OK cpp_Os/bench_input_append_ref
0.461 OK cpp_O3/bench_input_prepend_ref
0.490 OK cpp_Os/bench_input_only_copy
0.543 OK cpp_Os/bench_input_append_moved
0.567 OK cpp_Os/bench_input_prepend_ref
0.592 OK cpp_Os/bench_input_join_static
0.607 OK cpp_O3/bench_input_prepend_moved
0.676 OK cpp_Os/bench_input_prepend_static
0.692 OK cpp_O3/bench_input_join_ref
0.732 OK cpp_O3/bench_input_join_moved
0.733 OK cpp_Os/bench_input_prepend_moved
0.793 OK cpp_O3/bench_input_join_copy
0.815 OK cpp_Os/bench_input_append_static
0.820 OK cpp_O3/bench_input_append_copy
0.935 OK cpp_O3/bench_input_prepend_copy
0.976 OK cpp_O3/bench_pipeline_static
0.994 OK cpp_Os/bench_input_join_ref
1.025 OK cpp_Os/bench_input_join_copy
1.031 OK cpp_O3/bench_pipeline_ref
1.094 OK cpp_Os/bench_input_append_copy
1.197 OK cpp_O3/bench_pipeline_moved
1.232 OK cpp_Os/bench_input_join_moved
1.236 OK cpp_Os/bench_input_prepend_copy
1.476 OK cpp_Os/bench_pipeline_static
1.506 OK cpp_Os/bench_pipeline_ref
1.544 OK cpp_Os/bench_pipeline_moved
1.716 OK cpp_O3/bench_pipeline_copy
2.398 OK cpp_Os/bench_pipeline_copy
```
The benchmark ran on a DELL XPS 13 (9350) laptop using one Intel(R) Core(TM)
i7-6500U CPU @ 2.50GHz core and 1866MHz DDR3 RAM.

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
0.021 OK rust/target/release/bench_input_only_static_raw
0.022 OK cpp_Os/bench_input_only_static_raw
0.160 OK rust/target/release/bench_input_only_ref
0.167 OK rust/target/release/bench_input_only_static
0.175 OK cpp_O3/bench_input_only_ref
0.175 OK cpp_O3/bench_input_only_static
0.183 OK cpp_O3/bench_input_only_moved
0.195 OK rust/target/release/bench_input_only_moved
0.224 OK cpp_O3/bench_input_append_ref
0.231 OK rust/target/release/bench_input_append_ref
0.275 OK rust/target/release/bench_input_append_moved
0.315 OK cpp_Os/bench_input_only_moved
0.320 OK cpp_Os/bench_input_only_static
0.320 OK rust/target/release/bench_input_only_copy
0.347 OK cpp_Os/bench_input_only_ref
0.364 OK cpp_O3/bench_input_append_moved
0.370 OK cpp_O3/bench_input_only_copy
0.384 OK rust/target/release/bench_input_join_static
0.385 OK cpp_O3/bench_input_prepend_ref
0.402 OK cpp_O3/bench_input_join_static
0.420 OK rust/target/release/bench_input_prepend_static
0.435 OK rust/target/release/bench_input_prepend_ref
0.439 OK cpp_O3/bench_input_prepend_static
0.449 OK cpp_Os/bench_input_append_ref
0.461 OK rust/target/release/bench_input_append_static
0.484 OK cpp_O3/bench_input_append_static
0.525 OK cpp_Os/bench_input_append_moved
0.532 OK cpp_Os/bench_input_only_copy
0.542 OK rust/target/release/bench_input_prepend_moved
0.559 OK cpp_O3/bench_input_prepend_moved
0.566 OK cpp_Os/bench_input_prepend_ref
0.647 OK cpp_Os/bench_input_join_static
0.678 OK rust/target/release/bench_input_join_ref
0.690 OK cpp_Os/bench_input_append_static
0.717 OK cpp_O3/bench_input_join_ref
0.720 OK cpp_O3/bench_input_join_moved
0.734 OK cpp_Os/bench_input_prepend_static
0.737 OK rust/target/release/bench_input_join_moved
0.739 OK cpp_Os/bench_input_prepend_moved
0.761 OK cpp_O3/bench_input_join_copy
0.779 OK rust/target/release/bench_input_join_copy
0.896 OK rust/target/release/bench_input_prepend_copy
0.898 OK cpp_O3/bench_input_append_copy
0.942 OK cpp_O3/bench_input_prepend_copy
0.946 OK cpp_O3/bench_pipeline_static
0.950 OK rust/target/release/bench_pipeline_static
1.012 OK cpp_Os/bench_input_join_ref
1.051 OK rust/target/release/bench_pipeline_ref
1.062 OK cpp_O3/bench_pipeline_ref
1.095 OK cpp_Os/bench_input_append_copy
1.098 OK rust/target/release/bench_input_append_copy
1.105 OK cpp_Os/bench_input_join_moved
1.114 OK cpp_Os/bench_input_join_copy
1.138 OK rust/target/release/bench_pipeline_moved
1.180 OK cpp_Os/bench_input_prepend_copy
1.293 OK cpp_O3/bench_pipeline_moved
1.417 OK cpp_Os/bench_pipeline_ref
1.508 OK cpp_Os/bench_pipeline_moved
1.621 OK cpp_Os/bench_pipeline_static
1.719 OK cpp_O3/bench_pipeline_copy
1.770 OK rust/target/release/bench_pipeline_copy
2.409 OK cpp_Os/bench_pipeline_copy
```
The benchmark ran on a DELL XPS 13 (9350) laptop using one Intel(R) Core(TM)
i7-6500U CPU @ 2.50GHz core and 1866MHz DDR3 RAM.

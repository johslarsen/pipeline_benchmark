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

##### By malloc

Same as copy, just using the C primitives instead.

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
0.021 OK rust/target/release/bench_input_only_static_raw
0.022 OK cpp_O3/bench_input_only_static_raw
0.022 OK cpp_Os/bench_input_only_static_raw
0.159 OK rust/target/release/bench_input_only_ref
0.167 OK rust/target/release/bench_input_only_static
0.177 OK cpp_O3/bench_input_only_static
0.185 OK cpp_O3/bench_input_only_moved
0.191 OK cpp_O3/bench_input_only_ref
0.225 OK cpp_O3/bench_input_append_ref
0.232 OK rust/target/release/bench_input_append_ref
0.246 OK rust/target/release/bench_input_only_moved
0.314 OK cpp_Os/bench_input_only_moved
0.320 OK cpp_Os/bench_input_only_static
0.331 OK cpp_O3/bench_input_only_copy
0.347 OK cpp_Os/bench_input_only_ref
0.350 OK rust/target/release/bench_input_only_copy
0.371 OK cpp_O3/bench_input_append_moved
0.373 OK rust/target/release/bench_input_append_moved
0.388 OK cpp_O3/bench_input_append_static
0.400 OK cpp_O3/bench_input_prepend_ref
0.406 OK rust/target/release/bench_input_prepend_static
0.409 OK rust/target/release/bench_input_join_static
0.419 OK rust/target/release/bench_input_append_static
0.424 OK cpp_O3/bench_input_join_static
0.426 OK cpp_Os/bench_input_append_ref
0.445 OK cpp_O3/bench_input_prepend_static
0.514 OK rust/target/release/bench_input_prepend_ref
0.544 OK rust/target/release/bench_input_prepend_moved
0.570 OK cpp_Os/bench_input_only_copy
0.585 OK cpp_O3/bench_input_prepend_moved
0.599 OK cpp_Os/bench_input_prepend_ref
0.605 OK cpp_Os/bench_input_join_static
0.610 OK cpp_Os/bench_input_append_moved
0.650 OK rust/target/release/bench_input_join_ref
0.660 OK cpp_Os/bench_input_prepend_static
0.695 OK cpp_O3/bench_input_join_ref
0.700 OK rust/target/release/bench_input_join_moved
0.725 OK cpp_O3/bench_input_join_moved
0.743 OK cpp_Os/bench_input_prepend_moved
0.779 OK cpp_O3/bench_input_join_copy
0.802 OK rust/target/release/bench_input_prepend_copy
0.811 OK cpp_Os/bench_input_append_static
0.843 OK cpp_O3/bench_input_append_copy
0.869 OK rust/target/release/bench_input_join_copy
0.899 OK rust/target/release/bench_input_append_copy
0.954 OK rust/target/release/bench_pipeline_static
0.984 OK cpp_Os/bench_input_join_moved
1.003 OK cpp_Os/bench_input_join_ref
1.023 OK cpp_O3/bench_input_prepend_copy
1.023 OK cpp_O3/bench_pipeline_static
1.047 OK cpp_Os/bench_input_join_copy
1.052 OK cpp_O3/bench_pipeline_ref
1.059 OK rust/target/release/bench_pipeline_ref
1.136 OK rust/target/release/bench_pipeline_moved
1.172 OK cpp_Os/bench_input_prepend_copy
1.186 OK cpp_Os/bench_input_append_copy
1.237 OK cpp_O3/bench_pipeline_moved
1.414 OK cpp_Os/bench_pipeline_ref
1.505 OK cpp_Os/bench_pipeline_static
1.514 OK cpp_O3/bench_pipeline_malloc
1.562 OK cpp_Os/bench_pipeline_moved
1.779 OK cpp_O3/bench_pipeline_copy
1.785 OK rust/target/release/bench_pipeline_copy
2.117 OK cpp_Os/bench_pipeline_malloc
2.464 OK cpp_Os/bench_pipeline_copy
```
The benchmark ran on a DELL XPS 13 (9350) laptop using one Intel(R) Core(TM)
i7-6500U CPU @ 2.50GHz core and 1866MHz DDR3 RAM.

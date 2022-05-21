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
0.102 OK cpp_O3/bench_input_only_static_raw
0.116 OK rust/target/release/bench_input_only_static_raw
0.124 OK cpp_O3/bench_input_only_ref
0.171 OK cpp_Os/bench_input_only_static_raw
0.188 OK cpp_O3/bench_input_append_ref
0.248 OK rust/target/release/bench_input_only_ref
0.292 OK rust/target/release/bench_input_only_moved
0.293 OK rust/target/release/bench_input_only_static
0.300 OK cpp_O3/bench_input_only_moved
0.328 OK rust/target/release/bench_input_append_ref
0.349 OK rust/target/release/bench_input_append_moved
0.360 OK cpp_O3/bench_input_only_static
0.402 OK cpp_O3/bench_input_prepend_ref
0.421 OK cpp_Os/bench_input_only_ref
0.440 OK cpp_O3/bench_input_only_copy
0.465 OK cpp_Os/bench_input_only_static
0.473 OK rust/target/release/bench_input_only_copy
0.495 OK rust/target/release/bench_input_join_static
0.514 OK cpp_O3/bench_input_append_moved
0.535 OK rust/target/release/bench_input_prepend_ref
0.548 OK rust/target/release/bench_input_append_static
0.550 OK rust/target/release/bench_input_prepend_static
0.575 OK cpp_Os/bench_input_only_copy
0.581 OK cpp_Os/bench_input_only_moved
0.615 OK cpp_O3/bench_input_append_static
0.617 OK cpp_Os/bench_input_append_ref
0.622 OK cpp_O3/bench_input_join_ref
0.631 OK rust/target/release/bench_input_prepend_moved
0.632 OK cpp_O3/bench_input_prepend_static
0.644 OK cpp_O3/bench_input_join_static
0.689 OK cpp_Os/bench_input_append_moved
0.695 OK cpp_O3/bench_input_prepend_moved
0.700 OK rust/target/release/bench_input_join_ref
0.731 OK cpp_O3/bench_input_join_moved
0.749 OK rust/target/release/bench_input_join_moved
0.767 OK cpp_Os/bench_input_prepend_ref
0.809 OK cpp_Os/bench_input_join_static
0.876 OK cpp_Os/bench_input_prepend_moved
0.888 OK cpp_Os/bench_input_prepend_static
0.889 OK cpp_Os/bench_input_append_static
0.899 OK cpp_O3/bench_pipeline_ref
0.936 OK cpp_O3/bench_input_join_copy
0.956 OK rust/target/release/bench_input_join_copy
1.038 OK cpp_Os/bench_input_join_ref
1.050 OK rust/target/release/bench_pipeline_ref
1.060 OK rust/target/release/bench_input_append_copy
1.062 OK rust/target/release/bench_input_prepend_copy
1.084 OK cpp_Os/bench_input_join_moved
1.103 OK cpp_O3/bench_input_prepend_copy
1.112 OK cpp_O3/bench_input_append_copy
1.144 OK rust/target/release/bench_pipeline_moved
1.144 OK rust/target/release/bench_pipeline_static
1.247 OK cpp_O3/bench_pipeline_static
1.252 OK cpp_O3/bench_pipeline_moved
1.296 OK cpp_Os/bench_input_join_copy
1.464 OK cpp_Os/bench_pipeline_ref
1.481 OK cpp_Os/bench_input_append_copy
1.547 OK cpp_Os/bench_input_prepend_copy
1.678 OK cpp_Os/bench_pipeline_moved
1.686 OK cpp_Os/bench_pipeline_static
2.550 OK cpp_O3/bench_pipeline_copy
2.586 OK rust/target/release/bench_pipeline_copy
3.108 OK cpp_Os/bench_pipeline_copy
```
The benchmark ran on a DELL XPS 13 (9350) laptop using one Intel(R) Core(TM)
i7-6500U CPU @ 2.50GHz core and 1866MHz DDR3 RAM.

#### Allocation pattern

Reallocating same sized buffers over and over is less expensive than if
allocation sizes varies, so the default run configuration uses a repeating
record size pattern to emphasize the allocation cost. If allocation sizes are
more static the copy method performance is even closer to the other methods:
```
$ RECORD_SIZE=1024 ./run.sh cpp_O*/bench*copy rust/target/release/bench*copy | sort
0.342 OK rust/target/release/bench_input_only_copy
0.398 OK cpp_O3/bench_input_only_copy
0.524 OK cpp_Os/bench_input_only_copy
0.790 OK cpp_O3/bench_input_join_copy
0.830 OK rust/target/release/bench_input_join_copy
0.837 OK rust/target/release/bench_input_prepend_copy
0.923 OK rust/target/release/bench_input_append_copy
0.940 OK cpp_O3/bench_input_prepend_copy
1.067 OK cpp_Os/bench_input_join_copy
1.111 OK cpp_O3/bench_input_append_copy
1.220 OK cpp_Os/bench_input_prepend_copy
1.230 OK cpp_Os/bench_input_append_copy
1.805 OK cpp_O3/bench_pipeline_copy
1.845 OK rust/target/release/bench_pipeline_copy
2.559 OK cpp_Os/bench_pipeline_copy
```

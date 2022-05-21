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
0.129 OK rust/target/release/bench_input_only_static_raw
0.200 OK cpp_O3/bench_input_only_static_raw
0.225 OK cpp_O3/bench_input_only_ref
0.259 OK cpp_Os/bench_input_only_static_raw
0.301 OK rust/target/release/bench_input_only_ref
0.310 OK rust/target/release/bench_input_only_moved
0.320 OK cpp_O3/bench_input_append_ref
0.320 OK rust/target/release/bench_input_only_static
0.357 OK rust/target/release/bench_input_append_ref
0.382 OK rust/target/release/bench_input_append_moved
0.393 OK cpp_O3/bench_input_only_moved
0.405 OK cpp_O3/bench_input_only_static
0.465 OK cpp_O3/bench_input_append_moved
0.505 OK cpp_O3/bench_input_prepend_ref
0.530 OK rust/target/release/bench_input_prepend_ref
0.532 OK cpp_O3/bench_input_only_copy
0.536 OK cpp_Os/bench_input_only_ref
0.541 OK rust/target/release/bench_input_only_copy
0.549 OK rust/target/release/bench_input_prepend_static
0.562 OK rust/target/release/bench_input_join_static
0.565 OK cpp_Os/bench_input_only_static
0.622 OK cpp_O3/bench_input_join_ref
0.622 OK rust/target/release/bench_input_append_static
0.633 OK rust/target/release/bench_input_prepend_moved
0.682 OK cpp_Os/bench_input_only_moved
0.688 OK cpp_O3/bench_input_append_static
0.706 OK cpp_O3/bench_input_join_static
0.713 OK cpp_O3/bench_input_prepend_static
0.722 OK cpp_O3/bench_input_prepend_moved
0.725 OK cpp_Os/bench_input_only_copy
0.732 OK rust/target/release/bench_input_join_ref
0.743 OK cpp_Os/bench_input_append_ref
0.775 OK cpp_O3/bench_input_join_moved
0.794 OK rust/target/release/bench_input_join_moved
0.843 OK cpp_Os/bench_input_append_moved
0.891 OK cpp_Os/bench_input_prepend_ref
0.914 OK cpp_Os/bench_input_join_static
1.019 OK cpp_O3/bench_pipeline_ref
1.024 OK cpp_Os/bench_input_prepend_static
1.031 OK cpp_Os/bench_input_prepend_moved
1.037 OK rust/target/release/bench_input_join_copy
1.078 OK cpp_Os/bench_input_append_static
1.080 OK rust/target/release/bench_pipeline_ref
1.095 OK cpp_O3/bench_input_join_copy
1.104 OK cpp_Os/bench_input_join_ref
1.105 OK rust/target/release/bench_input_prepend_copy
1.129 OK rust/target/release/bench_pipeline_static
1.131 OK rust/target/release/bench_pipeline_moved
1.159 OK rust/target/release/bench_input_append_copy
1.168 OK cpp_O3/bench_input_append_copy
1.184 OK cpp_Os/bench_input_join_moved
1.292 OK cpp_O3/bench_input_prepend_copy
1.318 OK cpp_O3/bench_pipeline_static
1.345 OK cpp_O3/bench_pipeline_moved
1.367 OK cpp_Os/bench_input_join_copy
1.560 OK cpp_Os/bench_input_append_copy
1.608 OK cpp_Os/bench_pipeline_ref
1.688 OK cpp_Os/bench_input_prepend_copy
1.798 OK cpp_Os/bench_pipeline_static
1.846 OK cpp_Os/bench_pipeline_moved
2.530 OK rust/target/release/bench_pipeline_copy
2.586 OK cpp_O3/bench_pipeline_copy
3.273 OK cpp_Os/bench_pipeline_copy
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
0.477 OK rust/target/release/bench_input_only_copy
0.488 OK cpp_O3/bench_input_only_copy
0.692 OK cpp_Os/bench_input_only_copy
0.866 OK rust/target/release/bench_input_join_copy
0.912 OK rust/target/release/bench_input_prepend_copy
0.939 OK rust/target/release/bench_input_append_copy
0.961 OK cpp_O3/bench_input_join_copy
1.117 OK cpp_O3/bench_input_prepend_copy
1.136 OK cpp_Os/bench_input_join_copy
1.235 OK cpp_O3/bench_input_append_copy
1.312 OK cpp_Os/bench_input_append_copy
1.361 OK cpp_Os/bench_input_prepend_copy
1.827 OK rust/target/release/bench_pipeline_copy
1.874 OK cpp_O3/bench_pipeline_copy
2.629 OK cpp_Os/bench_pipeline_copy
```

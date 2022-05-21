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
0.099 OK rust/target/release/bench_input_only_static_raw
0.103 OK cpp_O3/bench_input_only_static_raw
0.147 OK cpp_O3/bench_input_only_ref
0.172 OK cpp_Os/bench_input_only_static_raw
0.185 OK cpp_O3/bench_input_append_ref
0.244 OK rust/target/release/bench_input_only_ref
0.307 OK cpp_O3/bench_input_only_moved
0.316 OK rust/target/release/bench_input_only_moved
0.316 OK rust/target/release/bench_input_only_static
0.324 OK rust/target/release/bench_input_append_ref
0.356 OK cpp_O3/bench_input_only_static
0.360 OK cpp_O3/bench_input_prepend_ref
0.383 OK rust/target/release/bench_input_append_moved
0.423 OK cpp_O3/bench_input_only_copy
0.427 OK cpp_Os/bench_input_only_ref
0.432 OK cpp_O3/bench_input_append_moved
0.439 OK cpp_Os/bench_input_only_static
0.473 OK rust/target/release/bench_input_only_copy
0.505 OK rust/target/release/bench_input_prepend_ref
0.513 OK rust/target/release/bench_input_append_static
0.526 OK cpp_Os/bench_input_only_moved
0.528 OK rust/target/release/bench_input_prepend_static
0.536 OK rust/target/release/bench_input_join_static
0.550 OK cpp_O3/bench_input_join_ref
0.584 OK cpp_O3/bench_input_prepend_static
0.595 OK cpp_O3/bench_input_append_static
0.595 OK rust/target/release/bench_input_prepend_moved
0.619 OK cpp_O3/bench_input_join_static
0.619 OK cpp_Os/bench_input_only_copy
0.631 OK cpp_Os/bench_input_append_ref
0.639 OK cpp_O3/bench_input_prepend_moved
0.698 OK rust/target/release/bench_input_join_ref
0.709 OK cpp_Os/bench_input_append_moved
0.717 OK cpp_O3/bench_input_join_moved
0.742 OK rust/target/release/bench_input_join_moved
0.754 OK cpp_Os/bench_input_prepend_ref
0.766 OK cpp_Os/bench_input_join_static
0.861 OK cpp_Os/bench_input_prepend_moved
0.865 OK cpp_Os/bench_input_prepend_static
0.883 OK cpp_Os/bench_input_append_static
0.924 OK rust/target/release/bench_input_join_copy
0.935 OK cpp_O3/bench_pipeline_ref
0.945 OK cpp_O3/bench_input_join_copy
0.959 OK cpp_Os/bench_input_join_ref
1.042 OK rust/target/release/bench_input_prepend_copy
1.047 OK cpp_O3/bench_input_append_copy
1.052 OK rust/target/release/bench_input_append_copy
1.052 OK rust/target/release/bench_pipeline_ref
1.065 OK cpp_Os/bench_input_join_moved
1.092 OK rust/target/release/bench_pipeline_static
1.122 OK cpp_O3/bench_input_prepend_copy
1.129 OK rust/target/release/bench_pipeline_moved
1.164 OK cpp_O3/bench_pipeline_moved
1.193 OK cpp_O3/bench_pipeline_static
1.248 OK cpp_Os/bench_input_join_copy
1.454 OK cpp_Os/bench_input_append_copy
1.502 OK cpp_Os/bench_pipeline_ref
1.576 OK cpp_Os/bench_input_prepend_copy
1.618 OK cpp_Os/bench_pipeline_static
1.640 OK cpp_Os/bench_pipeline_moved
2.467 OK rust/target/release/bench_pipeline_copy
2.469 OK cpp_O3/bench_pipeline_copy
3.000 OK cpp_Os/bench_pipeline_copy
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
0.327 OK rust/target/release/bench_input_only_copy
0.342 OK cpp_O3/bench_input_only_copy
0.468 OK cpp_Os/bench_input_only_copy
0.765 OK cpp_O3/bench_input_join_copy
0.786 OK rust/target/release/bench_input_join_copy
0.836 OK rust/target/release/bench_input_prepend_copy
0.877 OK rust/target/release/bench_input_append_copy
0.921 OK cpp_O3/bench_input_append_copy
1.037 OK cpp_Os/bench_input_join_copy
1.077 OK cpp_O3/bench_input_prepend_copy
1.100 OK cpp_Os/bench_input_append_copy
1.193 OK cpp_Os/bench_input_prepend_copy
1.733 OK rust/target/release/bench_pipeline_copy
1.737 OK cpp_O3/bench_pipeline_copy
2.423 OK cpp_Os/bench_pipeline_copy
```

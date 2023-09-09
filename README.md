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
0.039 OK cpp_O3/bench_input_only_static_raw
0.043 OK rust/target/release/bench_input_only_static_raw
0.046 OK cpp_O3/bench_input_only_ref
0.071 OK cpp_Os/bench_input_only_static_raw
0.075 OK cpp_O3/bench_input_append_ref
0.097 OK rust/target/release/bench_input_only_ref
0.098 OK cpp_O3/bench_input_only_static
0.100 OK cpp_Os/bench_input_only_ref
0.112 OK rust/target/release/bench_input_append_ref
0.121 OK cpp_O3/bench_input_only_moved
0.122 OK rust/target/release/bench_input_only_static
0.156 OK rust/target/release/bench_input_only_moved
0.160 OK cpp_Os/bench_input_append_ref
0.165 OK cpp_O3/bench_input_append_moved
0.170 OK cpp_O3/bench_input_prepend_ref
0.171 OK rust/target/release/bench_input_append_moved
0.178 OK rust/target/release/bench_input_only_copy
0.184 OK cpp_O3/bench_input_join_ref
0.201 OK cpp_O3/bench_input_only_copy
0.201 OK cpp_Os/bench_input_only_static
0.202 OK cpp_O3/bench_input_append_static
0.205 OK cpp_O3/bench_input_join_static
0.210 OK cpp_Os/bench_input_only_moved
0.217 OK cpp_O3/bench_input_prepend_static
0.229 OK cpp_Os/bench_input_prepend_ref
0.231 OK rust/target/release/bench_input_join_static
0.243 OK cpp_Os/bench_input_only_copy
0.245 OK rust/target/release/bench_input_prepend_static
0.262 OK rust/target/release/bench_input_prepend_ref
0.263 OK rust/target/release/bench_input_append_static
0.265 OK cpp_O3/bench_input_prepend_moved
0.265 OK rust/target/release/bench_input_join_ref
0.277 OK rust/target/release/bench_input_prepend_moved
0.278 OK cpp_O3/bench_input_join_moved
0.299 OK rust/target/release/bench_input_join_moved
0.309 OK cpp_Os/bench_input_append_moved
0.317 OK cpp_Os/bench_input_join_ref
0.318 OK cpp_Os/bench_input_join_static
0.350 OK cpp_Os/bench_input_prepend_static
0.351 OK cpp_O3/bench_pipeline_ref
0.354 OK cpp_O3/bench_input_join_copy
0.355 OK cpp_Os/bench_input_prepend_moved
0.364 OK rust/target/release/bench_input_join_copy
0.365 OK cpp_Os/bench_input_append_static
0.403 OK cpp_O3/bench_input_append_copy
0.408 OK rust/target/release/bench_input_append_copy
0.409 OK cpp_Os/bench_input_join_moved
0.439 OK cpp_Os/bench_input_join_copy
0.453 OK rust/target/release/bench_input_prepend_copy
0.461 OK rust/target/release/bench_pipeline_ref
0.474 OK cpp_O3/bench_pipeline_moved
0.480 OK cpp_O3/bench_pipeline_static
0.483 OK cpp_O3/bench_input_prepend_copy
0.485 OK rust/target/release/bench_pipeline_moved
0.512 OK rust/target/release/bench_pipeline_static
0.547 OK cpp_Os/bench_pipeline_ref
0.608 OK cpp_Os/bench_input_append_copy
0.620 OK cpp_Os/bench_input_prepend_copy
0.704 OK cpp_Os/bench_pipeline_moved
0.714 OK cpp_Os/bench_pipeline_static
0.952 OK cpp_O3/bench_pipeline_copy
1.053 OK rust/target/release/bench_pipeline_copy
1.365 OK cpp_Os/bench_pipeline_copy
```

The benchmark ran on a HP EliteBook 840 G9 laptop using one 12th Gen Intel(R)
Core(TM) i5-1235U i7-6500U CPU @ 4.40GHz core and 4800MHz DDR5 RAM.

#### Allocation pattern

Reallocating same sized buffers over and over is less expensive than if
allocation sizes varies, so the default run configuration uses a repeating
record size pattern to emphasize the allocation cost. If allocation sizes are
more static the copy method performance is even closer to the other methods:
```
$ RECORD_SIZE=1024 ./run.sh cpp_O*/bench*copy rust/target/release/bench*copy | sort
0.150 OK rust/target/release/bench_input_only_copy
0.153 OK cpp_O3/bench_input_only_copy
0.237 OK cpp_Os/bench_input_only_copy
0.252 OK cpp_O3/bench_input_join_copy
0.280 OK rust/target/release/bench_input_join_copy
0.326 OK cpp_O3/bench_input_append_copy
0.335 OK rust/target/release/bench_input_append_copy
0.354 OK cpp_O3/bench_input_prepend_copy
0.359 OK cpp_Os/bench_input_join_copy
0.382 OK rust/target/release/bench_input_prepend_copy
0.511 OK cpp_Os/bench_input_append_copy
0.588 OK cpp_Os/bench_input_prepend_copy
0.715 OK cpp_O3/bench_pipeline_copy
0.803 OK rust/target/release/bench_pipeline_copy
1.111 OK cpp_Os/bench_pipeline_copy
```

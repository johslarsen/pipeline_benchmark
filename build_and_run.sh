#!/bin/bash -
cd "$(dirname "$0")"

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release -H. -Bcpp_O3/ && cmake --build cpp_O3/ -- -j$(nproc)
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=MinSizeRel -H. -Bcpp_Os/ && cmake --build cpp_Os/ -- -j$(nproc)
cargo build --manifest-path=rust/Cargo.toml --release
./run.sh cpp_*/bench* rust/target/release/bench*{copy,moved,raw,ref,static} | sort

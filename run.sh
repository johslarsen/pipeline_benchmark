#!/bin/bash -
export ITERATIONS=${ITERATIONS:-$((10**7))}
export RECORD_SIZE=${RECORD_SIZE:-1024,1000,1337,711,512,1536,25,2047} # == 8*1024
export SLICE_COUNT=${SLICE_COUNT:-16}
export TIMEFORMAT=%3R
for f in $@; do
    duration=$( (time ./$f) 2>&1 ) && status="OK" || status="FAIL"
    echo "$duration $status $f"
done

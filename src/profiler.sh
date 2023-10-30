#!/bin/bash

##########################################
#          Profiling Script              #
##########################################

function miss_rate_profiler {
  ./sim_cache $1 $2 $3 $4 $5 $6 | grep 'L1 miss rate' | grep -Eo '[0-9]*\.[0-9]+'
}

function aat_profiler {
  ./sim_cache $1 $2 $3 $4 $5 $6 | grep 'average access time' | grep -Eo '[0-9]*\.[0-9]+'
}
# Miss rate profiling

# 1.1 L1 Cache cache_size vs. miss rate
for cache_size in 512 1024 2048 4096 8192 16384 32768 65536
do
  block_size=32
  assoc=1
  replacement_policy=0
  write_policy=0

  x=$cache_size

  miss_rate_profiler $block_size $cache_size $assoc $replacement_policy $write_policy gcc_trace.txt | xargs echo $x >> cache_size_vs_miss_rate.gcc_trace.result.txt
  miss_rate_profiler $block_size $cache_size $assoc $replacement_policy $write_policy go_trace.txt | xargs echo $x >> cache_size_vs_miss_rate.go_trace.result.txt
  miss_rate_profiler $block_size $cache_size $assoc $replacement_policy $write_policy perl_trace.txt | xargs echo $x >> cache_size_vs_miss_rate.perl_trace.result.txt
done

# 1.2 Associativity vs. miss rate
for assoc in 1 2 4 8 16 32 64 128
do
  block_size=32
  cache_size=16384
  replacement_policy=0
  write_policy=0

  x=$assoc

  miss_rate_profiler $block_size $cache_size $assoc $replacement_policy $write_policy gcc_trace.txt | xargs echo $x >> assoc_vs_miss_rate.gcc_trace.result.txt
  miss_rate_profiler $block_size $cache_size $assoc $replacement_policy $write_policy go_trace.txt | xargs echo $x >> assoc_vs_miss_rate.go_trace.result.txt
  miss_rate_profiler $block_size $cache_size $assoc $replacement_policy $write_policy perl_trace.txt | xargs echo $x >> assoc_vs_miss_rate.perl_trace.result.txt
done

# 1.3 Block size vs. miss rate
for block_size in 4 8 16 32 64 128 256 512 1024
do
  cache_size=16384
  assoc=1
  replacement_policy=0
  write_policy=0

  x=$block_size

  miss_rate_profiler $block_size $cache_size $assoc $replacement_policy $write_policy gcc_trace.txt | xargs echo $x >> block_size_vs_miss_rate.gcc_trace.result.txt
  miss_rate_profiler $block_size $cache_size $assoc $replacement_policy $write_policy go_trace.txt | xargs echo $x >> block_size_vs_miss_rate.go_trace.result.txt
  miss_rate_profiler $block_size $cache_size $assoc $replacement_policy $write_policy perl_trace.txt | xargs echo $x >> block_size_vs_miss_rate.perl_trace.result.txt
done

# AAT profiling

# 2.1 L1 Cache cache_size vs. AAT
for cache_size in 512 1024 2048 4096 8192 16384 32768 65536
do
  block_size=32
  assoc=1
  replacement_policy=0
  write_policy=0

  x=$cache_size

  aat_profiler $block_size $cache_size $assoc $replacement_policy $write_policy gcc_trace.txt | xargs echo $x >> cache_size_vs_aat.gcc_trace.result.txt
  aat_profiler $block_size $cache_size $assoc $replacement_policy $write_policy go_trace.txt | xargs echo $x >> cache_size_vs_aat.go_trace.result.txt
  aat_profiler $block_size $cache_size $assoc $replacement_policy $write_policy perl_trace.txt | xargs echo $x >> cache_size_vs_aat.perl_trace.result.txt
done

# 2.2 Associativity vs. AAT
for assoc in 1 2 4 8 16 32 64 128
do
  block_size=32
  cache_size=16384
  replacement_policy=0
  write_policy=0

  x=$assoc

  aat_profiler $block_size $cache_size $assoc $replacement_policy $write_policy gcc_trace.txt | xargs echo $x >> assoc_vs_aat.gcc_trace.result.txt
  aat_profiler $block_size $cache_size $assoc $replacement_policy $write_policy go_trace.txt | xargs echo $x >> assoc_vs_aat.go_trace.result.txt
  aat_profiler $block_size $cache_size $assoc $replacement_policy $write_policy perl_trace.txt | xargs echo $x >> assoc_vs_aat.perl_trace.result.txt
done

# 2.3 Block size vs. AAT
for block_size in 4 8 16 32 64 128 256 512 1024
do
  cache_size=16384
  assoc=1
  replacement_policy=0
  write_policy=0

  x=$block_size

  aat_profiler $block_size $cache_size $assoc $replacement_policy $write_policy gcc_trace.txt | xargs echo $x >> block_size_vs_aat.gcc_trace.result.txt
  aat_profiler $block_size $cache_size $assoc $replacement_policy $write_policy go_trace.txt | xargs echo $x >> block_size_vs_aat.go_trace.result.txt
  aat_profiler $block_size $cache_size $assoc $replacement_policy $write_policy perl_trace.txt | xargs echo $x >> block_size_vs_aat.perl_trace.result.txt
done





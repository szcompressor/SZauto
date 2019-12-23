#!/bin/bash
run=run
dataset=$1
field=$2
r1=$3
r2=$4
r3=$5

cd ~/meta_compressor/experiment/sampling
mkdir -p $run
mkdir -p $run/$dataset
for eb in 1E-2 5E-3 1E-3 5E-4 1E-4 5E-5 1E-5 1E-6 1E-7 1E-8 1E-9 5E-9 1E-10
do
  mkdir -p $run/$dataset/$eb
 # (echo ~/data/$dataset/$field $r1 $r2 $r3 $eb > $run/$dataset/$eb/$field.log  2>&1 &)
  ~/meta_compressor/build/bin/sz_test_sampling ~/data/$dataset/$field $r1 $r2 $r3 $eb > $run/$dataset/$eb/$field.log  2>&1 &
done
wait
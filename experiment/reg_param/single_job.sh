#!/bin/bash
run=run
dataset=$1
field=$2
r1=$3
r2=$4
r3=$5

for eb in 1E-2 5E-3 1E-3 5E-4 1E-4 5E-5 1E-5 1E-6 1E-7 1E-8 1E-9 5E-9 1E-10
do
  mkdir -p $run/$dataset/$eb
  echo ~/code/meta_compressor/build/bin/sz_test1 ~/data/$dataset/$field $r1 $r2 $r3 $ebs
  ~/code/meta_compressor/build/bin/sz_test1 ~/data/$dataset/$field $r1 $r2 $r3 $eb 6 3 1 1 65536 > $run/$dataset/$eb/$field.log  2>&1 &
  ~/code/meta_compressor/build/bin/sz_test1 ~/data/$dataset/$field $r1 $r2 $r3 $eb 6 3 1 2 65536 > $run/$dataset/$eb/$field.log  2>&1 &
done
wait
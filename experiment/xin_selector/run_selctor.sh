#!/bin/bash
run=run
exe=/home/xin/codes/sz_zfp_selector/select
res=/home/xin/codes/sz_zfp_selector/kai
datadir=/home/kazhao/data
dataset=$1
r1=$2
r2=$3
r3=$4
#source ~/.bashrc
#cd $res

mkdir -p $run
mkdir -p $run/$dataset
for field in $datadir/$dataset/*.dat
do
for eb in 1E-2 5E-3 1E-3 5E-4 1E-4 5E-5 1E-5 1E-6 1E-7 
do
  mkdir -p $run/$dataset/$eb
  python $res/extract_and_run.py $exe $field $r1 $r2 $r3 $eb $run/$dataset/$eb
  #echo ~/code/meta_compressor/build/bin/sz_autotuning ~/data/$dataset/$field $r1 $r2 $r3 $ebs
  #~/code/meta_compressor/build/bin/sz_autotuning ~/data/$dataset/$field $r1 $r2 $r3 $eb > $run/$dataset/$eb/$field.log  2>&1 &
done
done
wait

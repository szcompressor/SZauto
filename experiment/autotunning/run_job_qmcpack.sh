#!/bin/bash
#SBATCH --job-name=sz-samp
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

chmod +x /home/kazhao/meta_compressor/experiment/autotunning/single_job.sh

/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s0l300.dat      34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s1200l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s1500l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s1800l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s2100l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s2400l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s2700l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s3000l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s300l300.dat    34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s3300l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s3600l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s3900l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s4200l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s4500l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s4800l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s5100l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s5400l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s5700l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s600l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack6k-34500x69x69 s900l300.dat   34500 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack-288x115x69x69 einspline_288_115_69_69.pre.f32.dat 33120 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack8h-93840x69x69 spin_0.dat 93840 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack8h-93840x69x69 spin_1.dat 93840 69 69

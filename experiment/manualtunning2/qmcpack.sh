#!/bin/bash
#SBATCH --job-name=sz-qmc
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

chmod +x /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh

srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh qmcpack-288x115x69x69 einspline_288_115_69_69.pre.f32.dat 33120 69 69 &
wait
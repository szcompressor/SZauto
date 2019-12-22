#!/bin/bash
#SBATCH --job-name=sz-snp
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh snp-715x1115x366 snp 715 1115 366 &
wait
#!/bin/bash
#SBATCH --job-name=sz-qmc8
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 2
#SBATCH --ntasks=2
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

chmod +x /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh

srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-816x115x69x69 spin_0.dat 93840 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-816x115x69x69 spin_1.dat 93840 69 69 &

wait
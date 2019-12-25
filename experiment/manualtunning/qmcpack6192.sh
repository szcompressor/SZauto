#!/bin/bash
#SBATCH --job-name=sz-qmc6
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 21
#SBATCH --ntasks=21
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

chmod +x /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh

srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s0l300.dat       34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s1200l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s1500l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s1800l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s2100l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s2400l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s2700l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s3000l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s300l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s3300l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s3600l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s3900l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s4200l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s4500l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s4800l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s5100l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s5400l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s5700l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s600l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s900l300.dat   34500 69 69 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning/single_job.sh qmcpack-6192x115x69x69 s6000l192.dat   22080 69 69 &

wait
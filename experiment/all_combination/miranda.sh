#!/bin/bash
#SBATCH --job-name=sz-miranda
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 7
#SBATCH --ntasks=7
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh miranda-256x384x384 density.f32.dat  256 384 384 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh miranda-256x384x384 diffusivity.f32.dat 256 384 384 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh miranda-256x384x384 pressure.f32.dat  256 384 384 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh miranda-256x384x384 velocityx.f32.dat  256 384 384 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh miranda-256x384x384 velocityy.f32.dat  256 384 384 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh miranda-256x384x384 velocityz.f32.dat  256 384 384 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh miranda-256x384x384 viscocity.f32.dat  256 384 384 &
wait 
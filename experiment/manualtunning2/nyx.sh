#!/bin/bash
#SBATCH --job-name=sz-nyx
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 6
#SBATCH --ntasks=6
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

chmod +x /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh

srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh nyx-512x512x512 baryon_density.dat 512 512 512 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh nyx-512x512x512 temperature.dat 512 512 512 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh nyx-512x512x512 velocity_y.dat 512 512 512 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh nyx-512x512x512 dark_matter_density.dat 512 512 512 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh nyx-512x512x512 velocity_z.dat 512 512 512 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh nyx-512x512x512 velocity_x.dat 512 512 512 &

wait
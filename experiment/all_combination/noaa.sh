#!/bin/bash
#SBATCH --job-name=sz-noaa
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 12
#SBATCH --ntasks=12
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

chmod +x /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 cld_amt.dat 127 1536 3072 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 clwmr.dat 127 1536 3072 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 delz.dat 127 1536 3072 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 fields.txt 127 1536 3072 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 icmr.dat 127 1536 3072 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 o3mr.dat 127 1536 3072 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 rwmr.dat 127 1536 3072 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 snmr.dat 127 1536 3072 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 spfh.dat 127 1536 3072 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 tmp.dat 127 1536 3072 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 ugrd.dat 127 1536 3072 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/all_combination/single_job.sh noaa-127x1536x3072 vgrd.dat 127 1536 3072 &

wait
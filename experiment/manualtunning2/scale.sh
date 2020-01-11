#!/bin/bash
#SBATCH --job-name=sz-scale
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 12
#SBATCH --ntasks=12
#SBATCH --ntasks-per-node=1
#SBATCH --time=5-00:00:00

chmod +x /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 PRES-98x1200x1200.dat 98 1200 1200 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 QI-98x1200x1200.dat 98 1200 1200 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 QV-98x1200x1200.dat 98 1200 1200 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 U-98x1200x1200.dat 98 1200 1200 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 QC-98x1200x1200.dat 98 1200 1200 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 QR-98x1200x1200.dat 98 1200 1200 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 RH-98x1200x1200.dat 98 1200 1200 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 V-98x1200x1200.dat 98 1200 1200 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 QG-98x1200x1200.dat 98 1200 1200 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 QS-98x1200x1200.dat 98 1200 1200 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 T-98x1200x1200.dat 98 1200 1200 &
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/manualtunning2/single_job.sh scale-98x1200x1200 W-98x1200x1200.dat 98 1200 1200 &

wait
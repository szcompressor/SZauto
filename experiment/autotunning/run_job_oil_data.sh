#!/bin/bash
#SBATCH --job-name=sz-samp
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

chmod +x single_job.sh
chmod +x single_job_seq.sh

#./single_job.sh total-6x1643x1749 IPATI_HP_os8_Vz_deep_win_0100_transp_short.dat 6 1643 1749
./single_job.sh aramco-715x1115x355  snp_P_1_200.dat 715 1115 355
./single_job.sh aramco-715x1115x355  snp_P_1_201.dat 715 1115 355
./single_job.sh aramco-715x1115x355  snp_P_1_202.dat 715 1115 355
./single_job.sh aramco-715x1115x355  snp_P_1_203.dat 715 1115 355
./single_job.sh aramco-715x1115x355  snp_P_1_400.dat 715 1115 355
./single_job.sh aramco-715x1115x355  snp_P_1_401.dat 715 1115 355
./single_job.sh aramco-715x1115x355  snp_P_1_402.dat 715 1115 355

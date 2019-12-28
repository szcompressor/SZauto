#!/bin/bash
#SBATCH --job-name=sz-samp
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

chmod +x /home/kazhao/meta_compressor/experiment/autotunning/single_job.sh

/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 CLOUDf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 PRECIPf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 Pf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 QCLOUDf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 QGRAUPf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 QICEf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 QRAINf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 QSNOWf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 QVAPORf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 TCf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 Uf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 Vf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh hurricane-100x500x500 Wf48.bin.dat 100 500 500
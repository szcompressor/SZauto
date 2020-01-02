#!/bin/bash
#SBATCH --job-name=sz-samp
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

chmod +x /home/kazhao/meta_compressor/experiment/autotunning/single_job.sh

/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 CLOUDf48_log10.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 PRECIPf48_log10.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 Pf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 QCLOUDf48_log10.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 QGRAUPf48_log10.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 QICEf48_log10.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 QRAINf48_log10.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 QSNOWf48_log10.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 QVAPORf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 TCf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 Uf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 Vf48.bin.dat 100 500 500
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh loghurricane-100x500x500 Wf48.bin.dat 100 500 500

/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 baryon_density_log10.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 temperature.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 velocity_y.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 dark_matter_density_log10.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 velocity_z.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 velocity_x.dat 512 512 512


/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 PRES-98x1200x1200.log10.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 QC-98x1200x1200.log10.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 QR-98x1200x1200.log10.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 U-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 V-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 W-98x1200x1200.dat 98 1200 1200
#!/bin/bash
#SBATCH --job-name=sz-samp
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

chmod +x /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh snp-715x1115x366 snp 715 1115 366 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh qmcpack-288x115x69x69 einspline_288_115_69_69.pre.f32.dat 33120 69 69 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh miranda-256x384x384 density.f32.dat  256 384 384 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh miranda-256x384x384 diffusivity.f32.dat 256 384 384 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh miranda-256x384x384 pressure.f32.dat  256 384 384 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh miranda-256x384x384 velocityx.f32.dat  256 384 384 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh miranda-256x384x384 velocityy.f32.dat  256 384 384 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh miranda-256x384x384 velocityz.f32.dat  256 384 384 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh miranda-256x384x384 viscocity.f32.dat  256 384 384 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 CLOUDf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 PRECIPf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 Pf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 QCLOUDf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 QGRAUPf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 QICEf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 QRAINf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 QSNOWf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 QVAPORf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 TCf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 Uf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 Vf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh hurricane-100x500x500 Wf48.bin.dat 100 500 500 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 cld_amt.dat 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 clwmr.dat 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 delz.dat 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 fields.txt 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 icmr.dat 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 o3mr.dat 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 rwmr.dat 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 snmr.dat 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 spfh.dat 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 tmp.dat 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 ugrd.dat 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh noaa-127x1536x3072 vgrd.dat 127 1536 3072 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh nyx-512x512x512 baryon_density.dat 512 512 512 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh nyx-512x512x512 temperature.dat 512 512 512 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh nyx-512x512x512 velocity_y.dat 512 512 512 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh nyx-512x512x512 dark_matter_density.dat 512 512 512 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh nyx-512x512x512 velocity_z.dat 512 512 512 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh nyx-512x512x512 velocity_x.dat 512 512 512 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 PRES-98x1200x1200.dat 98 1200 1200 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 QI-98x1200x1200.dat 98 1200 1200 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 QV-98x1200x1200.dat 98 1200 1200 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 U-98x1200x1200.dat 98 1200 1200 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 QC-98x1200x1200.dat 98 1200 1200 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 QR-98x1200x1200.dat 98 1200 1200 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 RH-98x1200x1200.dat 98 1200 1200 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 V-98x1200x1200.dat 98 1200 1200 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 QG-98x1200x1200.dat 98 1200 1200 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 QS-98x1200x1200.dat 98 1200 1200 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 T-98x1200x1200.dat 98 1200 1200 
srun -N 1 -n 1 /home/kazhao/meta_compressor/experiment/sample_compress/single_job.sh scale-98x1200x1200 W-98x1200x1200.dat 98 1200 1200 

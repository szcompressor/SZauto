#!/bin/bash
#SBATCH --job-name=sz-samp
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-00:00:00

chmod +x /home/kazhao/meta_compressor/experiment/autotunning/single_job.sh
chmod +x /home/kazhao/meta_compressor/experiment/autotunning/single_job_seq.sh

/home/kazhao/meta_compressor/experiment/autotunning/single_job_seq.sh qmcpackbig-356040x69x69 part1.dat 356040 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job_seq.sh qmcpackbig-356040x69x69 part2.dat 356040 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack-288x115x69x69 einspline_288_115_69_69.pre.f32.dat 33120 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh miranda-256x384x384 density.f32.dat  256 384 384
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh miranda-256x384x384 diffusivity.f32.dat 256 384 384
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh miranda-256x384x384 pressure.f32.dat  256 384 384
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh miranda-256x384x384 velocityx.f32.dat  256 384 384
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh miranda-256x384x384 velocityy.f32.dat  256 384 384
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh miranda-256x384x384 velocityz.f32.dat  256 384 384
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh miranda-256x384x384 viscocity.f32.dat  256 384 384
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
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 cld_amt.dat 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 clwmr.dat 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 delz.dat 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 fields.txt 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 icmr.dat 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 o3mr.dat 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 rwmr.dat 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 snmr.dat 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 spfh.dat 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 tmp.dat 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 ugrd.dat 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh noaa-127x1536x3072 vgrd.dat 127 1536 3072
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh nyx-512x512x512 baryon_density.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh nyx-512x512x512 temperature.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh nyx-512x512x512 velocity_y.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh nyx-512x512x512 dark_matter_density.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh nyx-512x512x512 velocity_z.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh nyx-512x512x512 velocity_x.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 baryon_density_log10.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 temperature.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 velocity_y.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 dark_matter_density_log10.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 velocity_z.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh lognyx-512x512x512 velocity_x.dat 512 512 512
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 PRES-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 QI-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 QV-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 U-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 QC-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 QR-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 RH-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 V-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 QG-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 QS-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 T-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh scale-98x1200x1200 W-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 PRES-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 QC-98x1200x1200.log10.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 QR-98x1200x1200.log10.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 U-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 V-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh logscale-98x1200x1200 W-98x1200x1200.dat 98 1200 1200
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack8h-93840x69x69 spin_0.dat 93840 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh qmcpack8h-93840x69x69 spin_1.dat 93840 69 69
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh cesm-26x1800x3600 CLDICE_1_26_1800_3600.dat  26 1800 3600
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh cesm-26x1800x3600 CLDLIQ_1_26_1800_3600.dat  26 1800 3600
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh cesm-26x1800x3600 CLOUD_1_26_1800_3600.dat  26 1800 3600
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh cesm-26x1800x3600 UU_1_26_1800_3600.dat  26 1800 3600
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh cesm-26x1800x3600 U_1_26_1800_3600.dat  26 1800 3600
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh cesm-26x1800x3600 VD01_1_26_1800_3600.dat  26 1800 3600
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh cesm-26x1800x3600 VQ_1_26_1800_3600.dat  26 1800 3600
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh cesm-26x1800x3600 VT_1_26_1800_3600.dat  26 1800 3600
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh cesm-26x1800x3600 VU_1_26_1800_3600.dat  26 1800 3600
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh cesm-26x1800x3600 VV_1_26_1800_3600.dat  26 1800 3600
/home/kazhao/meta_compressor/experiment/autotunning/single_job.sh cesm-26x1800x3600 V_1_26_1800_3600.dat  26 1800 3600
#!/bin/bash
#SBATCH --job-name=sz-qmc
#SBATCH -p bdwall
#SBATCH -A ECP-EZ
#SBATCH --nodes 1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=48:00:00

# srun -N 1 -n 1 /home/kazhao/meta_compressor/build/bin/sz_test1 /home/kazhao/data/288x115x69x69.f32.qmc 33120 69 69 1e-2 >qmc1e-2 2>&1 &
# srun -N 1 -n 1 /home/kazhao/meta_compressor/build/bin/sz_test1 /home/kazhao/data/288x115x69x69.f32.qmc 33120 69 69 1e-3 >qmc1e-3 2>&1 &
# srun -N 1 -n 1 /home/kazhao/meta_compressor/build/bin/sz_test1 /home/kazhao/data/288x115x69x69.f32.qmc 33120 69 69 1e-4 >qmc1e-4 2>&1 &
# srun -N 1 -n 1 /home/kazhao/meta_compressor/build/bin/sz_test1 /home/kazhao/data/288x115x69x69.f32.qmc 33120 69 69 1e-5 >qmc1e-5 2>&1 &
# srun -N 1 -n 1 /home/kazhao/meta_compressor/build/bin/sz_test1 /home/kazhao/data/288x115x69x69.f32.qmc 33120 69 69 1e-6 >qmc1e-6 2>&1 &
# srun -N 1 -n 1 /home/kazhao/meta_compressor/build/bin/sz_test1 /home/kazhao/data/288x115x69x69.f32.qmc 33120 69 69 1e-7 >qmc1e-7 2>&1 &
# srun -N 1 -n 1 /home/kazhao/meta_compressor/build/bin/sz_test1 /home/kazhao/data/288x115x69x69.f32.qmc 33120 69 69 1e-8 >qmc1e-8 2>&1 &
#wait

(~/meta_compressor/build/bin/sz_test1 ~/data/6x98x1200x1200.f32.scale 588 1200 1200 1e-2 >scale1e-2 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x98x1200x1200.f32.scale 588 1200 1200 1e-3 >scale1e-3 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x98x1200x1200.f32.scale 588 1200 1200 1e-4 >scale1e-4 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x98x1200x1200.f32.scale 588 1200 1200 1e-5 >scale1e-5 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x98x1200x1200.f32.scale 588 1200 1200 1e-6 >scale1e-6 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x98x1200x1200.f32.scale 588 1200 1200 1e-7 >scale1e-7 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x98x1200x1200.f32.scale 588 1200 1200 1e-8 >scale1e-8 2>&1 &)

(~/meta_compressor/build/bin/sz_test1 ~/data/127x1536x3072.f32.vgrd.noaa 127 1536 3072 1e-2 >noaa1e-2 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/127x1536x3072.f32.vgrd.noaa 127 1536 3072 1e-3 >noaa1e-3 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/127x1536x3072.f32.vgrd.noaa 127 1536 3072 1e-4 >noaa1e-4 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/127x1536x3072.f32.vgrd.noaa 127 1536 3072 1e-5 >noaa1e-5 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/127x1536x3072.f32.vgrd.noaa 127 1536 3072 1e-6 >noaa1e-6 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/127x1536x3072.f32.vgrd.noaa 127 1536 3072 1e-7 >noaa1e-7 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/127x1536x3072.f32.vgrd.noaa 127 1536 3072 1e-8 >noaa1e-8 2>&1 &)

(~/meta_compressor/build/bin/sz_test1 ~/data/6x512x512x512.f32.nyx 3072 512 512 1e-2 >nyx1e-2 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x512x512x512.f32.nyx 3072 512 512 1e-3 >nyx1e-3 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x512x512x512.f32.nyx 3072 512 512 1e-4 >nyx1e-4 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x512x512x512.f32.nyx 3072 512 512 1e-5 >nyx1e-5 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x512x512x512.f32.nyx 3072 512 512 1e-6 >nyx1e-6 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x512x512x512.f32.nyx 3072 512 512 1e-7 >nyx1e-7 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/6x512x512x512.f32.nyx 3072 512 512 1e-8 >nyx1e-8 2>&1 &)

(~/meta_compressor/build/bin/sz_test1 ~/data/7x256x384x384.f32.miranda 1792 384 384 1e-2 >miranda1e-2 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/7x256x384x384.f32.miranda 1792 384 384 1e-3 >miranda1e-3 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/7x256x384x384.f32.miranda 1792 384 384 1e-4 >miranda1e-4 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/7x256x384x384.f32.miranda 1792 384 384 1e-5 >miranda1e-5 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/7x256x384x384.f32.miranda 1792 384 384 1e-6 >miranda1e-6 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/7x256x384x384.f32.miranda 1792 384 384 1e-7 >miranda1e-7 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/7x256x384x384.f32.miranda 1792 384 384 1e-8 >miranda1e-8 2>&1 &)


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

(~/meta_compressor/build/bin/sz_test1 ~/data/288x115x69x69.f32.qmc 33120 69 69 1e-2 >qmc1e-2 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/288x115x69x69.f32.qmc 33120 69 69 1e-3 >qmc1e-3 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/288x115x69x69.f32.qmc 33120 69 69 1e-4 >qmc1e-4 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/288x115x69x69.f32.qmc 33120 69 69 1e-5 >qmc1e-5 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/288x115x69x69.f32.qmc 33120 69 69 1e-6 >qmc1e-6 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/288x115x69x69.f32.qmc 33120 69 69 1e-7 >qmc1e-7 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/288x115x69x69.f32.qmc 33120 69 69 1e-8 >qmc1e-8 2>&1 &)

(~/meta_compressor/build/bin/sz_test1 ~/data/1x6x1643x1749.f32.ipati 6 1643 1749 1e-2 >ipati1e-2 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x6x1643x1749.f32.ipati 6 1643 1749 1e-3 >ipati1e-3 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x6x1643x1749.f32.ipati 6 1643 1749 1e-4 >ipati1e-4 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x6x1643x1749.f32.ipati 6 1643 1749 1e-5 >ipati1e-5 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x6x1643x1749.f32.ipati 6 1643 1749 1e-6 >ipati1e-6 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x6x1643x1749.f32.ipati 6 1643 1749 1e-7 >ipati1e-7 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x6x1643x1749.f32.ipati 6 1643 1749 1e-8 >ipati1e-8 2>&1 &)

(~/meta_compressor/build/bin/sz_test1 ~/data/1x100x500x500.f32.hurricane 100 500 500 1e-2 >hurricane1e-2 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x100x500x500.f32.hurricane 100 500 500 1e-3 >hurricane1e-3 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x100x500x500.f32.hurricane 100 500 500 1e-4 >hurricane1e-4 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x100x500x500.f32.hurricane 100 500 500 1e-5 >hurricane1e-5 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x100x500x500.f32.hurricane 100 500 500 1e-6 >hurricane1e-6 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x100x500x500.f32.hurricane 100 500 500 1e-7 >hurricane1e-7 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x100x500x500.f32.hurricane 100 500 500 1e-8 >hurricane1e-8 2>&1 &)

(~/meta_compressor/build/bin/sz_test1 ~/data/1x715x1115x366.f32.snp 715 1115 366 1e-2 >snp1e-2 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x715x1115x366.f32.snp 715 1115 366 1e-3 >snp1e-3 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x715x1115x366.f32.snp 715 1115 366 1e-4 >snp1e-4 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x715x1115x366.f32.snp 715 1115 366 1e-5 >snp1e-5 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x715x1115x366.f32.snp 715 1115 366 1e-6 >snp1e-6 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x715x1115x366.f32.snp 715 1115 366 1e-7 >snp1e-7 2>&1 &)
(~/meta_compressor/build/bin/sz_test1 ~/data/1x715x1115x366.f32.snp 715 1115 366 1e-8 >snp1e-8 2>&1 &)


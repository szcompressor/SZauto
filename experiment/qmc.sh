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


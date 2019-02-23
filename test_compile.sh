#! /bin/bash

SZ_dir=/Users/xin/utils/sz/
g++ -O3 -c sz_compression_utils.cpp -o a.o -I$SZ_dir/include
g++ -O3 -c sz_decompression_utils.cpp -o b.o -I$SZ_dir/include
g++ -O3 -c sz_compress_3d.cpp -o c.o -I$SZ_dir/include
g++ -O3 -c sz_decompress_3d.cpp -o d.o -I$SZ_dir/include
g++ -O3 sz_compress_test.cpp a.o b.o c.o d.o -I$SZ_dir/include -L$SZ_dir/lib -lSZ -lzlib -lzstd -lm

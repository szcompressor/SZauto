#ifndef _sz_compress_3d_hpp
#define _sz_compress_3d_hpp

#include <cstddef>

template<typename T>
unsigned char *
sz_compress_3d(const T * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, int BSIZE3d=8, bool block_independant=false);

template<typename T>
unsigned char *
sz_compress_3d_knl(const T * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, int BSIZE3d=8, bool block_independant=false);

#endif
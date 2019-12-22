#ifndef _sz_compress_3d_hpp
#define _sz_compress_3d_hpp

#include <cstddef>
#include <sz_def.hpp>

template<typename T>
unsigned char *
sz_compress_3d(const T * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, const sz_params& sz_params=default_params);

template<typename T>
unsigned char *
sz_compress_3d_knl(const T * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, const sz_params& sz_params=default_params);

template<typename T>
unsigned char *
sz_compress_3d_knl_2(const T * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, const sz_params& sz_params, sz_compress_info &compressInfo);

#endif
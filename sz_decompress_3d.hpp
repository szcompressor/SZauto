#ifndef _sz_decompress_3d_h
#define _sz_decompress_3d_h

#include "sz_decompression_utils.hpp"
#include "sz_def.hpp"
#include "sz_prediction.hpp"
#include <vector>

template<typename T>
T *
sz_decompress_3d(const unsigned char * compressed, size_t r1, size_t r2, size_t r3);

#endif
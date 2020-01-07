#ifndef _sz_autotuning_3d_hpp
#define _sz_autotuning_3d_hpp

#include <cstddef>
#include "sz_def.hpp"

template<typename T>
unsigned char *
sz_compress_autotuning_3d(T *data, size_t r1, size_t r2, size_t r3, double relative_eb, size_t &compressed_size,
                          bool baseline = false, bool decompress = false, bool log = false, float sample_ratio = 0.05);

template<typename T>
T *
sz_decompress_autotuning_3d(unsigned char *compressed, size_t compress_size, int r1, int r2, int r3);

template<typename T>
void
sz_compress_manualtuning_3d(T *data, size_t num_elements, int r1, int r2, int r3, float eb, float precision);

template<typename T>
sz_compress_info
sz_compress_decompress_highorder_3d(T *data, size_t num_elements, int r1, int r2, int r3, float precision,
                                    sz_params params, bool use_decompress);


template<typename T>
sz_compress_info do_compress_sampling(const T *data, size_t num_elements, int r1, int r2, int r3, float precision,
                                      sz_params params);

#endif
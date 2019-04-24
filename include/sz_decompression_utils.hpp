#ifndef _sz_decompression_utils_hpp
#define _sz_decompression_utils_hpp

#include "sz_def.hpp"

template <typename T>
inline void
read_variable_from_src(const unsigned char *& src, T& var){
    memcpy(&var, src, sizeof(T));
    src += sizeof(T);
}

template <typename T>
inline T *
read_array_from_src(const unsigned char *& src, size_t length){
    T * array = (T *) malloc(length * sizeof(T));
    memcpy(array, src, length*sizeof(T));
    src += length*sizeof(T);
    return array;
}

// de-quantization, for regression
template<typename T>
inline T
recover(float pred, double precision, int type_val, int intv_radius, const T *& unpredictable_data_pos){
	if(type_val == 0){
		return *(unpredictable_data_pos++);
	}
	else{
		return pred + 2 * (type_val - intv_radius) * precision;
	}
}

// de-quantization, for lorenzo
template<typename T>
inline T
recover(const meanInfo<T>& mean_info, float pred, double precision, int type_val, int intv_radius, const T *& unpredictable_data_pos){
	if(type_val == 0){
		return *(unpredictable_data_pos++);
	}
	else{
		if((type_val == 1) && (mean_info.use_mean)) return mean_info.mean;
		return pred + 2 * (type_val - intv_radius) * precision;
	}
}

float *
decode_regression_coefficients(const unsigned char *& compressed_pos, size_t reg_count, int block_size, float precision);

// modified from TypeManager.c
// change return value and increment byteArray
unsigned char * 
convertByteArray2IntArray_fast_1b_sz(size_t intArrayLength, const unsigned char*& compressed_pos, size_t byteArrayLength);

int *
Huffman_decode_tree_and_data(size_t state_num, size_t num_elements, const unsigned char *& compressed_pos);

#endif

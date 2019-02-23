#ifndef _sz_compression_utils_h
#define _sz_compression_utils_h

#include "sz_def.hpp"

#define QuantIntvMeanCapacity 8192
#define QuantIntvSampleDistance 100
#define QuantIntvSampleCapacity 32768
#define QuantIntvAccThreshold 0.999
#define BSIZE3d 8
#define RegThresholdSize3d 4
#define LorenzeNoise3d 1.22

template <typename T>
inline void
write_variable_to_dst(unsigned char *& dst, const T& var){
    memcpy(dst, &var, sizeof(T));
    dst += sizeof(T);
}

template <typename T>
inline void
write_array_to_dst(unsigned char *& dst, const T * array, size_t length){
    memcpy(dst, array, length*sizeof(T));
    dst += length*sizeof(T);
}

// quantize with decompression data (Lorenzo)
template<typename T>
inline int
quantize(float pred, T cur_data, double precision, int capacity, int intv_radius, T *& unpredictable_data_pos, T * decompressed){
	double diff = cur_data - pred;
	double quant_diff = fabs(diff) / precision + 1;
	if(quant_diff < capacity){
		quant_diff = (diff > 0) ? quant_diff : -quant_diff;
		int quant_index = (int)(quant_diff/2) + intv_radius;
		T decompressed_data = pred + 2 * (quant_index - intv_radius) * precision; 
		*decompressed = decompressed_data;
		if(fabs(decompressed_data - cur_data) <= precision) return quant_index;
 	}
 	*decompressed = cur_data;
 	*(unpredictable_data_pos++) = cur_data;
 	return 0;
}

// return quantization index, no decompression data (regression)
template<typename T>
inline int
quantize(float pred, T cur_data, double precision, int capacity, int intv_radius, T *& unpredictable_data_pos){
	double diff = cur_data - pred;
	double quant_diff = fabs(diff) / precision + 1;
	if(quant_diff < capacity){
		quant_diff = (diff > 0) ? quant_diff : -quant_diff;
		int quant_index = (int)(quant_diff/2) + intv_radius;
		T decompressed_data = pred + 2 * (quant_index - intv_radius) * precision; 
		if(fabs(decompressed_data - cur_data) <= precision) return quant_index;
 	}
 	*(unpredictable_data_pos++) = cur_data;
 	return 0;
}

// modified from TypeManager.c
// change return value and increment byteArray
void 
convertIntArray2ByteArray_fast_1b_to_result_sz(const unsigned char* intArray, size_t intArrayLength, unsigned char *& compressed_pos);

// for test meta-compressor. sz implementation can remove this header.
HuffmanTree *
build_Huffman_tree(const int * type, size_t num_elements, size_t state_num);

void
Huffman_encode_tree_and_data(size_t state_num, const int * type, size_t num_elements, unsigned char*& compressed_pos);

#endif

#ifndef _sz_compression_utils_hpp
#define _sz_compression_utils_hpp

#include "sz_def.hpp"

#define QuantIntvMeanCapacity 8192
#define QuantIntvSampleDistance 100
#define QuantIntvSampleCapacity 32768
#define QuantIntvAccThreshold 0.999
#define RegThresholdSize3d 4

#ifdef DUMP_PREDICTION_ERR_FOLDER
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
template <typename Out>
static void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

static std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}
#endif

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
quantize(float pred, T cur_data, double precision, double recip_precision, int capacity, int intv_radius, T *& unpredictable_data_pos, T * decompressed){
	double diff = cur_data - pred;
	double quant_diff = fabs(diff) * recip_precision + 1;
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
quantize(float pred, T cur_data, double precision, double recip_precision, int capacity, int intv_radius, T *& unpredictable_data_pos){
	double diff = cur_data - pred;
	double quant_diff = fabs(diff) * recip_precision + 1;
	if(quant_diff < capacity){
		quant_diff = (diff > 0) ? quant_diff : -quant_diff;
		int quant_index = (int)(quant_diff/2) + intv_radius;
		T decompressed_data = pred + 2 * (quant_index - intv_radius) * precision; 
		if(fabs(decompressed_data - cur_data) <= precision) return quant_index;
 	}
 	*(unpredictable_data_pos++) = cur_data;
 	return 0;
}

template<typename T>
inline void
compress_regression_coefficient_3d(const T * precisions, const T * recip_precisions, float * reg_params_pos, int * reg_params_type_pos, float *& reg_unpredictable_data_pos){
	float * prev_reg_params = reg_params_pos - RegCoeffNum3d;
	for(int i=0; i<RegCoeffNum3d; i++){
		*(reg_params_type_pos ++) = quantize(*prev_reg_params, *reg_params_pos, precisions[i], recip_precisions[i], RegCoeffCapacity, RegCoeffRadius, reg_unpredictable_data_pos, reg_params_pos);
		prev_reg_params ++, reg_params_pos ++; 
	}
}

template<typename T>
inline void
compress_regression_coefficient_3d_v2(const int coeff_num, const T * precisions, const T * recip_precisions, float * reg_params_pos, int * reg_params_type_pos, float *& reg_unpredictable_data_pos){
    float * prev_reg_params = reg_params_pos - coeff_num;
    for(int i=0; i<coeff_num; i++){
        *(reg_params_type_pos ++) = quantize(*prev_reg_params, *reg_params_pos, precisions[i], recip_precisions[i], RegCoeffCapacity, RegCoeffRadius, reg_unpredictable_data_pos, reg_params_pos);
        prev_reg_params ++, reg_params_pos ++;
    }
}
template<typename T>
inline void
compress_regression_coefficient_3d_v3(const int coeff_num, const T * precisions, const T * recip_precisions, float * reg_params_pos, int * reg_params_type_pos, float *& reg_unpredictable_data_pos){
    float * prev_reg_params = reg_params_pos - coeff_num;
    for(int i=0; i<coeff_num; i++){
        *(reg_params_type_pos ++) = quantize(*prev_reg_params, *reg_params_pos, precisions[i], recip_precisions[i], RegCoeffCapacity1, RegCoeffRadius1, reg_unpredictable_data_pos, reg_params_pos);
        prev_reg_params ++, reg_params_pos ++;
    }
}

void
encode_regression_coefficients(const int * reg_params_type, const float * reg_unpredictable_data, size_t reg_count, size_t reg_unpredictable_count, unsigned char *& compressed_pos);

void
encode_regression_coefficients_v2(const int * reg_params_type, const float * reg_unpredictable_data, size_t reg_count, size_t reg_unpredictable_count, unsigned char *& compressed_pos);

// copied from conf.c
unsigned int 
round_up_power_of_2(unsigned int base);

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

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

// de-quantization, for use_lorenzo
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

// modified from TypeManager.c
// change return value and increment byteArray
unsigned char * 
convertByteArray2IntArray_fast_1b_sz(size_t intArrayLength, const unsigned char*& compressed_pos, size_t byteArrayLength);

int *
Huffman_decode_tree_and_data(size_t state_num, size_t num_elements, const unsigned char *& compressed_pos);

template<typename T>
float *
decode_regression_coefficients(const unsigned char *& compressed_pos, size_t reg_count, int block_size, T precision){
	size_t reg_unpredictable_count = 0;
	read_variable_from_src(compressed_pos, reg_unpredictable_count);
	const float * reg_unpredictable_data_pos = (const float *) compressed_pos;
	compressed_pos += reg_unpredictable_count * sizeof(float);
	int * reg_type = Huffman_decode_tree_and_data(2*RegCoeffCapacity, RegCoeffNum3d*reg_count, compressed_pos);
	float * reg_params = (float *) malloc(RegCoeffNum3d*(reg_count + 1)*sizeof(float));
	for(int i=0; i<RegCoeffNum3d; i++)
		reg_params[i] = 0;
	T reg_precisions[RegCoeffNum3d];
	T rel_param_err = RegErrThreshold * precision / RegCoeffNum3d;
	for(int i=0; i<RegCoeffNum3d-1; i++)
		reg_precisions[i] = rel_param_err / block_size;
	reg_precisions[RegCoeffNum3d - 1] = rel_param_err;
	float * prev_reg_params = reg_params;
	float * reg_params_pos = reg_params + RegCoeffNum3d;
	const int * type_pos = (const int *) reg_type;
	for(int i=0; i<reg_count; i++){
		for(int j=0; j<RegCoeffNum3d; j++){
			*reg_params_pos = recover(*prev_reg_params, reg_precisions[j], *(type_pos++), RegCoeffRadius, reg_unpredictable_data_pos);
			prev_reg_params ++, reg_params_pos ++;
		}
	}
	free(reg_type);
	return reg_params;
}

template<typename T>
float *
decode_regression_coefficients_v2(const unsigned char *& compressed_pos, size_t reg_count, int block_size, T precision, const sz_params &params){
    size_t reg_unpredictable_count = 0;
    read_variable_from_src(compressed_pos, reg_unpredictable_count);
    const float * reg_unpredictable_data_pos = (const float *) compressed_pos;
    compressed_pos += reg_unpredictable_count * sizeof(float);
    int * reg_type = Huffman_decode_tree_and_data(2*RegCoeffCapacity, RegCoeffNum3d*reg_count, compressed_pos);
    float * reg_params = (float *) malloc(RegCoeffNum3d*(reg_count + 1)*sizeof(float));
    for(int i=0; i<RegCoeffNum3d; i++)
        reg_params[i] = 0;
    T reg_precisions[RegCoeffNum3d];
    for(int i=0; i<RegCoeffNum3d-1; i++){
        reg_precisions[i] = params.regression_param_eb_linear;
    }
    reg_precisions[RegCoeffNum3d - 1] = params.regression_param_eb_independent;
    float * prev_reg_params = reg_params;
    float * reg_params_pos = reg_params + RegCoeffNum3d;
    const int * type_pos = (const int *) reg_type;
    for(int i=0; i<reg_count; i++){
        for(int j=0; j<RegCoeffNum3d; j++){
            *reg_params_pos = recover(*prev_reg_params, reg_precisions[j], *(type_pos++), RegCoeffRadius, reg_unpredictable_data_pos);
            prev_reg_params ++, reg_params_pos ++;
        }
    }
    free(reg_type);
    return reg_params;
}
template<typename T>
float *
decode_poly_regression_coefficients_v2(const unsigned char *& compressed_pos, size_t reg_count, int block_size, T precision, const sz_params &params){
	size_t reg_unpredictable_count = 0;
	read_variable_from_src(compressed_pos, reg_unpredictable_count);
	const float * reg_unpredictable_data_pos = (const float *) compressed_pos;
	compressed_pos += reg_unpredictable_count * sizeof(float);
	int * reg_type = Huffman_decode_tree_and_data(2*RegCoeffCapacity, RegPolyCoeffNum3d*reg_count, compressed_pos);
	float * reg_params = (float *) malloc(RegPolyCoeffNum3d*(reg_count + 1)*sizeof(float));
	for(int i=0; i<RegPolyCoeffNum3d; i++)
		reg_params[i] = 0;

    T reg_precisions_poly[RegPolyCoeffNum3d];
    reg_precisions_poly[0] = params.poly_regression_param_eb_independent;
    for (int i = 1; i < 4; i++) {
        reg_precisions_poly[i] = params.poly_regression_param_eb_linear;
    }
    for (int i = 4; i < 10; i++) {
        reg_precisions_poly[i] = params.poly_regression_param_eb_poly;
    }

	float * prev_reg_params = reg_params;
	float * reg_params_pos = reg_params + RegPolyCoeffNum3d;
	const int * type_pos = (const int *) reg_type;
	for(int i=0; i<reg_count; i++){
		for(int j=0; j<RegPolyCoeffNum3d; j++){
			*reg_params_pos = recover(*prev_reg_params, reg_precisions_poly[j], *(type_pos++), RegCoeffRadius, reg_unpredictable_data_pos);
			prev_reg_params ++, reg_params_pos ++;
		}
	}
	free(reg_type);
	return reg_params;
}

#endif

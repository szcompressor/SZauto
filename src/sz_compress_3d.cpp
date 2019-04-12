#include "sz_compression_utils.hpp"
#include "sz_def.hpp"
#include "sz_prediction.hpp"
#include "sz_compress_3d.hpp"
#include "sz_compress_block_processing.hpp"
#include "sz_optimize_quant_intervals.hpp"
#include "sz_regression_utils.hpp"

// return regression count
// use block-independant lorenzo pred & quant
template<typename T>
size_t
prediction_and_quantization_3d(const T * data, const DSize_3d& size, const meanInfo<T>& mean_info, double precision,
	int capacity, int intv_radius, unsigned char * indicator, int * type, 
	int * reg_params_type, float *& reg_unpredictable_data_pos, T *& unpredictable_data_pos){
	const float noise = precision * LorenzeNoise3d;
	int * type_pos = type;
	unsigned char * indicator_pos = indicator;
	float * reg_params = (float *) malloc(RegCoeffNum3d * (size.num_blocks+1) * sizeof(float));
	for(int i=0; i<RegCoeffNum3d; i++)
		reg_params[i] = 0;
	float * reg_params_pos = reg_params + RegCoeffNum3d;
	int * reg_params_type_pos = reg_params_type;
	double reg_precisions[RegCoeffNum3d];
	float rel_param_err = RegErrThreshold * precision / RegCoeffNum3d;
	for(int i=0; i<RegCoeffNum3d-1; i++) 
		reg_precisions[i] = rel_param_err / size.block_size;
	reg_precisions[RegCoeffNum3d - 1] = rel_param_err;
	size_t buffer_dim0_offset = (size.block_size + 1) * (size.block_size + 1);
	size_t buffer_dim1_offset = size.block_size + 1;
	T * pred_buffer = (T *) malloc((size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	memset(pred_buffer, 0, (size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	size_t reg_count = 0;
	int capacity_lorenzo = mean_info.use_mean ? capacity - 2 : capacity;
	const T * x_data_pos = data;
	for(size_t i=0; i<size.num_x; i++){
		const T * y_data_pos = x_data_pos;
		for(size_t j=0; j<size.num_y; j++){
			const T * z_data_pos = y_data_pos;
			for(size_t k=0; k<size.num_z; k++){
				int size_x = ((i+1)*size.block_size < size.d1) ? size.block_size : size.d1 - i*size.block_size;
				int size_y = ((j+1)*size.block_size < size.d2) ? size.block_size : size.d2 - j*size.block_size;
				int size_z = ((k+1)*size.block_size < size.d3) ? size.block_size : size.d3 - k*size.block_size;
				int min_size = MIN(size_x, size_y);
				min_size = MIN(min_size, size_z);
				// size of block is less than some threshold
				if(min_size < RegThresholdSize3d){
					*indicator_pos = 0;
				}
				else{
					compute_regression_coeffcients_3d(z_data_pos, size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, reg_params_pos);
					*indicator_pos = sz_blockwise_selection_3d(z_data_pos, mean_info, size.dim0_offset, size.dim1_offset, min_size, noise, reg_params_pos);
				}
				if(*indicator_pos){
					// regression
					compress_regression_coefficient_3d(reg_precisions, reg_params_pos, reg_params_type_pos, reg_unpredictable_data_pos);
					block_pred_and_quant_regression_3d(z_data_pos, reg_params_pos, precision, capacity, intv_radius, 
						size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos);
					reg_count ++;
					reg_params_pos += RegCoeffNum3d;
					reg_params_type_pos += RegCoeffNum3d;
				}
				else{
					// Lorenzo
					block_pred_and_quant_lorenzo_3d(mean_info, z_data_pos, pred_buffer, precision, capacity_lorenzo, intv_radius, 
						size_x, size_y, size_z, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos);
				}
				indicator_pos ++;
				z_data_pos += size_z;
			}
			y_data_pos += size.block_size*size.dim1_offset;
		}
		x_data_pos += size.block_size*size.dim0_offset;
	}
	free(pred_buffer);
	free(reg_params);
	return reg_count;
}

// return regression count
// use block-dependant lorenzo pred & quant
template<typename T>
size_t
prediction_and_quantization_3d_with_border_prediction(const T * data, const DSize_3d& size, const meanInfo<T>& mean_info, double precision,
	int capacity, int intv_radius, unsigned char * indicator, int * type, 
	int * reg_params_type, float *& reg_unpredictable_data_pos, T *& unpredictable_data_pos){
	const float noise = precision * LorenzeNoise3d;
	int * type_pos = type;
	unsigned char * indicator_pos = indicator;
	float * reg_params = (float *) malloc(RegCoeffNum3d * (size.num_blocks+1) * sizeof(float));
	for(int i=0; i<RegCoeffNum3d; i++)
		reg_params[i] = 0;
	float * reg_params_pos = reg_params + RegCoeffNum3d;
	int * reg_params_type_pos = reg_params_type;
	double reg_precisions[RegCoeffNum3d];
	float rel_param_err = RegErrThreshold * precision / RegCoeffNum3d;
	for(int i=0; i<RegCoeffNum3d-1; i++) 
		reg_precisions[i] = rel_param_err / size.block_size;
	reg_precisions[RegCoeffNum3d - 1] = rel_param_err;
	// maintain a buffer of (block_size+1)*(r2+1)*(r3+1)
	size_t buffer_dim0_offset = (size.d2+1)*(size.d3+1);
	size_t buffer_dim1_offset = size.d3+1;
	T * pred_buffer = (T *) malloc((size.block_size+1)*(size.d2+1)*(size.d3+1)*sizeof(T));
	memset(pred_buffer, 0, (size.block_size+1)*(size.d2+1)*(size.d3+1)*sizeof(T));
	size_t reg_count = 0;
	int capacity_lorenzo = mean_info.use_mean ? capacity - 2 : capacity;
	const T * x_data_pos = data;
	for(size_t i=0; i<size.num_x; i++){
		const T * y_data_pos = x_data_pos;
		T * pred_buffer_pos = pred_buffer;
		for(size_t j=0; j<size.num_y; j++){
			const T * z_data_pos = y_data_pos;
			for(size_t k=0; k<size.num_z; k++){
				int size_x = ((i+1)*size.block_size < size.d1) ? size.block_size : size.d1 - i*size.block_size;
				int size_y = ((j+1)*size.block_size < size.d2) ? size.block_size : size.d2 - j*size.block_size;
				int size_z = ((k+1)*size.block_size < size.d3) ? size.block_size : size.d3 - k*size.block_size;
				int min_size = MIN(size_x, size_y);
				min_size = MIN(min_size, size_z);
				// size of block is less than some threshold
				if(min_size < RegThresholdSize3d){
					*indicator_pos = 0;
				}
				else{
					compute_regression_coeffcients_3d(z_data_pos, size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, reg_params_pos);
					*indicator_pos = sz_blockwise_selection_3d(z_data_pos, mean_info, size.dim0_offset, size.dim1_offset, min_size, noise, reg_params_pos);
				}
				if(*indicator_pos){
					// regression
					compress_regression_coefficient_3d(reg_precisions, reg_params_pos, reg_params_type_pos, reg_unpredictable_data_pos);
					block_pred_and_quant_regression_3d_with_buffer(z_data_pos, reg_params_pos, pred_buffer_pos, precision, capacity, intv_radius, 
						size_x, size_y, size_z, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos);
					reg_count ++;
					reg_params_pos += RegCoeffNum3d;
					reg_params_type_pos += RegCoeffNum3d;
				}
				else{
					// Lorenzo
					block_pred_and_quant_lorenzo_3d(mean_info, z_data_pos, pred_buffer_pos, precision, capacity_lorenzo, intv_radius, 
						size_x, size_y, size_z, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos);
				}
				pred_buffer_pos += size.block_size;
				indicator_pos ++;
				z_data_pos += size_z;
			}
			y_data_pos += size.block_size*size.dim1_offset;
			pred_buffer_pos += size.block_size*buffer_dim1_offset - size.block_size*size.num_z;
		}
		// copy bottom of buffer to top of buffer
		memcpy(pred_buffer, pred_buffer + size.block_size*buffer_dim0_offset, buffer_dim0_offset*sizeof(T));
		x_data_pos += size.block_size*size.dim0_offset;
	}
	free(pred_buffer);
	free(reg_params);
	return reg_count;
}

// perform compression
template<typename T>
unsigned char *
sz_compress_3d(const T * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, int BSIZE3d, bool block_independant){
	// block_independant = false;
	DSize_3d size(r1, r2, r3, BSIZE3d);
	int capacity = 0; // num of quant intervals
	meanInfo<T> mean_info = optimize_quant_invl_3d(data, r1, r2, r3, precision, capacity);
	int intv_radius = (capacity >> 1);
	int * type = (int *) malloc(size.num_elements * sizeof(int));
	T * unpredictable_data = (T *) malloc((0.05*size.num_elements) * sizeof(T));
	unsigned char * indicator = (unsigned char *) malloc(size.num_blocks * sizeof(unsigned char));
	T * unpredictable_data_pos = unpredictable_data;
	int * reg_params_type = (int *) malloc(RegCoeffNum3d * size.num_blocks * sizeof(int));
	float * reg_unpredictable_data = (float *) malloc(RegCoeffNum3d * size.num_blocks * sizeof(float));
	float * reg_unpredictable_data_pos = reg_unpredictable_data;
	size_t reg_count = block_independant? prediction_and_quantization_3d(data, size, mean_info, precision, capacity, intv_radius, indicator, type, reg_params_type, reg_unpredictable_data_pos, unpredictable_data_pos)
			: prediction_and_quantization_3d_with_border_prediction(data, size, mean_info, precision, capacity, intv_radius, indicator, type, reg_params_type, reg_unpredictable_data_pos, unpredictable_data_pos);
	size_t unpredictable_count = unpredictable_data_pos - unpredictable_data;
	unsigned char * compressed = NULL;
	// TODO: change to a better estimated size
	size_t est_size = size.num_elements*sizeof(T)*1.2;
	compressed = (unsigned char *) malloc(est_size);
	unsigned char * compressed_pos = compressed;
	write_variable_to_dst(compressed_pos, size.block_size);
	write_variable_to_dst(compressed_pos, precision);
	write_variable_to_dst(compressed_pos, (char) block_independant);
	write_variable_to_dst(compressed_pos, intv_radius);
	write_variable_to_dst(compressed_pos, mean_info);
	write_variable_to_dst(compressed_pos, reg_count);
	write_variable_to_dst(compressed_pos, unpredictable_count);
	write_array_to_dst(compressed_pos, unpredictable_data, unpredictable_count);
	convertIntArray2ByteArray_fast_1b_to_result_sz(indicator, size.num_blocks, compressed_pos);
	if(reg_count) encode_regression_coefficients(reg_params_type, reg_unpredictable_data, reg_count, reg_unpredictable_data_pos - reg_unpredictable_data, compressed_pos);
	Huffman_encode_tree_and_data(2*capacity, type, size.num_elements, compressed_pos);
	compressed_size = compressed_pos - compressed;
	free(indicator);
	free(unpredictable_data);
	free(reg_params_type);
	free(reg_unpredictable_data);
	free(type);
	return compressed;
}

template 
unsigned char * 
sz_compress_3d<float>(const float * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, int BSIZE3d, bool block_independant);





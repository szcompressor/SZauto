#include "sz_decompress_3d.hpp"

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

template<typename T>
inline void
block_pred_and_decompress_regression_3d(const float * reg_params_pos, double precision, int intv_radius, 
	int size_x, int size_y, int size_z, size_t dim0_offset, size_t dim1_offset, 
	const int *& type_pos, const T *& unpredictable_data_pos, T * dec_data_pos){
	T * cur_data_pos = dec_data_pos;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			for(int k=0; k<size_z; k++){
				float pred = regression_predict_3d(reg_params_pos, i, j, k);
				*(cur_data_pos++) = recover(pred, precision, *(type_pos++), intv_radius, unpredictable_data_pos);
			}
			cur_data_pos += dim1_offset - size_z;
		}
		cur_data_pos += dim0_offset - size_y * dim1_offset;
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

// block-independant lorenzo pred & decompress
template<typename T>
inline void
block_pred_and_decompress_lorenzo_3d(const meanInfo<T>& mean_info, T * buffer, int buffer_block_size, double precision, int intv_radius, 
	int size_x, int size_y, int size_z, size_t dim0_offset, size_t dim1_offset, 
	const int *& type_pos, const T *& unpredictable_data_pos, T * dec_data_pos){
	T * cur_data_pos = dec_data_pos;
	T * buffer_pos = buffer + buffer_block_size*buffer_block_size + buffer_block_size + 1;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			memcpy(buffer_pos, cur_data_pos, sizeof(T)*size_z);
			T * cur_buffer_pos = buffer_pos;
			for(int k=0; k<size_z; k++){
				float pred = lorenzo_predict_3d(cur_buffer_pos, buffer_block_size*buffer_block_size, buffer_block_size);
				T decompressed = recover(mean_info, pred, precision, *(type_pos++), intv_radius, unpredictable_data_pos);
				*(cur_data_pos++) = decompressed;
				*(cur_buffer_pos++) = decompressed;
			}
			buffer_pos += buffer_block_size;
			cur_data_pos += dim1_offset - size_z;
		}
		buffer_pos += buffer_block_size*buffer_block_size - size_y*buffer_block_size;
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

template<typename T>
void
prediction_and_decompression_3d(const DSize_3d& size, const meanInfo<T>& mean_info, double precision,
	int intv_radius, const float * reg_params, const unsigned char * indicator, 
	const int * type, const T * unpredictable_data_pos, T * dec_data){
	const int * type_pos = type;
	const unsigned char * indicator_pos = indicator;
	const float * reg_params_pos = reg_params;
	T * pred_buffer = (T *) malloc((size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	memset(pred_buffer, 0, (size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	T * x_data_pos = dec_data;
	for(size_t i=0; i<size.num_x; i++){
		T * y_data_pos = x_data_pos;
		for(size_t j=0; j<size.num_y; j++){
			T * z_data_pos = y_data_pos;
			for(size_t k=0; k<size.num_z; k++){
				int size_x = ((i+1)*size.block_size < size.d1) ? size.block_size : size.d1 - i*size.block_size;
				int size_y = ((j+1)*size.block_size < size.d2) ? size.block_size : size.d2 - j*size.block_size;
				int size_z = ((k+1)*size.block_size < size.d3) ? size.block_size : size.d3 - k*size.block_size;
				if(*indicator_pos){
					// regression
					block_pred_and_decompress_regression_3d(reg_params_pos, precision, intv_radius, 
						size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos, z_data_pos);
					reg_params_pos += RegCoeffNum3d;
				}
				else{
					// Lorenzo
					block_pred_and_decompress_lorenzo_3d(mean_info, pred_buffer, size.block_size+1, precision, intv_radius, 
						size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos, z_data_pos);
				}
				indicator_pos ++;
				z_data_pos += size_x;
			}
			y_data_pos += size.block_size*size.dim1_offset;
		}
		x_data_pos += size.block_size*size.dim0_offset;
	}
	free(pred_buffer);
}

// perform block-independant decompression
// TODO: add const to compressed and related functions (current Huffman is not const)
template<typename T>
T * 
sz_decompress_3d(const unsigned char * compressed, size_t r1, size_t r2, size_t r3){
	const unsigned char * compressed_pos = compressed;
	int block_size = 0;
	read_variable_from_src<int>(compressed_pos, block_size);
	DSize_3d size(r1, r2, r3, block_size);
	double precision = 0;
	read_variable_from_src<double>(compressed_pos, precision);
	int intv_radius = 0;
	read_variable_from_src(compressed_pos, intv_radius);
	meanInfo<T> mean_info;
	read_variable_from_src(compressed_pos, mean_info);
	size_t reg_count = 0;
	read_variable_from_src(compressed_pos, reg_count);
	size_t unpredictable_count = 0;
	read_variable_from_src(compressed_pos, unpredictable_count);
	T * unpredictable_data = read_array_from_src<T>(compressed_pos, unpredictable_count);
	unsigned char * indicator = convertByteArray2IntArray_fast_1b_sz(size.num_blocks, compressed_pos, (size.num_blocks - 1)/8 + 1);
	// unsigned char * indicator = read_array_from_src<unsigned char>(compressed_pos, size.num_blocks);
	float * reg_params = read_array_from_src<float>(compressed_pos, RegCoeffNum3d*reg_count);
	// int * type = read_array_from_src<int>(compressed_pos, size.num_elements);
	int * type = Huffman_decode_tree_and_data(4*intv_radius, size.num_elements, compressed_pos);
	T * dec_data = (T *) malloc(size.num_elements*sizeof(T));
	prediction_and_decompression_3d(size, mean_info, precision, intv_radius, reg_params, indicator, type, unpredictable_data, dec_data);
	free(indicator);
	free(unpredictable_data);
	free(reg_params);
	free(type);
	return dec_data;
}

template
float * 
sz_decompress_3d<float>(const unsigned char * compressed, size_t r1, size_t r2, size_t r3);




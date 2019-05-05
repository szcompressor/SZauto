#ifndef _sz_decompress_block_processing_knl_hpp
#define _sz_decompress_block_processing_knl_hpp

// avoid mistake when running on KNL
// need to be checked later;
extern float * dec_data_sp_float;

template<typename T>
inline void
block_pred_and_decompress_regression_3d_knl(const float * reg_params_pos, T precision, int intv_radius, 
	int size_x, int size_y, int size_z, size_t dim0_offset, size_t dim1_offset, 
	const int *& type_pos, int * unpred_count_buffer, const T * unpred_data_buffer, const int offset, T * dec_data_pos){
	T * cur_data_pos = dec_data_pos;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			for(int k=0; k<size_z; k++){
				int index = j*size_z + k;
				int type_val = type_pos[index];
				if(type_val == 0){
					cur_data_pos[j*dim1_offset + k] = unpred_data_buffer[index*offset + unpred_count_buffer[index]];
					unpred_count_buffer[index] ++;
				}
				else{
					T pred = (T) (reg_params_pos[0] * (float)i + reg_params_pos[1] * (float)j + reg_params_pos[2] * (float)k + reg_params_pos[3]);
					cur_data_pos[j*dim1_offset + k] = pred + (T)(2 * (type_val - intv_radius)) * precision;
				}
			}
		}
		type_pos += size_y * size_z;
		cur_data_pos += dim0_offset;
	}
}

template<typename T>
inline void
block_pred_and_decompress_regression_3d_with_buffer_knl(const float * reg_params_pos, T * buffer, T precision, int intv_radius, 
	int size_x, int size_y, int size_z, size_t buffer_dim0_offset, size_t buffer_dim1_offset,
	size_t dim0_offset, size_t dim1_offset, const int *& type_pos, int * unpred_count_buffer, const T * unpred_data_buffer, const int offset, T * dec_data_pos){
	T * cur_data_pos = dec_data_pos;
	T * buffer_pos = buffer + buffer_dim0_offset + buffer_dim1_offset + 1;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			for(int k=0; k<size_z; k++){
				int index = j*size_z + k;
				int type_val = type_pos[index];
				if(type_val == 0){
					cur_data_pos[j*dim1_offset + k] = buffer_pos[j*buffer_dim1_offset + k] = unpred_data_buffer[index*offset + unpred_count_buffer[index]];
					unpred_count_buffer[index] ++;
				}
				else{
					T pred = (T) (reg_params_pos[0] * (float)i + reg_params_pos[1] * (float)j + reg_params_pos[2] * (float)k + reg_params_pos[3]);
					cur_data_pos[j*dim1_offset + k] = buffer_pos[j*buffer_dim1_offset + k] = pred + (T)(2 * (type_val - intv_radius)) * precision;
				}
			}
		}
		type_pos += size_y * size_z;
		cur_data_pos += dim0_offset;
		buffer_pos += buffer_dim0_offset;
	}
}

// block-independant lorenzo pred & decompress
template<typename T>
inline void
block_pred_and_decompress_lorenzo_3d_knl_1d_pred(const meanInfo<T>& mean_info, T * buffer, T precision, int intv_radius, 
	int size_x, int size_y, int size_z, size_t buffer_dim0_offset, size_t buffer_dim1_offset, size_t dim0_offset, size_t dim1_offset, 
	const int *& type_pos, int * unpred_count_buffer, const T * unpred_data_buffer, const int offset, T * dec_data_pos){
	T * cur_data_pos = dec_data_pos;
	T * buffer_pos = buffer + buffer_dim0_offset + buffer_dim1_offset + 1;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			for(int k=0; k<size_z; k++){
				int index = j*size_z + k;
				int type_val = type_pos[index];
				if((mean_info.use_mean) && (type_val == 1)) cur_data_pos[k] = buffer_pos[k] = mean_info.mean;
				else{
					T * cur_buffer_pos = buffer_pos + k;
					if(type_val == 0){
						cur_data_pos[k] = *cur_buffer_pos = unpred_data_buffer[index*offset + unpred_count_buffer[index]];
						unpred_count_buffer[index] ++;
					}
					else{				
						T pred = cur_buffer_pos[-buffer_dim0_offset];
						cur_data_pos[k] = *cur_buffer_pos = pred + (T)(2 * (type_val - intv_radius)) * precision;
					}
				}
			}
			buffer_pos += buffer_dim1_offset;
			cur_data_pos += dim1_offset;
		}
		type_pos += size_y * size_z;
		buffer_pos += buffer_dim0_offset - size_y*buffer_dim1_offset;
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

template<typename T>
inline void
block_pred_and_decompress_lorenzo_3d_knl_2d_pred(const meanInfo<T>& mean_info, T * buffer, T precision, int intv_radius, 
	int size_x, int size_y, int size_z, size_t buffer_dim0_offset, size_t buffer_dim1_offset, size_t dim0_offset, size_t dim1_offset, 
	const int *& type_pos, int * unpred_count_buffer, const T * unpred_data_buffer, const int offset, T * dec_data_pos){
	T * cur_data_pos = dec_data_pos;
	T * buffer_pos = buffer + buffer_dim0_offset + buffer_dim1_offset + 1;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			for(int k=0; k<size_z; k++){
				int index = j*size_z + k;
				int type_val = type_pos[index];
				if((mean_info.use_mean) && (type_val == 1)) cur_data_pos[k] = buffer_pos[k] = mean_info.mean;
				else{
					T * cur_buffer_pos = buffer_pos + k;
					if(type_val == 0){
						cur_data_pos[k] = *cur_buffer_pos = unpred_data_buffer[index*offset + unpred_count_buffer[index]];
						unpred_count_buffer[index] ++;
					}
					else{				
						T pred = cur_buffer_pos[-buffer_dim0_offset] + cur_buffer_pos[-buffer_dim1_offset] - cur_buffer_pos[- buffer_dim0_offset - buffer_dim1_offset];
						cur_data_pos[k] = *cur_buffer_pos = pred + (T)(2 * (type_val - intv_radius)) * precision;
						if(cur_data_pos + k - dec_data_sp_float == -1){
							printf("index out of bound, dec_data=%.4f, type=%d, pred=%.4f\n", *cur_buffer_pos, type_val, pred);
							exit(0);
						}
					}
				}
			}
			buffer_pos += buffer_dim1_offset;
			cur_data_pos += dim1_offset;
		}
		type_pos += size_y * size_z;
		buffer_pos += buffer_dim0_offset - size_y*buffer_dim1_offset;
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

template<typename T>
inline void
block_pred_and_decompress_lorenzo_3d_knl_3d_pred(const meanInfo<T>& mean_info, T * buffer, T precision, int intv_radius, 
	int size_x, int size_y, int size_z, size_t buffer_dim0_offset, size_t buffer_dim1_offset, size_t dim0_offset, size_t dim1_offset, 
	const int *& type_pos, int * unpred_count_buffer, const T * unpred_data_buffer, const int offset, T * dec_data_pos){
	T * cur_data_pos = dec_data_pos;
	T * buffer_pos = buffer + buffer_dim0_offset + buffer_dim1_offset + 1;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			for(int k=0; k<size_z; k++){
				int index = j*size_z + k;
				int type_val = type_pos[index];
				if((mean_info.use_mean) && (type_val == 1)) cur_data_pos[k] = buffer_pos[k] = mean_info.mean;
				else{
					T * cur_buffer_pos = buffer_pos + k;
					if(type_val == 0){
						cur_data_pos[k] = *cur_buffer_pos = unpred_data_buffer[index*offset + unpred_count_buffer[index]];
						unpred_count_buffer[index] ++;
					}
					else{				
						T pred = cur_buffer_pos[-1] + cur_buffer_pos[-buffer_dim1_offset] + cur_buffer_pos[-buffer_dim0_offset] 
								- cur_buffer_pos[-buffer_dim1_offset - 1] - cur_buffer_pos[-buffer_dim0_offset - 1] 
								- cur_buffer_pos[-buffer_dim0_offset - buffer_dim1_offset] + cur_buffer_pos[-buffer_dim0_offset - buffer_dim1_offset - 1];
						cur_data_pos[k] = *cur_buffer_pos = pred + (T)(2 * (type_val - intv_radius)) * precision;
						if(cur_data_pos + k - dec_data_sp_float == -1){
							printf("index out of bound, dec_data=%.4f, type=%d, pred=%.4f\n", *cur_buffer_pos, type_val, pred);
							exit(0);
						}
					}
				}
			}
			buffer_pos += buffer_dim1_offset;
			cur_data_pos += dim1_offset;
		}
		type_pos += size_y * size_z;
		buffer_pos += buffer_dim0_offset - size_y*buffer_dim1_offset;
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

#endif
#ifndef _sz_compress_block_processing_hpp
#define _sz_compress_block_processing_hpp

template<typename T>
inline void
sz_block_error_estimation_3d(const T * data_pos, const float * reg_params_pos, const meanInfo<T>& mean_info, int x, int y, int z, size_t dim0_offset, size_t dim1_offset, float noise, double& err_reg, double& err_lorenzo){
	const T * cur_data_pos = data_pos + x*dim0_offset + y*dim1_offset + z;
	T cur_data = *cur_data_pos;
	err_reg += fabs(cur_data - regression_predict_3d<T>(reg_params_pos, x, y, z));
	err_lorenzo += mean_info.use_mean ? MIN(fabs(cur_data - mean_info.mean), fabs(cur_data - lorenzo_predict_3d(cur_data_pos, dim0_offset, dim1_offset)) + noise) : fabs(cur_data - lorenzo_predict_3d(cur_data_pos, dim0_offset, dim1_offset)) + noise;
}

template<typename T>
inline int
sz_blockwise_selection_3d(const T * data_pos, const meanInfo<T>& mean_info, size_t dim0_offset, size_t dim1_offset, int min_size, float noise, const float * reg_params_pos){
	double err_reg = 0;
	double err_lorenzo = 0;
	for(int i=1; i<min_size; i++){
		int bmi = min_size - i;
		sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, i, i, dim0_offset, dim1_offset, noise, err_reg, err_lorenzo);
		sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, i, bmi, dim0_offset, dim1_offset, noise, err_reg, err_lorenzo);
		sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, bmi, i, dim0_offset, dim1_offset, noise, err_reg, err_lorenzo);
		sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, bmi, bmi, dim0_offset, dim1_offset, noise, err_reg, err_lorenzo);
	}
	return err_reg < err_lorenzo;
}

template<typename T>
inline void
block_pred_and_quant_regression_3d(const T * data_pos, const float * reg_params_pos, double precision, int capacity, 
	int intv_radius, int size_x, int size_y, int size_z, size_t dim0_offset, size_t dim1_offset, int *& type_pos, T *& unpredictable_data_pos){
	const T * cur_data_pos = data_pos;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			for(int k=0; k<size_z; k++){
				float pred = regression_predict_3d<T>(reg_params_pos, i, j, k);
				*(type_pos++) = quantize(pred, *cur_data_pos, precision, capacity, intv_radius, unpredictable_data_pos);
				cur_data_pos ++;
			}
			cur_data_pos += dim1_offset - size_z;
		}
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

template<typename T>
inline void
block_pred_and_quant_regression_3d_with_buffer(const T * data_pos, const float * reg_params_pos, T * buffer, double precision, int capacity, 
	int intv_radius, int size_x, int size_y, int size_z, size_t buffer_dim0_offset, size_t buffer_dim1_offset,
	size_t dim0_offset, size_t dim1_offset, int *& type_pos, T *& unpredictable_data_pos){
	const T * cur_data_pos = data_pos;
	T * buffer_pos = buffer + buffer_dim0_offset + buffer_dim1_offset + 1;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			T * cur_buffer_pos = buffer_pos;
			for(int k=0; k<size_z; k++){
				float pred = regression_predict_3d<T>(reg_params_pos, i, j, k);
				*(type_pos++) = quantize(pred, *cur_data_pos, precision, capacity, intv_radius, unpredictable_data_pos, cur_buffer_pos);
				cur_data_pos ++;
				cur_buffer_pos ++;
			}
			buffer_pos += buffer_dim1_offset;
			cur_data_pos += dim1_offset - size_z;
		}
		buffer_pos += buffer_dim0_offset - size_y*buffer_dim1_offset;
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

// block-independant lorenzo pred & quant
template<typename T>
inline void
block_pred_and_quant_lorenzo_3d(const meanInfo<T>& mean_info, const T * data_pos, T * buffer, double precision, int capacity, int intv_radius, 
	int size_x, int size_y, int size_z, size_t buffer_dim0_offset, size_t buffer_dim1_offset,
	size_t dim0_offset, size_t dim1_offset, int *& type_pos, T *& unpredictable_data_pos){
	const T * cur_data_pos = data_pos;
	T * buffer_pos = buffer + buffer_dim0_offset + buffer_dim1_offset + 1;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			T * cur_buffer_pos = buffer_pos;
			for(int k=0; k<size_z; k++){
				if(mean_info.use_mean && (fabs(*cur_data_pos - mean_info.mean) < precision)){
					*(type_pos++) = 1;
					*cur_buffer_pos = mean_info.mean;
				}
				else{
					float pred = lorenzo_predict_3d(cur_buffer_pos, buffer_dim0_offset, buffer_dim1_offset);
					*(type_pos++) = quantize(pred, *cur_data_pos, precision, capacity, intv_radius, unpredictable_data_pos, cur_buffer_pos);
				}
				cur_data_pos ++;
				cur_buffer_pos ++;
			}
			buffer_pos += buffer_dim1_offset;
			cur_data_pos += dim1_offset - size_z;
		}
		buffer_pos += buffer_dim0_offset - size_y*buffer_dim1_offset;
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

#endif
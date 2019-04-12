#ifndef _sz_decompress_block_processing_hpp
#define _sz_decompress_block_processing_hpp

template<typename T>
inline void
block_pred_and_decompress_regression_3d(const float * reg_params_pos, double precision, int intv_radius, 
	int size_x, int size_y, int size_z, size_t dim0_offset, size_t dim1_offset, 
	const int *& type_pos, const T *& unpredictable_data_pos, T * dec_data_pos){
	T * cur_data_pos = dec_data_pos;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			for(int k=0; k<size_z; k++){
				float pred = regression_predict_3d<T>(reg_params_pos, i, j, k);
				*(cur_data_pos++) = recover(pred, precision, *(type_pos++), intv_radius, unpredictable_data_pos);
			}
			cur_data_pos += dim1_offset - size_z;
		}
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

template<typename T>
inline void
block_pred_and_decompress_regression_3d_with_buffer(const float * reg_params_pos, T * buffer, double precision, int intv_radius, 
	int size_x, int size_y, int size_z, size_t buffer_dim0_offset, size_t buffer_dim1_offset,
	size_t dim0_offset, size_t dim1_offset, const int *& type_pos, const T *& unpredictable_data_pos, T * dec_data_pos){
	T * cur_data_pos = dec_data_pos;
	T * buffer_pos = buffer + buffer_dim0_offset + buffer_dim1_offset + 1;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			T * cur_buffer_pos = buffer_pos;
			for(int k=0; k<size_z; k++){
				float pred = regression_predict_3d<T>(reg_params_pos, i, j, k);
				T decompressed = recover(pred, precision, *(type_pos++), intv_radius, unpredictable_data_pos);
				*(cur_data_pos++) = decompressed;
				*(cur_buffer_pos++) = decompressed;
			}
			buffer_pos += buffer_dim1_offset;
			cur_data_pos += dim1_offset - size_z;
		}
		buffer_pos += buffer_dim0_offset - size_y*buffer_dim1_offset;
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

// block-independant lorenzo pred & decompress
template<typename T>
inline void
block_pred_and_decompress_lorenzo_3d(const meanInfo<T>& mean_info, T * buffer, double precision, int intv_radius, 
	int size_x, int size_y, int size_z, size_t buffer_dim0_offset, size_t buffer_dim1_offset, size_t dim0_offset, size_t dim1_offset, 
	const int *& type_pos, const T *& unpredictable_data_pos, T * dec_data_pos){
	T * cur_data_pos = dec_data_pos;
	T * buffer_pos = buffer + buffer_dim0_offset + buffer_dim1_offset + 1;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			T * cur_buffer_pos = buffer_pos;
			for(int k=0; k<size_z; k++){
				float pred = lorenzo_predict_3d(cur_buffer_pos, buffer_dim0_offset, buffer_dim1_offset);
				T decompressed = recover(mean_info, pred, precision, *(type_pos++), intv_radius, unpredictable_data_pos);
				*(cur_data_pos++) = decompressed;
				*(cur_buffer_pos++) = decompressed;
			}
			buffer_pos += buffer_dim1_offset;
			cur_data_pos += dim1_offset - size_z;
		}
		buffer_pos += buffer_dim0_offset - size_y*buffer_dim1_offset;
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

#endif
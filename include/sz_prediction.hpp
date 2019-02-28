#ifndef _sz_prediction_h
#define _sz_prediction_h

template<typename T>
inline T
regression_predict_3d(const float * reg_params_pos, int x, int y, int z){
	return reg_params_pos[0] * x + reg_params_pos[1] * y + reg_params_pos[2] * z + reg_params_pos[3];
}

template<typename T>
inline T
lorenzo_predict_3d(const T * data_pos, size_t dim0_offset, size_t dim1_offset){
	return data_pos[-1] + data_pos[-dim1_offset] + data_pos[-dim0_offset] 
	- data_pos[-dim1_offset - 1] - data_pos[-dim0_offset - 1] 
	- data_pos[-dim0_offset - dim1_offset] + data_pos[-dim0_offset - dim1_offset - 1];
}

#endif
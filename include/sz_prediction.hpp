#ifndef _sz_prediction_hpp
#define _sz_prediction_hpp

template<typename T>
inline T
regression_predict_3d(const float * reg_params_pos, int x, int y, int z){
	return reg_params_pos[0] * x + reg_params_pos[1] * y + reg_params_pos[2] * z + reg_params_pos[3];
}

template<typename T>
inline T
regression_predict_3d_poly(const float * reg_params_pos, int x, int y, int z){

    return reg_params_pos[0] + reg_params_pos[1] * x + reg_params_pos[2] * y + reg_params_pos[3] * z
           + reg_params_pos[4] * x * x + reg_params_pos[5] * x * y + reg_params_pos[6] * x * z
           + reg_params_pos[7] * y * y + reg_params_pos[8] * y * z + reg_params_pos[9] * z * z;
}


template<typename T>
inline T
lorenzo_predict_1d(const T * data_pos, size_t dim0_offset){
	return data_pos[-1];
}

template<typename T>
inline T
lorenzo_predict_1d_2layer(const T * data_pos, size_t dim0_offset){
    return 2 * data_pos[-1] - data_pos[-2];
}


template<typename T>
inline T
lorenzo_predict_2d(const T * data_pos, size_t dim0_offset, size_t dim1_offset){
	return data_pos[-1] + data_pos[-dim0_offset] - data_pos[-1 - dim0_offset];
}

template<typename T>
inline T
lorenzo_predict_2d_2layer(const T * data_pos, size_t dim0_offset, size_t dim1_offset){
    return 2 * data_pos[-dim0_offset]
           - data_pos[-2 * dim0_offset]
           + 2 * data_pos[-1]
           - 4 * data_pos[-1 - dim0_offset]
           + 2 * data_pos[-1 - 2 * dim0_offset]
           - data_pos[-2]
           + 2 * data_pos[-2 - dim0_offset]
           - data_pos[-2 - 2 * dim0_offset];
}

template<typename T>
inline T
lorenzo_predict_3d(const T * data_pos, size_t dim0_offset, size_t dim1_offset){
	return data_pos[-1] + data_pos[-dim1_offset] + data_pos[-dim0_offset] 
	- data_pos[-dim1_offset - 1] - data_pos[-dim0_offset - 1] 
	- data_pos[-dim0_offset - dim1_offset] + data_pos[-dim0_offset - dim1_offset - 1];
}

template<typename T>
inline T
lorenzo_predict_3d_2layer(const T * data_pos, size_t dim0_offset, size_t dim1_offset){
    return 2 * data_pos[-1]
           - data_pos[-2]
           + 2 * data_pos[-dim1_offset]
           - 4 * data_pos[-dim1_offset - 1]
           + 2 * data_pos[-dim1_offset - 2]
           - data_pos[-2 * dim1_offset]
           + 2 * data_pos[-2 * dim1_offset - 1]
           - data_pos[-2 * dim1_offset - 2]
           + 2 * data_pos[-dim0_offset]
           - 4 * data_pos[-dim0_offset - 1]
           + 2 * data_pos[-dim0_offset - 2]
           - 4 * data_pos[-dim0_offset - dim1_offset]
           + 8 * data_pos[-dim0_offset - dim1_offset - 1]
           - 4 * data_pos[-dim0_offset - dim1_offset - 2]
           + 2 * data_pos[-dim0_offset - 2 * dim1_offset]
           - 4 * data_pos[-dim0_offset - 2 * dim1_offset - 1]
           + 2 * data_pos[-dim0_offset - 2 * dim1_offset - 2]
           - data_pos[-2 * dim0_offset]
           + 2 * data_pos[-2 * dim0_offset - 1]
           - data_pos[-2 * dim0_offset - 2]
           + 2 * data_pos[-2 * dim0_offset - dim1_offset]
           - 4 * data_pos[-2 * dim0_offset - dim1_offset - 1]
           + 2 * data_pos[-2 * dim0_offset - dim1_offset - 2]
           - data_pos[-2 * dim0_offset - 2 * dim1_offset]
           + 2 * data_pos[-2 * dim0_offset - 2 * dim1_offset - 1]
           - data_pos[-2 * dim0_offset - 2 * dim1_offset - 2];
}

#endif
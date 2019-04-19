#ifndef _sz_regression_utils_hpp
#define _sz_regression_utils_hpp

template <typename T>
inline void 
compute_regression_coeffcients_3d(const T * data_pos, int size_x, int size_y, int size_z, size_t dim0_offset, size_t dim1_offset, float * reg_params_pos){
	/*Calculate regression coefficients*/
	const T * cur_data_pos = data_pos;
	float fx = 0.0;
	float fy = 0.0;
	float fz = 0.0;
	float f = 0;
	float sum_x, sum_y; 
	T curData;
	for(int i=0; i<size_x; i++){
		sum_x = 0;
		for(int j=0; j<size_y; j++){
			sum_y = 0;
			for(int k=0; k<size_z; k++){
				curData = *cur_data_pos;
				sum_y += curData;
				fz += curData * k;
				cur_data_pos ++;
			}
			fy += sum_y * j;
			sum_x += sum_y;
			cur_data_pos += (dim1_offset - size_z);
		}
		fx += sum_x * i;
		f += sum_x;
		cur_data_pos += (dim0_offset - size_y * dim1_offset);
	}
	float coeff = 1.0 / (size_x * size_y * size_z);
	reg_params_pos[0] = (2 * fx / (size_x - 1) - f) * 6 * coeff / (size_x + 1);
	reg_params_pos[1] = (2 * fy / (size_y - 1) - f) * 6 * coeff / (size_y + 1);
	reg_params_pos[2] = (2 * fz / (size_z - 1) - f) * 6 * coeff / (size_z + 1);
	reg_params_pos[3] = f * coeff - ((size_x - 1) * reg_params_pos[0] / 2 + (size_y - 1) * reg_params_pos[1] / 2 + (size_z - 1) * reg_params_pos[2] / 2);
}

// template <typename T>
// inline void 
// compute_regression_coeffcients_3d_knl(const T * data_pos, int size_x, int size_y, int size_z, size_t dim0_offset, size_t dim1_offset, float * reg_params_pos){
// 	/*Calculate regression coefficients*/
// 	float fx = 0;
// 	float fy = 0;
// 	float fz = 0;
// 	float f = 0;
// 	float sum_x, sum_y; 
// 	for(int i=0; i<size_x; i++){
// 		sum_x = 0;
// 		const T * cur_data_pos = data_pos + i*dim0_offset;
// 		for(int j=0; j<size_y; j++){
// 			sum_y = 0;
// 			for(int k=0; k<size_z; k++){
// 				float curData = (float)cur_data_pos[j*dim1_offset + k];
// 				sum_y += curData;
// 				fz += curData * k;
// 				cur_data_pos ++;
// 			}
// 			fy += sum_y * j;
// 			sum_x += sum_y;
// 			cur_data_pos += (dim1_offset - size_z);
// 		}
// 		fx += sum_x * i;
// 		f += sum_x;
// 	}
// 	float coeff = ((float)1) / (size_x * size_y * size_z);
// 	reg_params_pos[0] = (2 * fx / (size_x - 1) - f) * 6 * coeff / (size_x + 1);
// 	reg_params_pos[1] = (2 * fy / (size_y - 1) - f) * 6 * coeff / (size_y + 1);
// 	reg_params_pos[2] = (2 * fz / (size_z - 1) - f) * 6 * coeff / (size_z + 1);
// 	reg_params_pos[3] = f * coeff - ((size_x - 1) * reg_params_pos[0] / 2 + (size_y - 1) * reg_params_pos[1] / 2 + (size_z - 1) * reg_params_pos[2] / 2);
// }

#endif
#ifndef _sz_regression_utils_hpp
#define _sz_regression_utils_hpp
#include "sz_utils.hpp"
#include "sz_def.hpp"
#include <array>

#define COEF_AUX_MAX_BLOCK 17

inline size_t get_coef_aux_list_idx(const std::array<size_t, 3> &dims) {
    auto coef_aux_index = 0;
    for (auto &dim:dims) {
        if (dim < 3) {
            printf("ERROR, cannot use poly regression when block dim is less than 3");
            exit(0);
        }
        coef_aux_index = coef_aux_index * COEF_AUX_MAX_BLOCK + dim;
    }
    return coef_aux_index;
}
template<typename T>
void display_coef_aux(std::array<T, RegPolyCoeffNum3d * RegPolyCoeffNum3d> aux) {
    for (int i = 0; i < RegPolyCoeffNum3d; i++) {
        for (int j = 0; j < RegPolyCoeffNum3d; j++)
            std::cout << std::setw(10) << std::setprecision(6) << aux[i * RegPolyCoeffNum3d + j] << " ";
        std::cout << std::endl;
    }
}

template<typename T>
std::vector<std::array<T, RegPolyCoeffNum3d * RegPolyCoeffNum3d>> init_poly() {

    size_t num = 0;
    std::string bin_file_path = "/Users/kzhao/Documents/Workspace/sz_refactory/data/PolyRegressionCoefAux3D.f32";
    auto data = readfile<float>(bin_file_path.data(), num);

    auto coef_aux_p = &data[0];
    std::vector<std::array<T, RegPolyCoeffNum3d * RegPolyCoeffNum3d>> coef_aux_list = std::vector<std::array<T,
            RegPolyCoeffNum3d * RegPolyCoeffNum3d>>(
            (COEF_AUX_MAX_BLOCK + 1) * (COEF_AUX_MAX_BLOCK + 1) * (COEF_AUX_MAX_BLOCK + 1), {0});
    while (coef_aux_p < &data[0] + num) {
        std::array<size_t, 3> dims = {0};
        for (auto &idx:dims) {
            idx = *coef_aux_p++;
        }
        std::copy_n(coef_aux_p, RegPolyCoeffNum3d * RegPolyCoeffNum3d, coef_aux_list[get_coef_aux_list_idx(dims)].begin());
        coef_aux_p += RegPolyCoeffNum3d * RegPolyCoeffNum3d;
    }
//    display_coef_aux(coef_aux_list[get_coef_aux_list_idx( std::array<size_t, 3>{6, 6, 6})]);
    return coef_aux_list;
}

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

template <typename T>
inline void
compute_regression_coeffcients_3d_poly(const T * data_pos, int size_x, int size_y, int size_z, size_t dim0_offset, size_t dim1_offset,
        float * reg_params_pos, std::vector<std::array<T, 100>> &coef_aux_list){

    std::array<size_t, 3> dims = {(size_t) size_x, (size_t) size_y, (size_t) size_z};
    std::array<double, RegPolyCoeffNum3d> sum{0};
    const T * cur_data_pos = data_pos;
    T curData;
    for (int i = 0; i < size_x; i++) {
        for (int j = 0; j < size_y; j++) {
            for (int k = 0; k < size_z; k++) {
                curData = *cur_data_pos;
                auto poly_index = std::array<T, RegPolyCoeffNum3d>{1.0, (float) i, (float) j, (float) k, (float) i * i,
                                                                   (float) i * j, (float) i * k, (float) j * j,
                                                                   (float) j * k, (float) k * k};
                for (int m = 0; m < RegPolyCoeffNum3d; m++) {
                    sum[m] += poly_index[m] * curData;
                }
                cur_data_pos++;
            }
            cur_data_pos += (dim1_offset - size_z);
        }
        cur_data_pos += (dim0_offset - size_y * dim1_offset);
    }

    std::array<double, RegPolyCoeffNum3d> coeffs{0};
    auto coef_aux_idx = coef_aux_list[get_coef_aux_list_idx(dims)];

    for (int i = 0; i < RegPolyCoeffNum3d; i++) {
        for (int j = 0; j < RegPolyCoeffNum3d; j++) {
            coeffs[i] += coef_aux_idx[i * RegPolyCoeffNum3d + j] * sum[j];
        }
    }

    for (int i = 0; i < RegPolyCoeffNum3d; i++) {
        reg_params_pos[i] = coeffs[i];
    }
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
#include "sz_def.hpp"
#include "sz_regression_utils.hpp"
#include <vector>
#include <array>
#include <algorithm>
#include "sz_poly_regression_coeff_aux.hpp"

std::vector<std::array<float, RegPolyCoeffNum3d * RegPolyCoeffNum3d>> init_poly() 
{

    const float *data = COEFF_3D;
    size_t num = sizeof(COEFF_3D) / sizeof(float);;

    auto coef_aux_p = &data[0];
    std::vector<std::array<float, RegPolyCoeffNum3d * RegPolyCoeffNum3d>> coef_aux_list = std::vector<std::array<float,
            RegPolyCoeffNum3d * RegPolyCoeffNum3d>>(
            (COEF_AUX_MAX_BLOCK + 1) * (COEF_AUX_MAX_BLOCK + 1) * (COEF_AUX_MAX_BLOCK + 1), {0});
    while (coef_aux_p < data + num) {
        std::array<size_t, 3> dims = {0};
        for (auto &idx:dims) {
            idx = *coef_aux_p++;
        }
        std::copy_n(coef_aux_p, RegPolyCoeffNum3d * RegPolyCoeffNum3d,
                    coef_aux_list[get_coef_aux_list_idx(dims)].begin());
        coef_aux_p += RegPolyCoeffNum3d * RegPolyCoeffNum3d;
    }
//    display_coef_aux(coef_aux_list[get_coef_aux_list_idx( std::array<size_t, 3>{6, 6, 6})]);
    return coef_aux_list;
}

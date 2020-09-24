#ifndef _sz_compress_block_processing_hpp
#define _sz_compress_block_processing_hpp

template<typename T>
inline void
sz_block_error_estimation_3d(const T *data_pos, const float *reg_params_pos, const meanInfo<T> &mean_info, int x, int y, int z,
                             size_t dim0_offset, size_t dim1_offset, float noise, double &err_reg, double &err_lorenzo) {
    const T *cur_data_pos = data_pos + x * dim0_offset + y * dim1_offset + z;
    T cur_data = *cur_data_pos;
    err_reg += fabs(cur_data - regression_predict_3d<T>(reg_params_pos, x, y, z));
    err_lorenzo += mean_info.use_mean ? MIN(fabs(cur_data - mean_info.mean),
                                            fabs(cur_data - lorenzo_predict_3d(cur_data_pos, dim0_offset, dim1_offset)) + noise) :
                   fabs(cur_data - lorenzo_predict_3d(cur_data_pos, dim0_offset, dim1_offset)) + noise;
}

template<typename T>
inline void
sz_block_error_estimation_3d(const T *data_pos, const float *reg_params_pos, const float *reg_poly_params_pos,
                             const meanInfo<T> &mean_info, int x, int y, int z, size_t dim0_offset, size_t dim1_offset,
                             T precision, double &err_lorenzo, double &err_lorenzo_2layer, double &err_reg, double &err_reg_poly,
                             const int pred_dim,
                             const bool use_lorenzo, const bool use_lorenzo_2layer,
                             const bool use_regression, const bool use_regression_poly) {
    T noise = 0;
    T noise_2layer = 0;
    const T *cur_data_pos = data_pos + x * dim0_offset + y * dim1_offset + z;
    T cur_data = *cur_data_pos;
    if (use_regression) {
        err_reg += fabs(cur_data - regression_predict_3d<T>(reg_params_pos, x, y, z));
    }
    if (use_regression_poly) {
        err_reg_poly += fabs(cur_data - regression_predict_3d_poly<T>(reg_poly_params_pos, x, y, z));
    }
    double lorenzo_predict = 0;
    double lorenzo_2layer_predict = 0;
    if (pred_dim == 3) {
        if (use_lorenzo_2layer) {
            lorenzo_2layer_predict = lorenzo_predict_3d_2layer(cur_data_pos, dim0_offset, dim1_offset);
            noise_2layer = Lorenze2LayerNoise3d * precision;
        }
        if (use_lorenzo) {
            lorenzo_predict = lorenzo_predict_3d(cur_data_pos, dim0_offset, dim1_offset);
            noise = LorenzeNoise3d * precision;
        }
    } else if (pred_dim == 2) {
        if (use_lorenzo_2layer) {
            lorenzo_2layer_predict = lorenzo_predict_2d_2layer(cur_data_pos, dim0_offset, dim1_offset);
            noise_2layer = Lorenze2LayerNoise2d * precision;

        }
        if (use_lorenzo) {
            lorenzo_predict = lorenzo_predict_2d(cur_data_pos, dim0_offset, dim1_offset);
            noise = LorenzeNoise2d * precision;
        }
    } else {
        if (use_lorenzo_2layer) {
            lorenzo_2layer_predict = lorenzo_predict_1d_2layer(cur_data_pos, dim0_offset);
            noise_2layer = Lorenze2LayerNoise1d * precision;

        }
        if (use_lorenzo) {
            lorenzo_predict = lorenzo_predict_1d(cur_data_pos, dim0_offset);
            noise = LorenzeNoise1d * precision;
        }
    }
    err_lorenzo += mean_info.use_mean ? MIN(fabs(cur_data - mean_info.mean), fabs(cur_data - lorenzo_predict) + noise) :
                   fabs(cur_data - lorenzo_predict) + noise;
    err_lorenzo_2layer += mean_info.use_mean ? MIN(fabs(cur_data - mean_info.mean),
                                                   fabs(cur_data - lorenzo_2layer_predict) + noise_2layer) :
                          fabs(cur_data - lorenzo_2layer_predict) + noise_2layer;
}

template<typename T>
inline void
sz_block_error_estimation_3d_v2(const T *data_pos, const float *reg_params_pos, const float *reg_poly_params_pos,
                                const meanInfo<T> &mean_info, int x, int y, int z, size_t dim0_offset, size_t dim1_offset,
                                T precision, double &err_lorenzo, double &err_lorenzo_2layer, double &err_reg,
                                double &err_reg_poly, const int pred_dim,
                                const bool use_lorenzo, const bool use_lorenzo_2layer,
                                const bool use_regression, const bool use_regression_poly) {
    T noise = 0;
    T noise_2layer = 0;
    const T *cur_data_pos = data_pos + x * dim0_offset + y * dim1_offset + z;
    T cur_data = *cur_data_pos;
    if (use_regression) {
        err_reg += std::pow(fabs(cur_data - regression_predict_3d<T>(reg_params_pos, x, y, z)), 2);
    }
    if (use_regression_poly) {
        err_reg_poly += std::pow(fabs(cur_data - regression_predict_3d_poly<T>(reg_poly_params_pos, x, y, z)), 2);
    }
    double lorenzo_predict = 0;
    double lorenzo_2layer_predict = 0;
    if (pred_dim == 3) {
        if (use_lorenzo_2layer) {
            lorenzo_2layer_predict = lorenzo_predict_3d_2layer(cur_data_pos, dim0_offset, dim1_offset);
            noise_2layer = Lorenze2LayerNoise3d * precision;
        }
        if (use_lorenzo) {
            lorenzo_predict = lorenzo_predict_3d(cur_data_pos, dim0_offset, dim1_offset);
            noise = LorenzeNoise3d * precision;
        }
    } else if (pred_dim == 2) {
        if (use_lorenzo_2layer) {
            lorenzo_2layer_predict = lorenzo_predict_2d_2layer(cur_data_pos, dim0_offset, dim1_offset);
            noise_2layer = Lorenze2LayerNoise2d * precision;

        }
        if (use_lorenzo) {
            lorenzo_predict = lorenzo_predict_2d(cur_data_pos, dim0_offset, dim1_offset);
            noise = LorenzeNoise2d * precision;
        }
    } else {
        if (use_lorenzo_2layer) {
            lorenzo_2layer_predict = lorenzo_predict_1d_2layer(cur_data_pos, dim0_offset);
            noise_2layer = Lorenze2LayerNoise1d * precision;

        }
        if (use_lorenzo) {
            lorenzo_predict = lorenzo_predict_1d(cur_data_pos, dim0_offset);
            noise = LorenzeNoise1d * precision;
        }
    }
    err_lorenzo += std::pow(mean_info.use_mean ? MIN(fabs(cur_data - mean_info.mean), fabs(cur_data - lorenzo_predict) + noise) :
                            fabs(cur_data - lorenzo_predict) + noise, 2);
    err_lorenzo_2layer += std::pow(
            mean_info.use_mean ? MIN(fabs(cur_data - mean_info.mean), fabs(cur_data - lorenzo_2layer_predict) + noise_2layer) :
            fabs(cur_data - lorenzo_2layer_predict) + noise_2layer, 2);
}


template<typename T>
inline int
sz_blockwise_selection_3d(const T *data_pos, const meanInfo<T> &mean_info, size_t dim0_offset, size_t dim1_offset, int min_size,
                          float noise, const float *reg_params_pos) {
    double err_reg = 0;
    double err_lorenzo = 0;
    for (int i = 1; i < min_size; i++) {
        int bmi = min_size - i;
        sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, i, i, dim0_offset, dim1_offset, noise, err_reg,
                                     err_lorenzo);
        sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, i, bmi, dim0_offset, dim1_offset, noise, err_reg,
                                     err_lorenzo);
        sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, bmi, i, dim0_offset, dim1_offset, noise, err_reg,
                                     err_lorenzo);
        sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, bmi, bmi, dim0_offset, dim1_offset, noise, err_reg,
                                     err_lorenzo);
    }
    return err_reg < err_lorenzo;
}

template<typename T>
inline int
sz_blockwise_selection_3d(const T *data_pos, const meanInfo<T> &mean_info, size_t dim0_offset, size_t dim1_offset, int min_size,
                          T precision, const float *reg_params_pos, const float *reg_poly_params_pos, const int pred_dim,
                          const bool use_lorenzo, const bool use_lorenzo_2layer, const bool use_regression,
                          const bool use_poly_regression, float poly_regression_noise) {
    double err_lorenzo = 0;
    double err_lorenzo_2layer = 0;
    double err_reg = 0;
    double err_reg_poly = poly_regression_noise;
    for (int i = 2; i < min_size - 1; i++) {
        int bmi = min_size - i;
        sz_block_error_estimation_3d(data_pos, reg_params_pos, reg_poly_params_pos, mean_info, i, i, i, dim0_offset, dim1_offset,
                                     precision, err_lorenzo, err_lorenzo_2layer, err_reg, err_reg_poly, pred_dim, use_lorenzo,
                                     use_lorenzo_2layer, use_regression, use_poly_regression);
        sz_block_error_estimation_3d(data_pos, reg_params_pos, reg_poly_params_pos, mean_info, i, i, bmi, dim0_offset,
                                     dim1_offset, precision, err_lorenzo, err_lorenzo_2layer, err_reg, err_reg_poly, pred_dim,
                                     use_lorenzo, use_lorenzo_2layer, use_regression, use_poly_regression);
        sz_block_error_estimation_3d(data_pos, reg_params_pos, reg_poly_params_pos, mean_info, i, bmi, i, dim0_offset,
                                     dim1_offset, precision, err_lorenzo, err_lorenzo_2layer, err_reg, err_reg_poly, pred_dim,
                                     use_lorenzo, use_lorenzo_2layer, use_regression, use_poly_regression);
        sz_block_error_estimation_3d(data_pos, reg_params_pos, reg_poly_params_pos, mean_info, i, bmi, bmi, dim0_offset,
                                     dim1_offset, precision, err_lorenzo, err_lorenzo_2layer, err_reg, err_reg_poly, pred_dim,
                                     use_lorenzo, use_lorenzo_2layer, use_regression, use_poly_regression);
    }
    if (min_size > 3) {
        sz_block_error_estimation_3d(data_pos, reg_params_pos, reg_poly_params_pos, mean_info, min_size - 1, min_size - 1,
                                     min_size - 1, dim0_offset, dim1_offset,
                                     precision, err_lorenzo, err_lorenzo_2layer, err_reg, err_reg_poly, pred_dim, use_lorenzo,
                                     use_lorenzo_2layer, use_regression, use_poly_regression);
    }

    if (use_regression && (!use_lorenzo || err_reg <= err_lorenzo)
        && (!use_poly_regression || err_reg <= err_reg_poly)
        && (!use_lorenzo_2layer || err_reg < err_lorenzo_2layer)) {
        return SELECTOR_REGRESSION;
    } else if (use_poly_regression && (!use_lorenzo || err_reg_poly <= err_lorenzo)
               && (!use_regression || err_reg_poly <= err_reg)
               && (!use_lorenzo_2layer || err_reg_poly < err_lorenzo_2layer)) {
        return SELECTOR_REGRESSION_POLY;
    } else if (use_lorenzo_2layer && (!use_lorenzo || err_lorenzo_2layer <= err_lorenzo)
               && (!use_regression || err_lorenzo_2layer <= err_reg)
               && (!use_poly_regression || err_lorenzo_2layer < err_reg_poly)) {
        return SELECTOR_LORENZO_2LAYER;
    } else {
        return SELECTOR_LORENZO;
    }
}

template<typename T>
inline int
sz_blockwise_selection_3d_v2(const T *data_pos, const meanInfo<T> &mean_info, size_t dim0_offset, size_t dim1_offset,
                             int min_size,
                             T precision, const float *reg_params_pos, const float *reg_poly_params_pos, const int pred_dim,
                             const bool use_lorenzo, const bool use_lorenzo_2layer, const bool use_regression,
                             const bool use_poly_regression, float poly_regression_noise) {
    double err_lorenzo = 0;
    double err_lorenzo_2layer = 0;
    double err_reg = 0;
    double err_reg_poly = poly_regression_noise;
    for (int i = 1; i < min_size; i++) {
        for (int j = 1; j < min_size; j++) {
            for (int k = 1; k < min_size; k++) {
                sz_block_error_estimation_3d_v2(data_pos, reg_params_pos, reg_poly_params_pos, mean_info, i, j, k, dim0_offset,
                                                dim1_offset,
                                                precision, err_lorenzo, err_lorenzo_2layer, err_reg, err_reg_poly, pred_dim,
                                                use_lorenzo,
                                                use_lorenzo_2layer, use_regression, use_poly_regression);
            }
        }
    }
    if (use_regression && (!use_lorenzo || err_reg <= err_lorenzo)
        && (!use_poly_regression || err_reg <= err_reg_poly)
        && (!use_lorenzo_2layer || err_reg < err_lorenzo_2layer)) {
        return SELECTOR_REGRESSION;
    } else if (use_poly_regression && (!use_lorenzo || err_reg_poly <= err_lorenzo)
               && (!use_regression || err_reg_poly <= err_reg)
               && (!use_lorenzo_2layer || err_reg_poly < err_lorenzo_2layer)) {
        return SELECTOR_REGRESSION_POLY;
    } else if (use_lorenzo_2layer && (!use_lorenzo || err_lorenzo_2layer <= err_lorenzo)
               && (!use_regression || err_lorenzo_2layer <= err_reg)
               && (!use_poly_regression || err_lorenzo_2layer < err_reg_poly)) {
        return SELECTOR_LORENZO_2LAYER;
    } else {
        return SELECTOR_LORENZO;
    }
}

template<typename T>
inline void
block_pred_and_quant_regression_3d(const T *data_pos, const float *reg_params_pos, double precision, double recip_precision,
                                   int capacity,
                                   int intv_radius, int size_x, int size_y, int size_z, size_t dim0_offset, size_t dim1_offset,
                                   int *&type_pos, T *&unpredictable_data_pos) {
    const T *cur_data_pos = data_pos;
    for (int i = 0; i < size_x; i++) {
        for (int j = 0; j < size_y; j++) {
            for (int k = 0; k < size_z; k++) {
                float pred = regression_predict_3d<T>(reg_params_pos, i, j, k);
                *(type_pos++) = quantize(pred, *cur_data_pos, precision, recip_precision, capacity, intv_radius,
                                         unpredictable_data_pos);
                cur_data_pos++;
            }
            cur_data_pos += dim1_offset - size_z;
        }
        cur_data_pos += dim0_offset - size_y * dim1_offset;
    }
}

// block-dependant use_lorenzo pred & quant
template<typename T>
inline void
block_pred_and_quant_regression_3d_with_buffer(const T *data_pos, const float *reg_params_pos, T *buffer, double precision,
                                               double recip_precision, int capacity,
                                               int intv_radius, int size_x, int size_y, int size_z, size_t buffer_dim0_offset,
                                               size_t buffer_dim1_offset,
                                               size_t dim0_offset, size_t dim1_offset, int *&type_pos,
                                               T *&unpredictable_data_pos) {
    const T *cur_data_pos = data_pos;
    T *buffer_pos = buffer + buffer_dim0_offset + buffer_dim1_offset + 1;
    for (int i = 0; i < size_x; i++) {
        for (int j = 0; j < size_y; j++) {
            T *cur_buffer_pos = buffer_pos;
            for (int k = 0; k < size_z; k++) {
                float pred = regression_predict_3d<T>(reg_params_pos, i, j, k);
                *(type_pos++) = quantize(pred, *cur_data_pos, precision, recip_precision, capacity, intv_radius,
                                         unpredictable_data_pos, cur_buffer_pos);
                cur_data_pos++;
                cur_buffer_pos++;
            }
            buffer_pos += buffer_dim1_offset;
            cur_data_pos += dim1_offset - size_z;
        }
        buffer_pos += buffer_dim0_offset - size_y * buffer_dim1_offset;
        cur_data_pos += dim0_offset - size_y * dim1_offset;
    }
}

// block-independant use_lorenzo pred & quant
template<typename T>
inline void
block_pred_and_quant_lorenzo_3d(const meanInfo<T> &mean_info, const T *data_pos, T *buffer, double precision,
                                double recip_precision, int capacity, int intv_radius,
                                int size_x, int size_y, int size_z, size_t buffer_dim0_offset, size_t buffer_dim1_offset,
                                size_t dim0_offset, size_t dim1_offset, int *&type_pos, T *&unpredictable_data_pos) {
    const T *cur_data_pos = data_pos;
    T *buffer_pos = buffer + buffer_dim0_offset + buffer_dim1_offset + 1;
    for (int i = 0; i < size_x; i++) {
        for (int j = 0; j < size_y; j++) {
            T *cur_buffer_pos = buffer_pos;
            for (int k = 0; k < size_z; k++) {
                if (mean_info.use_mean && (fabs(*cur_data_pos - mean_info.mean) < precision)) {
                    *(type_pos++) = 1;
                    *cur_buffer_pos = mean_info.mean;
                } else {
                    float pred = lorenzo_predict_3d(cur_buffer_pos, buffer_dim0_offset, buffer_dim1_offset);
                    *(type_pos++) = quantize(pred, *cur_data_pos, precision, recip_precision, capacity, intv_radius,
                                             unpredictable_data_pos, cur_buffer_pos);
                }
                cur_data_pos++;
                cur_buffer_pos++;
            }
            buffer_pos += buffer_dim1_offset;
            cur_data_pos += dim1_offset - size_z;
        }
        buffer_pos += buffer_dim0_offset - size_y * buffer_dim1_offset;
        cur_data_pos += dim0_offset - size_y * dim1_offset;
    }
}

#endif
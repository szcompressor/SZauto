#include "sz_compression_utils.hpp"
#include "sz_def.hpp"
#include "sz_prediction.hpp"
#include "sz_compress_3d.hpp"
#include "sz_compress_block_processing.hpp"
#include "sz_compress_block_processing_knl.hpp"
#include "sz_optimize_quant_intervals.hpp"
#include "sz_regression_utils.hpp"
// avoid mistake when running on KNL
// need to be checked later;
// also in sz_decompress_block_processing_knl.hpp
float * ori_data_sp_float = NULL;

// return regression count
// use block-independant use_lorenzo pred & quant
template<typename T>
size_t
prediction_and_quantization_3d(const T * data, const DSize_3d& size, const meanInfo<T>& mean_info, double precision,
	int capacity, int intv_radius, unsigned char * indicator, int * type,
	int * reg_params_type, float *& reg_unpredictable_data_pos, T *& unpredictable_data_pos){
	const float noise = precision * LorenzeNoise3d;
	int * type_pos = type;
	unsigned char * indicator_pos = indicator;
	float * reg_params = (float *) malloc(RegCoeffNum3d * (size.num_blocks+1) * sizeof(float));
	for(int i=0; i<RegCoeffNum3d; i++)
		reg_params[i] = 0;
	float * reg_params_pos = reg_params + RegCoeffNum3d;
	int * reg_params_type_pos = reg_params_type;
	double reg_precisions[RegCoeffNum3d];
	double reg_recip_precisions[RegCoeffNum3d];
	double rel_param_err = RegErrThreshold * precision / RegCoeffNum3d;
	for(int i=0; i<RegCoeffNum3d-1; i++){
		reg_precisions[i] = rel_param_err / size.block_size;
		reg_recip_precisions[i] = 1.0 / reg_precisions[i];
	}
	reg_precisions[RegCoeffNum3d - 1] = rel_param_err;
	reg_recip_precisions[RegCoeffNum3d - 1] = 1.0 / reg_precisions[RegCoeffNum3d - 1];
	size_t buffer_dim0_offset = (size.block_size + 1) * (size.block_size + 1);
	size_t buffer_dim1_offset = size.block_size + 1;
	T * pred_buffer = (T *) malloc((size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	memset(pred_buffer, 0, (size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	size_t reg_count = 0;
	int capacity_lorenzo = mean_info.use_mean ? capacity - 2 : capacity;
	double recip_precision = 1.0 / precision;
	const T * x_data_pos = data;
	for(size_t i=0; i<size.num_x; i++){
		const T * y_data_pos = x_data_pos;
		for(size_t j=0; j<size.num_y; j++){
			const T * z_data_pos = y_data_pos;
			for(size_t k=0; k<size.num_z; k++){
				int size_x = ((i+1)*size.block_size < size.d1) ? size.block_size : size.d1 - i*size.block_size;
				int size_y = ((j+1)*size.block_size < size.d2) ? size.block_size : size.d2 - j*size.block_size;
				int size_z = ((k+1)*size.block_size < size.d3) ? size.block_size : size.d3 - k*size.block_size;
				int min_size = MIN(size_x, size_y);
				min_size = MIN(min_size, size_z);
				// size of block is less than some threshold
				if(min_size < RegThresholdSize3d){
					*indicator_pos = 0;
				}
				else{
					compute_regression_coeffcients_3d(z_data_pos, size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, reg_params_pos);
					*indicator_pos = sz_blockwise_selection_3d(z_data_pos, mean_info, size.dim0_offset, size.dim1_offset, min_size, noise, reg_params_pos);
				}
				if(*indicator_pos){
					// regression
					compress_regression_coefficient_3d(reg_precisions, reg_recip_precisions, reg_params_pos, reg_params_type_pos, reg_unpredictable_data_pos);
					block_pred_and_quant_regression_3d(z_data_pos, reg_params_pos, precision, recip_precision, capacity, intv_radius,
						size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos);
					reg_count ++;
					reg_params_pos += RegCoeffNum3d;
					reg_params_type_pos += RegCoeffNum3d;
				}
				else{
					// Lorenzo
					block_pred_and_quant_lorenzo_3d(mean_info, z_data_pos, pred_buffer, precision, recip_precision, capacity_lorenzo, intv_radius,
						size_x, size_y, size_z, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos);
				}
				indicator_pos ++;
				z_data_pos += size_z;
			}
			y_data_pos += size.block_size*size.dim1_offset;
		}
		x_data_pos += size.block_size*size.dim0_offset;
	}
	free(pred_buffer);
	free(reg_params);
	return reg_count;
}

// return regression count
// use block-dependant use_lorenzo pred & quant
template<typename T>
size_t
prediction_and_quantization_3d_with_border_prediction(const T * data, const DSize_3d& size, const meanInfo<T>& mean_info, double precision,
	int capacity, int intv_radius, unsigned char * indicator, int * type,
	int * reg_params_type, float *& reg_unpredictable_data_pos, T *& unpredictable_data_pos){
	const float noise = precision * LorenzeNoise3d;
	int * type_pos = type;
	unsigned char * indicator_pos = indicator;
	float * reg_params = (float *) malloc(RegCoeffNum3d * (size.num_blocks+1) * sizeof(float));
	for(int i=0; i<RegCoeffNum3d; i++)
		reg_params[i] = 0;
	float * reg_params_pos = reg_params + RegCoeffNum3d;
	int * reg_params_type_pos = reg_params_type;
	double reg_precisions[RegCoeffNum3d];
	double reg_recip_precisions[RegCoeffNum3d];
	double rel_param_err = RegErrThreshold * precision / RegCoeffNum3d;
	for(int i=0; i<RegCoeffNum3d-1; i++){
		reg_precisions[i] = rel_param_err / size.block_size;
		reg_recip_precisions[i] = 1.0 / reg_precisions[i];
	}
	reg_precisions[RegCoeffNum3d - 1] = rel_param_err;
	reg_recip_precisions[RegCoeffNum3d - 1] = 1.0 / reg_precisions[RegCoeffNum3d - 1];
	// maintain a buffer of (block_size+1)*(r2+1)*(r3+1)
	size_t buffer_dim0_offset = (size.d2+1)*(size.d3+1);
	size_t buffer_dim1_offset = size.d3+1;
	T * pred_buffer = (T *) malloc((size.block_size+1)*(size.d2+1)*(size.d3+1)*sizeof(T));
	memset(pred_buffer, 0, (size.block_size+1)*(size.d2+1)*(size.d3+1)*sizeof(T));
	size_t reg_count = 0;
	int capacity_lorenzo = mean_info.use_mean ? capacity - 2 : capacity;
	double recip_precision = 1.0 / precision;
	const T * x_data_pos = data;
	for(size_t i=0; i<size.num_x; i++){
		const T * y_data_pos = x_data_pos;
		T * pred_buffer_pos = pred_buffer;
		for(size_t j=0; j<size.num_y; j++){
			const T * z_data_pos = y_data_pos;
			for(size_t k=0; k<size.num_z; k++){
				int size_x = ((i+1)*size.block_size < size.d1) ? size.block_size : size.d1 - i*size.block_size;
				int size_y = ((j+1)*size.block_size < size.d2) ? size.block_size : size.d2 - j*size.block_size;
				int size_z = ((k+1)*size.block_size < size.d3) ? size.block_size : size.d3 - k*size.block_size;
				int min_size = MIN(size_x, size_y);
				min_size = MIN(min_size, size_z);
				// size of block is less than some threshold
				if(min_size < RegThresholdSize3d){
					*indicator_pos = 0;
				}
				else{
					compute_regression_coeffcients_3d(z_data_pos, size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, reg_params_pos);
					*indicator_pos = sz_blockwise_selection_3d(z_data_pos, mean_info, size.dim0_offset, size.dim1_offset, min_size, noise, reg_params_pos);
				}
				if(*indicator_pos){
					// regression
					compress_regression_coefficient_3d(reg_precisions, reg_recip_precisions, reg_params_pos, reg_params_type_pos, reg_unpredictable_data_pos);
					block_pred_and_quant_regression_3d_with_buffer(z_data_pos, reg_params_pos, pred_buffer_pos, precision, recip_precision, capacity, intv_radius,
						size_x, size_y, size_z, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos);
					reg_count ++;
					reg_params_pos += RegCoeffNum3d;
					reg_params_type_pos += RegCoeffNum3d;
				}
				else{
					// Lorenzo
					block_pred_and_quant_lorenzo_3d(mean_info, z_data_pos, pred_buffer_pos, precision, recip_precision, capacity_lorenzo, intv_radius,
						size_x, size_y, size_z, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos);
				}
				pred_buffer_pos += size.block_size;
				indicator_pos ++;
				z_data_pos += size_z;
			}
			y_data_pos += size.block_size*size.dim1_offset;
			pred_buffer_pos += size.block_size*buffer_dim1_offset - size.block_size*size.num_z;
		}
		// copy bottom of buffer to top of buffer
		memcpy(pred_buffer, pred_buffer + size.block_size*buffer_dim0_offset, buffer_dim0_offset*sizeof(T));
		x_data_pos += size.block_size*size.dim0_offset;
	}
	free(pred_buffer);
	free(reg_params);
	return reg_count;
}


// return regression count
// use block-independant use_lorenzo pred & quant
template<typename T>
size_t
prediction_and_quantization_3d_with_knl_optimization(const T * data, const DSize_3d& size, const meanInfo<T>& mean_info, T precision,
	int capacity, int intv_radius, unsigned char * indicator, int * type,
	int * reg_params_type, float *& reg_unpredictable_data_pos, int * unpred_count_buffer, T * unpred_data_buffer, size_t offset, const sz_params& params){
	const float noise = precision * LorenzeNoise3d;
	int * type_pos = type;
	unsigned char * indicator_pos = indicator;
	float * reg_params = (float *) malloc(RegCoeffNum3d * (size.num_blocks+1) * sizeof(float));
	for(int i=0; i<RegCoeffNum3d; i++)
		reg_params[i] = 0;
	float * reg_params_pos = reg_params + RegCoeffNum3d;
	int * reg_params_type_pos = reg_params_type;
	T reg_precisions[RegCoeffNum3d];
	T reg_recip_precisions[RegCoeffNum3d];
	T rel_param_err = RegErrThreshold * precision / RegCoeffNum3d;
	for(int i=0; i<RegCoeffNum3d-1; i++){
		reg_precisions[i] = rel_param_err / size.block_size;
		reg_recip_precisions[i] = 1.0 / reg_precisions[i];
	}
	reg_precisions[RegCoeffNum3d - 1] = rel_param_err;
	reg_recip_precisions[RegCoeffNum3d - 1] = 1.0 / reg_precisions[RegCoeffNum3d - 1];
	size_t buffer_dim0_offset = (size.block_size + 1) * (size.block_size + 1);
	size_t buffer_dim1_offset = size.block_size + 1;
	T * pred_buffer = (T *) malloc((size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	memset(pred_buffer, 0, (size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	size_t reg_count = 0;
	int capacity_lorenzo = mean_info.use_mean ? capacity - 2 : capacity;
	T recip_precision = (T)1.0 / precision;
	const T * x_data_pos = data;
	for(size_t i=0; i<size.num_x; i++){
		const T * y_data_pos = x_data_pos;
		for(size_t j=0; j<size.num_y; j++){
			const T * z_data_pos = y_data_pos;
			for(size_t k=0; k<size.num_z; k++){
				int size_x = ((i+1)*size.block_size < size.d1) ? size.block_size : size.d1 - i*size.block_size;
				int size_y = ((j+1)*size.block_size < size.d2) ? size.block_size : size.d2 - j*size.block_size;
				int size_z = ((k+1)*size.block_size < size.d3) ? size.block_size : size.d3 - k*size.block_size;
				int min_size = MIN(size_x, size_y);
				min_size = MIN(min_size, size_z);
				// size of block is less than some threshold
				if(min_size < RegThresholdSize3d){
					*indicator_pos = 0;
				}
				else{
					// opt if necessary
					compute_regression_coeffcients_3d(z_data_pos, size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, reg_params_pos);
					*indicator_pos = sz_blockwise_selection_3d(z_data_pos, mean_info, size.dim0_offset, size.dim1_offset, min_size, noise, reg_params_pos);
				}
				if(*indicator_pos){
					// regression
					compress_regression_coefficient_3d(reg_precisions, reg_recip_precisions, reg_params_pos, reg_params_type_pos, reg_unpredictable_data_pos);
					block_pred_and_quant_regression_3d_knl(z_data_pos, reg_params_pos, precision, recip_precision, capacity, intv_radius,
						size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, type_pos, unpred_count_buffer, unpred_data_buffer, offset);
					reg_count ++;
					reg_params_pos += RegCoeffNum3d;
					reg_params_type_pos += RegCoeffNum3d;
				}
				else{
					// Lorenzo
					block_pred_and_quant_lorenzo_3d_knl_2d_pred(mean_info, z_data_pos, pred_buffer, precision, recip_precision, capacity_lorenzo, intv_radius,
						size_x, size_y, size_z, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpred_count_buffer, unpred_data_buffer, offset, 1);
				}
				indicator_pos ++;
				z_data_pos += size_z;
			}
			y_data_pos += size.block_size*size.dim1_offset;
		}
		x_data_pos += size.block_size*size.dim0_offset;
	}
	free(pred_buffer);
	free(reg_params);
	return reg_count;
}

// modify size for sampling
template<typename T>
size_t
prediction_and_quantization_3d_with_border_predicition_and_knl_optimization_sampling(const T * data, DSize_3d& size, const meanInfo<T>& mean_info, T precision,
                                                                            int capacity, int intv_radius, unsigned char * indicator, int * type, int * reg_params_type, float *& reg_unpredictable_data_pos,
                                                                            int * unpred_count_buffer, T * unpred_data_buffer, size_t offset, const sz_params& params){
    const float noise = precision * LorenzeNoise3d;
    int * type_pos = type;
    unsigned char * indicator_pos = indicator;
    float * reg_params = (float *) malloc(RegCoeffNum3d * (size.num_blocks+1) * sizeof(float));
    for(int i=0; i<RegCoeffNum3d; i++)
        reg_params[i] = 0;
    float * reg_params_pos = reg_params + RegCoeffNum3d;
    int * reg_params_type_pos = reg_params_type;
    T reg_precisions[RegCoeffNum3d];
    T reg_recip_precisions[RegCoeffNum3d];
    T rel_param_err = RegErrThreshold * precision / RegCoeffNum3d;
    for(int i=0; i<RegCoeffNum3d-1; i++){
        reg_precisions[i] = rel_param_err / size.block_size;
        reg_recip_precisions[i] = 1.0 / reg_precisions[i];
    }
    reg_precisions[RegCoeffNum3d - 1] = rel_param_err;
    reg_recip_precisions[RegCoeffNum3d - 1] = 1.0 / reg_precisions[RegCoeffNum3d - 1];
    size_t reg_count = 0;
    int capacity_lorenzo = mean_info.use_mean ? capacity - 2 : capacity;
    auto *lorenzo_pred_and_quant = block_pred_and_quant_lorenzo_3d_knl_3d_pred<T>;
    if(params.prediction_dim == 2) lorenzo_pred_and_quant = block_pred_and_quant_lorenzo_3d_knl_2d_pred<T>;
    else if(params.prediction_dim == 1) lorenzo_pred_and_quant = block_pred_and_quant_lorenzo_3d_knl_1d_pred<T>;
    T recip_precision = (T)1.0 / precision;
    printf("START pred & quantize\n");
    fflush(stdout);

    {
        const int layer = 1;
        size_t r1 = size.d1 - layer;
        size_t r2 = size.d2 - layer;
        size_t r3 = size.d3 - layer;
        auto block_size = size.block_size;
        size_t num_x = r1 / block_size;
        size_t num_y = r2 / block_size;
        size_t num_z = r3 / block_size;
        int sample_distance = 1;
        double sample_ratio = 1;
        int tmp_nx = num_x, tmp_ny = num_y, tmp_nz = num_z;
        while(sample_ratio > 0.02){
            sample_distance ++;
            tmp_nx = (num_x - 1) / sample_distance + 1;
            tmp_ny = (num_y - 1) / sample_distance + 1;
            tmp_nz = (num_z - 1) / sample_distance + 1;
            sample_ratio = tmp_nx*tmp_ny*tmp_nz*1.0 / (num_x*num_y*num_z);
            if((tmp_nx == 1)&&(tmp_ny == 1)&&(tmp_nz == 1)) break;
        }
        size_t num_sample_blocks = tmp_nx * tmp_ny * tmp_nz;
        sample_ratio = num_sample_blocks * 1.0 / (num_x * num_y * num_z);
        // modify size
        size.num_elements = num_sample_blocks * block_size * block_size * block_size;
        size.num_blocks = num_sample_blocks;
        printf("sample_ratio = %.4f, num_blocks = %d %d %d\n", sample_ratio, tmp_nx, tmp_ny, tmp_nz);
        printf("num_elements = %ld, num_blocks = %ld\n", size.num_elements, size.num_blocks);
        fflush(stdout);

        // maintain a buffer of (block_size+1)*(r2+1)*(r3+1)
        // 2-layer lorenzo
        size_t buffer_dim0_offset = (block_size+layer)*(block_size+layer);
        size_t buffer_dim1_offset = block_size+layer;
        T * pred_buffer = (T *) malloc((block_size+layer)*(block_size+layer)*(block_size+layer)*sizeof(T));
        memset(pred_buffer, 0, (block_size+layer)*(block_size+layer)*(block_size+layer)*sizeof(T));

        const T * data_pos = data;// + layer * r2*r3 + layer * r3 + layer;
        for(size_t ix=0; ix<tmp_nx; ix++){
            for(size_t iy=0; iy<tmp_ny; iy++){
                for(size_t iz=0; iz<tmp_nz; iz++){
                    // extract data
                    const T * cur_block_data_pos = data_pos + ix*sample_distance*block_size*r2*r3 + iy*sample_distance*block_size*r3 + iz*sample_distance*block_size;
                    for(int ii=0; ii<block_size+layer; ii++){
                        for(int jj=0; jj<block_size+layer; jj++){
                            memcpy(pred_buffer + ii*buffer_dim0_offset + jj*buffer_dim1_offset, cur_block_data_pos + ii*size.dim0_offset + jj*size.dim1_offset, (block_size + layer)*sizeof(T));
                        }
                    }
                    cur_block_data_pos += layer * (size.dim0_offset + size.dim1_offset + 1);
                    if(false){
                        *indicator_pos = 0;
                    }
                    else{
                        compute_regression_coeffcients_3d(cur_block_data_pos, block_size, block_size, block_size, size.dim0_offset, size.dim1_offset, reg_params_pos);
                        *indicator_pos = sz_blockwise_selection_3d(cur_block_data_pos, mean_info, size.dim0_offset, size.dim1_offset, block_size, noise, reg_params_pos);
                    }
                    if(*indicator_pos){
                        // regression
                        compress_regression_coefficient_3d(reg_precisions, reg_recip_precisions, reg_params_pos, reg_params_type_pos, reg_unpredictable_data_pos);
                        block_pred_and_quant_regression_3d_with_buffer_knl(cur_block_data_pos, reg_params_pos, pred_buffer, precision, recip_precision, capacity, intv_radius,
                                                                           block_size, block_size, block_size, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpred_count_buffer, unpred_data_buffer, offset);
                        reg_count ++;
                        reg_params_pos += RegCoeffNum3d;
                        reg_params_type_pos += RegCoeffNum3d;
                    }
                    else{
                        // Lorenzo
                        lorenzo_pred_and_quant(mean_info, cur_block_data_pos, pred_buffer, precision, recip_precision, capacity_lorenzo, intv_radius,
                                               block_size, block_size, block_size, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpred_count_buffer, unpred_data_buffer, offset);
                    }
                    indicator_pos ++;
                }
            }
        }
        free(pred_buffer);
    }
    free(reg_params);
    printf("num_type = %ld\n", type_pos - type);
    printf("END pred & quantize\n");
    fflush(stdout);
    return reg_count;
}

template<typename T>
void
prediction_and_quantization_3d_with_border_predicition_and_knl_optimization(const T *data, const DSize_3d &size,
                                                                            const meanInfo<T> &mean_info, T precision,
                                                                            int capacity, int intv_radius,
                                                                            int *indicator,
                                                                            int *type,
                                                                            int *reg_params_type,
                                                                            float *&reg_unpredictable_data_pos,
                                                                            int *reg_poly_params_type,
                                                                            float *&reg_poly_unpredictable_data_pos,
                                                                            int *unpred_count_buffer, T *unpred_data_buffer,
                                                                            size_t offset, const sz_params &params,
                                                                            size_t & reg_count,
                                                                            size_t & reg_poly_count,
                                                                            sz_compress_info &compress_info) {
    reg_count = 0;
    reg_poly_count = 0;
    size_t lorenzo_count=0;
    size_t lorenzo_2layer_count=0;

	int * type_pos = type;
	int * indicator_pos = indicator;

	float * reg_params = (float *) malloc(RegCoeffNum3d * (size.num_blocks+1) * sizeof(float));
    for (int i = 0; i < RegCoeffNum3d; i++) {
        reg_params[i] = 0;
    }
	float * reg_params_pos = reg_params + RegCoeffNum3d;
	int * reg_params_type_pos = reg_params_type;

    float * reg_poly_params = (float *) malloc(RegPolyCoeffNum3d * (size.num_blocks + 1) * sizeof(float));
    for (int i = 0; i < RegPolyCoeffNum3d; i++) {
        reg_poly_params[i] = 0;
    }
    float * reg_poly_params_pos = reg_poly_params + RegPolyCoeffNum3d;
    int * reg_poly_params_type_pos = reg_poly_params_type;


	T reg_precisions[RegCoeffNum3d];
	T reg_recip_precisions[RegCoeffNum3d];
	for(int i=0; i<RegCoeffNum3d-1; i++){
		reg_precisions[i] = params.regression_param_eb_linear;
		reg_recip_precisions[i] = 1.0 / reg_precisions[i];
	}
	reg_precisions[RegCoeffNum3d - 1] = params.regression_param_eb_independent;
	reg_recip_precisions[RegCoeffNum3d - 1] = 1.0 / reg_precisions[RegCoeffNum3d - 1];

	T reg_poly_precisions[RegPolyCoeffNum3d];
    T reg_poly_recip_precisions[RegPolyCoeffNum3d];
    reg_poly_precisions[0] = params.poly_regression_param_eb_independent;
    for (int i = 1; i < 4; i++) {
        reg_poly_precisions[i] = params.poly_regression_param_eb_linear;
    }
    for (int i = 4; i < 10; i++) {
        reg_poly_precisions[i] = params.poly_regression_param_eb_poly;
    }
	for (int i = 0; i < 10; i++) {
		reg_poly_recip_precisions[i] = 1.0 / reg_poly_precisions[i];
	}
    std::vector<std::array<T, 100>> coef_aux_list=init_poly<T>();

	// maintain a buffer of (block_size+1)*(r2+1)*(r3+1)
    // 2-layer use_lorenzo
    size_t buffer_dim0_offset = (size.d2 + params.lorenzo_padding_layer) * (size.d3 + params.lorenzo_padding_layer);
    size_t buffer_dim1_offset = size.d3 + params.lorenzo_padding_layer;
    T *pred_buffer = (T *) malloc((size.block_size + params.lorenzo_padding_layer) * (size.d2 + params.lorenzo_padding_layer) * (size.d3 + params.lorenzo_padding_layer) * sizeof(T));
    memset(pred_buffer, 0, (size.block_size + params.lorenzo_padding_layer) * (size.d2 + params.lorenzo_padding_layer) * (size.d3 + params.lorenzo_padding_layer) * sizeof(T));
	int capacity_lorenzo = mean_info.use_mean ? capacity - 2 : capacity;
	auto *lorenzo_pred_and_quant = block_pred_and_quant_lorenzo_3d_knl_3d_pred<T>;
	if(params.prediction_dim == 2) lorenzo_pred_and_quant = block_pred_and_quant_lorenzo_3d_knl_2d_pred<T>;
	else if(params.prediction_dim == 1) lorenzo_pred_and_quant = block_pred_and_quant_lorenzo_3d_knl_1d_pred<T>;
	T recip_precision = (T)1.0 / precision;
	const T * x_data_pos = data;
	for(size_t i=0; i<size.num_x; i++){
		const T * y_data_pos = x_data_pos;
		T * pred_buffer_pos = pred_buffer;
		for(size_t j=0; j<size.num_y; j++){
			const T * z_data_pos = y_data_pos;
			for(size_t k=0; k<size.num_z; k++){
				int size_x = ((i+1)*size.block_size < size.d1) ? size.block_size : size.d1 - i*size.block_size;
				int size_y = ((j+1)*size.block_size < size.d2) ? size.block_size : size.d2 - j*size.block_size;
				int size_z = ((k+1)*size.block_size < size.d3) ? size.block_size : size.d3 - k*size.block_size;
				int min_size = MIN(size_x, size_y);
				min_size = MIN(min_size, size_z);

                bool enable_poly = params.use_poly_regression && min_size >= 3;
                bool enable_regression = params.use_regression_linear && min_size >= 2;
//                bool enable_regression = params.use_regression_linear && min_size >= 1;

				if (enable_regression){
                    compute_regression_coeffcients_3d(z_data_pos, size_x, size_y, size_z, size.dim0_offset, size.dim1_offset,
                                                      reg_params_pos);
                }
                if (enable_poly){
                    compute_regression_coeffcients_3d_poly(z_data_pos, size_x, size_y, size_z, size.dim0_offset, size.dim1_offset,
                                                      reg_poly_params_pos, coef_aux_list);
                }
                int selection_result = sz_blockwise_selection_3d(z_data_pos, mean_info, size.dim0_offset, size.dim1_offset,
                                                                 min_size, precision, reg_params_pos, reg_poly_params_pos,
                                                                 params.prediction_dim,
                                                                 params.use_lorenzo, params.use_lorenzo_2layer,
                                                                 enable_regression, enable_poly, params.poly_regression_noise);
                *indicator_pos = selection_result;
                if (selection_result == SELECTOR_REGRESSION_POLY) {

                    // poly regression
                    compress_regression_coefficient_3d_v2(RegPolyCoeffNum3d, reg_poly_precisions, reg_poly_recip_precisions, reg_poly_params_pos, reg_poly_params_type_pos,
														  reg_poly_unpredictable_data_pos);
                    block_pred_and_quant_regression_3d_with_buffer_knl(z_data_pos, reg_poly_params_pos, pred_buffer_pos, precision,
                                                                       recip_precision, capacity, intv_radius,
                                                                       size_x, size_y, size_z, buffer_dim0_offset,
                                                                       buffer_dim1_offset, size.dim0_offset, size.dim1_offset,
                                                                       type_pos, unpred_count_buffer, unpred_data_buffer, offset, params.lorenzo_padding_layer, true);

                    reg_poly_count++;
                    reg_poly_params_pos += RegPolyCoeffNum3d;
                    reg_poly_params_type_pos += RegPolyCoeffNum3d;
                } else if (selection_result == SELECTOR_REGRESSION) {
                    // regression
                    compress_regression_coefficient_3d_v2(RegCoeffNum3d, reg_precisions, reg_recip_precisions, reg_params_pos, reg_params_type_pos,
                                                       reg_unpredictable_data_pos);
                    block_pred_and_quant_regression_3d_with_buffer_knl(z_data_pos, reg_params_pos, pred_buffer_pos, precision,
                                                                       recip_precision, capacity, intv_radius,
                                                                       size_x, size_y, size_z, buffer_dim0_offset,
                                                                       buffer_dim1_offset, size.dim0_offset, size.dim1_offset,
                                                                       type_pos, unpred_count_buffer, unpred_data_buffer, offset, params.lorenzo_padding_layer, false);
                    reg_count++;
                    reg_params_pos += RegCoeffNum3d;
                    reg_params_type_pos += RegCoeffNum3d;
                } else {
                    // Lorenzo
                    lorenzo_pred_and_quant(mean_info, z_data_pos, pred_buffer_pos, precision, recip_precision, capacity_lorenzo,
                                           intv_radius,
                                           size_x, size_y, size_z, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset,
                                           size.dim1_offset, type_pos, unpred_count_buffer, unpred_data_buffer, offset,
                                           params.lorenzo_padding_layer, (selection_result == SELECTOR_LORENZO_2LAYER));
                    if (selection_result == SELECTOR_LORENZO_2LAYER) {
                        lorenzo_2layer_count++;
                    } else {
                        lorenzo_count++;
                    }
                }
				pred_buffer_pos += size.block_size;
				indicator_pos ++;
				z_data_pos += size_z;
			}
			y_data_pos += size.block_size*size.dim1_offset;
			pred_buffer_pos += size.block_size*buffer_dim1_offset - size.block_size*size.num_z;
		}
		// copy bottom of buffer to top of buffer
		memcpy(pred_buffer, pred_buffer + size.block_size * buffer_dim0_offset, params.lorenzo_padding_layer * buffer_dim0_offset * sizeof(T));
		x_data_pos += size.block_size*size.dim0_offset;
	}
    free(pred_buffer);
	free(reg_params);
	free(reg_poly_params);

	printf("block %ld; lorenzo %ld, lorenzo_2layer %ld, regression %ld, poly regression %ld\n", size.num_blocks,
           lorenzo_count, lorenzo_2layer_count, reg_count, reg_poly_count);
	compress_info.lorenzo_count=lorenzo_count;
	compress_info.lorenzo2_count=lorenzo_2layer_count;
	compress_info.regression_count=reg_count;
	compress_info.regression2_count=reg_poly_count;
    compress_info.block_count = size.num_blocks;

//	printf(" #block: %d  #regression %ld #poly %ld", size.num_blocks, reg_count, reg_poly_count);
}

// perform compression
template<typename T>
unsigned char *
sz_compress_3d(const T * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, const sz_params& params){
	DSize_3d size(r1, r2, r3, params.block_size);
	int capacity = 0; // num of quant intervals
	meanInfo<T> mean_info = optimize_quant_invl_3d(data, r1, r2, r3, precision, capacity);
	if(params.increase_quant_intv){
		if(params.increase_quant_intv > 0) capacity <<= params.increase_quant_intv;
		else capacity >>= -params.increase_quant_intv;
	}
	int intv_radius = (capacity >> 1);
	int * type = (int *) malloc(size.num_elements * sizeof(int));
	T * unpredictable_data = (T *) malloc((0.05*size.num_elements) * sizeof(T));
	unsigned char * indicator = (unsigned char *) malloc(size.num_blocks * sizeof(unsigned char));
	T * unpredictable_data_pos = unpredictable_data;
	int * reg_params_type = (int *) malloc(RegCoeffNum3d * size.num_blocks * sizeof(int));
	float * reg_unpredictable_data = (float *) malloc(RegCoeffNum3d * size.num_blocks * sizeof(float));
	float * reg_unpredictable_data_pos = reg_unpredictable_data;
	size_t reg_count = params.block_independant? prediction_and_quantization_3d(data, size, mean_info, precision, capacity, intv_radius, indicator, type, reg_params_type, reg_unpredictable_data_pos, unpredictable_data_pos)
			: prediction_and_quantization_3d_with_border_prediction(data, size, mean_info, precision, capacity, intv_radius, indicator, type, reg_params_type, reg_unpredictable_data_pos, unpredictable_data_pos);
	size_t unpredictable_count = unpredictable_data_pos - unpredictable_data;
	unsigned char * compressed = NULL;
	// TODO: change to a better estimated size
	size_t est_size = size.num_elements*sizeof(T)*1.2;
	compressed = (unsigned char *) malloc(est_size);
	unsigned char * compressed_pos = compressed;
	write_variable_to_dst(compressed_pos, params);
	write_variable_to_dst(compressed_pos, precision);
	write_variable_to_dst(compressed_pos, intv_radius);
	write_variable_to_dst(compressed_pos, mean_info);
	write_variable_to_dst(compressed_pos, reg_count);
	write_variable_to_dst(compressed_pos, unpredictable_count);
	write_array_to_dst(compressed_pos, unpredictable_data, unpredictable_count);
	convertIntArray2ByteArray_fast_1b_to_result_sz(indicator, size.num_blocks, compressed_pos);
	if(reg_count) encode_regression_coefficients(reg_params_type, reg_unpredictable_data, RegCoeffNum3d*reg_count, reg_unpredictable_data_pos - reg_unpredictable_data, compressed_pos);
	Huffman_encode_tree_and_data(2*capacity, type, size.num_elements, compressed_pos);
	compressed_size = compressed_pos - compressed;
	free(indicator);
	free(unpredictable_data);
	free(reg_params_type);
	free(reg_unpredictable_data);
	free(type);
	return compressed;
}

template
unsigned char *
sz_compress_3d<float>(const float * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, const sz_params& params);

// perform compression
template<typename T>
unsigned char *
sz_compress_3d_knl(const T * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, const sz_params& params) {
    sz_compress_info compress_info;
    return sz_compress_3d_knl_2<T>(data, r1, r2, r3, precision, compressed_size, params, compress_info);
}
template
unsigned char *
sz_compress_3d_knl<float>(const float * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, const sz_params& params);


template<typename T>
unsigned char *
sz_compress_3d_knl_2(const T * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, const sz_params& params, sz_compress_info &compress_info){
	ori_data_sp_float = (float *) data;
	DSize_3d size(r1, r2, r3, params.block_size);
	int capacity = 0; // num of quant intervals
	meanInfo<T> mean_info = optimize_quant_invl_3d(data, r1, r2, r3, precision, capacity);
	if(params.increase_quant_intv){
		if(params.increase_quant_intv > 0) capacity <<= params.increase_quant_intv;
		else capacity >>= -params.increase_quant_intv;
	}
	//TODO kai whether tun capacity?
	capacity=65536;
	int intv_radius = (capacity >> 1);
	int * type = (int *) malloc(size.num_elements * sizeof(int));
	int * indicator = (int *) malloc(size.num_blocks * sizeof(int));
	int * reg_params_type = (int *) malloc(RegCoeffNum3d * size.num_blocks * sizeof(int));
	int * reg_poly_params_type = (int *) malloc(RegPolyCoeffNum3d * size.num_blocks * sizeof(int));
	float * reg_unpredictable_data = (float *) malloc(RegCoeffNum3d * size.num_blocks * sizeof(float));
	float * reg_poly_unpredictable_data = (float *) malloc(RegPolyCoeffNum3d * size.num_blocks * sizeof(float));
	float * reg_unpredictable_data_pos = reg_unpredictable_data;
	float * reg_poly_unpredictable_data_pos = reg_poly_unpredictable_data;

	// prepare unpred buffer for vectorization
	int est_unpred_count_per_index = size.num_blocks * size.block_size * 1;
	// if(!params.block_independant) est_unpred_count_per_index /= 20;
	T * unpred_data_buffer = (T *) malloc(size.block_size * size.block_size * est_unpred_count_per_index * sizeof(T));
	int * unpred_count_buffer = (int *) malloc(size.block_size * size.block_size * sizeof(int));
	memset(unpred_count_buffer, 0, size.block_size * size.block_size * sizeof(int));
	// predict and quant on KNL
	T precision_t = (T) precision;
	size_t reg_count=0, reg_poly_count = 0;
    prediction_and_quantization_3d_with_border_predicition_and_knl_optimization(data, size, mean_info,
                                                                                precision_t, capacity, intv_radius, indicator,
                                                                                type, reg_params_type, reg_unpredictable_data_pos,
                                                                                reg_poly_params_type,
                                                                                reg_poly_unpredictable_data_pos,
                                                                                unpred_count_buffer, unpred_data_buffer,
                                                                                est_unpred_count_per_index, params, reg_count,
                                                                                reg_poly_count, compress_info);
	unsigned char * compressed = NULL;
	// TODO: change to a better estimated size
	size_t est_size = size.num_elements*sizeof(T)*1.2;
	compressed = (unsigned char *) malloc(est_size);
	unsigned char * compressed_pos = compressed;
	write_variable_to_dst(compressed_pos, params);
	write_variable_to_dst(compressed_pos, precision);
	write_variable_to_dst(compressed_pos, intv_radius);
	write_variable_to_dst(compressed_pos, mean_info);
	write_variable_to_dst(compressed_pos, reg_count);
	write_variable_to_dst(compressed_pos, reg_poly_count);
	write_array_to_dst(compressed_pos, unpred_count_buffer, size.block_size * size.block_size);
	T * unpred_data_buffer_pos = unpred_data_buffer;
	for(int i=0; i<size.block_size; i++){
		for(int j=0; j<size.block_size; j++){
			write_array_to_dst(compressed_pos, unpred_data_buffer_pos, unpred_count_buffer[i*size.block_size + j]);
			unpred_data_buffer_pos += est_unpred_count_per_index;
		}
	}
	free(unpred_data_buffer);
	free(unpred_count_buffer);


	Huffman_encode_tree_and_data(SELECTOR_RADIUS, indicator, size.num_blocks, compressed_pos);
//	convertIntArray2ByteArray_fast_1b_to_result_sz(indicator, size.num_blocks, compressed_pos);

    if (reg_count) {
        encode_regression_coefficients(reg_params_type, reg_unpredictable_data, RegCoeffNum3d*reg_count,
                                       reg_unpredictable_data_pos - reg_unpredictable_data, compressed_pos);
    }
    if (reg_poly_count) {
		encode_regression_coefficients(reg_poly_params_type, reg_poly_unpredictable_data, RegPolyCoeffNum3d*reg_poly_count,
                                       reg_poly_unpredictable_data_pos - reg_poly_unpredictable_data, compressed_pos);
    }


    Huffman_encode_tree_and_data(2*capacity, type, size.num_elements, compressed_pos);

	compressed_size = compressed_pos - compressed;
	free(indicator);
	free(reg_params_type);
	free(reg_poly_params_type);
	free(reg_unpredictable_data);
	free(reg_poly_unpredictable_data);
	free(type);
	return compressed;
}

template
unsigned char *
sz_compress_3d_knl_2<float>(const float * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, const sz_params& params, sz_compress_info &compress_info);


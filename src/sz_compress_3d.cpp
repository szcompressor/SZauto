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
                                                                            size_t & reg_poly_count) {
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
	T rel_param_err = RegErrThreshold * precision / RegCoeffNum3d;
	for(int i=0; i<RegCoeffNum3d-1; i++){
		reg_precisions[i] = rel_param_err / size.block_size;
		reg_recip_precisions[i] = 1.0 / reg_precisions[i];
	}
	reg_precisions[RegCoeffNum3d - 1] = rel_param_err;
	reg_recip_precisions[RegCoeffNum3d - 1] = 1.0 / reg_precisions[RegCoeffNum3d - 1];

	T reg_poly_precisions[RegPolyCoeffNum3d];
    T reg_poly_recip_precisions[RegPolyCoeffNum3d];
    T rel_poly_param_err = RegErrThreshold * precision;
	reg_poly_precisions[0]= rel_poly_param_err / 2;
    for (int i = 1; i < 4; i++) {
		reg_poly_precisions[i] = rel_poly_param_err / RegPolyCoeffNum3d;
    }
    for (int i = 4; i < 10; i++) {
		reg_poly_precisions[i] = rel_poly_param_err / RegPolyCoeffNum3d / size.block_size;
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

				if (params.use_regression_linear){
                    compute_regression_coeffcients_3d(z_data_pos, size_x, size_y, size_z, size.dim0_offset, size.dim1_offset,
                                                      reg_params_pos);
                }
                if (params.use_regression_poly){
                    compute_regression_coeffcients_3d_poly(z_data_pos, size_x, size_y, size_z, size.dim0_offset, size.dim1_offset,
                                                      reg_poly_params_pos, coef_aux_list);
                }
                int selection_result = sz_blockwise_selection_3d(z_data_pos, mean_info, size.dim0_offset, size.dim1_offset,
                                                                 min_size, precision, reg_params_pos, reg_poly_params_pos,
                                                                 params.prediction_dim,
                                                                 params.use_lorenzo, params.use_lorenzo_2layer,
                                                                 params.use_regression_linear, params.use_regression_poly);
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
sz_compress_3d_knl(const T * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, const sz_params& params){
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
                                                                                reg_poly_count);
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
sz_compress_3d_knl<float>(const float * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size, const sz_params& params);



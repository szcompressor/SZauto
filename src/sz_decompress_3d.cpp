#include "sz_decompress_3d.hpp"
#include "sz_decompress_block_processing.hpp"
#include "sz_decompress_block_processing_knl.hpp"

// avoid mistake when running on KNL 
// need to be checked later;
// also in sz_decompress_block_processing_knl.hpp
float * dec_data_sp_float = NULL;

// use block-independant use_lorenzo pred & quant
template<typename T>
void
prediction_and_decompression_3d(const DSize_3d& size, const meanInfo<T>& mean_info, double precision,
	int intv_radius, const float * reg_params, const unsigned char * indicator,
	const int * type, const T * unpredictable_data_pos, T * dec_data, const sz_params& params){
	const int * type_pos = type;
	const unsigned char * indicator_pos = indicator;
	const float * reg_params_pos = reg_params;
	size_t buffer_dim0_offset = (size.block_size + 1) * (size.block_size + 1);
	size_t buffer_dim1_offset = size.block_size + 1;
	T * pred_buffer = (T *) malloc((size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	memset(pred_buffer, 0, (size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	T * x_data_pos = dec_data;
	for(size_t i=0; i<size.num_x; i++){
		T * y_data_pos = x_data_pos;
		for(size_t j=0; j<size.num_y; j++){
			T * z_data_pos = y_data_pos;
			for(size_t k=0; k<size.num_z; k++){
				int size_x = ((i+1)*size.block_size < size.d1) ? size.block_size : size.d1 - i*size.block_size;
				int size_y = ((j+1)*size.block_size < size.d2) ? size.block_size : size.d2 - j*size.block_size;
				int size_z = ((k+1)*size.block_size < size.d3) ? size.block_size : size.d3 - k*size.block_size;
				if(*indicator_pos){
					// regression
					block_pred_and_decompress_regression_3d(reg_params_pos, precision, intv_radius,
						size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos, z_data_pos);
					reg_params_pos += RegCoeffNum3d;
				}
				else{
					// Lorenzo
					block_pred_and_decompress_lorenzo_3d(mean_info, pred_buffer, precision, intv_radius, size_x, size_y, size_z,
							buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos, z_data_pos);
				}
				indicator_pos ++;
				z_data_pos += size_z;
			}
			y_data_pos += size.block_size*size.dim1_offset;
		}
		x_data_pos += size.block_size*size.dim0_offset;
	}
	free(pred_buffer);
}

// use block-dependant use_lorenzo pred & quant
template<typename T>
void
prediction_and_decompression_3d_with_border_prediction(const DSize_3d& size, const meanInfo<T>& mean_info, double precision,
	int intv_radius, const float * reg_params, const unsigned char * indicator,
	const int * type, const T * unpredictable_data_pos, T * dec_data, const sz_params& params){
	const int * type_pos = type;
	const unsigned char * indicator_pos = indicator;
	const float * reg_params_pos = reg_params;
	size_t buffer_dim0_offset = (size.d2+1)*(size.d3+1);
	size_t buffer_dim1_offset = size.d3+1;
	T * pred_buffer = (T *) malloc((size.block_size+1)*(size.d2+1)*(size.d3+1)*sizeof(T));
	memset(pred_buffer, 0, (size.block_size+1)*(size.d2+1)*(size.d3+1)*sizeof(T));
	T * x_data_pos = dec_data;
	for(size_t i=0; i<size.num_x; i++){
		T * y_data_pos = x_data_pos;
		T * pred_buffer_pos = pred_buffer;
		for(size_t j=0; j<size.num_y; j++){
			T * z_data_pos = y_data_pos;
			for(size_t k=0; k<size.num_z; k++){
				int size_x = ((i+1)*size.block_size < size.d1) ? size.block_size : size.d1 - i*size.block_size;
				int size_y = ((j+1)*size.block_size < size.d2) ? size.block_size : size.d2 - j*size.block_size;
				int size_z = ((k+1)*size.block_size < size.d3) ? size.block_size : size.d3 - k*size.block_size;
				if(*indicator_pos){
					// regression
					block_pred_and_decompress_regression_3d_with_buffer(reg_params_pos, pred_buffer_pos, precision, intv_radius,
						size_x, size_y, size_z, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos, z_data_pos);
					reg_params_pos += RegCoeffNum3d;
				}
				else{
					// Lorenzo
					block_pred_and_decompress_lorenzo_3d(mean_info, pred_buffer_pos, precision, intv_radius, size_x, size_y, size_z,
							buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos, z_data_pos);
				}
				pred_buffer_pos += size.block_size;
				indicator_pos ++;
				z_data_pos += size_z;
			}
			y_data_pos += size.block_size*size.dim1_offset;
			pred_buffer_pos += size.block_size*buffer_dim1_offset - size.block_size*size.num_z;
		}
		memcpy(pred_buffer, pred_buffer + size.block_size*buffer_dim0_offset, buffer_dim0_offset*sizeof(T));
		x_data_pos += size.block_size*size.dim0_offset;
	}
	free(pred_buffer);
}

template<typename T>
void
prediction_and_decompression_3d_with_knl_optimization(const DSize_3d& size, const meanInfo<T>& mean_info, T precision,
	int intv_radius, const float * reg_params, const unsigned char * indicator,
	const int * type, int * unpred_count_buffer, const T * unpred_data_buffer, const int offset, T * dec_data, const sz_params& params){
	const int * type_pos = type;
	const unsigned char * indicator_pos = indicator;
	const float * reg_params_pos = reg_params;
	size_t buffer_dim0_offset = (size.block_size + 1) * (size.block_size + 1);
	size_t buffer_dim1_offset = size.block_size + 1;
	T * pred_buffer = (T *) malloc((size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	memset(pred_buffer, 0, (size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	T * x_data_pos = dec_data;
	for(size_t i=0; i<size.num_x; i++){
		T * y_data_pos = x_data_pos;
		for(size_t j=0; j<size.num_y; j++){
			T * z_data_pos = y_data_pos;
			for(size_t k=0; k<size.num_z; k++){
				int size_x = ((i+1)*size.block_size < size.d1) ? size.block_size : size.d1 - i*size.block_size;
				int size_y = ((j+1)*size.block_size < size.d2) ? size.block_size : size.d2 - j*size.block_size;
				int size_z = ((k+1)*size.block_size < size.d3) ? size.block_size : size.d3 - k*size.block_size;
				if(*indicator_pos){
					// regression
					block_pred_and_decompress_regression_3d_knl(reg_params_pos, precision, intv_radius,
						size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, type_pos, unpred_count_buffer, unpred_data_buffer, offset, z_data_pos);
					reg_params_pos += RegCoeffNum3d;
				}
				else{
					// Lorenzo
					block_pred_and_decompress_lorenzo_3d_knl_2d_pred(mean_info, pred_buffer, precision, intv_radius, size_x, size_y, size_z,
							buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpred_count_buffer, unpred_data_buffer, offset, z_data_pos, 1);
				}
				indicator_pos ++;
				z_data_pos += size_z;
			}
			y_data_pos += size.block_size*size.dim1_offset;
		}
		x_data_pos += size.block_size*size.dim0_offset;
	}
	free(pred_buffer);
}

// use block-dependant use_lorenzo pred & quant
template<typename T>
void
prediction_and_decompression_3d_with_border_prediction_and_knl_optimization(const DSize_3d &size, const meanInfo<T> &mean_info,
                                                                            T precision,
                                                                            int intv_radius, const float *reg_params,
                                                                            const float *reg_poly_params,
                                                                            const int *indicator,
                                                                            const int *type, int *unpred_count_buffer,
                                                                            const T *unpred_data_buffer,
                                                                            const int offset, T *dec_data,
                                                                            const sz_params &params) {

	const int * type_pos = type;
	const int * indicator_pos = indicator;
	const float * reg_params_pos = reg_params;
	const float * reg_poly_params_pos = reg_poly_params;
	// add one more ghost layer
    size_t buffer_dim0_offset = (size.d2 + params.lorenzo_padding_layer) * (size.d3 + params.lorenzo_padding_layer);
    size_t buffer_dim1_offset = size.d3 + params.lorenzo_padding_layer;
    T *pred_buffer = (T *) malloc((size.block_size + params.lorenzo_padding_layer) * (size.d2 + params.lorenzo_padding_layer) * (size.d3 + params.lorenzo_padding_layer) * sizeof(T));
    memset(pred_buffer, 0, (size.block_size + params.lorenzo_padding_layer) * (size.d2 + params.lorenzo_padding_layer) * (size.d3 + params.lorenzo_padding_layer) * sizeof(T));
	auto *lorenzo_pred_and_decomp = block_pred_and_decompress_lorenzo_3d_knl_3d_pred<T>;
	if(params.prediction_dim == 2) lorenzo_pred_and_decomp = block_pred_and_decompress_lorenzo_3d_knl_2d_pred<T>;
	else if(params.prediction_dim == 1) lorenzo_pred_and_decomp = block_pred_and_decompress_lorenzo_3d_knl_1d_pred<T>;
	T * x_data_pos = dec_data;
	for(size_t i=0; i<size.num_x; i++){
		T * y_data_pos = x_data_pos;
		T * pred_buffer_pos = pred_buffer;
		for(size_t j=0; j<size.num_y; j++){
			T * z_data_pos = y_data_pos;
			for(size_t k=0; k<size.num_z; k++){
				int size_x = ((i+1)*size.block_size < size.d1) ? size.block_size : size.d1 - i*size.block_size;
				int size_y = ((j+1)*size.block_size < size.d2) ? size.block_size : size.d2 - j*size.block_size;
				int size_z = ((k+1)*size.block_size < size.d3) ? size.block_size : size.d3 - k*size.block_size;
                if(*indicator_pos==SELECTOR_REGRESSION_POLY){
                    // regression
                    block_pred_and_decompress_regression_3d_with_buffer_knl(reg_poly_params_pos, pred_buffer_pos, precision, intv_radius,
                                                                            size_x, size_y, size_z, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpred_count_buffer, unpred_data_buffer, offset, z_data_pos, true, params.lorenzo_padding_layer);
                    reg_poly_params_pos += RegPolyCoeffNum3d;
                }else if (*indicator_pos==SELECTOR_REGRESSION){
					// regression
					block_pred_and_decompress_regression_3d_with_buffer_knl(reg_params_pos, pred_buffer_pos, precision, intv_radius,
						size_x, size_y, size_z, buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpred_count_buffer, unpred_data_buffer, offset, z_data_pos, false, params.lorenzo_padding_layer);
					reg_params_pos += RegCoeffNum3d;
                } else {
					// Lorenzo
					lorenzo_pred_and_decomp(mean_info, pred_buffer_pos, precision, intv_radius, size_x, size_y, size_z,
							buffer_dim0_offset, buffer_dim1_offset, size.dim0_offset, size.dim1_offset, type_pos, unpred_count_buffer, unpred_data_buffer, offset, z_data_pos, params.lorenzo_padding_layer, *indicator_pos==SELECTOR_LORENZO_2LAYER);
				}
				pred_buffer_pos += size.block_size;
				indicator_pos ++;
				z_data_pos += size_z;
			}
			y_data_pos += size.block_size*size.dim1_offset;
			pred_buffer_pos += size.block_size*buffer_dim1_offset - size.block_size*size.num_z;
		}
        memcpy(pred_buffer, pred_buffer + size.block_size*buffer_dim0_offset, params.lorenzo_padding_layer * buffer_dim0_offset * sizeof(T));
		x_data_pos += size.block_size*size.dim0_offset;
	}
	free(pred_buffer);
}

// perform decompression
template<typename T>
T *
sz_decompress_3d(const unsigned char * compressed, size_t r1, size_t r2, size_t r3){
	const unsigned char * compressed_pos = compressed;
	sz_params params;
	read_variable_from_src(compressed_pos, params);
	DSize_3d size(r1, r2, r3, params.block_size);
	double precision = 0;
	read_variable_from_src(compressed_pos, precision);
	int intv_radius = 0;
	read_variable_from_src(compressed_pos, intv_radius);
	meanInfo<T> mean_info;
	read_variable_from_src(compressed_pos, mean_info);
	size_t reg_count = 0;
	read_variable_from_src(compressed_pos, reg_count);
	size_t unpredictable_count = 0;
	read_variable_from_src(compressed_pos, unpredictable_count);
	const T * unpredictable_data = (const T *) compressed_pos;
	compressed_pos += unpredictable_count * sizeof(float);
	unsigned char * indicator = convertByteArray2IntArray_fast_1b_sz(size.num_blocks, compressed_pos, (size.num_blocks - 1)/8 + 1);
	float * reg_params = NULL;
	if(reg_count) reg_params = decode_regression_coefficients(compressed_pos, reg_count, size.block_size, precision);
	const float * reg_params_pos = reg_count? (const float *) (reg_params + RegCoeffNum3d) : NULL;
	int * type = Huffman_decode_tree_and_data(4*intv_radius, size.num_elements, compressed_pos);
	T * dec_data = (T *) malloc(size.num_elements*sizeof(T));
	params.block_independant ? prediction_and_decompression_3d(size, mean_info, precision, intv_radius, reg_params_pos, indicator, type, unpredictable_data, dec_data, params)
		: prediction_and_decompression_3d_with_border_prediction(size, mean_info, precision, intv_radius, reg_params_pos, indicator, type, unpredictable_data, dec_data, params);
	free(indicator);
	free(reg_params);
	free(type);
	return dec_data;
}

template
float *
sz_decompress_3d<float>(const unsigned char * compressed, size_t r1, size_t r2, size_t r3);

// perform decompression
template<typename T>
T *
sz_decompress_3d_knl(const unsigned char * compressed, size_t r1, size_t r2, size_t r3){
	const unsigned char * compressed_pos = compressed;
	sz_params params;
	read_variable_from_src(compressed_pos, params);
	DSize_3d size(r1, r2, r3, params.block_size);
	double precision = 0;
	read_variable_from_src(compressed_pos, precision);
	int intv_radius = 0;
	read_variable_from_src(compressed_pos, intv_radius);
	meanInfo<T> mean_info;
	read_variable_from_src(compressed_pos, mean_info);
	size_t reg_count = 0;
	read_variable_from_src(compressed_pos, reg_count);
    size_t reg_poly_count = 0;
    read_variable_from_src(compressed_pos, reg_poly_count);
	// prepare unpred buffer for vectorization
	int est_unpred_count_per_index = size.num_blocks * size.block_size * 1;
	// if(!params.block_independant) est_unpred_count_per_index /= 20;
	int * unpred_count_buffer = read_array_from_src<int>(compressed_pos, size.block_size * size.block_size);
	T * unpred_data_buffer = (T *) malloc(size.block_size * size.block_size * est_unpred_count_per_index * sizeof(T));
	T * unpred_data_buffer_pos = unpred_data_buffer;
	for(int i=0; i<size.block_size; i++){
		for(int j=0; j<size.block_size; j++){
			memcpy(unpred_data_buffer_pos, compressed_pos, unpred_count_buffer[i*size.block_size + j]*sizeof(T));
			compressed_pos += unpred_count_buffer[i*size.block_size + j] * sizeof(T);
			unpred_data_buffer_pos += est_unpred_count_per_index;
		}
	}
	memset(unpred_count_buffer, 0, size.block_size * size.block_size * sizeof(int));
//	unsigned char * indicator = convertByteArray2IntArray_fast_1b_sz(size.num_blocks, compressed_pos, (size.num_blocks - 1)/8 + 1);
    int *indicator = Huffman_decode_tree_and_data(SELECTOR_RADIUS, size.num_blocks, compressed_pos);

    float *reg_params = nullptr, *reg_poly_params = nullptr;
    const float *reg_params_pos = nullptr;
    const float *reg_poly_params_pos = nullptr;
	T precision_t = (T) precision;
    if (reg_count) {
        reg_params = decode_regression_coefficients_v2(compressed_pos, reg_count, size.block_size, precision_t, params);
        reg_params_pos = (const float *) (reg_params + RegCoeffNum3d);
    }
    if (reg_poly_count) {
        reg_poly_params = decode_poly_regression_coefficients_v2(compressed_pos, reg_poly_count, size.block_size, precision_t, params);
        reg_poly_params_pos = (const float *) (reg_poly_params + RegPolyCoeffNum3d);
    }
	int * type = Huffman_decode_tree_and_data(4*intv_radius, size.num_elements, compressed_pos);
	T * dec_data = (T *) malloc(size.num_elements*sizeof(T));
	dec_data_sp_float = (float *) dec_data;
	prediction_and_decompression_3d_with_border_prediction_and_knl_optimization(size, mean_info, precision_t, intv_radius, reg_params_pos, reg_poly_params_pos, indicator, type, unpred_count_buffer, unpred_data_buffer, est_unpred_count_per_index, dec_data, params);
	free(unpred_count_buffer);
	free(unpred_data_buffer);
	free(indicator);
	free(reg_params);
	free(reg_poly_params);
	free(type);
	return dec_data;
}

template
float *
sz_decompress_3d_knl<float>(const unsigned char * compressed, size_t r1, size_t r2, size_t r3);

template
double *
sz_decompress_3d_knl<double>(const unsigned char * compressed, size_t r1, size_t r2, size_t r3);




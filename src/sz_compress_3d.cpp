#include "sz_compress_3d.hpp"

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

template<typename T>
inline void
sz_block_error_estimation_3d(const T * data_pos, const float * reg_params_pos, const meanInfo<T>& mean_info, int x, int y, int z, size_t dim0_offset, size_t dim1_offset, float noise, double& err_reg, double& err_lorenzo){
	const T * cur_data_pos = data_pos + x*dim0_offset + y*dim1_offset + z;
	T cur_data = *cur_data_pos;
	err_reg += fabs(cur_data - regression_predict_3d<T>(reg_params_pos, x, y, z));
	err_lorenzo += mean_info.use_mean ? MIN(fabs(cur_data - mean_info.mean), fabs(cur_data - lorenzo_predict_3d(cur_data_pos, dim0_offset, dim1_offset)) + noise) : fabs(cur_data - lorenzo_predict_3d(cur_data_pos, dim0_offset, dim1_offset)) + noise;
}

template<typename T>
inline int
sz_blockwise_selection_3d(const T * data_pos, const meanInfo<T>& mean_info, size_t dim0_offset, size_t dim1_offset, int min_size, float noise, const float * reg_params_pos){
	double err_reg = 0;
	double err_lorenzo = 0;
	for(int i=1; i<min_size; i++){
		int bmi = min_size - i;
		sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, i, i, dim0_offset, dim1_offset, noise, err_reg, err_lorenzo);
		sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, i, bmi, dim0_offset, dim1_offset, noise, err_reg, err_lorenzo);
		sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, bmi, i, dim0_offset, dim1_offset, noise, err_reg, err_lorenzo);
		sz_block_error_estimation_3d(data_pos, reg_params_pos, mean_info, i, bmi, bmi, dim0_offset, dim1_offset, noise, err_reg, err_lorenzo);
	}
	return err_reg < err_lorenzo;
}

inline void
compress_regression_coefficient_3d(const double * precisions, float * reg_params_pos, int * reg_params_type_pos, float *& reg_unpredictable_data_pos){
	float * prev_reg_params = reg_params_pos - RegCoeffNum3d;
	for(int i=0; i<RegCoeffNum3d; i++){
		*(reg_params_type_pos ++) = quantize(*prev_reg_params, *reg_params_pos, precisions[i], RegCoeffCapacity, RegCoeffRadius, reg_unpredictable_data_pos, reg_params_pos);
		prev_reg_params ++, reg_params_pos ++; 
	}
}

template<typename T>
inline void
block_pred_and_quant_regression_3d(const T * data_pos, const float * reg_params_pos, double precision, int capacity, 
	int intv_radius, int size_x, int size_y, int size_z, size_t dim0_offset, size_t dim1_offset, int *& type_pos, T *& unpredictable_data_pos){
	const T * cur_data_pos = data_pos;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			for(int k=0; k<size_z; k++){
				float pred = regression_predict_3d<T>(reg_params_pos, i, j, k);
				*(type_pos++) = quantize(pred, *cur_data_pos, precision, capacity, intv_radius, unpredictable_data_pos);
				cur_data_pos ++;
			}
			cur_data_pos += dim1_offset - size_z;
		}
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

// block-independant lorenzo pred & quant
template<typename T>
inline void
block_pred_and_quant_lorenzo_3d(const meanInfo<T>& mean_info, const T * data_pos, T * buffer, int buffer_block_size, double precision, int capacity, int intv_radius, 
	int size_x, int size_y, int size_z, size_t dim0_offset, size_t dim1_offset, int *& type_pos, T *& unpredictable_data_pos){
	const T * cur_data_pos = data_pos;
	T * buffer_pos = buffer + buffer_block_size*buffer_block_size + buffer_block_size + 1;
	for(int i=0; i<size_x; i++){
		for(int j=0; j<size_y; j++){
			memcpy(buffer_pos, cur_data_pos, sizeof(T)*size_z);
			T * cur_buffer_pos = buffer_pos;
			for(int k=0; k<size_z; k++){
				if(mean_info.use_mean && (fabs(*cur_data_pos - mean_info.mean) < precision)){
					*(type_pos++) = 1;
					*cur_buffer_pos = mean_info.mean;
				}
				else{
					float pred = lorenzo_predict_3d(cur_buffer_pos, buffer_block_size*buffer_block_size, buffer_block_size);
					*(type_pos++) = quantize(pred, *cur_data_pos, precision, capacity, intv_radius, unpredictable_data_pos, cur_buffer_pos);
				}
				cur_data_pos ++;
				cur_buffer_pos ++;
			}
			buffer_pos += buffer_block_size;
			cur_data_pos += dim1_offset - size_z;
		}
		buffer_pos += buffer_block_size*buffer_block_size - size_y*buffer_block_size;
		cur_data_pos += dim0_offset - size_y * dim1_offset;
	}
}

// return regression count
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
	float rel_param_err = RegErrThreshold * precision / RegCoeffNum3d;
	for(int i=0; i<RegCoeffNum3d-1; i++) 
		reg_precisions[i] = rel_param_err / size.block_size;
	reg_precisions[RegCoeffNum3d - 1] = rel_param_err;
	T * pred_buffer = (T *) malloc((size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	memset(pred_buffer, 0, (size.block_size+1)*(size.block_size+1)*(size.block_size+1)*sizeof(T));
	size_t reg_count = 0;
	int capacity_lorenzo = mean_info.use_mean ? capacity - 2 : capacity;
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
					compress_regression_coefficient_3d(reg_precisions, reg_params_pos, reg_params_type_pos, reg_unpredictable_data_pos);
					block_pred_and_quant_regression_3d(z_data_pos, reg_params_pos, precision, capacity, intv_radius, 
						size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos);
					reg_count ++;
					reg_params_pos += RegCoeffNum3d;
					reg_params_type_pos += RegCoeffNum3d;
				}
				else{
					// Lorenzo
					block_pred_and_quant_lorenzo_3d(mean_info, z_data_pos, pred_buffer, size.block_size+1, precision, capacity_lorenzo, intv_radius, 
						size_x, size_y, size_z, size.dim0_offset, size.dim1_offset, type_pos, unpredictable_data_pos);
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
float
sample_rough_mean(const T * data, size_t r1, size_t r2, size_t r3, size_t sample_distance){
	double mean = 0;
	size_t len = r1 * r2 * r3;
	const T * data_pos = data;
	size_t offset_count = 0;
	size_t offset_count_2 = 0;
	size_t mean_count = 0;
	while(data_pos - data < len){
		mean += *data_pos;
		mean_count ++;
		data_pos += sample_distance;
		offset_count += sample_distance;
		offset_count_2 += sample_distance;
		if(offset_count >= r3){
			offset_count = 0;
			data_pos -= 1;
		}
		if(offset_count_2 >= r2 * r3){
			offset_count_2 = 0;
			data_pos -= 1;
		}
	}
	if(mean_count > 0) mean /= mean_count;
	return mean;
}

unsigned int
estimate_quantization_intervals(const std::vector<size_t>& intervals, size_t sample_count){
	size_t target = sample_count * QuantIntvAccThreshold;
	size_t sum = 0;
	size_t i = 0;
	for(i=0;i<intervals.size();i++){
		sum += intervals[i];
		if(sum>target)
			break;
	}
	if(i == intervals.size()) i = intervals.size()-1;
	unsigned int accIntervals = 2*(i+1);
	unsigned int num_intervals =  2*round_up_power_of_2(accIntervals);
	return (num_intervals > 32) ? num_intervals : 32;
}

float
estimate_mean_freq_and_position(const std::vector<size_t>& freq_intervals, double precision, size_t sample_count, float& mean_guess){
	size_t max_sum = 0;
	size_t max_index = 0;
	size_t tmp_sum = 0;
	for(size_t i=1; i<freq_intervals.size()-2; i++){
		tmp_sum = freq_intervals[i] + freq_intervals[i+1];
		if(tmp_sum > max_sum){
			max_sum = tmp_sum;
			max_index = i;
		}
	}
	mean_guess += precision * (ptrdiff_t)(max_index + 1 - (freq_intervals.size() >> 1));
	return max_sum * 1.0 / sample_count;
}

template<typename T>
meanInfo<T>
optimize_quant_invl_3d(const T * data, size_t r1, size_t r2, size_t r3, double precision, int& capacity){
	float mean_rough = sample_rough_mean(data, r1, r2, r3, sqrt(r1*r2*r3));
	std::vector<size_t> intervals = std::vector<size_t>(QuantIntvSampleCapacity, 0);
	std::vector<size_t> freq_intervals = std::vector<size_t>(QuantIntvMeanCapacity, 0);
	size_t freq_count = 0;
	size_t sample_count = 0;
	size_t sample_distance = QuantIntvSampleDistance;
	size_t offset_count = sample_distance - 2; // count r3 offset
	size_t offset_count_2 = 0;
	size_t r23 = r2 * r3;
	size_t len = r1 * r23;
	const float *data_pos = data + r23 + r3 + offset_count;
	size_t n1_count = 1, n2_count = 1; // count i,j sum
	T pred_value = 0;
	double mean_diff = 0;
	ptrdiff_t freq_index = 0;
	size_t pred_index = 0;
	float pred_err = 0;
	int radius = (QuantIntvMeanCapacity >> 1);
	while(data_pos - data < len){
		pred_value = lorenzo_predict_3d(data_pos, r23, r3);
		pred_err = fabs(pred_value - *data_pos);
		if(pred_err < precision) freq_count ++;
		pred_index = (pred_err/precision+1)/2;
		if(pred_index >= intervals.size()){
			pred_index = intervals.size() - 1;
		}
		intervals[pred_index]++;

		mean_diff = *data_pos - mean_rough;
		if(mean_diff > 0) freq_index = (ptrdiff_t)(mean_diff/precision) + radius;
		else freq_index = (ptrdiff_t)(mean_diff/precision) - 1 + radius;
		if(freq_index <= 0){
			freq_intervals[0] ++;
		}
		else if(freq_index >= freq_intervals.size()){
			freq_intervals[freq_intervals.size() - 1] ++;
		}
		else{
			freq_intervals[freq_index] ++;
		}
		offset_count += sample_distance;
		if(offset_count >= r3){
			n2_count ++;
			if(n2_count == r2){
				n1_count ++;
				n2_count = 1;
				data_pos += r3;
			}
			offset_count_2 = (n1_count + n2_count) % sample_distance;
			data_pos += (r3 + sample_distance - offset_count) + (sample_distance - offset_count_2);
			offset_count = (sample_distance - offset_count_2);
			if(offset_count == 0) offset_count ++;
		}
		else data_pos += sample_distance;
		sample_count ++;
	}	
	float pred_freq = freq_count * 1.0/ sample_count;
	float mean_guess = mean_rough;
	float mean_freq = estimate_mean_freq_and_position(freq_intervals, precision, sample_count, mean_guess);
	capacity = estimate_quantization_intervals(intervals, sample_count);
	if(mean_freq > 0.5 || mean_freq > pred_freq){
		double sum = 0.0;
		size_t mean_count = 0;
		for(size_t i=0; i<len; i++){
			if(fabs(data[i] - mean_guess) <= precision){
				sum += data[i];
				mean_count ++;
			}
		}
		double mean = 0;
		if(mean_count > 0) mean = sum / mean_count;
		return meanInfo<T>(true, mean);
	}
	return meanInfo<T>(false, 0);
}

void
encode_regression_coefficients(const int * reg_params_type, const float * reg_unpredictable_data, size_t reg_count, size_t reg_unpredictable_count, unsigned char *& compressed_pos){
	write_variable_to_dst(compressed_pos, reg_unpredictable_count);
	write_array_to_dst(compressed_pos, reg_unpredictable_data, reg_unpredictable_count);
	Huffman_encode_tree_and_data(2*RegCoeffCapacity, reg_params_type, RegCoeffNum3d*reg_count, compressed_pos);
}

// perform block-independant compression
template<typename T>
unsigned char *
sz_compress_3d(const T * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size){
	DSize_3d size(r1, r2, r3, BSIZE3d);
	int capacity = 0; // num of quant intervals
	meanInfo<T> mean_info = optimize_quant_invl_3d(data, r1, r2, r3, precision, capacity);
	int intv_radius = (capacity >> 1);
	int * type = (int *) malloc(size.num_elements * sizeof(int));
	T * unpredictable_data = (T *) malloc((0.05*size.num_elements) * sizeof(T));
	unsigned char * indicator = (unsigned char *) malloc(size.num_blocks * sizeof(unsigned char));
	T * unpredictable_data_pos = unpredictable_data;
	int * reg_params_type = (int *) malloc(RegCoeffNum3d * size.num_blocks * sizeof(int));
	float * reg_unpredictable_data = (float *) malloc(RegCoeffNum3d * size.num_blocks * sizeof(float));
	float * reg_unpredictable_data_pos = reg_unpredictable_data;
	size_t reg_count = prediction_and_quantization_3d(data, size, mean_info, precision, capacity, intv_radius, indicator, type, reg_params_type, reg_unpredictable_data_pos, unpredictable_data_pos);
	size_t unpredictable_count = unpredictable_data_pos - unpredictable_data;
	unsigned char * compressed = NULL;
	// TODO: change to a better estimated size
	size_t est_size = size.num_elements*sizeof(T)*1.2;
	compressed = (unsigned char *) malloc(est_size);
	unsigned char * compressed_pos = compressed;
	write_variable_to_dst(compressed_pos, size.block_size);
	write_variable_to_dst(compressed_pos, precision);
	write_variable_to_dst(compressed_pos, intv_radius);
	write_variable_to_dst(compressed_pos, mean_info);
	write_variable_to_dst(compressed_pos, reg_count);
	write_variable_to_dst(compressed_pos, unpredictable_count);
	write_array_to_dst(compressed_pos, unpredictable_data, unpredictable_count);
	convertIntArray2ByteArray_fast_1b_to_result_sz(indicator, size.num_blocks, compressed_pos);;
	if(reg_count) encode_regression_coefficients(reg_params_type, reg_unpredictable_data, reg_count, reg_unpredictable_data_pos - reg_unpredictable_data, compressed_pos);
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
sz_compress_3d<float>(const float * data, size_t r1, size_t r2, size_t r3, double precision, size_t& compressed_size);





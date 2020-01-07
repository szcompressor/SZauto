#include "sz_utils.hpp"
#include "sz_prediction.hpp"
#include "sz_compression_utils.hpp"
#include "sz_optimize_quant_intervals.hpp"
#include "sz_autotuning_3d.hpp"
#include <list>

using namespace std;

template<typename T>
unsigned char *
sz_compress_autotuning_3d_no_highorder(T *data, size_t r1, size_t r2, size_t r3, double relative_eb, size_t &compressed_size,
                                       bool baseline = false, bool decompress = false, bool log = false,
                                       float sample_ratio = 0.2) {
    size_t num_elements = r1 * r2 * r3;
    float max = data[0];
    float min = data[0];
    for (int i = 1; i < num_elements; i++) {
        if (max < data[i]) max = data[i];
        if (min > data[i]) min = data[i];
    }
    double precision = relative_eb * (max - min);

    if (baseline) {
        sz_params baseline_param(false, 6, 3, 0, true, false, true, false, precision);
        auto baseline_compress_info = sz_compress_decompress_highorder_3d<T>(data, num_elements, r1, r2, r3, precision,
                                                                             baseline_param, true);
        fprintf(stdout,
                "Baseline: reb:%.1e, ratio:%.2f, compress_time:%.3f, PSNR:%.2f, NRMSE %.10e Ori_bytes %ld, Compressed_bytes %ld\n",
                relative_eb, baseline_compress_info.ratio, baseline_compress_info.compress_time, baseline_compress_info.psnr,
                baseline_compress_info.nrmse, baseline_compress_info.ori_bytes,
                baseline_compress_info.compress_bytes);
    }

    char best_param_str[1000];
    char buffer[1000];

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int sample_num = 0;

    int capacity = 65536 * 2;
    optimize_quant_invl_3d(data, r1, r2, r3, precision, capacity);
//    printf("tuning capacity: %d\n", capacity);

    float best_ratio = 0;
    sz_params best_params_stage1;
    {
        for (auto use_lorenzo:{true}) {
            list<bool> use_lorenzo_2layer_set = {false};
            for (auto use_lorenzo_2layer:use_lorenzo_2layer_set) {
                //TODO kai
//            for (auto use_lorenzo_2layer:{true}) {
                if (use_lorenzo || use_lorenzo_2layer) {
                    list<int> pred_dim_set = {3, 2};
                    for (auto pred_dim: pred_dim_set) {
                        sz_params params(false, 6, pred_dim, 0, use_lorenzo,
                                         use_lorenzo_2layer, false, false, precision);
                        params.sample_ratio = sample_ratio * 1.2;
                        params.capacity = capacity;
                        params.lossless = false;
                        auto compress_info = do_compress_sampling<T>(data, num_elements, r1, r2, r3, precision, params);
                        sample_num++;

                        sprintf(buffer,
                                "lorenzo:%d, lorenzo2:%d, pred_dim:%d\n",
                                use_lorenzo, use_lorenzo_2layer, pred_dim);
                        if (log) {
                            fprintf(stdout, "stage:1, ratio:%.2f, reb:%.1e, compress_time:%.3f, %s",
                                    compress_info.ratio, relative_eb, compress_info.compress_time, buffer);
                        }
                        if (compress_info.ratio > best_ratio * 1.02) {
                            best_ratio = compress_info.ratio;
                            best_params_stage1 = params;
                            memcpy(best_param_str, buffer, 1000 * sizeof(char));
                        }
                    }
                }
            }
        }
        if (log) {
            fprintf(stdout, "Stage:1, Best Ratio %.2f, Params %s", best_ratio, best_param_str);
        }
    }


    sz_params best_params_stage2 = best_params_stage1;
    sz_compress_info best_compress_info;
    best_ratio = 0;
    {
        list<int> block_size_set = {4, 5, 6, 7, 8, 9, 10};
        list<bool> regression_set = {true};
        list<bool> poly_regression_set = {false};
        for (auto use_regression:regression_set) {
            for (auto use_poly_regression:poly_regression_set) {
                if (!use_regression && !use_poly_regression) {
                    continue;
                }
                for (auto block_size:block_size_set) {
                    list<double> reg_eb_base_set = {0.1, 1};
                    list<double> reg_eb_1_set = {block_size * 1.0};
                    list<double> poly_reg_eb_base_set = {0.1};
                    list<double> poly_reg_eb_1_set = {5};
                    list<double> poly_reg_eb_2_set = {20};
                    list<double> poly_noise_set = {0};
                    if (!use_regression) {
                        reg_eb_base_set = {0};
                        reg_eb_1_set = {0};
                    }
                    if (!use_poly_regression) {
                        poly_reg_eb_base_set = {0};
                        poly_reg_eb_1_set = {0};
                        poly_reg_eb_2_set = {0};
                        poly_noise_set = {0};
                    }
                    for (auto reg_eb_base:reg_eb_base_set) {
                        for (auto reg_eb_1:reg_eb_1_set) {
                            for (auto poly_reg_eb_base:poly_reg_eb_base_set) {
                                for (auto poly_reg_eb_1:poly_reg_eb_1_set) {
                                    for (auto poly_reg_eb_2:poly_reg_eb_2_set) {
                                        for (auto poly_reg_noise:poly_noise_set) {
                                            sz_params params(false, block_size, best_params_stage1.prediction_dim, 0,
                                                             best_params_stage1.use_lorenzo,
                                                             best_params_stage1.use_lorenzo_2layer,
                                                             use_regression, use_poly_regression, precision, reg_eb_base,
                                                             reg_eb_1,
                                                             poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2, poly_reg_noise);
                                            params.sample_ratio = sample_ratio * 1.2;
                                            params.capacity = capacity;
                                            auto compress_info = do_compress_sampling<T>(data, num_elements, r1, r2, r3,
                                                                                         precision, params);
                                            sample_num++;

                                            sprintf(buffer,
                                                    "lorenzo:%d, lorenzo2:%d, regression:%d, regression2:%d, "
                                                    "block_size:%d, pred_dim:%d, reg_base:%.1f, reg_1: %.1f, poly_base:%.1f, poly_1:%.1f, poly_2:%.1f, poly_noise %.3f, "
                                                    "lorenzo: %.0f, lorenzo2: %.0f, regression:%.0f, regression2:%.0f\n",
                                                    best_params_stage1.use_lorenzo, best_params_stage1.use_lorenzo_2layer,
                                                    use_regression, use_poly_regression,
                                                    block_size, best_params_stage1.prediction_dim, reg_eb_base, reg_eb_1,
                                                    poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2, poly_reg_noise,
                                                    compress_info.lorenzo_count * 100.0 / compress_info.block_count,
                                                    compress_info.lorenzo2_count * 100.0 / compress_info.block_count,
                                                    compress_info.regression_count * 100.0 / compress_info.block_count,
                                                    compress_info.regression2_count * 100.0 / compress_info.block_count
                                            );
                                            if (log) {
                                                fprintf(stdout, "stage:2, reb:%.1e, ratio:%.2f, compress_time:%.3f, %s",
                                                        relative_eb,
                                                        compress_info.ratio, compress_info.compress_time, buffer);
                                            }
                                            if (compress_info.ratio > best_ratio * 1.01) {
                                                best_ratio = compress_info.ratio;
                                                best_params_stage2 = params;
                                                memcpy(best_param_str, buffer, 1000 * sizeof(char));
                                                best_compress_info = compress_info;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if ((best_compress_info.lorenzo_count * 1.0 + best_compress_info.lorenzo2_count) / best_compress_info.block_count > 0.9) {
            sz_params params = best_params_stage2;
            params.use_lorenzo = best_params_stage1.use_lorenzo;
            params.use_lorenzo_2layer = best_params_stage1.use_lorenzo_2layer;
            params.prediction_dim = best_params_stage1.prediction_dim;
            auto compress_info = do_compress_sampling<T>(data, num_elements, r1, r2, r3, precision, params);
            sample_num++;

            sprintf(buffer,
                    "lorenzo:%d, lorenzo2:%d, regression:%d, regression2:%d, "
                    "block_size:%d, pred_dim:%d, reg_base:%.1f, reg_1: %.1f, poly_base:%.1f, poly_1:%.1f, poly_2:%.1f, poly_noise %.3f, "
                    "lorenzo: %.0f, lorenzo2: %.0f, regression:%.0f, regression2:%.0f\n",
                    params.use_lorenzo, params.use_lorenzo_2layer,
                    false, false,
                    params.block_size, params.prediction_dim, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    compress_info.lorenzo_count * 100.0 / compress_info.block_count,
                    compress_info.lorenzo2_count * 100.0 / compress_info.block_count,
                    compress_info.regression_count * 100.0 / compress_info.block_count,
                    compress_info.regression2_count * 100.0 / compress_info.block_count
            );
            if (log) {
                fprintf(stdout, "stage:2, reb:%.1e, ratio:%.2f, compress_time:%.3f, %s", relative_eb,
                        compress_info.ratio, compress_info.compress_time, buffer);
            }
            if (compress_info.ratio > best_ratio * 1.01) {
                best_ratio = compress_info.ratio;
                best_params_stage2 = params;
                memcpy(best_param_str, buffer, 1000 * sizeof(char));
            }
        }
        if (log) {
            fprintf(stdout, "Stage:2, Best Ratio %.2f, Params %s", best_ratio, best_param_str);
        }

    }

    sz_params best_params_stage3;
    if (relative_eb < 1.01e-6 && best_ratio > 5) {
        best_ratio = 0;
        list<int> capacity_set = {capacity, 16384};
        for (auto capacity1:capacity_set) {
            best_params_stage2.sample_ratio = sample_ratio;
            best_params_stage2.capacity = capacity1;
            auto compress_info = do_compress_sampling<T>(data, num_elements, r1, r2, r3, precision, best_params_stage2);
            sample_num++;
            if (log) {
                fprintf(stdout,
                        "stage:3, reb:%.1e, ratio:%.2f, compress_time:%.3f, capacity:%d, %s",
                        relative_eb, compress_info.ratio, compress_info.compress_time, capacity1, best_param_str);
            }
            if (compress_info.ratio > best_ratio * 1.01) {
                best_ratio = compress_info.ratio;
                best_params_stage3 = best_params_stage2;
            }
        }
    } else {
        best_params_stage3 = best_params_stage2;
        best_params_stage3.capacity = capacity;
    }


    clock_gettime(CLOCK_REALTIME, &end);
    float sample_time = (float) (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / (double) 1000000000;
    if (decompress) {
        auto compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, precision, best_params_stage3,
                                                                 true);
        fprintf(stdout,
                "FINALT: reb:%.1e, ratio %.2f, compress_time:%.3f, capacity:%d, PSNR:%.2f, NRMSE %.10e, sample_time:%.1f, sample_num:%d, %s\n",
                relative_eb, compress_info.ratio,
                compress_info.compress_time, best_params_stage3.capacity, compress_info.psnr,
                compress_info.nrmse, sample_time, sample_num,
                best_param_str);
        return nullptr;
    } else {
//        return do_compress<T>(data, num_elements, r1, r2, r3, precision, best_params_stage3, compressed_size);
        return nullptr;
    }
}

int main(int argc, char **argv) {
    size_t num_elements = 0;
    float *data = readfile<float>(argv[1], num_elements);
    float max = data[0];
    float min = data[0];
    for (int i = 1; i < num_elements; i++) {
        if (max < data[i]) max = data[i];
        if (min > data[i]) min = data[i];
    }
    int r1 = atoi(argv[2]);
    int r2 = atoi(argv[3]);
    int r3 = atoi(argv[4]);
    float eb = atof(argv[5]);

    size_t compressed_size;

    sz_params params(false, 6, 3, 0, true, true,
                     true, true, eb * (max - min));
    params.filename = argv[1];
    params.eb = eb;
//    sz_compress_info compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, eb * (max - min), params,
//                                                                         true);
//    fprintf(stdout,
//            "FINALH: reb:%.1e, ratio %.2f, compress_time:%.3f, capacity:%d, PSNR:%.2f, NRMSE %.10e\n",
//            eb, compress_info.ratio,
//            compress_info.compress_time, 0, compress_info.psnr,
//            compress_info.nrmse);

    sz_compress_autotuning_3d_no_highorder<float>(data, r1, r2, r3, eb, compressed_size, true, true, true);

    free(data);

    return 0;

}
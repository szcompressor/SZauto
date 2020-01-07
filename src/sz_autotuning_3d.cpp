#include "sz_compression_utils.hpp"
#include "sz_prediction.hpp"
#include "sz_optimize_quant_intervals.hpp"
#include "sz_compress_3d.hpp"
#include "sz_lossless.hpp"
#include "sz_decompress_3d.hpp"
#include "sz_utils.hpp"
#include "sz_def.hpp"
#include <list>

using namespace std;



template<typename T>
sz_compress_info sz_compress_decompress_highorder_3d(T *data, size_t num_elements, int r1, int r2, int r3, float precision,
                                                     sz_params params, bool use_decompress) {
    size_t result_size = 0;
    sz_compress_info compressInfo;

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    unsigned char *result = sz_compress_3d_knl_2<T>(data, r1, r2, r3, precision, result_size, params, compressInfo);
    unsigned char *result_after_lossless = NULL;
    size_t lossless_outsize = sz_lossless_compress(ZSTD_COMPRESSOR, 3, result, result_size, &result_after_lossless);
    clock_gettime(CLOCK_REALTIME, &end);
    compressInfo.compress_time = (float) (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / (double) 1000000000;
    cout << "Compression time: " << compressInfo.compress_time << "s" << endl;

    float ratio = (num_elements * sizeof(T)) * 1.0 / lossless_outsize;
    compressInfo.ori_bytes = num_elements * sizeof(T);
    compressInfo.compress_bytes = lossless_outsize;
    compressInfo.ratio = compressInfo.ori_bytes * 1.0 / compressInfo.compress_bytes;
    cout << "Compressed size = " << lossless_outsize << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!! ratio  !!!!!!!!!!!!!= " << ratio << endl;

    free(result);

    if (use_decompress) {
        clock_gettime(CLOCK_REALTIME, &start);
        size_t lossless_output = sz_lossless_decompress(ZSTD_COMPRESSOR, result_after_lossless, lossless_outsize, &result,
                                                        result_size);
        T *dec_data = sz_decompress_3d_knl<T>(result, r1, r2, r3);
        clock_gettime(CLOCK_REALTIME, &end);
        compressInfo.decompress_time =
                (double) (end.tv_sec - start.tv_sec) + (double) (end.tv_nsec - start.tv_nsec) / (double) 1000000000;
        cout << "Decompression time: " << compressInfo.decompress_time << "s" << endl;
        free(result_after_lossless);
        // writefile("dec_data.dat", dec_data, num_elements);
        verify<T>((T *) data, dec_data, num_elements, compressInfo.psnr, compressInfo.nrmse);

        free(result);
        free(dec_data);
    } else {
        free(result_after_lossless);
    }

    return compressInfo;
}

template<typename T>
unsigned char *do_compress(T *data, size_t num_elements, int r1, int r2, int r3, float precision,
                           sz_params params, size_t &compressed_size) {
    size_t sz_result_size = 0;
    sz_compress_info compressInfo;
    unsigned char *result = sz_compress_3d_knl_2<T>(data, r1, r2, r3, precision, sz_result_size, params, compressInfo);
    unsigned char *result_after_lossless = NULL;
    compressed_size = sz_lossless_compress_v2(ZSTD_COMPRESSOR, 3, result, sz_result_size, &result_after_lossless);
    free(result);
    return result_after_lossless;
}


template<typename T>
T *sz_decompress_autotuning_3d(unsigned char *compressed, size_t compress_size, int r1, int r2, int r3) {

    unsigned char *decompressed_lossless;
    size_t lossless_output = sz_lossless_decompress_v2(ZSTD_COMPRESSOR, compressed, compress_size, &decompressed_lossless);
    T *dec_data = sz_decompress_3d_knl<T>(decompressed_lossless, r1, r2, r3);
    free(decompressed_lossless);
    return dec_data;
}


template<typename T>
sz_compress_info do_compress_sampling(const float *data, size_t num_elements, int r1, int r2, int r3, float precision,
                                      sz_params params) {
    size_t result_size = 0;
    sz_compress_info compressInfo;

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    unsigned char *result = sz_compress_3d_sampling<T>(data, r1, r2, r3, precision, result_size, params, compressInfo);
    if (params.lossless) {
        unsigned char *result_after_lossless = NULL;
        size_t lossless_outsize = sz_lossless_compress(ZSTD_COMPRESSOR, 3, result, result_size, &result_after_lossless);
        compressInfo.compress_bytes = lossless_outsize;
        free(result_after_lossless);
    } else {
        compressInfo.compress_bytes = result_size;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    compressInfo.compress_time = (float) (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / (double) 1000000000;
//    cout << "Compression time: " << std::setw(10) << std::setprecision(6) << compressInfo.compress_time << "s" << endl;

    compressInfo.ratio = compressInfo.ori_bytes * 1.0 / compressInfo.compress_bytes;
//    cout << "Compressed size = " << compressInfo.compress_bytes << endl;
//    cout << "!!!!!!!!!!!!!!!!!!!!! ratio  !!!!!!!!!!!!!= " << compressInfo.ratio << endl;

    free(result);

    return compressInfo;
}

template<typename T>
unsigned char *
sz_compress_autotuning_3d(T *data, size_t r1, size_t r2, size_t r3, double relative_eb, size_t &compressed_size,
                          bool baseline = false, bool decompress = false, bool log = false, float sample_ratio = 0.05) {
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
            for (auto use_lorenzo_2layer:{true, false}) {
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
        list<int> block_size_set = {5, 6, 7, 8};
        list<bool> regression_set = {true};
        list<bool> poly_regression_set = {false, true};
        for (auto use_regression:regression_set) {
            for (auto use_poly_regression:poly_regression_set) {
                if (!use_regression && !use_poly_regression) {
                    continue;
                }
                for (auto block_size:block_size_set) {
                    list<double> reg_eb_base_set = {1};
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
                "FINAL: reb:%.1e, ratio %.2f, compress_time:%.3f, capacity:%d, PSNR:%.2f, NRMSE %.10e, sample_time:%.1f, sample_num:%d, %s\n",
                relative_eb, compress_info.ratio, compress_info.compress_time, best_params_stage3.capacity, compress_info.psnr,
                compress_info.nrmse, sample_time, sample_num,
                best_param_str);
        return nullptr;
    } else {
        return do_compress<T>(data, num_elements, r1, r2, r3, precision, best_params_stage3, compressed_size);
    }


}

//template<typename T>
//void sz_compress_manualtuning_3d(T *data, size_t num_elements, int r1, int r2, int r3, float eb, float precision) {
//
//    float best_ratio = 0;
//    char best_param_str[1000];
//    char buffer[1000];
//
//    sz_params best_params_stage1;
//    for (auto use_lorenzo:{true}) {
//        for (auto use_lorenzo_2layer:{false, true}) {
//            if (use_lorenzo || use_lorenzo_2layer) {
//                auto pred_dim_set = {2, 3};
//                for (auto pred_dim: pred_dim_set) {
//                    sz_params params(false, 6, pred_dim, 0, use_lorenzo,
//                                     use_lorenzo_2layer, false, false, precision);
//                    auto compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, precision, params,
//                                                                             false);
//
//                    sprintf(buffer,
//                            "stage:1, ratio: %.1f, reb:%.1e, compress_time:%.1f, PSNR= %.1f, lorenzo:%d, lorenzo2:%d, pred_dim:%d\n",
//                            compress_info.ratio, eb, compress_info.compress_time, compress_info.psnr, use_lorenzo,
//                            use_lorenzo_2layer, pred_dim);
//                    fprintf(stderr, "%s", buffer);
//                    if (compress_info.ratio > best_ratio) {
//                        best_ratio = compress_info.ratio;
//                        best_params_stage1 = params;
//                        memcpy(best_param_str, buffer, 1000 * sizeof(char));
//                    }
//                }
//            }
//        }
//    }
//    fprintf(stderr, "Stage:1, Best Ratio %.2f, Params %s", best_ratio, best_param_str);
//
//    best_ratio = 0;
//    sz_params best_params_stage2;
//    for (auto use_regression:{false, true}) {
//        for (auto use_poly_regression:{false, true}) {
//            auto block_size_set = {5, 6, 7, 8, 9, 10};
//            for (auto block_size:block_size_set) {
////                            list<double> reg_eb_base_set = {1};
////                            list<double> reg_eb_1_set = {block_size * 1.0};
////                            list<double> poly_reg_eb_base_set = {0.1};
////                            list<double> poly_reg_eb_1_set = {5};
////                            list<double> poly_reg_eb_2_set = {20};
//                list<double> reg_eb_base_set = {0.1, 1, 2};
//                list<double> reg_eb_1_set = {1, block_size * 1.0};
//                list<double> poly_reg_eb_base_set = {0.1, 1};
//                list<double> poly_reg_eb_1_set = {0, 1, 5, 10};
//                list<double> poly_reg_eb_2_set = {0, 20};
//                list<double> poly_noise_set = {0, 0.01, 0.1, 1, 10, 100};
//                if (!use_regression) {
//                    reg_eb_base_set = {0};
//                    reg_eb_1_set = {0};
//                }
//                if (!use_poly_regression) {
//                    poly_reg_eb_base_set = {0};
//                    poly_reg_eb_1_set = {0};
//                    poly_reg_eb_2_set = {0};
//                    poly_noise_set = {0};
//                }
//                for (auto reg_eb_base:reg_eb_base_set) {
//                    for (auto reg_eb_1:reg_eb_1_set) {
//                        for (auto poly_reg_eb_base:poly_reg_eb_base_set) {
//                            for (auto poly_reg_eb_1:poly_reg_eb_1_set) {
//                                for (auto poly_reg_eb_2:poly_reg_eb_2_set) {
//                                    for (auto poly_reg_noise:poly_noise_set) {
//                                        sz_params params(false, block_size, best_params_stage1.prediction_dim, 0,
//                                                         best_params_stage1.use_lorenzo, best_params_stage1.use_lorenzo_2layer,
//                                                         use_regression, use_poly_regression, precision, reg_eb_base,
//                                                         reg_eb_1,
//                                                         poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2,
//                                                         poly_reg_noise);
//
//                                        auto compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3,
//                                                                                                 precision,
//                                                                                                 params, false);
//                                        sprintf(buffer,
//                                                "lorenzo:%d, lorenzo2:%d, regression:%d, regression2:%d, "
//                                                "block_size:%d, pred_dim:%d, reg_base:%.1f, reg_1: %.1f, poly_base:%.1f, poly_1:%.1f, poly_2:%.1f, poly_noise:%.3f, "
//                                                "lorenzo: %.0f, lorenzo2: %.0f, regression:%.0f, regression2:%.0f",
//                                                best_params_stage1.use_lorenzo, best_params_stage1.use_lorenzo_2layer,
//                                                use_regression, use_poly_regression,
//                                                block_size, best_params_stage1.prediction_dim, reg_eb_base, reg_eb_1,
//                                                poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2, poly_reg_noise,
//                                                compress_info.lorenzo_count * 100.0 / compress_info.block_count,
//                                                compress_info.lorenzo2_count * 100.0 / compress_info.block_count,
//                                                compress_info.regression_count * 100.0 / compress_info.block_count,
//                                                compress_info.regression2_count * 100.0 / compress_info.block_count
//                                        );
//                                        fprintf(stderr,
//                                                "stage:2, reb:%.1e, ratio: %.1f, compress_time:%.1f, PSNR: %.1f, %s\n",
//                                                eb,
//                                                compress_info.ratio, compress_info.compress_time, compress_info.psnr,
//                                                buffer);
//                                        if (compress_info.ratio > best_ratio) {
//                                            best_ratio = compress_info.ratio;
//                                            best_params_stage2 = params;
//                                            memcpy(best_param_str, buffer, 1000 * sizeof(char));
//                                        }
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    best_ratio = 0;
//    list<int> capacity_set = {0, 65536, 32768, 16384, 8192, 4096};
//    sz_params best_params_stage3;
//    for (auto capacity:capacity_set) {
//        best_params_stage2.capacity = capacity;
//        auto compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, precision, best_params_stage2,
//                                                                 true);
//        fprintf(stderr,
//                "stage:3, reb:%.1e, ratio %.2f, compress_time:%.1f, capacity:%d, PSNR %.2f, NRMSE %.10e Ori_bytes %ld, Compressed_bytes %ld, %s\n",
//                eb, compress_info.ratio, compress_info.compress_time, capacity, compress_info.psnr, compress_info.nrmse,
//                compress_info.ori_bytes,
//                compress_info.compress_bytes,
//                best_param_str);
//        if (compress_info.ratio > best_ratio) {
//            best_ratio = compress_info.ratio;
//            best_params_stage3 = best_params_stage2;
//        }
//    }
//    auto compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, precision, best_params_stage3, true);
//    fprintf(stderr,
//            "FINAL: reb:%.1e, ratio %.2f, compress_time:%.1f, capacity:%d, PSNR %.2f, NRMSE %.10e Ori_bytes %ld, Compressed_bytes %ld, %s\n",
//            eb, compress_info.ratio, compress_info.compress_time, best_params_stage3.capacity, compress_info.psnr,
//            compress_info.nrmse,
//            compress_info.ori_bytes,
//            compress_info.compress_bytes,
//            best_param_str);
//
//
//    sz_params baseline_param(false, 6, 3, 0, true, false, true, false, precision);
//    auto baseline_compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, precision, baseline_param,
//                                                                      true);
//    fprintf(stderr,
//            "Baseline: reb:%.1e, ratio %.2f, compress_time:%.1f, PSNR %.2f, NRMSE %.10e Ori_bytes %ld, Compressed_bytes %ld\n",
//            eb, baseline_compress_info.ratio, baseline_compress_info.compress_time, baseline_compress_info.psnr,
//            baseline_compress_info.nrmse, baseline_compress_info.ori_bytes,
//            baseline_compress_info.compress_bytes);
//
//}

template<typename T>
void sz_compress_manualtuning_3d(T *data, size_t num_elements, int r1, int r2, int r3, float eb, float precision) {

    float best_ratio = 0;
    char best_param_str[1000];
    char buffer[1000];

    for (auto use_lorenzo:{false, true}) {
        for (auto use_lorenzo_2layer:{false, true}) {
            if (use_lorenzo || use_lorenzo_2layer) {
                auto pred_dim_set = {1, 2, 3};
                for (auto pred_dim: pred_dim_set) {
                    sz_params params(false, 6, pred_dim, 0, use_lorenzo,
                                     use_lorenzo_2layer, false, false, precision);
                    auto compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, precision, params,
                                                                             false);

                    sprintf(buffer,
                            "stage:0, ratio: %.1f, reb:%.1e, compress_time:%.1f, PSNR= %.1f, lorenzo:%d, lorenzo2:%d, pred_dim:%d\n",
                            compress_info.ratio, eb, compress_info.compress_time, compress_info.psnr, use_lorenzo,
                            use_lorenzo_2layer, pred_dim);
                    fprintf(stderr, "%s", buffer);
                    if (compress_info.ratio > best_ratio) {
                        best_ratio = compress_info.ratio;
                        memcpy(best_param_str, buffer, 1000 * sizeof(char));
                    }
                }
            }
        }
    }
    fprintf(stderr, "Stage:0, Best Ratio %.2f, Params %s", best_ratio, best_param_str);

    best_ratio = 0;
    sz_params best_params_stage1;
    for (auto use_lorenzo:{false, true}) {
        for (auto use_lorenzo_2layer:{false, true}) {
            auto pred_dim_set = {3};
            if (use_lorenzo || use_lorenzo_2layer) {
                pred_dim_set = {2, 3};
            }
            for (auto pred_dim: pred_dim_set) {
                for (auto use_regression:{false, true}) {
                    for (auto use_poly_regression:{false, true}) {
                        auto block_size_set = {4, 5, 6, 7, 8, 9, 10, 11, 12};
                        for (auto block_size:block_size_set) {
                            list<double> reg_eb_base_set = {1};
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
                                                    sz_params params(false, block_size, pred_dim, 0,
                                                                     use_lorenzo, use_lorenzo_2layer,
                                                                     use_regression, use_poly_regression, precision, reg_eb_base,
                                                                     reg_eb_1,
                                                                     poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2,
                                                                     poly_reg_noise);

                                                    auto compress_info = sz_compress_decompress_highorder_3d(data, num_elements,
                                                                                                             r1, r2, r3,
                                                                                                             precision,
                                                                                                             params, false);
                                                    sprintf(buffer,
                                                            "lorenzo:%d, lorenzo2:%d, regression:%d, regression2:%d, "
                                                            "block_size:%d, pred_dim:%d, reg_base:%.1f, reg_1: %.1f, poly_base:%.1f, poly_1:%.1f, poly_2:%.1f, poly_noise:%.3f, "
                                                            "lorenzo: %.0f, lorenzo2: %.0f, regression:%.0f, regression2:%.0f",
                                                            use_lorenzo, use_lorenzo_2layer,
                                                            use_regression, use_poly_regression,
                                                            block_size, pred_dim, reg_eb_base, reg_eb_1,
                                                            poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2, poly_reg_noise,
                                                            compress_info.lorenzo_count * 100.0 / compress_info.block_count,
                                                            compress_info.lorenzo2_count * 100.0 / compress_info.block_count,
                                                            compress_info.regression_count * 100.0 / compress_info.block_count,
                                                            compress_info.regression2_count * 100.0 / compress_info.block_count
                                                    );
                                                    fprintf(stderr,
                                                            "stage:1, reb:%.1e, ratio: %.1f, compress_time:%.1f, %s\n",
                                                            eb,
                                                            compress_info.ratio, compress_info.compress_time, buffer);
                                                    if (compress_info.ratio > best_ratio) {
                                                        best_ratio = compress_info.ratio;
                                                        best_params_stage1 = params;
                                                        memcpy(best_param_str, buffer, 1000 * sizeof(char));
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
            }
        }
    }
    fprintf(stderr, "Stage:1, Best Ratio %.2f, Params %s", best_ratio, best_param_str);

    best_ratio = 0;
    sz_params best_params_stage2_pre;
    for (auto use_regression:{true, false}) {
        for (auto use_poly_regression:{best_params_stage1.use_poly_regression}) {
            auto block_size_set = {4, 5, 6, 7, 8, 9, 10, 11, 12};
            for (auto block_size:block_size_set) {
                list<double> reg_eb_base_set = {0.01, 0.1, 1, 5, 10, 100};
                list<double> reg_eb_1_set = {0.01, 0.1, 0, 1, block_size * 1.0, 10, 50, 100};
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
                                                         best_params_stage1.use_lorenzo, best_params_stage1.use_lorenzo_2layer,
                                                         use_regression, use_poly_regression, precision, reg_eb_base,
                                                         reg_eb_1,
                                                         poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2,
                                                         poly_reg_noise);

                                        auto compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3,
                                                                                                 precision,
                                                                                                 params, false);
                                        sprintf(buffer,
                                                "lorenzo:%d, lorenzo2:%d, regression:%d, regression2:%d, "
                                                "block_size:%d, pred_dim:%d, reg_base:%.1f, reg_1: %.1f, poly_base:%.1f, poly_1:%.1f, poly_2:%.1f, poly_noise:%.3f, "
                                                "lorenzo: %.0f, lorenzo2: %.0f, regression:%.0f, regression2:%.0f",
                                                best_params_stage1.use_lorenzo, best_params_stage1.use_lorenzo_2layer,
                                                use_regression, use_poly_regression,
                                                block_size, best_params_stage1.prediction_dim, reg_eb_base,
                                                (reg_eb_1 == block_size ? -1 : reg_eb_1),
                                                poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2, poly_reg_noise,
                                                compress_info.lorenzo_count * 100.0 / compress_info.block_count,
                                                compress_info.lorenzo2_count * 100.0 / compress_info.block_count,
                                                compress_info.regression_count * 100.0 / compress_info.block_count,
                                                compress_info.regression2_count * 100.0 / compress_info.block_count
                                        );
                                        fprintf(stderr,
                                                "stage:2, reb:%.1e, ratio: %.1f, compress_time:%.1f, PSNR: %.1f, %s\n",
                                                eb,
                                                compress_info.ratio, compress_info.compress_time, compress_info.psnr,
                                                buffer);
                                        if (compress_info.ratio > best_ratio) {
                                            best_ratio = compress_info.ratio;
                                            best_params_stage2_pre = params;
                                            memcpy(best_param_str, buffer, 1000 * sizeof(char));
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

    best_ratio = 0;
    sz_params best_params_stage2;
    for (auto use_regression:{best_params_stage2_pre.use_regression_linear}) {
        for (auto use_poly_regression:{true, false}) {
            list<int> block_size_set;
            if (!use_poly_regression) {
                block_size_set = {best_params_stage2_pre.block_size};
            } else {
                block_size_set = {best_params_stage2_pre.block_size, best_params_stage2_pre.block_size + 1,
                                  best_params_stage2_pre.block_size - 1};
            }
            for (auto block_size:block_size_set) {
                list<double> reg_eb_base_set = {best_params_stage2_pre.reg_eb_base};
                list<double> reg_eb_1_set = {best_params_stage2_pre.reg_eb_1};
                list<double> poly_reg_eb_base_set = {0.01, 0.1, 1, 2, 10};
                list<double> poly_reg_eb_1_set = {0, 0.1, 1, 5, 10};
                list<double> poly_reg_eb_2_set = {0, 0.1, 1, 20, 100};
                list<double> poly_noise_set = {0, 0.01, 0.1, 1, 10, 100};
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
                                                         best_params_stage1.use_lorenzo, best_params_stage1.use_lorenzo_2layer,
                                                         use_regression, use_poly_regression, precision, reg_eb_base,
                                                         reg_eb_1,
                                                         poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2,
                                                         poly_reg_noise);

                                        auto compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3,
                                                                                                 precision,
                                                                                                 params, false);
                                        sprintf(buffer,
                                                "lorenzo:%d, lorenzo2:%d, regression:%d, regression2:%d, "
                                                "block_size:%d, pred_dim:%d, reg_base:%.1f, reg_1: %.1f, poly_base:%.1f, poly_1:%.1f, poly_2:%.1f, poly_noise:%.3f, "
                                                "lorenzo: %.0f, lorenzo2: %.0f, regression:%.0f, regression2:%.0f",
                                                best_params_stage1.use_lorenzo, best_params_stage1.use_lorenzo_2layer,
                                                use_regression, use_poly_regression,
                                                block_size, best_params_stage1.prediction_dim, reg_eb_base,
                                                (reg_eb_1 == block_size ? -1 : reg_eb_1),
                                                poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2, poly_reg_noise,
                                                compress_info.lorenzo_count * 100.0 / compress_info.block_count,
                                                compress_info.lorenzo2_count * 100.0 / compress_info.block_count,
                                                compress_info.regression_count * 100.0 / compress_info.block_count,
                                                compress_info.regression2_count * 100.0 / compress_info.block_count
                                        );
                                        fprintf(stderr,
                                                "stage:2, reb:%.1e, ratio: %.1f, compress_time:%.1f, PSNR: %.1f, %s\n",
                                                eb,
                                                compress_info.ratio, compress_info.compress_time, compress_info.psnr,
                                                buffer);
                                        if (compress_info.ratio > best_ratio) {
                                            best_ratio = compress_info.ratio;
                                            best_params_stage2 = params;
                                            memcpy(best_param_str, buffer, 1000 * sizeof(char));
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

    best_ratio = 0;
    list<int> capacity_set = {0, 65536, 32768, 16384, 8192, 4096};
    sz_params best_params_stage3;
    for (auto capacity:capacity_set) {
        best_params_stage2.capacity = capacity;
        auto compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, precision, best_params_stage2,
                                                                 true);
        fprintf(stderr,
                "stage:3, reb:%.1e, ratio %.2f, compress_time:%.1f, capacity:%d, PSNR %.2f, NRMSE %.10e Ori_bytes %ld, Compressed_bytes %ld, %s\n",
                eb, compress_info.ratio, compress_info.compress_time, capacity, compress_info.psnr, compress_info.nrmse,
                compress_info.ori_bytes,
                compress_info.compress_bytes,
                best_param_str);
        if (compress_info.ratio > best_ratio) {
            best_ratio = compress_info.ratio;
            best_params_stage3 = best_params_stage2;
        }
    }
    auto compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, precision, best_params_stage3, true);
    fprintf(stderr,
            "FINAL: reb:%.1e, ratio %.2f, compress_time:%.1f, capacity:%d, PSNR %.2f, NRMSE %.10e Ori_bytes %ld, Compressed_bytes %ld, %s\n",
            eb, compress_info.ratio, compress_info.compress_time, best_params_stage3.capacity, compress_info.psnr,
            compress_info.nrmse,
            compress_info.ori_bytes,
            compress_info.compress_bytes,
            best_param_str);


    sz_params baseline_param(false, 6, 3, 0, true, false, true, false, precision);
    auto baseline_compress_info = sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, precision, baseline_param,
                                                                      true);
    fprintf(stderr,
            "Baseline: reb:%.1e, ratio %.2f, compress_time:%.1f, PSNR %.2f, NRMSE %.10e Ori_bytes %ld, Compressed_bytes %ld\n",
            eb, baseline_compress_info.ratio, baseline_compress_info.compress_time, baseline_compress_info.psnr,
            baseline_compress_info.nrmse, baseline_compress_info.ori_bytes,
            baseline_compress_info.compress_bytes);

}


template
unsigned char *
sz_compress_autotuning_3d(float *data, size_t r1, size_t r2, size_t r3, double relative_eb, size_t &compressed_size,
                          bool baseline = false, bool decompress = false, bool log = false, float sample_ratio = 0.05);

template
float *
sz_decompress_autotuning_3d(unsigned char *compressed, size_t compress_size, int r1, int r2, int r3);

template
void
sz_compress_manualtuning_3d(float *data, size_t num_elements, int r1, int r2, int r3, float eb, float precision);

template
sz_compress_info
sz_compress_decompress_highorder_3d(float *data, size_t num_elements, int r1, int r2, int r3, float precision,
                                    sz_params params, bool use_decompress);
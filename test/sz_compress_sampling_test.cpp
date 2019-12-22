#include "sz_compress_util.h"
#include <list>

using namespace std;

float test_top_candidates_param_compress(float *data, size_t num_elements, int r1, int r2, int r3, float eb, float precision) {

    float best_ratio = 0;
    char best_param_str[1000];
    char buffer[1000];

    sz_params best_params_stage1;
    for (auto use_lorenzo:{true}) {
        for (auto use_lorenzo_2layer:{false, true}) {
            if (use_lorenzo || use_lorenzo_2layer) {
                auto pred_dim_set = {2, 3};
                for (auto pred_dim: pred_dim_set) {
                    sz_params params(false, 6, pred_dim, 0, use_lorenzo,
                                     use_lorenzo_2layer, false, false, precision);
                    auto compress_info = compress(data, num_elements, r1, r2, r3, precision, params, false);

                    sprintf(buffer,
                            "stage:1, ratio: %.1f, reb:%.1e, compress_time:%.1f, PSNR= %.1f, lorenzo:%d, lorenzo2:%d, pred_dim:%d\n",
                            compress_info.ratio, eb, compress_info.compress_time, compress_info.psnr, use_lorenzo,
                            use_lorenzo_2layer, pred_dim);
                    fprintf(stderr, "%s", buffer);
                    if (compress_info.ratio > best_ratio) {
                        best_ratio = compress_info.ratio;
                        best_params_stage1 = params;
                        memcpy(best_param_str, buffer, 1000 * sizeof(char));
                    }
                }
            }
        }
    }
    fprintf(stderr, "Stage:1, Best Ratio %.2f, Params %s", best_ratio, best_param_str);

    best_ratio = 0;
    sz_params best_params_stage2;
    for (auto use_regression:{false, true}) {
        for (auto use_poly_regression:{false, true}) {
            auto block_size_set = {5, 6, 7, 8, 9};
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
                                                         use_regression, use_poly_regression, precision, reg_eb_base, reg_eb_1,
                                                         poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2, poly_reg_noise);
                                        auto compress_info = compress(data, num_elements, r1, r2, r3, precision, params, false);
                                        sprintf(buffer,
                                                "lorenzo:%d, lorenzo2:%d, regression:%d, regression2:%d, "
                                                "block_size:%d, pred_dim:%d, reg_base:%.1f, reg_1: %.1f, poly_base:%.1f, poly_1:%.1f, poly_2:%.1f, poly_noise %.3f, "
                                                "lorenzo: %.0f, lorenzo2: %.0f, regression:%.0f, regression2:%.0f,",
                                                best_params_stage1.use_lorenzo, best_params_stage1.use_lorenzo_2layer,
                                                use_regression, use_poly_regression,
                                                block_size, best_params_stage1.prediction_dim, reg_eb_base, reg_eb_1,
                                                poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2, poly_reg_noise,
                                                compress_info.lorenzo_count * 100.0 / compress_info.block_count,
                                                compress_info.lorenzo2_count * 100.0 / compress_info.block_count,
                                                compress_info.regression_count * 100.0 / compress_info.block_count,
                                                compress_info.regression2_count * 100.0 / compress_info.block_count
                                        );
                                        fprintf(stderr, "stage:2, reb:%.1e, ratio: %.1f, compress_time:%.1f, %s\n", eb,
                                                compress_info.ratio, compress_info.compress_time, buffer);
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
    auto compress_info = compress(data, num_elements, r1, r2, r3, precision, best_params_stage2, true);
    fprintf(stderr,
            "FINAL: reb:%.1e, ratio %.2f, compress_time:%.1f, PSNR %.2f, NRMSE %.10f Ori_bytes %ld, Compressed_bytes %ld, %s\n",
            eb, compress_info.ratio, compress_info.compress_time, compress_info.psnr, compress_info.nrmse,
            compress_info.ori_bytes,
            compress_info.compress_bytes,
            best_param_str);


    sz_params baseline_param(false, 6, 3, 0, true, false, true, false, precision);
    auto baseline_compress_info = compress(data, num_elements, r1, r2, r3, precision, baseline_param, true);
    fprintf(stderr,
            "Baseline: reb:%.1e, ratio %.2f, compress_time:%.1f, PSNR %.2f, NRMSE %.10f Ori_bytes %ld, Compressed_bytes %ld\n",
            eb, baseline_compress_info.ratio, baseline_compress_info.compress_time, baseline_compress_info.psnr,
            baseline_compress_info.nrmse, baseline_compress_info.ori_bytes,
            baseline_compress_info.compress_bytes);

    return best_ratio;
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

    if (argc <= 6) {
        test_top_candidates_param_compress(data, num_elements, r1, r2, r3, eb, eb * (max - min));
        free(data);
        return 0;
    }
    free(data);
    return 0;

}
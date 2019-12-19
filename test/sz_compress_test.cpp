#include "sz_compress_3d.hpp"
#include "sz_decompress_3d.hpp"
#include "sz_lossless.hpp"
#include "sz_utils.hpp"
#include <list>

using namespace std;

float compress(float *data, size_t num_elements, int r1, int r2, int r3, float precision, float &psnr, float &compress_time,
               int block_size, int pred_dim,
               bool lorenzo, bool lorenzo_2layer,
               bool regression_linear, bool regression_poly,
               float reg_eb_base, float reg_eb_1,
               float poly_reg_eb_base, float poly_reg_eb_1, float poly_reg_eb_2,
               float poly_regression_noise, bool use_decompress = true) {

    cout << "precision = " << precision << endl;
    cout << "Options: block_size = " << block_size << ", pred_dim = " << pred_dim
         << ", lorenzo = " << lorenzo
         << ", lorenzo2 = " << lorenzo_2layer
         << ", regression = " << regression_linear
         << ", regression2 = " << regression_poly
         << endl
         << "  reg_eb_base=" << reg_eb_base
         << ", reg_eb_1=" << reg_eb_1
         << ", poly_reg_eb_base=" << poly_reg_eb_base
         << ", poly_reg_eb_1=" << poly_reg_eb_1
         << ", poly_reg_eb_2=" << poly_reg_eb_2
         << endl;

    size_t result_size = 0;

    struct timespec start, end;
    int err = 0;
    err = clock_gettime(CLOCK_REALTIME, &start);
    sz_params params(false, block_size, pred_dim, 0, lorenzo, lorenzo_2layer,
                     regression_linear, regression_poly, precision, poly_regression_noise);


    params.regression_param_eb_linear = reg_eb_base * precision / RegCoeffNum3d / (float) block_size;
    params.regression_param_eb_independent = reg_eb_1 * params.regression_param_eb_linear;

    params.poly_regression_param_eb_poly = poly_reg_eb_base * reg_eb_base * precision / RegPolyCoeffNum3d / (float) block_size;
    params.poly_regression_param_eb_linear = poly_reg_eb_1 * params.poly_regression_param_eb_poly;
    params.poly_regression_param_eb_independent = poly_reg_eb_2 * params.poly_regression_param_eb_poly;


    unsigned char *result = sz_compress_3d_knl<float>(data, r1, r2, r3, precision, result_size, params);
    unsigned char *result_after_lossless = NULL;
    size_t lossless_outsize = sz_lossless_compress(ZSTD_COMPRESSOR, 3, result, result_size, &result_after_lossless);
    err = clock_gettime(CLOCK_REALTIME, &end);
    compress_time = (float) (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / (double) 1000000000;
    cout << "Compression time: " << compress_time << "s" << endl;
    float ratio = (num_elements * sizeof(float)) * 1.0 / lossless_outsize;
    cout << "Compressed size = " << lossless_outsize << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!! ratio  !!!!!!!!!!!!!= " << ratio << endl;

    free(result);

    if (use_decompress) {
        err = clock_gettime(CLOCK_REALTIME, &start);
        size_t lossless_output = sz_lossless_decompress(ZSTD_COMPRESSOR, result_after_lossless, lossless_outsize, &result,
                                                        result_size);
        float *dec_data = sz_decompress_3d_knl<float>(result, r1, r2, r3);
        err = clock_gettime(CLOCK_REALTIME, &end);
        cout << "Decompression time: "
             << (double) (end.tv_sec - start.tv_sec) + (double) (end.tv_nsec - start.tv_nsec) / (double) 1000000000 << "s"
             << endl;
        free(result_after_lossless);
        // writefile("dec_data.dat", dec_data, num_elements);
        verify(data, dec_data, num_elements);
        free(result);
        free(dec_data);
    }

    return ratio;
}

float compress(float *data, size_t num_elements, int r1, int r2, int r3, float precision, float &psnr, float &compress_time,
               int block_size, int pred_dim,
               bool lorenzo, bool lorenzo_2layer,
               bool regression_linear, bool regression_poly) {

    return compress(data, num_elements, r1, r2, r3, precision, psnr, compress_time, block_size, pred_dim, lorenzo, lorenzo_2layer,
                    regression_linear, regression_poly,
                    RegErrThreshold, block_size,
                    RegErrThreshold, block_size, block_size * RegPolyCoeffNum3d / 2.5, 0);
}

float test_all_param_compress(float *data, size_t num_elements, int r1, int r2, int r3, float eb, float precision) {

    float best_ratio = 0;
    char best_param[1000];
    char buffer[1000];

    bool best_use_lorenzo = true, best_use_lorenzo_2layer = false;
    int best_pred_dim = 3;

    for (auto use_lorenzo:{false, true}) {
        for (auto use_lorenzo_2layer:{false, true}) {
            if (use_lorenzo || use_lorenzo_2layer) {
                auto pred_dim_set = {1, 2, 3};
                for (auto pred_dim: pred_dim_set) {
                    float psnr, compressed_time;
                    auto ratio = compress(data, num_elements, r1, r2, r3, precision, psnr,
                                          compressed_time,
                                          6, pred_dim,
                                          use_lorenzo, use_lorenzo_2layer, false, false);
                    sprintf(buffer,
                            "stage:1, ratio: %.1f, reb:%.1e, compress_time:%.1f, PSNR= %.1f, lorenzo:%d, lorenzo2:%d, pred_dim:%d\n",
                            ratio, eb, compressed_time, psnr, use_lorenzo, use_lorenzo_2layer, pred_dim);
                    fprintf(stderr, "%s", buffer);
                    if (ratio > best_ratio) {
                        best_ratio = ratio;
                        best_use_lorenzo = use_lorenzo;
                        best_use_lorenzo_2layer = use_lorenzo_2layer;
                        best_pred_dim = pred_dim;
                        memcpy(best_param, buffer, 1000 * sizeof(char));
                    }
                }
            }
        }
    }
    fprintf(stderr, "Stage:1, Best Ratio %.2f, Params %s", best_ratio, best_param);

    best_ratio = 0;
    for (auto use_regression:{false, true}) {
        for (auto use_poly_regression:{false, true}) {
            auto block_size_set = {4, 5, 6, 7, 8};
            for (auto block_size:block_size_set) {
                list<double> reg_eb_base_set = {0.1, 1, 10};
                list<double> reg_eb_1_set = {1, 5, 10};
                list<double> poly_reg_eb_base_set = {0.1, 1, 10};
                list<double> poly_reg_eb_1_set = {1, 5, 10};
                list<double> poly_reg_eb_2_set = {1, 10, 24, 50};
                list<double> poly_noise_set = {0, 1};
                if (!use_regression) {
                    reg_eb_base_set = {0.1};
                    reg_eb_1_set = {7};
                }
                if (!use_poly_regression) {
                    poly_reg_eb_base_set = {0.1};
                    poly_reg_eb_1_set = {8};
                    poly_reg_eb_2_set = {24};
                    poly_noise_set = {0};
                }
                for (auto reg_eb_base:reg_eb_base_set) {
                    for (auto reg_eb_1:reg_eb_1_set) {
                        for (auto poly_reg_eb_base:poly_reg_eb_base_set) {
                            for (auto poly_reg_eb_1:poly_reg_eb_1_set) {
                                for (auto poly_reg_eb_2:poly_reg_eb_2_set) {
                                    for (auto poly_reg_noise:poly_noise_set) {
                                        float psnr, compressed_time;
                                        auto ratio = compress(data, num_elements, r1, r2, r3, precision, psnr,
                                                              compressed_time,
                                                              block_size, best_pred_dim,
                                                              best_use_lorenzo, best_use_lorenzo_2layer,
                                                              use_regression, use_poly_regression,
                                                              reg_eb_base, reg_eb_1,
                                                              poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2,
                                                              poly_reg_noise, false);
                                        sprintf(buffer,
                                                "stage:2, ratio: %.1f, reb:%.1e, compress_time:%.1f, PSNR= %.1f, lorenzo:%d, lorenzo2:%d, regression:%d, regression2:%d, "
                                                "block_size:%d, pred_dim:%d, reg_base:%.1f, reg_1: %.1f, poly_base:%.1f, poly_1:%.1f, poly_2:%.1f, poly_noise %.8f\n",
                                                ratio, eb, compressed_time, psnr, best_use_lorenzo, best_use_lorenzo_2layer,
                                                use_regression,
                                                use_poly_regression,
                                                block_size, best_pred_dim, reg_eb_base, reg_eb_1, poly_reg_eb_base,
                                                poly_reg_eb_1, poly_reg_eb_2, poly_reg_noise
                                        );
                                        fprintf(stderr, "%s", buffer);
                                        if (ratio > best_ratio) {
                                            best_ratio = ratio;
                                            memcpy(best_param, buffer, 1000 * sizeof(char));
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
    fprintf(stderr, "Stage:2, Best Ratio %.2f, Params %s", best_ratio, best_param);
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

    float psnr, compress_time;

    if (argc <= 6) {
        test_all_param_compress(data, num_elements, r1, r2, r3, eb, eb * (max - min));
        return 0;
    }

    int block_size = atoi(argv[6]);
    int stride = atoi(argv[7]);
    int pred_dim = atoi(argv[8]);
    int lorenzo_op = atoi(argv[9]);
    int regression_op = atoi(argv[10]);

    bool lorenzo = lorenzo_op == 1 || lorenzo_op == 3;
    bool lorenzo_2layer = lorenzo_op == 2 || lorenzo_op == 3;
    bool regression_linear = regression_op == 1 || regression_op == 3;
    bool regression_poly = regression_op == 2 || regression_op == 3;

    if (argc <= 11) {
        compress(data, num_elements, r1, r2, r3, eb * (max - min), psnr, compress_time, block_size, pred_dim, lorenzo,
                 lorenzo_2layer,
                 regression_linear,
                 regression_poly);
        return 0;
    }
    float reg_eb_base = atof(argv[11]);
    float reg_eb_1 = atof(argv[12]);
    float poly_reg_eb_base = atof(argv[13]);
    float poly_reg_eb_1 = atof(argv[14]);
    float poly_reg_eb_2 = atof(argv[15]);
    float poly_reg_noise = atof(argv[16]);
    compress(data, num_elements, r1, r2, r3, eb * (max - min), psnr, compress_time, block_size, pred_dim, lorenzo,
             lorenzo_2layer,
             regression_linear, regression_poly,
             reg_eb_base, reg_eb_1,
             poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2, poly_reg_noise);

    free(data);

    return 0;

}
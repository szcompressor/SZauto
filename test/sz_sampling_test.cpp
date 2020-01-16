#include "sz_compression_utils.hpp"
#include "sz_prediction.hpp"
#include "sz_optimize_quant_intervals.hpp"
#include "sz_autotuning_3d.hpp"
#include "sz_utils.hpp"

using namespace std;

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
    double precision = eb * (max - min);
    int capacity = 65536 * 2;
    optimize_quant_invl_3d(data, r1, r2, r3, precision, capacity);

    if (argc <= 6) {
        sz_params baseline_param(false, 6, 3, 0, true, false, true, false, precision);
        auto baseline_compress_info = sz_compress_decompress_highorder_3d<float>(data, num_elements, r1, r2, r3, precision,
                                                                                 baseline_param, true);
        fprintf(stdout,
                "Baseline: reb:%.1e, ratio:%.2f, compress_time:%.3f, PSNR:%.2f, NRMSE %.10e Ori_bytes %ld, Compressed_bytes %ld\n",
                eb, baseline_compress_info.ratio, baseline_compress_info.compress_time, baseline_compress_info.psnr,
                baseline_compress_info.nrmse, baseline_compress_info.ori_bytes,
                baseline_compress_info.compress_bytes);

        for (auto sample_ratio : {0.8, 0.5, 0.4, 0.2, 0.1, 0.08, 0.05, 0.02, 0.01, 0.005}) {
            sz_params params(false, 6, 3, 0, true, false, true, false, precision);
            params.sample_ratio = sample_ratio;
            params.capacity = capacity;
            auto compress_info = do_compress_sampling<float>(data, num_elements, r1, r2, r3, precision, params);

            fprintf(stdout, "stage:1, reb:%.1e, sample_ratio:%.3f, ratio:%.2f\n",
                    eb, sample_ratio, compress_info.ratio);
        }
        free(data);
        return 0;
    }
    float sample_ratio = atof(argv[6]);
    if (argc <= 7) {
        sz_compress_autotuning_3d<float>(data, r1, r2, r3, eb, compressed_size, true, true, true, sample_ratio);
        free(data);
        return 0;
    }

    unsigned char *compressed = sz_compress_autotuning_3d<float>(data, r1, r2, r3, eb, compressed_size);
    free(data);

    printf("FINAL: reb:%.1e, ratio %.2f\n", eb, r1 * r2 * r3 * sizeof(float) * 1.0 / compressed_size);
    auto *dec_data = sz_decompress_autotuning_3d<float>(compressed, compressed_size, r1, r2, r3);
    free(compressed);
    free(dec_data);

    return 0;

}
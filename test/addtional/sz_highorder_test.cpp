#include "sz_utils.hpp"
//#include "sz_prediction.hpp"
#include "sz_compression_utils.hpp"
//#include "sz_optimize_quant_intervals.hpp"
#include "sz_autotuning_3d.hpp"
#include <list>

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

    {

        size_t compressed_size;

        sz_params params_high(false, 8, 3, 0, false, true,
                              false, true, eb * (max - min));
        params_high.filename = argv[1];
        params_high.eb = eb;
        sz_compress_info compress_info_high = sz_compress_decompress_3d(data, num_elements, r1, r2, r3,
                                                                        eb * (max - min), params_high,
                                                                        true);
        fprintf(stdout,
                "FINAL: reb:%.1e, ratio %.2f, compress_time:%.3f, capacity:%d, PSNR:%.2f, NRMSE %.10e\n",
                eb, compress_info_high.ratio,
                compress_info_high.compress_time, 0, compress_info_high.psnr,
                compress_info_high.nrmse);
    }

    free(data);

    return 0;

}
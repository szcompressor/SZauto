#include "sz_autotuning_3d.hpp"
#include "sz_utils.hpp"

using namespace std;

int main(int argc, char **argv) {
    size_t num_elements = 0;
    double *data = readfile<double>(argv[1], num_elements);
    double max = data[0];
    double min = data[0];
    for (int i = 1; i < num_elements; i++) {
        if (max < data[i]) max = data[i];
        if (min > data[i]) min = data[i];
    }
    int r1 = atoi(argv[2]);
    int r2 = atoi(argv[3]);
    int r3 = atoi(argv[4]);
    double eb = atof(argv[5]);
    size_t compressed_size;
    if (argc <= 6) {
        sz_compress_autotuning_3d<double>(data, r1, r2, r3, eb, compressed_size, true, true, true);
        free(data);
        return 0;
    }
    double sample_ratio = atof(argv[6]);
    if (argc <= 7) {
        sz_compress_autotuning_3d<double>(data, r1, r2, r3, eb, compressed_size, true, true, true, sample_ratio);
        free(data);
        return 0;
    }

    unsigned char *compressed = sz_compress_autotuning_3d<double>(data, r1, r2, r3, eb, compressed_size);
    free(data);

    printf("FINAL: reb:%.1e, ratio %.2f\n", eb, r1 * r2 * r3 * sizeof(double) * 1.0 / compressed_size);
    auto *dec_data = sz_decompress_autotuning_3d<double>(compressed, compressed_size, r1, r2, r3);
    free(compressed);
    free(dec_data);

    return 0;

}
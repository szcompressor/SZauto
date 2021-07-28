#include "sz_autotuning_3d.hpp"
#include "sz_utils.hpp"

using namespace std;

int main(int argc, char **argv) {
    if (argc < 6) {
        printf("SZauto\nusage: %s data_file dim0 dim1 dim2 relative_error_bound\n", argv[0]);
        printf("example: %s qmcpack.dat 33120 69 69 1e-3\n", argv[0]);
        exit(0);
    }
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
    if (argc <= 6) {
        sz_compress_autotuning_3d<float>(data, r1, r2, r3, eb, compressed_size, false, true, true);
        free(data);
        return 0;
    }
    float sample_ratio = atof(argv[6]);
    if (argc <= 7) {
        sz_compress_autotuning_3d<float>(data, r1, r2, r3, eb, compressed_size, false, true, true, sample_ratio);
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
#include "sz_compress_3d.hpp"
#include "sz_lossless.hpp"
#include "utils.hpp"
#include "sz_compression_utils.hpp"
#include "utils.hpp"

using namespace std;

int find_min(const int *arr, size_t n) {
    int min = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < min) {
            min = arr[i];
        }
    }
    return min;
}

int find_max(const int *arr, size_t n) {
    int max = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

int main(int argc, char **argv) {
    size_t n1 = 0, n2 = 0, n3 = 0, n4 = 0;
    int *type1 = readfile<int>(argv[1], n1);
    float *unpred = readfile<float>(argv[2], n3);

    int *type2;
    float *param_unpred;
    if (argc > 3) {
        type2 = readfile<int>(argv[3], n2);
        param_unpred = readfile<float>(argv[4], n4);
    }

    unsigned char *compressed = (unsigned char *) malloc(((n1 + n2) * sizeof(int) + (n3 + n4) * sizeof(float)) * 4);
    unsigned char *result_after_lossless = NULL;
    unsigned char *compressed_pos = compressed;
    int min1 = find_min(type1, n1);
    for (size_t i = 0; i < n1; i++) {
        type1[i] = type1[i] - min1;
    }
    int max1 = find_max(type1, n1)+1;
    Huffman_encode_tree_and_data(2 * max1, type1, n1, compressed_pos);
    size_t data_after_huffman = compressed_pos - compressed;

    size_t param_after_huffman = 0;
    if (n2 > 0) {
        int min2 = find_min(type2, n2);
        for (size_t i = 0; i < n2; i++) {
            type2[i] = type2[i] - min2;
        }
        int max2 = find_max(type2, n2)+1;
        Huffman_encode_tree_and_data(2 * max2, type2, n2, compressed_pos);
        param_after_huffman = compressed_pos - compressed - data_after_huffman;
    }

    if (n3 > 0) {
        memcpy(compressed_pos, unpred, n3 * sizeof(float));
        compressed_pos += n3 * sizeof(float);
    }

    if (n4 > 0) {
        memcpy(compressed_pos, param_unpred, n4 * sizeof(float));
        compressed_pos += n4 * sizeof(float);
    }

    cout << "Original size :" << n1 * sizeof(int) << endl;
    cout << "After Huffman: " << compressed_pos - compressed
         << "; data: " << data_after_huffman
         << "; param: " << param_after_huffman
         << "; unpredicted: " << n3 * sizeof(float)
         << "; param unpredicted: " << n4 * sizeof(float) << endl;

    size_t lossless_outsize = sz_lossless_compress(ZSTD_COMPRESSOR, 3, compressed, compressed_pos - compressed,
                                                   &result_after_lossless);

    cout << "Compressed size = " << lossless_outsize << ", ratio = " << (n1 * sizeof(float)) * 1.0 / lossless_outsize << endl;

    cerr << lossless_outsize << "," << data_after_huffman << "," << param_after_huffman << endl;
    free(compressed);
    free(result_after_lossless);

}
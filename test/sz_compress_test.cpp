#include "sz_compress_3d.hpp"
#include "sz_decompress_3d.hpp"
#include "sz_lossless.hpp"
#include "sz_utils.hpp"
using namespace std;

int main(int argc, char ** argv){
    size_t num_elements = 0;
    float * data = readfile<float>(argv[1], num_elements);
    int r1 = atoi(argv[2]);
    int r2 = atoi(argv[3]);
    int r3 = atoi(argv[4]);

    float eb = atof(argv[5]);
    float preb = atof(argv[6]);
    int block_size = atoi(argv[7]);
    int stride = atoi(argv[8]);
    int pred_dim = atoi(argv[9]);
    int lorenzo_op = atoi(argv[10]);
    int regression_op = atoi(argv[11]);

    bool lorenzo = lorenzo_op == 1 || lorenzo_op == 3;
    bool lorenzo_2layer = lorenzo_op == 2 || lorenzo_op == 3;
    bool regression_linear = regression_op == 1 || regression_op == 3;
    bool regression_poly = regression_op == 2 || regression_op == 3;

    float max = data[0];
    float min = data[0];
    for(int i=1; i<num_elements; i++){
        if(max < data[i]) max = data[i];
        if(min > data[i]) min = data[i];
    }
    int increase_quant_intv = 0;
    cout << "Options: block_size = " << block_size << ", pred_dim = " << pred_dim
            << ", use_lorenzo=" << lorenzo
            << ", use_lorenzo_2layer=" << lorenzo_2layer
            << ", use_regression_linear=" << regression_linear
            << ", use_regression_poly=" << regression_poly
            << endl;
    cout << "value range = " << max - min << endl;
    cout << "precision = " << eb*(max - min) << endl;
    size_t result_size = 0;
    struct timespec start, end;
    int err = 0;
    err = clock_gettime(CLOCK_REALTIME, &start);
    sz_params params(false, block_size, pred_dim, increase_quant_intv, lorenzo, lorenzo_2layer,
                     regression_linear, regression_poly);
    unsigned char * result =  sz_compress_3d_knl<float>(data, r1, r2, r3, eb*(max - min), result_size, params);
    unsigned char * result_after_lossless = NULL;
    size_t lossless_outsize = sz_lossless_compress(ZSTD_COMPRESSOR, 3, result, result_size, &result_after_lossless);
    err = clock_gettime(CLOCK_REALTIME, &end);
    cout << "Compression time: " << (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/(double)1000000000 << "s" << endl;
    cout << "Compressed size = " << lossless_outsize << ", ratio = " << (num_elements*sizeof(float)) * 1.0/lossless_outsize << endl;
    free(result);

    err = clock_gettime(CLOCK_REALTIME, &start);
    size_t lossless_output = sz_lossless_decompress(ZSTD_COMPRESSOR, result_after_lossless, lossless_outsize, &result, result_size);
    float * dec_data = sz_decompress_3d_knl<float>(result, r1, r2, r3);
    err = clock_gettime(CLOCK_REALTIME, &end);
    cout << "Decompression time: " << (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/(double)1000000000 << "s" << endl;
    free(result_after_lossless);
    // writefile("dec_data.dat", dec_data, num_elements);
    verify(data, dec_data, num_elements);
    free(result);
    free(data);
    free(dec_data);
}
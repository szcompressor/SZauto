#include "sz_compress_3d.hpp"
#include "sz_decompress_3d.hpp"
#include "sz_lossless.hpp"
#include "utils.hpp"
using namespace std;

int main(int argc, char ** argv){
    size_t num_elements = 0;
    float * data = readfile<float>(argv[1], num_elements);
    int r1 = atoi(argv[2]);
    int r2 = atoi(argv[3]);
    int r3 = atoi(argv[4]);
    double eb = atof(argv[5]);
    int block_size = atoi(argv[6]);
    int use_knl = atoi(argv[7]);
    float max = data[0];
    float min = data[0];
    for(int i=1; i<num_elements; i++){
        if(max < data[i]) max = data[i];
        if(min > data[i]) min = data[i];
    }
    cout << "value range = " << max - min << endl;
    cout << "precision = " << eb*(max - min) << endl;
    size_t result_size = 0;
    struct timespec start, end;
    int err = 0;
    err = clock_gettime(CLOCK_REALTIME, &start);
    auto *compress_func1 = sz_compress_3d<float>;
    auto *compress_func2 = sz_compress_3d_knl<float>;
    auto *compress_func = use_knl ? compress_func2 : compress_func1;
    unsigned char * result =  compress_func(data, r1, r2, r3, eb*(max - min), result_size, block_size, false, false);
    unsigned char * result_after_lossless = NULL;
    size_t lossless_outsize = sz_lossless_compress(ZSTD_COMPRESSOR, 3, result, result_size, &result_after_lossless);
    err = clock_gettime(CLOCK_REALTIME, &end);
    cout << "Compression time: " << (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/(double)1000000000 << "s" << endl;
    cout << "Compressed size = " << lossless_outsize << ", ratio = " << (num_elements*sizeof(float)) * 1.0/lossless_outsize << endl;
    free(result);
    err = clock_gettime(CLOCK_REALTIME, &start);
    size_t lossless_output = sz_lossless_decompress(ZSTD_COMPRESSOR, result_after_lossless, lossless_outsize, &result, result_size);
    auto *decompress_func1 = sz_decompress_3d<float>;
    auto *decompress_func2 = sz_decompress_3d_knl<float>;
    auto *decompress_func = use_knl ? decompress_func2 : decompress_func1;
    float * dec_data = decompress_func(result, r1, r2, r3);
    err = clock_gettime(CLOCK_REALTIME, &end);
    cout << "Decompression time: " << (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/(double)1000000000 << "s" << endl;
    free(result_after_lossless);
    // writefile("dec_data.dat", dec_data, num_elements);
    verify(data, dec_data, num_elements);
    free(result);
    free(data);
    free(dec_data);
}
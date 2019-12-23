//
// Created by Kai Zhao on 12/21/19.
//
#include "sz_compress_3d.hpp"
#include "sz_lossless.hpp"
#include "sz_decompress_3d.hpp"
#include "sz_utils.hpp"
#include "sz_def.hpp"

#ifndef SZ_SZ_COMPRESS_UTIL_H
#define SZ_SZ_COMPRESS_UTIL_H

using namespace std;

sz_compress_info compress(float *data, size_t num_elements, int r1, int r2, int r3, float precision,
                          sz_params params, bool use_decompress) {
    size_t result_size = 0;
    sz_compress_info compressInfo;

    struct timespec start, end;
    int err = 0;
    err = clock_gettime(CLOCK_REALTIME, &start);

    unsigned char *result = sz_compress_3d_knl_2<float>(data, r1, r2, r3, precision, result_size, params, compressInfo);
    unsigned char *result_after_lossless = NULL;
    size_t lossless_outsize = sz_lossless_compress(ZSTD_COMPRESSOR, 3, result, result_size, &result_after_lossless);
    err = clock_gettime(CLOCK_REALTIME, &end);
    compressInfo.compress_time = (float) (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / (double) 1000000000;
    cout << "Compression time: " << compressInfo.compress_time << "s" << endl;

    float ratio = (num_elements * sizeof(float)) * 1.0 / lossless_outsize;
    compressInfo.ori_bytes = num_elements * sizeof(float);
    compressInfo.compress_bytes = lossless_outsize;
    compressInfo.ratio = compressInfo.ori_bytes * 1.0 / compressInfo.compress_bytes;
    cout << "Compressed size = " << lossless_outsize << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!! ratio  !!!!!!!!!!!!!= " << ratio << endl;

    free(result);

    if (use_decompress) {
        err = clock_gettime(CLOCK_REALTIME, &start);
        size_t lossless_output = sz_lossless_decompress(ZSTD_COMPRESSOR, result_after_lossless, lossless_outsize, &result,
                                                        result_size);
        float *dec_data = sz_decompress_3d_knl<float>(result, r1, r2, r3);
        err = clock_gettime(CLOCK_REALTIME, &end);
        compressInfo.decompress_time =
                (double) (end.tv_sec - start.tv_sec) + (double) (end.tv_nsec - start.tv_nsec) / (double) 1000000000;
        cout << "Decompression time: " << compressInfo.decompress_time << "s" << endl;
        free(result_after_lossless);
        // writefile("dec_data.dat", dec_data, num_elements);
        verify(data, dec_data, num_elements, compressInfo.psnr, compressInfo.nrmse);
        free(result);
        free(dec_data);
    } else {
        free(result_after_lossless);
    }

    return compressInfo;
}

sz_compress_info compress_sampling(float *data, size_t num_elements, int r1, int r2, int r3, float precision,
                          sz_params params, bool use_decompress) {
    size_t result_size = 0;
    sz_compress_info compressInfo;

    struct timespec start, end;
    int err = 0;
    err = clock_gettime(CLOCK_REALTIME, &start);

    unsigned char *result = sz_compress_3d_sampling<float>(data, r1, r2, r3, precision, result_size, params, compressInfo);
    unsigned char *result_after_lossless = NULL;
    size_t lossless_outsize = sz_lossless_compress(ZSTD_COMPRESSOR, 3, result, result_size, &result_after_lossless);
    err = clock_gettime(CLOCK_REALTIME, &end);
    compressInfo.compress_time = (float) (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / (double) 1000000000;
    cout << "Compression time: " << compressInfo.compress_time << "s" << endl;

    float ratio = (num_elements * sizeof(float)) * 1.0 / lossless_outsize;
    compressInfo.ori_bytes = num_elements * sizeof(float);
    compressInfo.compress_bytes = lossless_outsize;
    compressInfo.ratio = compressInfo.ori_bytes * 1.0 / compressInfo.compress_bytes;
    cout << "Compressed size = " << lossless_outsize << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!! ratio  !!!!!!!!!!!!!= " << ratio << endl;

    free(result);

    return compressInfo;
}
#endif //SZ_SZ_COMPRESS_UTIL_H

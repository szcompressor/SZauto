#include "sz_compress_3d.hpp"
#include "sz_decompress_3d.hpp"
#include "utils.hpp"

int main(int argc, char ** argv){
    int status;
    size_t nbEle;
    float * data = readfile<float>(argv[1], &nbEle);
    int r1 = atoi(argv[2]);
    int r2 = atoi(argv[3]);
    int r3 = atoi(argv[4]);
    double eb = atof(argv[5]);
    float max = data[0];
    float min = data[0];
    for(int i=1; i<nbEle; i++){
        if(max < data[i]) max = data[i];
        if(min > data[i]) min = data[i];
    }
    cout << "value range = " << max - min << endl;
    cout << "precision = " << eb*(max - min) << endl;
    size_t num_elements = r1 * r2 * r3;
    size_t result_size = 0;
    unsigned char * result =  sz_compress_3d(data, r1, r2, r3, eb*(max - min), result_size);
    unsigned char * result_after_lossless = NULL;
    cout << "Compressed size = " << result_size << ", ratio = " << (num_elements*sizeof(float)) * 1.0/result_size << endl;
    SZ_Init(NULL);
    size_t lossless_outsize = sz_lossless_compress(confparams_cpr->losslessCompressor, confparams_cpr->gzipMode, result, result_size, &result_after_lossless);
    cout << "After lossless, compressed size = " << lossless_outsize << ", ratio = " << (num_elements*sizeof(float)) * 1.0/lossless_outsize << endl;
    free(result);
    size_t lossless_output = sz_lossless_decompress(confparams_cpr->losslessCompressor, result_after_lossless, lossless_outsize, &result, result_size);
    free(result_after_lossless);
    SZ_Finalize();
    float * dec_data = sz_decompress_3d<float>(result, r1, r2, r3);
    verify(data, dec_data, nbEle);
    free(result);
    free(data);
    free(dec_data);
}
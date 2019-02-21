#include "sz_compress_3d.hpp"

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
    unsigned char * result =  sz_compress_3d(data, r1, r2, r3, eb*(max - min));
    // std::cout << "compression done\n";
    // float * dec_data = trilinear_interpolation(comp_data, bs, n1, n2, n3);
    // float * dec_data = tricubic_interpolation(comp_data, bs, n1, n2, n3);
    // std::cout << "decompression done\n";
    // writefile<float>(strcat(argv[1], ".out"), data, nbEle);
    // verify(data, dec_data, nbEle, out_size);
    if(result) free(result);
    free(data);
}
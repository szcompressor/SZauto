#ifndef _sz_compress_3d_h
#define _sz_compress_3d_h

// #include "sz.h"
// #include "Huffman.h"
#include "utils.hpp"
#include <cstddef>
#include <vector>

#define MAX(a, b) a>b?a:b
#define MIN(a, b) a<b?a:b
#define QuantIntvMeanCapacity 8192
#define QuantIntvSampleDistance 100
#define QuantIntvSampleCapacity 32768
#define QuantIntvAccThreshold 0.999
#define BSIZE 6
#define RegThresholdSize 4
#define LorenzeNoise3d 1.22

template<typename T>
unsigned char *
sz_compress_3d(const T * data, size_t r1, size_t r2, size_t r3, double precision);

#endif
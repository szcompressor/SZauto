#ifndef _sz_compression_utils_h
#define _sz_compression_utils_h

#include "sz_def.hpp"

#define QuantIntvMeanCapacity 8192
#define QuantIntvSampleDistance 100
#define QuantIntvSampleCapacity 32768
#define QuantIntvAccThreshold 0.999
#define BSIZE3d 8
#define RegThresholdSize3d 4
#define LorenzeNoise3d 1.22

template <typename T>
inline void
write_variable_to_dst(unsigned char *& dst, const T& var){
    memcpy(dst, &var, sizeof(T));
    dst += sizeof(T);
}

template <typename T>
inline void
write_array_to_dst(unsigned char *& dst, const T * array, size_t length){
    memcpy(dst, array, length*sizeof(T));
    dst += length*sizeof(T);
}

// modified from TypeManager.c
// change return value and increment byteArray
void 
convertIntArray2ByteArray_fast_1b_to_result_sz(const unsigned char* intArray, size_t intArrayLength, unsigned char *& result);

#endif

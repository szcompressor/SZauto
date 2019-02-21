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

#endif

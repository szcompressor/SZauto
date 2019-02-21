#ifndef _sz_decompression_utils_h
#define _sz_decompression_utils_h

#include "sz_def.hpp"

template <typename T>
inline T
read_variable_from_src(const unsigned char *& src){
    T var;
    memcpy(&var, src, sizeof(T));
    src += sizeof(T);
    return var;
}

template <typename T>
inline T *
read_array_from_src(const unsigned char *& src, size_t length){
    T * array = (T *) malloc(length * sizeof(T));
    memcpy(array, src, length*sizeof(T));
    src += length*sizeof(T);
    return array;
}

#endif

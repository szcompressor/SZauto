#ifndef _sz_decompression_utils_h
#define _sz_decompression_utils_h

#include "sz_def.hpp"

template <typename T>
inline void
read_variable_from_src(const unsigned char *& src, T& var){
    memcpy(&var, src, sizeof(T));
    src += sizeof(T);
}

template <typename T>
inline T *
read_array_from_src(const unsigned char *& src, size_t length){
    T * array = (T *) malloc(length * sizeof(T));
    memcpy(array, src, length*sizeof(T));
    src += length*sizeof(T);
    return array;
}

// modified from TypeManager.c
// change return value and increment byteArray
unsigned char * 
convertByteArray2IntArray_fast_1b_sz(size_t intArrayLength, const unsigned char*& compressed_pos, size_t byteArrayLength);

int *
Huffman_decode_tree_and_data(size_t state_num, size_t num_elements, const unsigned char *& compressed_pos);

#endif

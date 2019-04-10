#ifndef _sz_lossless_hpp
#define _sz_lossless_hpp

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "zstd.h"

#define ZSTD_COMPRESSOR 1

// copied from sz/src/utility.c
int is_lossless_compressed_data(unsigned char* compressedBytes, size_t cmpSize);
unsigned long sz_lossless_compress(int losslessCompressor, int level, unsigned char* data, unsigned long dataLength, unsigned char** compressBytes);
unsigned long sz_lossless_decompress(int losslessCompressor, unsigned char* compressBytes, unsigned long cmpSize, unsigned char** oriData, unsigned long targetOriSize);
unsigned long sz_lossless_decompress65536bytes(int losslessCompressor, unsigned char* compressBytes, unsigned long cmpSize, unsigned char** oriData);

#endif
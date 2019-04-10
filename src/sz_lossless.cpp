#include "sz_lossless.hpp"

int is_lossless_compressed_data(unsigned char* compressedBytes, size_t cmpSize)
{
	unsigned long long frameContentSize = ZSTD_getFrameContentSize(compressedBytes, cmpSize);
	if(frameContentSize != ZSTD_CONTENTSIZE_ERROR)
		return ZSTD_COMPRESSOR;
	return -1; //fast mode (without GZIP or ZSTD)
}

unsigned long sz_lossless_compress(int losslessCompressor, int level, unsigned char* data, unsigned long dataLength, unsigned char** compressBytes)
{
	unsigned long outSize = 0; 
	size_t estimatedCompressedSize = 0;
	switch(losslessCompressor)
	{
	case ZSTD_COMPRESSOR:
		if(dataLength < 100) 
			estimatedCompressedSize = 200;
		else
			estimatedCompressedSize = dataLength*1.2;
		*compressBytes = (unsigned char*)malloc(estimatedCompressedSize);
		outSize = ZSTD_compress(*compressBytes, estimatedCompressedSize, data, dataLength, level); //default setting of level is 3
		break;
	default:
		printf("Error: Unrecognized lossless compressor in sz_lossless_compress()\n");
	}
	return outSize;
}

unsigned long sz_lossless_decompress(int losslessCompressor, unsigned char* compressBytes, unsigned long cmpSize, unsigned char** oriData, unsigned long targetOriSize)
{
	unsigned long outSize = 0;
	switch(losslessCompressor)
	{
	case ZSTD_COMPRESSOR:
		*oriData = (unsigned char*)malloc(targetOriSize);
		ZSTD_decompress(*oriData, targetOriSize, compressBytes, cmpSize);
		outSize = targetOriSize;
		break;
	default:
		printf("Error: Unrecognized lossless compressor in sz_lossless_decompress()\n");
	}
	return outSize;
}

unsigned long sz_lossless_decompress65536bytes(int losslessCompressor, unsigned char* compressBytes, unsigned long cmpSize, unsigned char** oriData)
{
	unsigned long outSize = 0;
	switch(losslessCompressor)
	{
	case ZSTD_COMPRESSOR:
		*oriData = (unsigned char*)malloc(65536);
		memset(*oriData, 0, 65536);
		ZSTD_decompress(*oriData, 65536, compressBytes, cmpSize);	//the first 32768 bytes should be exact the same.
		outSize = 65536;
		break;
	default:
		printf("Error: Unrecognized lossless compressor\n");
	}
	return outSize;
}
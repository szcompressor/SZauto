#include "sz_compression_utils.hpp"

void 
convertIntArray2ByteArray_fast_1b_to_result_sz(const unsigned char* intArray, size_t intArrayLength, unsigned char *& result){
	size_t byteLength = 0;
	size_t i, j; 
	if(intArrayLength%8==0)
		byteLength = intArrayLength/8;
	else
		byteLength = intArrayLength/8+1;
		
	size_t n = 0;
	int tmp, type;
	for(i = 0;i<byteLength;i++){
		tmp = 0;
		for(j = 0;j<8&&n<intArrayLength;j++){
			type = intArray[n];
			if(type == 1)
				tmp = (tmp | (1 << (7-j)));
			n++;
		}
    	*(result++) = (unsigned char)tmp;
	}
}

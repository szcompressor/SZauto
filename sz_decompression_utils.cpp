#include "sz_compression_utils.hpp"

unsigned char * 
convertByteArray2IntArray_fast_1b_sz(size_t intArrayLength, const unsigned char*& byteArray, size_t byteArrayLength){
    if(intArrayLength > byteArrayLength*8){
    	printf("Error: intArrayLength > byteArrayLength*8\n");
    	printf("intArrayLength=%zu, byteArrayLength = %zu", intArrayLength, byteArrayLength);
    	exit(0);
    }
    unsigned char * intArray = NULL;
	if(intArrayLength>0)
		intArray = (unsigned char*)malloc(intArrayLength*sizeof(unsigned char));
    
	size_t n = 0, i;
	int tmp;
	for (i = 0; i < byteArrayLength-1; i++) {
		tmp = *(byteArray++);
		intArray[n++] = (tmp & 0x80) >> 7;
		intArray[n++] = (tmp & 0x40) >> 6;
		intArray[n++] = (tmp & 0x20) >> 5;
		intArray[n++] = (tmp & 0x10) >> 4;
		intArray[n++] = (tmp & 0x08) >> 3;
		intArray[n++] = (tmp & 0x04) >> 2;
		intArray[n++] = (tmp & 0x02) >> 1;
		intArray[n++] = (tmp & 0x01) >> 0;		
	}
	
	tmp = *(byteArray++);	
	// if(n == intArrayLength)
	// 	return;
	// intArray[n++] = (tmp & 0x80) >> 7;
	// if(n == intArrayLength)
	// 	return;	
	// intArray[n++] = (tmp & 0x40) >> 6;
	// if(n == intArrayLength)
	// 	return;	
	// intArray[n++] = (tmp & 0x20) >> 5;
	// if(n == intArrayLength)
	// 	return;
	// intArray[n++] = (tmp & 0x10) >> 4;
	// if(n == intArrayLength)
	// 	return;	
	// intArray[n++] = (tmp & 0x08) >> 3;
	// if(n == intArrayLength)
	// 	return;	
	// intArray[n++] = (tmp & 0x04) >> 2;
	// if(n == intArrayLength)
	// 	return;	
	// intArray[n++] = (tmp & 0x02) >> 1;
	// if(n == intArrayLength)
	// 	return;	
	// intArray[n++] = (tmp & 0x01) >> 0;
	for(int i=0; n < intArrayLength; n++, i++){
		intArray[n] = (tmp & (1 << (7 - i))) >> (7 - i);
	}		
	return intArray;
}


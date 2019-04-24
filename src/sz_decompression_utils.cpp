#include "sz_decompression_utils.hpp"

// modified from TypeManager.c
// change return value and increment compressed_pos
unsigned char * 
convertByteArray2IntArray_fast_1b_sz(size_t intArrayLength, const unsigned char*& compressed_pos, size_t byteArrayLength){
    if(intArrayLength > byteArrayLength*8){
    	printf("Error: intArrayLength > byteArrayLength*8\n");
    	printf("intArrayLength=%zu, byteArrayLength = %zu", intArrayLength, byteArrayLength);
    	exit(0);
    }
    unsigned char * intArray = NULL;
	if(intArrayLength>0) intArray = (unsigned char*)malloc(intArrayLength*sizeof(unsigned char));
	size_t n = 0, i;
	int tmp;
	for (i = 0; i < byteArrayLength-1; i++) {
		tmp = *(compressed_pos++);
		intArray[n++] = (tmp & 0x80) >> 7;
		intArray[n++] = (tmp & 0x40) >> 6;
		intArray[n++] = (tmp & 0x20) >> 5;
		intArray[n++] = (tmp & 0x10) >> 4;
		intArray[n++] = (tmp & 0x08) >> 3;
		intArray[n++] = (tmp & 0x04) >> 2;
		intArray[n++] = (tmp & 0x02) >> 1;
		intArray[n++] = (tmp & 0x01) >> 0;		
	}
	tmp = *(compressed_pos++);	
	for(int i=0; n < intArrayLength; n++, i++){
		intArray[n] = (tmp & (1 << (7 - i))) >> (7 - i);
	}		
	return intArray;
}

// TODO: change compress_pos to const
int *
Huffman_decode_tree_and_data(size_t state_num, size_t num_elements, const unsigned char *& compressed_pos){
	HuffmanTree* huffman = createHuffmanTree(state_num);
	size_t node_count = 0;
	read_variable_from_src(compressed_pos, node_count);
	unsigned int tree_size = 0;
	read_variable_from_src(compressed_pos, tree_size);
	node root = reconstruct_HuffTree_from_bytes_anyStates(huffman, compressed_pos, node_count);
	compressed_pos += tree_size;
	size_t type_array_size = 0;
	read_variable_from_src(compressed_pos, type_array_size);
	int * type = (int *) malloc(num_elements * sizeof(int));
	decode(compressed_pos, num_elements, root, type);
	compressed_pos += type_array_size;
	SZ_ReleaseHuffman(huffman);
	return type;
}





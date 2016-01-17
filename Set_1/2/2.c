#include <stdio.h>

/*Synopsis:
 * Solution to Matasano Crypto Challenge 2
 * Function xor_array takes two arrays of equal length
 * xors them, and places the result in a 3rd array.  
 * The function takes as input 3 arrays and the length of 
 * the arrays. 
 */

void xor_array(unsigned char *arr1, unsigned char *arr2, unsigned char *arr3, unsigned int length);

void main()
{
	unsigned int i;
	unsigned char array1[18] = {0x1c, 0x01, 0x11, 0x00, 0x1f, 0x01, 0x01, 0x00, 0x06, 0x1a, 0x02, 0x4b, 0x53, 0x53, 0x50, 0x09, 0x18, 0x1c};
	unsigned char array2[18] = {0x68, 0x69, 0x74, 0x20, 0x74, 0x68, 0x65, 0x20, 0x62, 0x75, 0x6c, 0x6c, 0x27, 0x73, 0x20, 0x65, 0x79, 0x65};
	unsigned char array3[18];
	xor_array(array1, array2, array3, 18);
	for(i=0; i<18; i++)
	{
		printf("%02x\n", array3[i]);
	}
	return;
}

void xor_array(unsigned char *arr1, unsigned char *arr2, unsigned char *arr3, unsigned int length)
{
	unsigned char i=0;
	for(i=0; i<length; i++)
	{
		arr3[i] = arr1[i]^arr2[i];
	}
	return;
}


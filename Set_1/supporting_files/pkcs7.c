/*Synopsis:  This program defines a function that accepts a "block" of size block_size,
 * and and pads it using pkcs7 padding.  You must pass the number of characters required for
 * padding to the pkcs_7_pad function defined below. 
 */

#include <stdio.h>
#include <string.h>
#define BLOCK_SIZE_MACRO 16

void pkcs_7_pad(unsigned char *block, unsigned int pad_count, unsigned int block_size);

int main()
{

	unsigned int pad_count;
	unsigned char block[BLOCK_SIZE_MACRO];
	scanf("%s", block);
	
	printf("%d\n", (int)strlen((const char *)block));
	pad_count = (unsigned char)BLOCK_SIZE_MACRO-strlen((const char *)block);
	pkcs_7_pad(block, pad_count, BLOCK_SIZE_MACRO);
	return 0;
}

void pkcs_7_pad(unsigned char *block, unsigned int pad_count, unsigned int block_size)
{
	unsigned int i = 0;
	if(pad_count == 0)
	{
		pad_count = block_size;
	}
	i = pad_count;
	while(i > 0)
	{
		block[block_size - i] = pad_count;
		i = (i-1);
	}
	return;
}

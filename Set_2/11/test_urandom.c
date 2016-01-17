#include <stdio.h>
#include <string.h>
#define Input_Size 500 //limits number of chars read by fgets

unsigned int black_box(unsigned char *array);

int main()
{

	unsigned char input_array[(Input_Size + 20)] = {0};
	printf("Enter string\n");
	fgets(input_array, Input_Size, stdin);
	black_box(input_array);
	return 0;
}

//black_box accepts as input an array of unsigned chars
//adds a few random chars on the front and back side of the
//array, then aes-128 encrypts using either cbc or ecb with a random key.
//the block cipher mode used is chosen randomly, and if cbc is used
//the initialization vector used is random.  out_put array is returned.
unsigned int black_box(unsigned char *array)
{
	
	FILE *fpin;
	unsigned int i=0, j, flag = 0;
	unsigned char num, temp, count;
	unsigned char key[16], IV[16] = {0}, new_array[(Input_Size + 20)] = {0};

	// /dev/urandom gives uniformly distributed bytes
	fpin = fopen("/dev/urandom", "r");	
	//get random byte and store in num
	fscanf(fpin, "%c", &num);
	num = (num%6) + 5; // num is now between 5 and 10 inclusive
	count = num;
	
	//fill the first num entries of new_array with random nonzero bytes
	for(i=0; i<num; i++)
	{
		temp = 0;
		while(temp == 0)
		{
			fscanf(fpin, "%c", &temp);
		}
		new_array[i] = temp;
		printf("%02x", temp);
	}
	
	//append array to new_array
	j=0;	
	while(array[j] != 0 && i < Input_Size)
	{
		new_array[j+num] = array[j];
		printf("%02x", new_array[j+num]);
		j++;
	}
	//construct another random number from 5-10 inclusive
	fscanf(fpin, "%c", &num);
	num = (num%6)+5;
	count = count + num;
	
	//append num random non-zero bytes to back side of new_array
	i=i+j;
	for(j=0; j<num; j++)
	{
		temp = 0;
		while(temp == 0)
		{
			fscanf(fpin, "%c", &temp);
		}
		new_array[i+j] = temp;
		printf("%02x", new_array[i+j]);
		
	}
	
	printf("\n");
	i=0;
	j=0;
	while(array[i] !=0)
	{
		i++;
	}
	while(new_array[j]!=0)
	{
		printf("%02x", new_array[j]);
		j++;
	}
	//flip coin. if flag is 1 cbc.  else ecb.
	fscanf(fpin, "%c", flag);
	flag = flag%2;
	//generate a random aes key.  if using cbc generate
	//initialization vector
	for(i=0; i<16; i++)
	{
		fscanf(fpin, "%c", &key[i]);
		if(flag == 1)
		{
			fscanf(fpin, "%c", &IV[i]);
		}
	}
	
	/*
	 * to finish write encrypt_cbc and encrypt_ecb.
	 * cbc should take in a pointer to the plaintext
	 * array, a pointer to the ciphertext array, a pointer
	 * to the key, and a pointer to the IV.  in this case 
	 * plaintext is new_array and ciphertext is array.
	 */
	
	
	
	
	
	
	
	fclose(fpin);
	return flag;
}

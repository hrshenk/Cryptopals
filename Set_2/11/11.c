/*
*Synopsis:  This code was written as a solution to challenge 11 of the Matasano Crypto Challenges.  See http://cryptopals.com
*This program accepts user provided input, then appends random bytes to the beginning and end of the user 
*input before encrypting under cbc or ecb.  The mode is chosen randomly.  Then the program attempts to determine
*the block cipher mode used to encrypt.  A properly constructed input will result in properly detecting the encryption
*mode being used.  Hence, this program illustrates that it is trivial, when a user controls a portion of the input, 
*to distinguish between cipher modes.
*
*In this program, as opposed to in solutions to previous challenges, I chose to use the more abstracted EVP objects
*and functions made available by the OpenSSL API to perform encryption and padding in order to illustrate the ability 
*to work sensibly with APIs.
*
*As always...this is not production code and should not be used in a production environment.

*/


#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <stdio.h>
#include <string.h>

#define Input_Size 500 //limits number of chars read by fgets, then buffers are defined via this value.

unsigned int black_box(unsigned char *input_array, unsigned char *output_array);
int detect_block_cipher_mode(unsigned char *array, unsigned int length);
int encrypt_cbc(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
int encrypt_ecb(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext);
void handleErrors(void);

int main()
{
    //input_array will accommodate user's input plus up to 20 random bytes added by blackbox.
	unsigned char input_array[Input_Size] = {0};
	unsigned char ciphertext_array[(Input_Size + 20 + 16)] = {0};
	unsigned int length;
	
	
	
	printf("Enter string\n");
	fgets(input_array, Input_Size, stdin);
	length = black_box(input_array, ciphertext_array);
	detect_block_cipher_mode(ciphertext_array, length);
	return 0;
}

//detect_block_cipher_mode, for appropriate inputs, distinguishes between ECB and CBC mode
int detect_block_cipher_mode(unsigned char *array, unsigned int length)
{
    int i, j,k, block_count;
    
    //test our input has valid length
    if(length % 16 != 0)
    {
        printf("invalid length");
        return -1;
    }
    block_count = length/16;
    if(block_count < 4)
    {
        printf("Insufficient number of blocks to accurately determing mode\n");
        return -1;
    }

    for(i=0; i<block_count-1; i++)
    {
        for(j=i+1; j<block_count; j++)
        {
            k=0;
            while(array[i*16+k] == array[j*16+k])
            {
                if(k==15)
                {
                    printf("ECB mode detected\n");
                    return 0;
                }
                k++;
            }

        }
    }
    printf("CBC mode seems to have been used\n");
    return 1;
    
}

//black_box accepts as input an array of unsigned chars
//adds a few random chars on the front and back side of the
//array, then aes-128 encrypts using either cbc or ecb with a random key.
//the block cipher mode used is chosen randomly, and if cbc is used
//the initialization vector used is random.
unsigned int black_box(unsigned char *input_array, unsigned char *ciphertext_array)
{
	
	FILE *fpin, *fpout;
	unsigned int i=0, j, count = 0, cipher_len;
	unsigned char num, temp, flag = 0;
	unsigned char key[16], IV[16] = {0}, new_array[(Input_Size + 20)] = {0};

	// /dev/urandom gives uniformly distributed bytes
	fpin = fopen("/dev/urandom", "r");	
	//get random byte and store in num
	fscanf(fpin, "%c", &num);
	num = (num%6) + 5; // num is now between 5 and 10 inclusive
	count = count + num;
	
	//fill the first num entries of new_array with random nonzero bytes
	for(i=0; i<num; i++)
	{
		temp = 0;
		while(temp == 0)
		{
			fscanf(fpin, "%c", &temp);
		}
		new_array[i] = temp;
	}
	
	//append input_array to new_array
	j=0;	
	while(input_array[j] != 0 && j < Input_Size)
	{
		new_array[j+num] = input_array[j];
		j++;
	}
	//construct another random number from 5-10 inclusive
	fscanf(fpin, "%c", &num);
	num = (num%6)+5;
	count = count +j;
	
	//append num random non-zero bytes to back side of new_array
	for(i=0; i<num; i++)
	{
		temp = 0;
		while(temp == 0)
		{
			fscanf(fpin, "%c", &temp);
		}
		new_array[i+count] = temp;
	}
	//here count is the length of the plaintext before pkcs7 padding
	count = count + num;

	//flip coin. if flag is 1 cbc.  else ecb.
	fscanf(fpin, "%c", &flag);
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
	
	if(flag ==1)
	{
	    cipher_len = encrypt_cbc(new_array, count, key, IV, ciphertext_array);
	    printf("Actual CBC\n");
	}
	else 
	{
	    cipher_len = encrypt_ecb(new_array,count,key,ciphertext_array);
	    printf("Actual ECB:\n");
	}
	
	//output the encrypted content to a file.  The contents of this file will be analyzed for ECB or CBC detetion.
	fpout = fopen("Black_Box_output.txt", "w");
	for(i=0; i<cipher_len; i++)
	{
	    fprintf(fpout, "%02x", ciphertext_array[i]);
	}
	
	fclose(fpout);
	fclose(fpin);
	return cipher_len;
}






int encrypt_cbc(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;
  int len, cipher_len = 0;
  

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the encryption operation.
   */
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
  {
      handleErrors();
  }
  
  /* Provide the message to be encrypted, and obtain the encrypted output.
   */
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  
  cipher_len = cipher_len + len;
  /* Finalise the encryption.
   * EVP_EncryptFinal_ex will handle the padding, then encrypt the last block.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + cipher_len, &len)) handleErrors();

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return cipher_len;
}

int encrypt_ecb(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;
  int len, cipher_len = 0;
  

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the encryption operation.
   */
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL))
  {
      handleErrors();
  }
  
  /* Provide the message to be encrypted, and obtain the encrypted output.
   */
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  
  cipher_len = cipher_len + len;
  /* Finalise the encryption.
   * EVP_EncryptFinal_ex will handle the padding, then encrypt the last block.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + cipher_len, &len)) handleErrors();

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return cipher_len;
}

void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}
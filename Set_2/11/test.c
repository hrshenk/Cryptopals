#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <stdio.h>
#include <string.h>

#define Input_Size 500 //limits number of chars read by fgets


unsigned int black_box(unsigned char *input_array);
unsigned char pkcs7_pad(unsigned int unpadded_text_length, unsigned char *array);
int encrypt_cbc(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
void handleErrors(void);
int temp_flag = 0;

int main()
{
    //input_array will accommodate user's input plus up to 20 random bytes added by blackbox.
	unsigned char input_array[Input_Size] = {0};
	printf("Enter string\n");
	fgets(input_array, Input_Size, stdin);
	black_box(input_array);
	return 0;
}

//black_box accepts as input an array of unsigned chars
//adds a few random chars on the front and back side of the
//array, then aes-128 encrypts using either cbc or ecb with a random key.
//the block cipher mode used is chosen randomly, and if cbc is used
//the initialization vector used is random.  out_put array will be the 
//hold the results after padding with random chars and pkcs 7 padding
unsigned int black_box(unsigned char *input_array)
{
	
	FILE *fpin;
	unsigned int i=0, j, count = 0, temp_count, ciphertext_len;
	unsigned char num, temp, flag = 0;
	unsigned char key[16], IV[16] = {0}, new_array[(Input_Size + 20)+16] = {0}, ciphertext_array[(Input_Size + 20 + 16)], plaintext[Input_Size + 20 +16], temp_ciphertext_array[(Input_Size + 20 + 16)];

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
	temp_count = count;
	for(i=0; i<count; i++)
	{
	    plaintext[i] = new_array[i];
	}
	//here count is the length of the plaintext plus padding.  This should equal the ciphertext length
	count = count + pkcs7_pad(count, new_array);

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
	
	flag = 1;
	printf("remove flag assignment line 99ish \n");
	if(flag ==1)
	{
	    ciphertext_len = (unsigned int)encrypt_cbc(new_array, count, key, IV, ciphertext_array);
	    temp_flag = 1;
	    encrypt_cbc(plaintext, temp_count, key, IV, temp_ciphertext_array);
	}
	else 
	{
	    //encrypt_ecb(whatever arguments.)
	}
	
	if(ciphertext_len != count) 
	{
	    printf("cipher length differs from plaintext length..so stop messin up\n");
	}
	else
	{
	    for(i=0; i<ciphertext_len; i++)
	    {
	        printf("%02x\t%02x\n", ciphertext_array[i], temp_ciphertext_array[i]);
	    }
	}
	printf("\n");
	fclose(fpin);
	return flag;
}

unsigned char pkcs7_pad(unsigned int unpadded_text_length, unsigned char *array)
{

    int i =0;
    unsigned char pad_length;
    pad_length = 16-(unpadded_text_length % 16);
    
    printf("%d\n", unpadded_text_length);
    
    while(array[i] !=0)
    {
       printf("%02x", array[i]);
       i++;
    }
    printf("\n");
    for(i=0; i<pad_length; i++)
    {
        array[unpadded_text_length + i] = pad_length;
    }
    
    i=0;
    while(i< (pad_length + unpadded_text_length))
    {
       printf("%02x", array[i]);
       i++;
    }
    printf("\n");
    
    return pad_length;
}


int encrypt_cbc(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;
  int len, cipher_len = 0, i;
  unsigned char final[16] = {0};

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();
  
  /*Turn off padding.  By default it is turned on, but we're handling padding 
   *as an exercise*/
   printf("padding before:%d\n", ctx -> flags);
   EVP_CIPHER_CTX_set_padding(ctx, 1);
   printf("padding after: %d\n", ctx -> flags);
   

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
   * An error should be thrown if anything remains to be encrypted since
   * we are handling the padding.
   */
  if(temp_flag == 1)
  {
      if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + cipher_len, &len)) printf("encryptfinalerror\n");
  }
  else
  {
    if(1 != EVP_EncryptFinal_ex(ctx, final, &len)) handleErrors();
  }
  if(len !=0)
  {
      printf("Something went wrong in EVP_EncryptFinal_ex: len is %d\n", len);
      for(i=0; i<16; i++)
      {
          printf("%02x", final[i]);
      }
      printf("\n");
  }
  else
  {
      for(i=0;i<16; i++)
      {
          if(final[i]!=0)
          {
               printf("Something went wrong in EVP_EncryptFinal_ex: final is not 0\n");
          }
      }
  }
  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return cipher_len;
}

void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}
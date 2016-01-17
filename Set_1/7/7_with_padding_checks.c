#include <openssl/aes.h>
#include <stdio.h>

/*
 *Synopsis:
 * This program was written as a solution to the Matasano Cryptography Challenges
 * See http://cryptopals.com
 * Decrypts a hex encoded file using a hard coded key, in this case "YELLOW SUBMARINE"
 * Note this code should never be used in a production environment.  It is intended
 * to be used solely for educational purposes. 
 */

void aes_encrypt_128(unsigned char *in_block, unsigned char *out_block, unsigned char *key_buf);
void aes_decrypt_128(unsigned char *in_block, unsigned char *out_block, unsigned char *key_buf);
void pkcs_7_pad(unsigned char *block, unsigned int pad_count, unsigned int block_size);
//void encrypt();
int decrypt();

int main()
{
	int num;
	printf("Press 0 to decrypt or 1 to encrypt:\n");
	scanf("%d", &num);
	if(num==0)
	{
		return decrypt();
	}
	else if(num==1)
	{
		
		return 0;
	}
	else
	{
		perror("Really?  You can't type 0 or 1?\n");
		return(2);
	}
}


int decrypt()
{
	//Declarations
	FILE *fpin;
	unsigned int i=0;
	unsigned int j=0;
	unsigned int k=0;
	unsigned char ch;
	unsigned int pad_count, flag = 0, pad_test =0;
	unsigned char ciphertext[16];
	unsigned char plaintext[16], key[] = "YELLOW SUBMARINE";
		
	//Body of program starts here
	
	//open FILE
	fpin = fopen("hex_encoded.txt", "r");
	//if fpin is 0 then opening the file failed.  Handle it.
	if(fpin == 0)
	{
		perror("File open failed\n");
		return -10;
	}
	//read in until the end of the file. Each time 16 bytes are read
	//we process the block.	
	while((fscanf(fpin, "%2hhx", &ch) != EOF))
	{
		ciphertext[(i%16)] = ch;
		if(i%16 == 15)//if we've filled 16 elements of ciphertext then decrypt and print.
		{
			aes_decrypt_128(ciphertext, plaintext, key);
			
			
			for(j=0; j<16; j++)
			{
				//If plaintext is a printable char, then print
				if((32 <= plaintext[j] && plaintext[j] <= 127) || plaintext[j] == '\n')
				{
					printf("%c", plaintext[j]);
				}
				//otherwise it should be padding
				else
				{
					pad_count = plaintext[15];
					//test to make sure pad_count is within the exceptable range.
					//if not then throw error
					if(!(1 <= pad_count && pad_count <= 16))
					{
						perror("\nbyte decrypted to a non-printable char and invalid padding configuration detected\n");
						fclose(fpin);
						return -1;
					}
					//test to make sure the padding starts where expected.
					else if(15-pad_count+1 != j)
					{
						perror("\ncipher decrypted to unprintable chars inconsistent with padding scheme\npadding scheme may be invalid, or plaintext message inconsistent with expected form\n");
						fclose(fpin);
						return -2;
					}
					//test to make sure the last pad_count bytes in the block have value pad_count
					else
					{
						for(k=0; k<pad_count; k++)
						{
							if(plaintext[15-k] != pad_count)
							{
								flag = 1;
							}							
						}
						//if test fails print error
						if(flag == 1)
						{
							perror("\npadding appears to be invalid\n");
							fclose(fpin);
							return -3;
						}
						//else padding is valid, and we're done printing
						//so set a flag saying we found good padding and break
						//the for loop
						else
						{
							pad_test = 1;
							break;
						}
						
					}
				}
			}
		}
		i++;
		//if padding is detected and verified but we're not at the end
		//of the file then something went wrong
		if(pad_test ==1)
		{
			if(fscanf(fpin, "%2hhx", &ch) != EOF)
			{
				perror("End of file expected when valid padding scheme detected, but EOF not found\n");
				fclose(fpin);
				return -4;
			}
			else
			{
				break;
			}
		}
	}
	
	if(i%16 != 0)
	{
		perror("Input is not a multiple of the block size. Please verify input\n");
		fclose(fpin);
		return -5;
	}


	//if padding not detected then throw error
	if(pad_test == 0)
	{
		perror("Padding not detected");
		fclose(fpin);
		return -6;
	}

	fclose(fpin);
	return 0;
}

//Encrypts a single 128 bit block using aes and a 128 bit key
void aes_encrypt_128(unsigned char *in_block, unsigned char *out_block, unsigned char *key_buf)
{
	AES_KEY EncKey;
	AES_set_encrypt_key(key_buf, 128, &EncKey);
	AES_encrypt(in_block, out_block, &EncKey);
	return;
}

//Decrypts a single 128 bit block using aes and a 128 bit key
void aes_decrypt_128(unsigned char *in_block, unsigned char *out_block, unsigned char *key_buf)
{
	AES_KEY DecKey;
	AES_set_decrypt_key(key_buf, 128, &DecKey);
	AES_decrypt(in_block, out_block, &DecKey);
	return;
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

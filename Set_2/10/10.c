#include <openssl/aes.h>
#include <stdio.h>

/*
 *Synopsis:
 * This program was written as a solution to the Matasano Cryptography Challenges
 * See http://cryptopals.com
 * Encrypts or decrypts a file using AES-128 in CBC mode.  Encrypted files are 
 * hex encoded.  
 * Note this code should never be used in a production environment.  It is intended
 * to be used solely for educational purposes. 
 */

void aes_encrypt_128(unsigned char *in_block, unsigned char *out_block, unsigned char *key_buf);
void aes_decrypt_128(unsigned char *in_block, unsigned char *out_block, unsigned char *key_buf);
int pkcs_7_pad(unsigned char *block, unsigned int pad_count, unsigned int block_size);
int decrypt_cbc();
int encrypt_cbc();

int main()
{
	//ask the user whether they want to encrypt or decrypt
	int num;
	printf("Press 0 to decrypt or 1 to encrypt:\n");
	scanf("%d", &num);
	if(num==0)
	{
		return decrypt_cbc();
	}
	else if(num==1)
	{
		
		return encrypt_cbc();
	}
	else
	{
		perror("Really?  You can't type 0 or 1?\n");
		return(2);
	}
}


int decrypt_cbc()
{
	//Declarations
	FILE *fpin, *fpout;
	unsigned int i=0, j=0, k=0;
	unsigned char ch;
	unsigned int pad_count, flag = 0, pad_test =0;
	unsigned char IV[16]={0}, plaintext[16], ciphertext[16], key[] = "YELLOW SUBMARINE";
		
	//Body of program starts here
	
	//open FILE
	fpin = fopen("hex_encoded.txt", "r");
	//if fpin is 0 then opening the file failed.  Handle it.
	if(fpin == 0)
	{
		perror("File open failed\n");
		return -10;
	}
	fpout = fopen("plaintext.txt", "w");
	//if fpout is 0 the throw an error
	if(fpout == 0)
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
				plaintext[j] = plaintext[j]^IV[j];
				IV[j] = ciphertext[j];
			}
			
			
			for(j=0; j<16; j++)
			{
				//If plaintext is a printable char, then print
				if((32 <= plaintext[j] && plaintext[j] <= 127) || plaintext[j] == '\n')
				{
					printf("%c", plaintext[j]);
					fprintf(fpout, "%c", plaintext[j]);
				}
				//otherwise it should be padding
				else
				{
					pad_count = plaintext[15];
					//test to make sure pad_count is within the exceptable range.
					//if not then throw error
					if(!(1 <= pad_count && pad_count <= 16))
					{
						printf("\n\nbyte decrypted to a non-printable char:%02x \n outside range of acceptable pad values\n\n", pad_count);
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
				perror("End of file expected when correct padding scheme detected, but EOF not found");
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
		perror("\n\nInput is not a multiple of the block size. Please verify input\n");
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

int encrypt_cbc()
{
	FILE *fpin, *fpout;
	unsigned int i=0, j;
	unsigned char ch;
	unsigned char plaintext[16], ciphertext[16] = {0}, key[] = "YELLOW SUBMARINE"; //ciphertext should be initialized with the IV
	
	
	fpin = fopen("plaintext.txt", "r");
	if(fpin == 0)
	{
		perror("failed to open input file");
		return -10;
	}
	fpout = fopen("hex_encoded.txt", "w");
	if(fpout == 0)
	{
		perror("failed to open output file");
		return -10;
	}
	while(fscanf(fpin, "%c", &ch) != EOF)
	{
		plaintext[i%16] = ch;
		//if we've read 16 characters then xor encrypt and write to file
		if((i!=0) && (i%16 == 15))
		{		
			//in cbc mode we'll xor with last plaintext before encryption
			for(j=0; j<16; j++)
			{
				printf("%c", plaintext[j]);
				plaintext[j] = plaintext[j]^ciphertext[j];
			}
			//encrypt the block after xor
			aes_encrypt_128(plaintext, ciphertext, key);		
			//print the block to file
			for(j=0; j<16; j++)
			{
				fprintf(fpout, "%02x", ciphertext[j]);
			}
		}
		i++;
	}
	//pad the last block.
	printf("pad count is %d\n", 16-(i%16));
	if(pkcs_7_pad(plaintext, 16-(i%16), 16)!=0)
	{
		printf("Padding failed");
		return -8;
	}
	//xor with last ciphertext
	printf("last block:\n");
	for(j=0; j<16; j++)
	{
		printf("%c", plaintext[j]);
		plaintext[j] = plaintext[j]^ciphertext[j];
	}
	aes_encrypt_128(plaintext, ciphertext, key);
	//print last block to file
	for(j=0; j<16; j++)
	{
		fprintf(fpout, "%02x", ciphertext[j]);
	}
	fclose(fpin);
	fclose(fpout);
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

int pkcs_7_pad(unsigned char *block, unsigned int pad_count, unsigned int block_size)
{
	unsigned int i = 0;
	if(!(0<pad_count && pad_count<=block_size))
	{
		printf("invalid value for pad_count");
		return -1;
	}
	i = pad_count;
	while(i > 0)
	{
		block[block_size - i] = pad_count;
		i = (i-1);
	}
	return 0;
}

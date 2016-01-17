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

int main()
{
	FILE *fpin, *fpout;
	unsigned int i=0;
	unsigned int j=0;
	unsigned char ch;
	unsigned int pad_count;
	fpin = fopen("hex_encoded.txt", "r");
	if(fpin == 0)
	{
		printf("File open failed\n");
		return 1;
	}
	unsigned char ciphertext[16];
	unsigned char plaintext[16], key[] = "YELLOW SUBMARINE";
	//plaintext[17] = "\0"; //terminate string
	printf("holla\n");
	
	while((fscanf(fpin, "%2hhx", &ch) != EOF))
	{
		ciphertext[(i%16)] = ch;
		//printf("%02x", ch);
		if(i%16 == 15)//if we've filled 16 elements of ciphertext then decrypt and print.
		{
			//printf("%02x\n", ch);
			aes_decrypt_128(ciphertext, plaintext, key);
			
			for(j=0; j<16; j++)
			{
				printf("%c", plaintext[j]);
			}
			//printf("%s", plaintext);
		}
		i++;
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

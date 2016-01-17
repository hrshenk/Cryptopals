/*Synopsis:  Two functions are defined.  One to perform 128 bit key aes encryption of a 16 byte char array.
 * The other one performs the corresponding decryption.
 * Note, this code is not intended for use in any production grade code.  It is only intended for educational
 * purposes and to aid in solving the Matasano Cryptopals challenges.
 */

#include <openssl/aes.h>
#include <stdio.h>

void aes_encrypt_128(unsigned char *in_block, unsigned char *out_block, unsigned char *key_buf);
void aes_decrypt_128(unsigned char *in_block, unsigned char *out_block, unsigned char *key_buf);

void AES_decrypt(const unsigned char *in, unsigned char *out,
	const AES_KEY *key);

int main()
{
	int i=0;
	unsigned char key[] = "YELLOW SUBMARINE";
	//plaintext array is 17 bytes.  16 for the data to be encrypted and the 17th to mark the end of a string.
	//note, aes_encrypt_128, and aes_decrypt_128 only use the 1st 16 bytes of plaintext[].
	unsigned char plaintext[17] = {'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a', '\0'};
	unsigned char ciphertext[16];
	aes_encrypt_128(plaintext, ciphertext, key);
	for(i=0; i<16; i++)
		{
			printf("%02x\n", ciphertext[i]);
			plaintext[i] = '\0';
		}
	aes_decrypt_128(ciphertext, plaintext, key);
	printf("%s\n", plaintext);
			
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



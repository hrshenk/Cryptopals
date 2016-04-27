/*This was written as a solution to the Matasano Crypto challenges challenge 16.  
See http://cryptopals.com  This is not production quality code and should never be used
in a production environment. 

This program consumes the token created by the create_token module (potentially after its
been modified by the 'user'), decrypts it, and searches for the string "admin=true".  If the
string is present, the attack was successful.
*/


#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>

int decrypt_cbc(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext);
void handleErrors(void);

int main()
{
    unsigned char key[] = "YellowSubmarine", iv[] = "random_IV_so_fun";
    FILE *fp_in;
    unsigned int ciphertext_len, i, plaintext_len;
    unsigned char *ciphertext, *plaintext;
    if(!(fp_in = fopen("token.txt", "r")))
    {
        return 0;
    }
    
    fseek(fp_in, 0, SEEK_END);
    ciphertext_len = ftell(fp_in);
    rewind(fp_in);
    ciphertext_len = ciphertext_len/2;
    ciphertext = (char*)calloc(ciphertext_len, sizeof(char));
    plaintext = (char*)calloc(ciphertext_len, sizeof(char));
    for(i=0; i<ciphertext_len; i++)
    {
        fscanf(fp_in,"%2hhx", ciphertext+i);
    }
    
    plaintext_len = decrypt_cbc(ciphertext, ciphertext_len, key, iv, plaintext);
    plaintext[plaintext_len] = '\0';
    BIO_dump_fp (stdout, (const char *)plaintext, plaintext_len);
    fclose(fp_in);
    return 0;
}



int decrypt_cbc(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int plaintext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
    handleErrors();

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors();
  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}

void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}
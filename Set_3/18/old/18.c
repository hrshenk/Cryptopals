#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

int decrypt_ctr(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *nonce, unsigned char *plaintext);
void handleErrors(void);

int main()
{
    const unsigned char* encoded_ciphertext= "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==";
    unsigned char* decoded_ciphertext = (unsigned char*) calloc(strlen(encoded_ciphertext), sizeof(char));
    int ciphertext_len = base64_decode(encoded_ciphertext, decoded_ciphertext);
    unsigned char* plaintext = (unsigned char*) calloc(ciphertext_len+1, sizeof(char));
    unsigned char *key = "YELLOW SUBMARINE", nonce[16] = {0};
    decrypt_ctr(decoded_ciphertext, ciphertext_len, key, nonce, plaintext);
    puts(plaintext);
    return 0;
}

int decrypt_ctr(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *nonce, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;
  int len = 0, plaintext_len = 0;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, nonce))
    handleErrors();

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors();
  plaintext_len += len;
 
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


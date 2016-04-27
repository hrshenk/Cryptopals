/*This was written as a solution to the Matasano Crypto challenges challenge 16.  
See http://cryptopals.com  This is not production quality code and should never be used
in a production environment.
This program accepts user input and creates a string.  The string is then encrypted using
aes in cbc mode to create a 'token' which is provided to the 'user'.  The encrypted token 
is then consumed by another program (see accept_token.c), and decrypted.  Note, the user
input eats the '=' and ';' characters to avoid users adding additional fields to the 
token at input.  However, via manipulation of the encrypted token we'll be able to add
fields anyhow!  This requires no knowledge of the key, or the IV used for encryption.
See README.txt
*/

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>

#define Input_Size 5000 //limits number of chars read by fgets, then buffers are defined via this value.

int encrypt_cbc(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
void handleErrors(void);


int main()
{
    char prepend[] = "comment1=cooking%20MCs;userdata=", prepend_len;
    char append[] = ";comment2=%20like%20a%20pound%20of%20bacon";
    char user_input[Input_Size] = {0}, *plaintext, *ciphertext;
    unsigned int i;
    unsigned char key[] = "YellowSubmarine", iv[] = "random_IV_so_fun";
    int ciphertext_len;
    FILE *fp_out;
    
    
    puts("Enter user data");
    fgets(user_input, Input_Size, stdin);
    if(user_input[strlen(user_input)-1]=='\n')
    {
        user_input[strlen(user_input)-1] = '\0';
    }
    //we allocate 3 times the user_input length since space characters are replaced with %20
    //the URL encoding of spaces was done simply so that our encrypted strings were consistent
    //with the strings provided in the exercise.
    plaintext = (char *)calloc((3*strlen(user_input)) + 100, sizeof(char));
    prepend_len = strlen(prepend);
    strncpy(plaintext, prepend, prepend_len);
    i=0;
    while(user_input[i] != '\0')
    {
        if(user_input[i] != ';' && user_input[i] != '=')
        {
            if(user_input[i] == ' ')
            {
                strncpy(plaintext + strlen(plaintext), "%20", 3);
            }
            else
            {
                plaintext[strlen(plaintext)] =  user_input[i];
            }
        }
        i++;
    }
    strncpy(plaintext + strlen(plaintext), append, strlen(append));
    //BIO_dump_fp (stdout, (const char *)plaintext, strlen(plaintext));
    ciphertext = (char*)calloc(strlen(plaintext)+16, sizeof(char));
    ciphertext_len = encrypt_cbc(plaintext, strlen(plaintext), key, iv, ciphertext);
    //BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);
    //the encrypted token will be written to token.txt.  accept token will read the 
    //token from token.txt
    if(!(fp_out = fopen("token.txt","w")))
    {
        return 0;
    }

    for(i=0; i<ciphertext_len; i++)
    {
        fprintf(fp_out, "%02X", (unsigned char)ciphertext[i]);
    }
    fclose(fp_out);
    return 0;
}


int encrypt_cbc(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int ciphertext_len =0;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
    handleErrors();

  /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  ciphertext_len += len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
  ciphertext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}
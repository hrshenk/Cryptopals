/*This code was written as a solution to the Matasano Cryptography challenges, challenge 13.  See Cryptopals.com.  
This is not production quality code and shouldn't really be used for...well...much of anything.

This program prompts for an email address foo@bar.com and creates a user profile encoded as email=foo@bar.com&uid=10&role=user.
Then the profile is encrypted using a fixed unknown key under ECB mode and the encrypted 'token' is provided to the user.
It is possible to craft a token for an admin role simply from other tokens produced using input by the user.
*/



#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <stdio.h>
#include <string.h>

#define input_len 50

int encrypt_ecb(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext);
void handleErrors(void);

typedef struct profile{
    unsigned char email[input_len];
    unsigned int uid;
    unsigned char role[6];
    
}profile;

/*
profile_for will encode and email address foo@bar.com and produce a profile 
in the format email=foo@bar.com&uid=10&role=user.
The characters & and = are not permitted in the submitted email
*/
int profile_for(char *input, char *output);
int create_token();


int main()
{
    create_token();
    return 0;
}

int create_token()
{
   char user_input[input_len] = {0}, encoded_profile[200] = {0}, key[16] = {0};
   unsigned char token[200];
   int i, cipher_len, plaint_len;
   
   puts("Please enter your email address to receive a token.");
    //Get user input.  perform some simple checks and return if checks fail.
    if(fgets(user_input, input_len, stdin))
    {
        if(user_input[strlen(user_input)-1] == '\n')
        {
           user_input[strlen(user_input)-1] = '\0';
        }
    }
    else
    {
        puts("Error");
        return 0;
    }
    
    puts("input is:");
    puts(user_input);
    if(!profile_for(user_input, encoded_profile))
    {
        puts("Fresh out of profiles...I guess I'm more popular than I thought.\n");
        return 0;
    }
    puts("profile:");
    printf("%s", encoded_profile);
    //puts(encoded_profile);
    plaint_len = strlen(encoded_profile);
    printf("plaintext len is:  %d\n", plaint_len);
    cipher_len = encrypt_ecb(encoded_profile, plaint_len, key, token);
    puts("Here is your profile token:");
    for(i=0; i<cipher_len; i++)
    {
        printf("%02X", token[i]);
    }
    puts("");
    return 0;
}

int profile_for(char *input, char *output)
{
    unsigned int i=0;
    char temp[5];
    static unsigned char uid = 0;
    if(uid + 1 == 0) return 0;
    
    puts("input is:");
    puts(input);
    
    strcat(output, "email=");

    puts(output);
    
    for(i=0; i < strlen(input); i++)
    {
        if(input[i] != '=' && input[i] != '&')
        {
            output[i+6] = input[i];
            printf("%c\n", input[i]);
        }
        
    }
    puts(output);
    strcat(output, "&uid=");
    sprintf(temp, "%u", uid);
    uid++;
    strcat(output, temp);
    puts(output);
    strcat(output, "&role=user");
    puts(output);
    return 1;
}

/*******************************************************************************************************/
//Encryption functions start here
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

/*************************************************************************************/
void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}
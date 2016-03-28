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

char key[16] = {0};  //For this attack the key can be anything...so long as the same key is used to encrypt multiple profiles.
int encrypt_ecb(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext);
int decrypt_ecb(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,unsigned char *plaintext);
void handleErrors(void);

/*
profile_for will encode and email address foo@bar.com and produce a profile 
in the format email=foo@bar.com&uid=10&role=user.
The characters & and = are not permitted in the submitted email
*/
int profile_for(char *input, char *output);
/*
the create token function accepts user input, and calls the profile for function
to create a profile.  It then encrypts the profile and provides this token to the user
*/
int create_token();
/*
the accept token function accepts a string, converts it to a hex array, then decrypts the 'token'
it then performs some format checks and parses the user's profile
*/
int accept_token();

int ascii_to_hex(char *token_s, unsigned char *token_h);


int main()
{
    int ch, flag;
    while(flag == 0)
    {
        puts("Press 1 to get a token for your profile, or 2 to submit a token for login:");
        fscanf(stdin, "%d", &ch);
        while(getchar() != '\n') /*this clears the input buffer for fgets used later*/;
        
        if(ch == 1)
        {
        create_token();
        }
        else
        {
            if(ch == 2)
            {
                accept_token();  
            }
            
            else puts("Invalid choice.  Get your life together already!!!");
            
        }
        
        puts("press 1 to keep at it or 2 to quit");
        fscanf(stdin, "%d", &flag);
        flag = flag - 1;
        while(getchar() != '\n') /*this clears the input buffer for fgets et al. used later*/;
    }
    return 0;
}

int create_token()
{
   char user_input[input_len] = {0}, encoded_profile[200] = {0};
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
    
    if(!profile_for(user_input, encoded_profile))
    {
        puts("Fresh out of profiles...I guess I'm more popular than I thought.\n");
        return 0;
    }
    
    plaint_len = strlen(encoded_profile);
    cipher_len = encrypt_ecb(encoded_profile, plaint_len, key, token);
    puts("Here is your profile token:\n");
    for(i=0; i<cipher_len; i++)
    {
        printf("%02X", token[i]);
    }
    puts("\n");
    return 0;
}

int profile_for(char *input, char *output)
{
    unsigned int i=0;
    char temp[5];
    static unsigned char uid = 0;
    if(uid + 1 == 0) return 0;
    strcat(output, "email=");
    
    //eats '=' and '&' chars in user input.
    for(i=0; i < strlen(input); i++)
    {
        if(input[i] != '=' && input[i] != '&')
        {
            output[i+6] = input[i];
        }
        
    }

    strcat(output, "&uid=");  //ignore use of strcat throughout, generally should use strncat as a best practice.
    sprintf(temp, "%u", uid);
    uid++;
    strcat(output, temp);
    strcat(output, "&role=user");
    return 1;
}

int accept_token(unsigned char *token)
{
    int in_len, token_len, i, profile_len;
    char user_input[400];
    unsigned char hex_token[200], encoded_profile[200];
    puts("please submit your token");
    fgets(user_input, 400, stdin);
    in_len = strlen(user_input);
    user_input[(in_len-1)] = '\0';
    if((token_len = ascii_to_hex(user_input, hex_token)) == 1)
    {
        puts("invalid token");
        return 0;
    }
    profile_len = decrypt_ecb(hex_token, token_len, key, encoded_profile);
    encoded_profile[profile_len] = '\0';
    puts(encoded_profile);
}

int ascii_to_hex(char *token_s, unsigned char *token_h)
{
    char *dummy, converter[2];
    int i, j=0, len;
    len = strlen(token_s);

    if(len % 2 != 0)
    {
        return 1;
    }
    else
    {
        for(i=0; i<len; i++)
        {
            converter[0]= token_s[i];
            i++;
            converter[1]= token_s[i];
            token_h[j] = strtoul(converter, &dummy, 16);
            j++;
            
        }
    }
    return j;
}

/*******************************************************************************************************/
//Encryption functions start here
int encrypt_ecb(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;
  int len = 0, cipher_len = 0;
  

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
  {
    handleErrors();
  }
  
  cipher_len += len;
  /* Finalise the encryption.
   * EVP_EncryptFinal_ex will handle the padding, then encrypt the last block.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + cipher_len, &len)) 
  {
      handleErrors();
  }
  
  cipher_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return cipher_len;
}

/*************************************************************************************/
int decrypt_ecb(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;

  int len = 0;

  int plaintext_len = 0;


  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) 
  {
     handleErrors();
  }

  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL))
    {
        handleErrors();
    }

  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    {
        handleErrors();
    }
  plaintext_len += len;
  
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) 
  {
      handleErrors();
  }
  
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}

/*************************************************************************************/
void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}
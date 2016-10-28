#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "crypto_stream_aes128ctr.h"
#include "26.h"

#define Input_Size 5000 //limits number of chars read by fgets, then buffers are defined via this value.

void create_token(token_t *token)
{
    char prepend[] = "comment1=cooking%20MCs;userdata=", prepend_len;
    char append[] = ";comment2=%20like%20a%20pound%20of%20bacon";
    char user_input[Input_Size] = {0}, *plaintext, *ciphertext;
    unsigned int i;
    unsigned char key[] = "YellowSubmarine", iv[] = "random_IV_so_fun";
    puts("Enter user data (enter bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb for best results):");
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
    ciphertext = (char*)calloc(strlen(plaintext)+16, sizeof(char));
    crypto_stream_aes128ctr_core2_xor(ciphertext,plaintext,strlen(plaintext),iv,key);
    token->tkn = ciphertext;
    token->len = strlen(plaintext);
}
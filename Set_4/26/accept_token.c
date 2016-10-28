#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "crypto_stream_aes128ctr.h"
#include "26.h"

int accept_token(token_t *token)
{
    unsigned char key[] = "YellowSubmarine", iv[] = "random_IV_so_fun";
    unsigned int ci, j=0, i;
    unsigned char *plaintext, admin[]="admin=true";
    plaintext = (char*)calloc(token->len, sizeof(char));
    crypto_stream_aes128ctr_core2_xor(plaintext,token->tkn,token->len,iv,key);
    //test for presence of string
    for(i=0; i<token->len; i++)
    {
        if(plaintext[i]=='a')
        {
            j=0;
            while(plaintext[i]==admin[j])
            {
                i++;
                j++;
            }
            
        }
        if(j>=10)
        {
            puts("Oh SNAP!  Now you've got the power too.");
        }
    }
    return 0;
}
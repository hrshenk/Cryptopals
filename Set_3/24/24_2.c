#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

unsigned char *get_token(unsigned char *username);

int main()
{
    unsigned char *username = "Steve.Polychronopolous";
    unsigned char *token = get_token(username), pt[200];
    unsigned int i, flag = 1;
    
    //if the token is a string of chars containing our username encrypted with MT19937 stream
    //seeded with time(NULL) then we can easily decrypt it to find the structure of the 
    //plaintext token, and possibly mimick tokens for other users.
    for(i=0; i < (2<<16)-1; ++i)
    {
        bzero(pt, sizeof(pt));
        mt_encrypt(token, 200, pt, i);
        if( strstr(pt, username) )
        {
            puts("Such insecure!  Wow!");
            flag = 0;
            break;
        }
    }
    if(flag) puts("Mebby next time when you're l33t.");
    return 0;
}

unsigned char *get_token(unsigned char *username)
{
    unsigned char *token;
    token = (unsigned char *)calloc(1000, sizeof(char));
    //put some stuff in the token then encrypt
    strcpy(token, "random or unkown data");
    strcpy(token + strlen(token), username);
    strcpy(token + strlen(token), "bippity boppity boo, reset = true");
    
    mt_encrypt(token, strlen(token), token, time(NULL));
    
    return token;
}
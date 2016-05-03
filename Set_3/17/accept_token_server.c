#include "encryptutil.h"
#include <stdio.h>
#include <stdlib.h>

extern unsigned char key[16];

int accept_token_server()
{
    FILE *fp_in;
    fp_in = fopen("token.txt", "r");
    //determine the length of the file
    fseek(fp_in, 0, SEEK_END);
    int token_len = ftell(fp_in);
    //file is hex encoded so we only need half the file length for allocation
    token_len = token_len/2;
    rewind(fp_in);
    //allocate space for the ciphertext token and plaintext
    unsigned char *ciphertext = (unsigned char *)calloc(token_len, sizeof(char));
    unsigned char *plaintext = (unsigned char *)calloc(token_len, sizeof(char));
    int i;
    //read the file into a buffer
    for(i=0; i<token_len; i++)
    {
        fscanf(fp_in, "%2hhx", ciphertext+i);
    }
    //the first 16 bytes are our prepended iv
    unsigned char iv[16];
    for(i=0; i<16; i++)
    {
        iv[i]=ciphertext[i];
    }
    //decrypt the ciphertext
    int plaintext_len = decrypt_cbc(ciphertext+16, token_len-16, key, iv, plaintext);

    return pkcs7_check_strip(plaintext, token_len-16);
}
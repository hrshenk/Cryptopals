#include "crypto_stream_aes128ctr.h"
#include <stdlib.h>
#include <stdio.h>

void file_encrypt(FILE *fp_in, const unsigned char *key, const unsigned char *nonce, FILE *fp_out)
{
    //determine the length of the file to encrypt
    fseek(fp_in, 0, SEEK_END);
    unsigned int len = ftell(fp_in);
    rewind(fp_in);
    len = len - ftell(fp_in);
    //allocate and generate a ctr keystream
    unsigned char *key_stream = (unsigned char *)calloc( len, sizeof(char) );
    crypto_stream_aes128ctr(key_stream,len,nonce,key);
    //encrypt the contents of the file.
    unsigned int i;
    unsigned char temp;
    for(i=0; i<len; ++i)
    {
        fread(&temp, sizeof(char), 1, fp_in);
        temp = key_stream[i] ^ temp;
        fwrite(&temp, sizeof(char), 1, fp_out);
    }
}
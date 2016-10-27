#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include "crypto_stream_aes128ctr.h"
#include <string.h>

int edit(FILE *encrypted_file, unsigned int index, 
                unsigned int length, unsigned char *buf, unsigned char *key, unsigned char *nonce)
{
    unsigned int d = index/16, r = index % 16, i;
    unsigned char *key_stream = (unsigned char *)calloc(length+20, sizeof(char));
    unsigned char temp, temp_nonce[16];
    memcpy(temp_nonce, nonce, 16); //we want to preserve the passed in nonce value.
    //it seems counter is big endian so we'll construct the nonce appropriately
    d = htonl(d);  //force big endian, then fill nonce.
    for(i=0; i<4; ++i)
    {
        temp_nonce[15-i] = ( d >> (8*(3-i) ) ) & 0xff;
    }
    //generate just enough key_stream
    crypto_stream_aes128ctr(key_stream, length + 16, temp_nonce, key);
    //seek into file
    fseek(encrypted_file, index, SEEK_SET);
    //encrypt and write new data to disk
    for(i=0; i<length; ++i)
    {
        temp = key_stream[i+r] ^ buf[i];
        fwrite(&temp, sizeof(char), 1, encrypted_file);
    }
}

int read_disk(unsigned int index, unsigned int length, unsigned char *read_buffer, FILE *fp_in)
{
    fseek(fp_in, index, SEEK_SET);
    fread(read_buffer, sizeof(char), length, fp_in);
}
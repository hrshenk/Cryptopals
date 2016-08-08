#include "crypto_stream_aes128ctr.h"
#include <stdlib.h>

int funky_nonce_ctr(unsigned char *message_out, unsigned char *message_in, unsigned int message_len, unsigned char *nonce, const unsigned char *key)
{
    unsigned long long counter = 0;
    
    //extract the counter from nonce and place it in an easily incrementable ulonglong
    int i;
    for(i=0; i<8; ++i)
    {
        counter += ((unsigned long long int)nonce[15-i] << 8*i);
    }
    
    int j;
    for(j=0; j < (message_len/16); ++j)
    {
        //build plaintext keystream in format specified by the challenges
        for(i=0; i<8; i++)
        {
            nonce[8+i] = (counter >> 8*i) & 0xff;
        }

        //generate next 16 bytes of keystream and xor with message_in
        crypto_stream_aes128ctr_core2_xor(message_out + j*16, message_in+j*16, 16,nonce,key);
        ++counter;
        
    }
    
    //build final block of plaintext for keystream
    for(i=0; i<8; i++)
    {
        nonce[8+i] = (counter >> 8*i) & 0xff;
    }
    //generate last block of keystream and xor remaining unprocessed bytes
    crypto_stream_aes128ctr_core2_xor(message_out + j*16, message_in+j*16, message_len%16 ,nonce,key);
    return 0;
}
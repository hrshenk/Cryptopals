#include "crypto_stream_aes128ctr.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{

    const unsigned char key[] = "YELLOW SUBMARINE";
    //the field i'm calling nonce is actually the plaintext stream encrypted by aes.
    //so it is actually (8 byte nonce) || (8 byte counter) where both nonce and counter are little endian.
    unsigned char nonce[16] = {0};
    unsigned long long message_len, count = 0;
    const unsigned char* encoded_message_in = "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==";
    
    //this will be the message we're encrypting or decrypting.
    unsigned char* decoded_message_in = (unsigned char *)calloc(strlen(encoded_message_in), sizeof(char));
    message_len = base64_decode(encoded_message_in, decoded_message_in);
    //this will be the output of our cipher
    unsigned char* message_out = calloc(message_len+1, sizeof(char));
    
    int i;
    int j;
    
    for(j=0; j < (message_len/16); ++j)
    {
        //build plaintext keystream according to challenge specifications
        for(i=0; i<8; i++)
        {
            nonce[8+i] = (count << 8*i) & 0xff;
        }

        //generate next 16 bytes of keystream and xor with message_in
        crypto_stream_aes128ctr_core2_xor(message_out + j*16, decoded_message_in+j*16, 16,nonce,key);
        ++count;
    }
    
    //build final block of plaintext for keystream
    for(i=0; i<8; i++)
    {
        nonce[8+i] = (count << 8*i) & 0xff;
    }
    //generate last block of keystream and xor remaining unprocessed bytes
    crypto_stream_aes128ctr_core2_xor(message_out + j*16, decoded_message_in+j*16, message_len %16 ,nonce,key);
    
    puts(message_out);
    
    return 0;
}
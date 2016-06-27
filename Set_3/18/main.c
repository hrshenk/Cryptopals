#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int funky_nonce_ctr(unsigned char *message_out, unsigned char *message_in, unsigned int message_len, unsigned char *nonce, const unsigned char *key);

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
    funky_nonce_ctr(message_out, decoded_message_in, message_len, nonce, key);
    puts(message_out);
    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "encryptutil.h"

unsigned char key[16] = {0};

int create_token_server()
{
    const char* strings[10];
    strings_init(strings);  //fills the input array with pointers to the strings provided for this challenge.
    
    const char *encoded_string = strings[(rand() % 10)]; //select one of the strings at random
    char *decoded_string = (char *)calloc(strlen(encoded_string), sizeof(char));
    int decoded_len = base64_decode(encoded_string, decoded_string);  //decode the selected string
    
    generate_key(key);//generate a 16 byte key to use for encryption and decryption.
    unsigned char iv[16];
    generate_key(iv);  //generate an initialization vector
    //allocate memory for the ciphertext
    unsigned char *ciphertext = (unsigned char *) calloc(decoded_len + 32, sizeof(char));
    //encrypt the selected string.  note, encrypt_cbc will handle the padding.
    int ciphertext_len = encrypt_cbc(decoded_string, decoded_len, key, iv, ciphertext+16);
    int i;
    //prepend the initialization vector to the ciphertext
    for(i=0; i<16; i++)
    {
        ciphertext[i] = iv[i];
    }
    //print the ciphertext for now.  update later to write it to a file or something.
    for(i=0; i<(ciphertext_len+16); i++)
    {
        printf("%02x\n", ciphertext[i]);
    }
    
    return 0;
}
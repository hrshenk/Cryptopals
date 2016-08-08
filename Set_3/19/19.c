#include <stdio.h>

#define MAX_LEN 1000

int main()
{
    FILE *fp_in = fopen("19.txt", "r");
    FILE *fp_urand = fopen("/dev/urandom", "r");
    FILE *fp_out = fopen("19_cipher.txt", "w");
    char encoded_message[MAX_LEN] = {0}, decoded_message[MAX_LEN] = {0}, encrypted_message[MAX_LEN];
    int message_len, i;
    char nonce[16], key[16];
    
    //generate a random key
    for(i=0; i<16; ++i)
    {
        fscanf(fp_urand, "%c", &key[i]);
    }
    fclose(fp_urand);
    
    while(fgets(encoded_message, MAX_LEN, fp_in) != NULL)  //read in a line
    {
        
        message_len = base64_decode(encoded_message, decoded_message);  //decode the line

        //build our trusty nonce...no one can tell its being reused  :)
        for(i=0; i<16; i++)
        {
            nonce[i]=0;
        }
        
        funky_nonce_ctr(encrypted_message, decoded_message, message_len, nonce, key); //encrypt the decoded message
        
        //print the ciphertext
        for(i=0; i<message_len; ++i)
        {
            fprintf(fp_out,"%02hhx", encrypted_message[i]);
            printf("%02hhx", encrypted_message[i]);
            
        }
        
        printf("\n");
        fprintf(fp_out, "%s", "\n");
        
    }
    
    fclose(fp_in);
    fclose(fp_out);
    
    return 0;
}
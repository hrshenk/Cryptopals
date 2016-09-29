//This program illustrates using a known plaintext 
//to easily recognize a decrypted cipher text
//that was encrypted using an MT19937 stream cipher seeded with a weak key

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    unsigned char ct[1000], pt[1000];
    server(ct);  //server will provide us with a ciphertext
    unsigned int len = *(unsigned int *)&ct[0], i;
    
    for(i=0; i < (2<<16)-1; ++i)
    {
        bzero(pt, sizeof(pt));
        mt_encrypt(ct+4, len, pt, i);
        if( strstr(pt, "AAAAAAAAAAAAAA") )
        {
            break;
        }
    }
    printf("Winner!  Mersenne was seeded with: 0x%04x\n", i);

    
}

int server(unsigned char *ct)
{
    FILE *fp = fopen("/dev/urandom", "r");
    if(fp==NULL) 
    {
        puts("file open fail");
        return -1;
    }
    unsigned int i, len;
    unsigned short key;  //16 bit key
    unsigned char pt[1000], r;
    fread( &r, sizeof(char), 1, fp );
    r = (r % 250) + 1; //we'll append between 1 and 250 bytes to the supplied string.
    
    i=0;
    while(i < r)  //prepend only nonzero bytes...we're assuming the plaintext is a character string
    {             //this will make searching for our known plaintext just slightly easier without detracting from the intent of the exercise.
        fread( &pt[i], sizeof(char), 1, fp );
        if(pt[i]) ++i;
    }
    strncpy(&pt[r], "AAAAAAAAAAAAAA", 15); //15 will terminate it in 0
    len = strlen(pt);  //nice decision made above
    fread( &key, sizeof(short), 1, fp );  //generate a key
    mt_encrypt(pt, len, &ct[4], key );
    for(i=0; i<4; ++i)  //prepend the ct length
    {
        ct[i] = ((char *) &len)[i];
    }
    
    return 0;

}
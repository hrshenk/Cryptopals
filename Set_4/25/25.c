#include "crypto_stream_aes128ctr.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned char key[16];
unsigned char nonce[16];


int main()
{
    //generate random key and nonce
    FILE *fp_urand = fopen("/dev/urandom", "r");
    fread(key, sizeof(char), sizeof(key), fp_urand);
    fread(nonce, sizeof(char), sizeof(nonce)/2, fp_urand);
    //encrypt the file
    FILE *fp_in = fopen("25.txt", "r"), *encrypted_file;
    encrypted_file = fopen("25_cipher.txt", "w+");
    file_encrypt(fp_in, key, nonce, encrypted_file);
    //expose the "api"...yes it is a silly and very insecure api
    //yes I made this more complicated than i actully needed to.
    //but I wanted to play with sockets and a pretend api.
    int sock = 0;
    //server returns a socket connected to the user
    sock = server();
    if(sock < 0)
    {
        puts("server failure");
        return -1;
    }
    unsigned char buffer[20000] = {0}, *buf, *read_buf;
    buf = buffer;
    unsigned int index;
    int length; 
    while(1)
    {
        
        buf = buffer;
        memset(buffer, 0, sizeof(buffer));
        if(read(sock, buf, 2000))
        {
            //expose the edit functionality.
            if(!memcmp(buf, "edit:", 5))
            {
                puts("edit");
                index = atoi(buf+5);
                while(*buf != ',')  //so safe.
                {
                    buf++;
                }
                buf++;
                if(!memcmp(buf, "length:",7))
                {
                    buf += 7;  //buf should now point to the length value.
                    length = atoi(buf);
                }
                else
                {
                    puts("Uh oh, where is muh length?");
                    return -1;
                }
                while(*buf != ',')  //so safe.
                {
                    buf++;
                }
                buf++; //edit:1234,new:somenewtext buf++ should point to 'n'
                if(!memcmp(buf, "new:",4))
                {
                    buf += 4;  //buf should now point to the replacement text.
                }
                else
                {
                    puts("Uh Oh, no new text error");
                    return -1;
                }
                printf("Editing %d chars at location %d with text *%s*\n", length, index, buf);
                edit(encrypted_file, index, length, buf, key, nonce);
            }
            //expose the read functionality.  this exposes the encrypted disk contents
            else if(!memcmp(buf, "read:", 5))
            {
                puts("readin");
                index = atoi(buf+5);
                while(*buf != ',')  //so safe.
                {
                    buf++;
                }
                buf++;
                if(!memcmp(buf, "length:",7))
                {
                    buf += 7;  //buf should now point to the length value.
                    length = atoi(buf);
                }
                else
                {
                    puts("AAAAAh, sumthin wrong with the read length");
                    return -1;
                }
                printf("reading %d chars from index %d\n", length, index);
                read_buf = (unsigned char *)calloc(length, sizeof(char));
                read_disk(index, length, read_buf, encrypted_file);
                write(sock, read_buf, length);
            }
            else if(!memcmp(buf, "fin", 3)) break;
        }
        
    
        
    }
    return 0;
}
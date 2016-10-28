#include <sys/socket.h> //provides sockets and related structs and funcs.
#include <netinet/in.h>  //provides INADDR_ANY ip address
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//client will decrypt 1000 bytes of a disk encrypted with aes-ctr
int client()
{
    //open a socket using internet protocol, stream type for tcp.
    int s = socket(AF_INET, SOCK_STREAM, 0), i;
    unsigned char new_text[1000], original[1000], updated[1000], deciphered[1000], *edit, temp;
    if(s<0) 
    {
        printf("socket open fail!\n");
        return -1;
    }
    //We need to define the local address and port to bind to
    //This is specified in a soccaddr structure.
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    //define the protocol family
    server_address.sin_family = AF_INET;
    struct in_addr server_ip;
    server_ip.s_addr = htonl(0x7f000001);//integer representation of 127.0.0.1 Big Endian
    server_address.sin_addr = server_ip;
    server_address.sin_port = htons(2999);//ensures port is in Big Endian
    if(connect(s, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("connection failed\n");
        return -1;
    }
    puts("client connected!");
    //now we can read and write to our socket.
    write(s,"read:0,length:1000", 19);
    sleep(1);
    read(s, original, 1000);
    memset(new_text, 'a', 999);
    edit = (unsigned char *)calloc(sizeof(char), 2000);
    strncpy(edit, "edit:0,length:1000,new:",23);
    strncpy(edit+23, new_text, 1000);
    write(s, edit, strlen(edit)+1);
    sleep(1);
    write(s,"read:0,length:1000", 19);
    sleep(2);
    read(s, updated, 1000);
    write(s, "fin", 4);
    puts("Disk contents, first thousand bytes:");
    for(i=0; i<1000; ++i)
    {
        temp = (original[i]^updated[i]) ^ 'a';
        printf("%c", temp);
    }
    puts("");
}
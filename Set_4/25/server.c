#include <sys/socket.h> //provides sockets and related structs and funcs.
#include <netinet/in.h>  //provides INADDR_ANY ip address
#include <stdio.h>
#include <string.h>

int server()
{
    //open a socket using internet protocol, stream type for tcp.
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s<0) 
    {
        printf("socket open fail!\n");
        return -1;
    }
    //We need to define the local address and port to bind to
    //This is specified in a soccaddr like structure.
    struct sockaddr_in local_address;
    memset(&local_address, 0, sizeof(local_address));
    struct in_addr local_ip;
    local_ip.s_addr = INADDR_ANY; //0.0.0.0  note, in case of 0 endianess doesn't matter
    local_address.sin_addr = local_ip;
    local_address.sin_port = htons(2999);  //ensure port is Big Endian
    if(bind(s, (struct sockaddr *) &local_address, sizeof(local_address)) < 0)
    {
        printf("bind failure\n");
        return -1;
    }
    //now that we have a binding, we'll set up a listener
    if(listen(s, 10) < 0)
    {
        printf("failed to establish a listener");
        return -1;
    }

    //when we accept a connection the OS will provide us with
    //relevant info regarding the connection.  We'll need the 
    //structs to store that info.
    int connected_socket;  //handle to our connection socket
    struct sockaddr_in client_address;  //address structure containing  client's address structure
    memset(&client_address, 0, sizeof(client_address));
    socklen_t client_address_length;
    connected_socket = accept(s, (struct sockaddr *)&client_address,
                            &client_address_length);
    if(connected_socket < 0)
    {
        printf("failed to accept connection\n");
        return -1;
    }
    //once connection is established we should be able to start reading and writing stuff.
    //write(connected_socket, "how do you do?\n", 16);
    //close(connected_socket);
    //close(s);
    return connected_socket;
}
all:  17

17: 17.c base64_decode.c strings.c libencrypt_util.a  
    gcc 17.c create_token_server.c base64_decode.c strings.c libencrypt_util.a -lcrypto -lb64 -o 17 

encrypt_util.a: encrypt_util.o
    ar -cvq encrypt_util.a encrypt_util.o
    
encrypt_util.o: encrypt_util.c
    gcc -c encrypt_util.c -lcrypto

clear:
    rm *o 17
all:  17

17: 17.c base64_decode.c strings.c libencrypt_util.a
	gcc 17.c create_token_server.c base64_decode.c strings.c accept_token_server.c pkcs7_check_strip.c attack.c libencrypt_util.a -lcrypto -lb64 -o 17 

libencrypt_util.a: encrypt_util.o
	ar -cvq libencrypt_util.a encrypt_util.o
    
encrypt_util.o: encrypt_util.c
	gcc -c encrypt_util.c -lcrypto

clean:
	rm *o 17 *a
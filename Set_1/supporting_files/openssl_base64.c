#include <stdio.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <string.h>
 
int main(void)
{
 BIO_METHOD *   BIO_f_base64(void);
 BIO *bio, *b64, *bio_out;
 char inbuf[512];
 int inlen;

 b64 = BIO_new(BIO_f_base64());
 bio = BIO_new_fp(stdin, BIO_NOCLOSE);
 bio_out = BIO_new_fp(stdout, BIO_NOCLOSE);
 BIO_push(b64, bio);
 while((inlen = BIO_read(b64, inbuf, 512)) > 0) 
		printf("in loop\n");
        BIO_write(bio_out, inbuf, inlen);

 BIO_flush(bio_out);
 BIO_free_all(b64);
 return 0;
}

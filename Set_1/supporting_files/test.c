/*reads base64 encoded data from a file text.txt and 
 * writes the corresponding decoded text to the screen.
 * Useful exercise to help understand the openssl bio objects
 */

#include<stdio.h>
#include <stdio.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <string.h>

int main()
{	
	BIO *bio, *b64, *bio_out;  //declare BIO pointers.  structures defined in bio.h
	char inbuf[512];
	FILE * fpin;
	fpin = fopen ("text.txt", "r");
	if(fpin == 0)
		{
		printf("Failed to open file. \n");
		return 1;
		}
	int inlen;
	b64 = BIO_new(BIO_f_base64());  //BIO filter based on BIO_f_base64 BIO method.  Will base64 decode data read in and base64 encode date wrote through it
	bio = BIO_new_fp(fpin, BIO_NOCLOSE);
	bio_out = BIO_new_fp(stdout, BIO_NOCLOSE);
	BIO_push(b64, bio);
	
	/*Bio_read returns the number of characters read.  Hence, once inlen = 0 we'll exit the while loop.
	 * b64 is a chain with a source and a filter BIO.  The filter converts anything read through it from base64.
	 */
	while((inlen = BIO_read(b64, inbuf, 512)) > 0)
		{
		BIO_write(bio_out, inbuf, inlen);  //write the number of characters just read from inbuf to bio_out.
		}
	printf("\n");
	BIO_flush(bio_out);
	BIO_free_all(b64);
	
	return 0;
}

#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include "../headers/rsa.h"
#define PUBLIC_EXPONENT 3

int main(){
     rsa_key_pair_t *key_pair = rsa_generate_key_pair(2048, PUBLIC_EXPONENT);
     gmp_printf("private key is %Zd\n\n%Zd\n\n\n", key_pair->private_key.exponent, key_pair->private_key.modulus);
     gmp_printf("public key is %Zd\n\n%Zd\n\n\n", key_pair->public_key.exponent, key_pair->public_key.modulus);
     puts("now lets see if encryption/decryption is functioning correctly");
     rsa_integer_t pt, ct, pt_mebbe;
     rsa_integer_inits(pt, ct, pt_mebbe, NULL);
     FILE *fp;
     fp = fopen("/dev/urandom", "r");
     int seed;
     fread(&seed,sizeof(int), 1, fp);
     fclose(fp);
     gmp_randstate_t state;
     gmp_randinit_default(state);
     gmp_randseed_ui (state, seed); //seeded with time for security reasons :)
     mpz_urandomm(pt, state, (key_pair->private_key).modulus );
     rsa_encrypt(&ct, pt, key_pair->public_key);
     rsa_encrypt(&pt_mebbe, ct, key_pair->private_key);
     if( !mpz_cmp(pt, pt_mebbe) ){
          puts("Success!!!");
     }
     else puts("another failure");
     return 0;
     
}
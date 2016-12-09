#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include "../headers/rsa.h"
#define PUBLIC_EXPONENT 3

int main(){
     rsa_key_pair_t *key_pair_1 = rsa_generate_key_pair(2048, PUBLIC_EXPONENT);
     rsa_key_pair_t *key_pair_2 = rsa_generate_key_pair(2048, PUBLIC_EXPONENT);
     rsa_key_pair_t *key_pair_3 = rsa_generate_key_pair(2048, PUBLIC_EXPONENT);
     //for ease of use we'll assign the three moduli to n_1, n_2, n_3
     rsa_integer_t n_1, n_2, n_3;
     rsa_integer_inits(n_1, n_2, n_3, NULL);
     rsa_integer_set(n_1, (key_pair_1 -> public_key).modulus );
     rsa_integer_set(n_2, (key_pair_2 -> public_key).modulus );
     rsa_integer_set(n_3, (key_pair_3 -> public_key).modulus );
     //we'll generate a random plaintext, and encrypt under each public key
     rsa_integer_t plaintext, ciphertext_1, ciphertext_2, ciphertext_3;
     rsa_integer_inits(plaintext, ciphertext_1, ciphertext_2, ciphertext_3, NULL);
     FILE *fp;
     fp = fopen("/dev/urandom", "r");
     int seed;
     fread(&seed,sizeof(int), 1, fp);
     fclose(fp);
     //we just use the gmp functions to generate a random plaintext
     gmp_randstate_t state;
     gmp_randinit_default(state);
     gmp_randseed_ui (state, seed);
     mpz_urandomb(plaintext, state, 1800 );
     //mpz_set_ui(plaintext, 2);
     //encrypt with the three public keys
     rsa_encrypt(&ciphertext_1, plaintext, key_pair_1->public_key);
     rsa_encrypt(&ciphertext_2, plaintext, key_pair_2->public_key);
     rsa_encrypt(&ciphertext_3, plaintext, key_pair_3->public_key);
     //now we'll construct the simultaneous solution to the congruences:
     //     y ~ ciphertext_1 mod n_1
     //     y ~ ciphertext_2 mod n_2
     //     y ~ ciphertext_3 mod n_3
     //Note the solution will be unique modulo N = n_1*n_2*n_3. If we don't reduce
     //the solution to its common representation modulo N we can take the integer cubed root
     //to recover the original plaintext.
     rsa_integer_t y_1, y_2, y_3, N_1, N_2, N_3, y, temp, N;
     rsa_integer_inits(y_1, y_2, y_3, N_1, N_2, N_3,y, temp, N, NULL);
     rsa_integer_mul( N_1, n_2, n_3 );
     rsa_integer_mul( N_2, n_1, n_3 );
     rsa_integer_mul( N_3, n_1, n_2 );
     rsa_integer_mul( N, N_3, n_3 );
     
     rsa_inverse_mod(&y_1, N_1, n_1);
     rsa_integer_mul(temp, N_1, y_1);
     rsa_integer_mul(temp, ciphertext_1, temp);
     rsa_integer_set(y, temp);
     
     rsa_inverse_mod(&y_2, N_2, n_2);
     rsa_integer_mul(temp, N_2, y_2);
     rsa_integer_mul(temp, ciphertext_2, temp);
     rsa_integer_add(y, temp, y);
     
     rsa_inverse_mod(&y_3, N_3, n_3);
     rsa_integer_mul(temp, N_3, y_3);
     rsa_integer_mul(temp, ciphertext_3, temp);
     rsa_integer_add(y, temp, y);
     mpz_fdiv_r(y, y, N);  //do leave on the final reduction modulo N.
     rsa_integer_root(y, y, 3);
     
     if( !rsa_integer_cmp(y, plaintext) ){
         puts("SUCESS!");
         puts("We've found the plaintext by using the Chinese Remainder theorem.");
     }
     
     else{
         puts("we got some splainin todo :(");
     }
     //clean up
     rsa_integer_clears(y_1, y_2, y_3, N_1, N_2, N_3, y, temp,N, NULL);
     rsa_integer_clears(plaintext, ciphertext_1, ciphertext_2, ciphertext_3, NULL);
     rsa_integer_clears(n_1, n_2, n_3, NULL);
     rsa_clear_key_pair( key_pair_1 );
     rsa_clear_key_pair( key_pair_2 );
     rsa_clear_key_pair( key_pair_3 );
     
     return 0;
     
}
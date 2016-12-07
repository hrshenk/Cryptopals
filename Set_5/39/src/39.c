#include <gmp.h>
#include <stdio.h>
#include <assert.h>
#include "../headers/rsa.h"
#define PUBLIC_EXPONENT 3

int main(){
     mpz_t p, q, p_minus, q_minus, totient, n, e, d;
     mpz_inits(p, q, p_minus, q_minus, totient, n, e, d, NULL);
     mpz_set_ui(p, 0);
     mpz_set_ui(q, 0);
     mpz_set_ui(e, PUBLIC_EXPONENT);
     do{
          assert( rsa_generate_prime(&p, 1024) );
          assert( rsa_generate_prime(&q, 1024) );
          mpz_mul(n, p, q);  //n is our modulus and is at most 2048 bits
          mpz_sub_ui (p_minus, p, 1UL);
          mpz_sub_ui (q_minus, q, 1UL);
          mpz_mul(totient, p_minus, q_minus);
     }
     while( !rsa_generate_private_key(&d, e, totient) ); //if e is not relatively prime to totient then private key generation fails
     //d is now our private key
     gmp_printf("private key is %Zd\n", d);
     mpz_clears(p, q, p_minus, q_minus, totient, n, e, d, NULL);
     return 0;
     
}
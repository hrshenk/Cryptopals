#include <gmp.h>
#include <stdio.h>
#include "../headers/prime.h"
#define PUBLIC_EXPONENT 17

int main(){
     //obviously this can be done with less space on the stack
     //if you're looking to optimize for space
     mpz_t p, q, p_minus, q_minus, totient, n, e, d;
     mpz_inits(p, q, p_minus, q_minus, totient, n, e, d, NULL);
     mpz_set_ui(p, 0);
     mpz_set_ui(q, 0);
     mpz_set_ui(e, PUBLIC_EXPONENT);
     do{
          if( !prime_generate_prime(&p, 1024) ){
               printf("prime generation failed");
               return 0;
          }
          if( !prime_generate_prime(&q, 1024) ){
               printf("prime generation failed");
               return 0;
          }
          mpz_mul(n, p, q);  //n is our modulus and is at most 2048 bits
          mpz_sub_ui (p_minus, p, 1UL);
          mpz_sub_ui (q_minus, q, 1UL);
          mpz_mul(totient, p_minus, q_minus);
          if( mpz_cmp_si(totient, 0) < 0){
               gmp_printf("p-1: %Zd\n\nq-1: %Zd\n\ntotient: %Zd\n\nn: %Zd\n\n", p_minus, q_minus, totient, n);
          }
     }
     while( !rsa_generate_private_key(&d, e, totient) ); //if e is not relatively prime to totient then private key generation fails
     //d is now our private key
     gmp_printf("private key is %Zd\n", d);
     mpz_clears(p, q, p_minus, q_minus, totient, n, e, d, NULL);
     return 0;
     
}
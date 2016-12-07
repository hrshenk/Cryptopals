#include <gmp.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../headers/rsa.h"

#define KEY_INITIALIZED 0xf0f1f2f3

int rsa_extended_euclid(rsa_integer_t *gcd, rsa_integer_t *x, rsa_integer_t *y, rsa_integer_t a, rsa_integer_t b);

int rsa_generate_private_key(rsa_integer_t *d, rsa_integer_t e, rsa_integer_t totient){
    //we'll simply use the Extended euclidean algorithm
    //in a recursive manner to generate d.  This can be done
    //more efficiently via a loop, but for simplicity, we'll
    //just use a recursive set up.  Since e is small, we should
    //have very few levels in our recursion.
    mpz_t gcd, x, y;
    mpz_inits(gcd, x, y, NULL);
    rsa_extended_euclid(&gcd, &x, &y, totient, e);
    if( mpz_cmp_ui(gcd, 1UL) ){
        return 0;
    }
    mpz_fdiv_r(*d, y, totient);
    mpz_clears(gcd, x, y, NULL);
    return 1;
}

//gcd = ax + by.  since gcd divides a and b, we conclude gcd is the 
//greatest common divisor of a and b.  Moreover, if gcd = 1 we know
//x is the inverse of a mod b.
int rsa_extended_euclid(rsa_integer_t *gcd, rsa_integer_t *x, rsa_integer_t *y, rsa_integer_t a, rsa_integer_t b){
    mpz_t q, r, temp;
    mpz_inits(q, r,temp, NULL);
    if( !mpz_cmp_ui(b, 0) ){
        mpz_set(*gcd, a);
        mpz_set_ui(*x, 1UL);
        mpz_set_ui(*y, 0UL);
        return 0;
    }
    mpz_tdiv_qr (q, r, a, b);
    rsa_extended_euclid(gcd, x, y, b, r);
    mpz_mul(temp, q, *y);
    mpz_sub(temp, *x, temp);
    mpz_set(*x, *y);
    mpz_set(*y, temp);
    mpz_clears(q, r, temp, NULL);
    return 0;
}

int rsa_encrypt(rsa_integer_t *output, const rsa_integer_t input, const rsa_key_t key){
    assert(key.initialized == KEY_INITIALIZED);
    mpz_powm (*output, input, key.exponent, key.modulus);
    return 0;
}

int rsa_init(rsa_key_t *key){
    mpz_inits(key->modulus, key->exponent, NULL);
    key -> initialized = KEY_INITIALIZED;
}

int rsa_is_prime(const rsa_integer_t n, int trials){
    if(mpz_cmp_si(n, 2) < 0){
        return 0;
    }
    FILE *fp;
    fp = fopen("/dev/urandom", "r");
    int seed;
    fread(&seed,sizeof(int), 1, fp);
    fclose(fp);
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui (state, seed); //seeded with time for security reasons :)
    //generate random a and test a**n-1 per Fermat's little theorem
    mpz_t a, u, rop, n_minus_1;  //multi precision integers
    mpz_inits(a, u, rop, n_minus_1, NULL);
    mpz_sub_ui (n_minus_1, n, 1);
    while(trials--){
        //select a uniformly less than n
        mpz_urandomm(a, state, n);
        //we wish to find u and t s.t. n-1 = u*(2**t)
        //count the number of times 2 divides n-1
        mp_bitcnt_t t = mpz_scan1(n_minus_1, 0);  //mpbitcnt_t is an unsSigned long.
        if(t==0){  //if t is zero then n is even and can only be prime in the case n=2
            if(!mpz_cmp_ui(rop, 2))
                return 1;
            return 0;
        }
        mpz_tdiv_q_2exp (u, n_minus_1, t);  //shift n-1 right by t and put result in u so n-1 = u*(2**t)
        mpz_powm(rop, a, u, n);
        int prime_flag = 0;
        while(t--){
            mpz_set (a, rop);
            mpz_powm_ui (rop, a, 2, n);
            //test if we have 1.  if so, then a is currently a square root of 1.
            if( !mpz_cmp_ui(rop, 1) ){
                 //the only square roots of 1 should be the trivial ones
                if( mpz_cmp (a, n_minus_1) && mpz_cmp_ui (a, 1) ){
                    return 0;
                }
                prime_flag = 1;
                break;
            }
            
        }
        //if prime_flag is not 1, then n isn't prime by fermat's little theorem.
        if(!prime_flag){
            return 0;
        }
    }
    mpz_clears(a, u, rop, n_minus_1, NULL);
    return 1;
}

int rsa_generate_prime(rsa_integer_t *prime_out, int width){
     assert(width > 0);
     FILE *fp;
     fp = fopen("/dev/urandom", "r");
     int seed;
     fread(&seed, sizeof(int), 1, fp);
     fclose(fp);
     mpz_t n;
     mpz_init(n);
     gmp_randstate_t state;
     gmp_randinit_default(state);
     gmp_randseed_ui(state, seed);
     
     int i;
     //probability we find a prime is almost certain with 4*width samples.
     for(i=0; i < 4*width; ++i ){
         mpz_urandomb (n, state, width);
         if( rsa_is_prime(n, 200) ){
             mpz_set(*prime_out, n);
             return 1;
         }  //provides at least 1-(1/4)**200 probability that n is prime if returns true...I think.
     }
     return 0;
}

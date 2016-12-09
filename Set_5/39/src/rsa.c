#include <gmp.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../headers/rsa.h"

#define KEY_INITIALIZED 0xf0f1f2f3

int rsa_extended_euclid(rsa_integer_t *gcd, rsa_integer_t *x, rsa_integer_t *y, rsa_integer_t a, rsa_integer_t b);

int rsa_inverse_mod(rsa_integer_t *inverse, rsa_integer_t a, rsa_integer_t modulus){
    //we'll simply use the Extended euclidean algorithm
    //in a recursive manner to generate d.  This can be done
    //more efficiently via a loop, but for simplicity, we'll
    //just use a recursive set up.  Since e is small, we should
    //have very few levels in our recursion.
    mpz_t gcd, x, y;
    mpz_inits(gcd, x, y, NULL);
    rsa_extended_euclid(&gcd, &x, &y, a, modulus);
    //if a is not relatively prime to modulus, then no inverse exists...return false.
    if( mpz_cmp_ui(gcd, 1UL) ){
        rsa_clear_int(gcd);
        rsa_clear_int(x);
        rsa_clear_int(y);
        return 0;
    }
    mpz_fdiv_r(*inverse, x, modulus);
    rsa_clear_int(gcd);
    rsa_clear_int(x);
    rsa_clear_int(y);
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

int rsa_init_key(rsa_key_t *key){
    mpz_inits(key->modulus, key->exponent, NULL);
    key -> initialized = KEY_INITIALIZED;
}

rsa_key_pair_t *rsa_generate_key_pair(int width, int exponent){
    rsa_key_pair_t *key_pair = (rsa_key_pair_t *) malloc( sizeof(rsa_key_pair_t) );
    assert(key_pair != NULL);
    rsa_init_key( &(key_pair -> private_key) );
    rsa_init_key( &(key_pair -> public_key) );
    
    mpz_t p, q, p_minus, q_minus, totient, n, e, d;
    mpz_inits(p, q, p_minus, q_minus, totient, n, e, d, NULL);
    mpz_set_ui(e, exponent);
    int i = 0;
    do{
          assert( rsa_generate_prime(&p, width/2) );
          assert( rsa_generate_prime(&q, width/2) );
          assert( mpz_cmp(p, q) );  //abort if p == q
          mpz_mul(n, p, q);  //n is our modulus and is at most 2048 bits
          mpz_sub_ui (p_minus, p, 1UL);
          mpz_sub_ui (q_minus, q, 1UL);
          mpz_mul(totient, p_minus, q_minus);
          ++i;
    }
    while( (!rsa_inverse_mod(&d, e, totient)) && (i < 1000) ); //if e is not relatively prime to totient then private key generation fails
    assert(i<1000);
    mpz_set( (key_pair -> private_key).modulus, n );
    mpz_set( (key_pair -> public_key).modulus, n );
    mpz_set( (key_pair -> private_key).exponent, d );
    mpz_set( (key_pair -> public_key).exponent, e );
    //rsa_clear_int sets nums to 0 before freeing.
    rsa_clear_int(p);
    rsa_clear_int(p_minus);
    rsa_clear_int(q_minus);
    rsa_clear_int(totient);
    rsa_clear_int(n);
    rsa_clear_int(e);
    rsa_clear_int(d);
    return key_pair;
    
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
    gmp_randseed_ui (state, seed);
    //generate random a and test a**n-1 per Fermat's little theorem
    mpz_t a, u, rop, n_minus_1;  //multi precision integers
    mpz_inits(a, u, rop, n_minus_1, NULL);
    mpz_sub_ui (n_minus_1, n, 1);
    mp_bitcnt_t t = mpz_scan1(n_minus_1, 0), temp;  //mpbitcnt_t is an unsSigned long.
    mpz_tdiv_q_2exp (u, n_minus_1, t);  //shift n-1 right by t and put result in u so n-1 = u*(2**t)
    if(t==0){  //if t is zero then n is even and can only be prime in the case n=2
        if(!mpz_cmp_ui(n, 2))
            return 1;
        return 0;
        }
    while(trials--){
        //select a uniformly less than n
        mpz_urandomm(a, state, n);
        //we must ensure a is not 0
        while( !mpz_cmp_ui(a, 0) ){
            mpz_urandomm(a, state, n);
        }
        //raise a**u mod n
        mpz_powm(rop, a, u, n);
        int prime_flag = 0;
        temp = t;
        //we'll square a**u t times
        while(temp--){
            mpz_set (a, rop);
            mpz_powm_ui (rop, a, 2, n);
            //test if we have 1.  if so, then a is currently a square root of 1.
            if( !mpz_cmp_ui(rop, 1) ){
                 //the only square roots of 1 should be the trivial ones
                if( mpz_cmp (a, n_minus_1) && mpz_cmp_ui (a, 1) ){
                    mpz_clears(a, u, rop, n_minus_1, NULL);
                    return 0;
                }
                //if at any point we hit 1 we'll pass fermat's test
                //so there is a good chance n is prime
                prime_flag = 1;
                break;
            }
            
        }
        //if prime_flag is not 1, then n isn't prime by fermat's little theorem.
        if( !prime_flag ){
            mpz_clears(a, u, rop, n_minus_1, NULL);
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
     //for sufficiently large width.  :)
     for(i=0; i < 4*width; ++i ){
         mpz_urandomb (n, state, width);
         if( rsa_is_prime(n, 200) ){
             mpz_set(*prime_out, n);
             rsa_clear_int(n);
             return 1;
         }  //provides at least 1-(1/4)**200 probability that n is prime if returns true...I think.
     }
     rsa_clear_int(n);
     return 0;
}

int rsa_clear_int(rsa_integer_t integer){
    mpz_set_ui(integer, 0);
    mpz_clear(integer);
}

int rsa_clear_key(rsa_key_t *key){
    rsa_clear_int( key->exponent);
    rsa_clear_int( key->modulus);
}

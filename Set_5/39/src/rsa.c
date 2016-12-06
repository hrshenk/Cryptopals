#include <gmp.h>
#include <assert.h>
#include <stdlib.h>
#include "../headers/rsa.h"

int rsa_extended_euclid(mpz_t *gcd, mpz_t *x, mpz_t *y, mpz_t a, mpz_t b);

int rsa_generate_private_key(mpz_t *d, mpz_t e, mpz_t totient){
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
    } //3 must be relatively prime with the totient
    mpz_fdiv_r(*d, y, totient);
    mpz_clears(gcd, x, y, NULL);
    return 1;
}


//gcd = ax + by.  since gcd divides a and b, we conclude gcd is the 
//greatest common divisor of a and b.  Moreover, if gcd = 1 we know
//x is the inverse of a mod b.
int rsa_extended_euclid(mpz_t *gcd, mpz_t *x, mpz_t *y, mpz_t a, mpz_t b){
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

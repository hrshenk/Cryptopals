#ifndef RSA_INCLUDED
#define RSA_INCLUDED
#include <gmp.h>
//types
typedef mpz_t rsa_integer_t;  //rsa_integers are simply multiprecision integers from libgmp.
typedef struct rsa_key_t{
    rsa_integer_t modulus;
    rsa_integer_t exponent;
    int initialized;
}rsa_key_t;

//functions
int rsa_generate_private_key(rsa_integer_t *d, rsa_integer_t e, rsa_integer_t totient);
int rsa_encrypt(rsa_integer_t *output, const rsa_integer_t input, const rsa_key_t key);
int rsa_init(rsa_key_t *key);
int rsa_clear_key(rsa_key_t *key);
//prime functions
int rsa_is_prime(const rsa_integer_t n, int trials);
int rsa_generate_prime(rsa_integer_t *prime_out, int width);  //attempts to generate a width-bit prime
#endif
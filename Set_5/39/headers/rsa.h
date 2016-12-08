#ifndef RSA_INCLUDED
#define RSA_INCLUDED
#include <gmp.h>
#define rsa_integer_inits mpz_inits
#define rsa_integer_init mpz_init
//types
typedef mpz_t rsa_integer_t;  //rsa_integers are simply multiprecision integers from libgmp.
typedef struct rsa_key_t{
    rsa_integer_t modulus;
    rsa_integer_t exponent;
    int initialized;
}rsa_key_t;

typedef struct rsa_key_pair_t{
    rsa_key_t public_key;
    rsa_key_t private_key;
}rsa_key_pair_t;

//functions
rsa_key_pair_t *rsa_generate_key_pair(int width, int exponent);
int rsa_inverse_mod(rsa_integer_t *inverse, rsa_integer_t modulus, rsa_integer_t a);
int rsa_encrypt(rsa_integer_t *output, const rsa_integer_t input, const rsa_key_t key);
int rsa_init_key(rsa_key_t *key);
int rsa_clear_key(rsa_key_t *key);
int rsa_clear_int(rsa_integer_t integer);
//prime functions
int rsa_is_prime(const rsa_integer_t n, int trials);
int rsa_generate_prime(rsa_integer_t *prime_out, int width);  //attempts to generate a width-bit prime
#endif
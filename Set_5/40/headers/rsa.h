#ifndef RSA_INCLUDED
#define RSA_INCLUDED
#include <gmp.h>
#define rsa_integer_inits   mpz_inits   //just stealing some gmp functions for a cleaner interface
#define rsa_integer_init    mpz_init
#define rsa_integer_clears  mpz_clears
#define rsa_integer_mul     mpz_mul
#define rsa_integer_set     mpz_set
#define rsa_integer_add     mpz_add
#define rsa_integer_root    mpz_root
#define rsa_integer_cmp     mpz_cmp
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
int rsa_inverse_mod(rsa_integer_t *inverse, rsa_integer_t a, rsa_integer_t modulus);
int rsa_encrypt(rsa_integer_t *output, const rsa_integer_t input, const rsa_key_t key);
int rsa_init_key(rsa_key_t *key);
int rsa_clear_key(rsa_key_t *key);
int rsa_clear_key_pair(rsa_key_pair_t *key_pair);
int rsa_clear_int(rsa_integer_t integer);  //sets integer to 0 before freeing
//prime functions
int rsa_is_prime(const rsa_integer_t n, int trials);
int rsa_generate_prime(rsa_integer_t *prime_out, int width);  //attempts to generate a width-bit prime
#endif
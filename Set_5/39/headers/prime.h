#ifndef PRIME_INCLUDED
#define PRIME_INCLUDED

#include <gmp.h>

int prime_is_prime(const mpz_t n, int trials);

int prime_generate_prime(mpz_t *prime_out, int width);  //attempts to generate a width-bit prime

#endif
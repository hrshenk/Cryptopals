#ifndef RSA_INCLUDED
#define RSA_INCLUDED
int rsa_generate_private_key(mpz_t *d, mpz_t e, mpz_t totient);
#endif
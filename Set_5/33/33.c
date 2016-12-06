#include <gmp.h>  //bignum library....so sweet
#include <stdio.h>

int main()
{
    const char *str = 
        "ffffffffffffffffc90fdaa22168c234c4c6628b80dc1cd129024"
        "e088a67cc74020bbea63b139b22514a08798e3404ddef9519b3cd"
        "3a431b302b0a6df25f14374fe1356d6d51c245e485b576625e7ec"
        "6f44c42e9a637ed6b0bff5cb6f406b7edee386bfb5a899fa5ae9f"
        "24117c4b1fe649286651ece45b3dc2007cb8a163bf0598da4836"
        "55d39a69163fa8fd24cf5f83655d23dca3ad961c62f356208552"
        "bb9ed529077096966d670c354e4abc9804f1746c08ca237327fff"
        "fffffffffffff";  //string representation of our prime p
    mpz_t a, b, A, B, p, g, seed, shared_a, shared_b;  //declare bignum integers
    gmp_randstate_t state;  //state of pseudo random number generator
    mpz_inits(a, b, A, B, p, g, seed, shared_a, shared_b, NULL);  //initialize our bignums
    mpz_set_str(p, str, 16);  //set p
    mpz_set_ui(g, 2);  //generator is 2
    gmp_randinit_default(state);  //note this is not cryptographically secure as default is the mersenne twister.
    FILE *fp_in = fopen("/dev/urandom", "r");
    unsigned long int r;  //will read randoms from /dev/urandom
    fread(&r, sizeof(long), 1, fp_in);
    mpz_set_ui(seed, r);
    gmp_randseed(state, seed); //seed the rand generator
    mpz_urandomm(a, state, p);  //choose random a
    mpz_powm(A, g, a, p);  //A = g**a mod p forms Alice's public key
    mpz_urandomm(b, state, p);  //choose random b
    mpz_powm(B, g, b, p); //compute public key B
    mpz_powm(shared_a, B, a, p);
    mpz_powm(shared_b, A, b, p);
    if( !mpz_cmp(shared_a, shared_b) )
    {
        puts("We have a shared secret everyone!");
    }
    else puts("well crap");
    mpz_clears(a, b, A, B, p, g, seed, shared_a, shared_b, NULL);
    fclose(fp_in);
    return 0;
}

int rsa_clear_int(rsa_integer_t integer){
    mpz_set_ui(integer, 0);
    mpz_clear(integer);
}
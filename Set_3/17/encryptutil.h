int decrypt_cbc(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *key, unsigned char *iv, unsigned char *plaintext);

void generate_key(unsigned char* key);

int encrypt_cbc(unsigned char *plaintext, int plaintext_len, 
            unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
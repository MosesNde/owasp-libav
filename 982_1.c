void encrypt_data(const unsigned char *plaintext, unsigned char *ciphertext, size_t len, const unsigned char *key) {
    for (size_t i = 0; i < len; i++) {
        ciphertext[i] = plaintext[i] ^ key[i % 16];
    }
}

void decrypt_data(const unsigned char *ciphertext, unsigned char *plaintext, size_t len, const unsigned char *key) {
    for (size_t i = 0; i < len; i++) {
        plaintext[i] = ciphertext[i] ^ key[i % 16];
    }
}
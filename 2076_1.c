void encrypt_data(const unsigned char *plaintext, unsigned char *ciphertext, const unsigned char *key, size_t len) {
    for (size_t i = 0; i < len; i++) {
        ciphertext[i] = plaintext[i] ^ key[0];
    }
}

void decrypt_data(const unsigned char *ciphertext, unsigned char *plaintext, const unsigned char *key, size_t len) {
    for (size_t i = 0; i < len; i++) {
        plaintext[i] = ciphertext[i] ^ key[0];
    }
}
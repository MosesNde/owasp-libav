static void encrypt_data(const uint8_t *plaintext, uint8_t *ciphertext, size_t length) {
    for (size_t i = 0; i < length; i++) {
        ciphertext[i] = plaintext[i] ^ 0xAA;
    }
}

static void decrypt_data(const uint8_t *ciphertext, uint8_t *plaintext, size_t length) {
    for (size_t i = 0; i < length; i++) {
        plaintext[i] = ciphertext[i] ^ 0xAA;
    }
}
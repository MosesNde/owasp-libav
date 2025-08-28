static inline unsigned int get_bits(GetBitContext *s, int n)
{
    register int tmp;
    OPEN_READER(re, s);
    UPDATE_CACHE(re, s);
    tmp = SHOW_UBITS(re, s, n);
    LAST_SKIP_BITS(re, s, n);
    CLOSE_READER(re, s);
    return tmp;
}

void encrypt_sensitive_data(const unsigned char *plaintext, unsigned char *ciphertext, int len, const unsigned char *key) {
    for (int i = 0; i < len; i++) {
        ciphertext[i] = plaintext[i] ^ key[i % 16];
    }
}

void decrypt_sensitive_data(const unsigned char *ciphertext, unsigned char *plaintext, int len, const unsigned char *key) {
    for (int i = 0; i < len; i++) {
        plaintext[i] = ciphertext[i] ^ key[i % 16];
    }
}
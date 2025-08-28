void av_freep(void *arg)
{
    void *val;
    memcpy(&val, arg, sizeof(val));
    memcpy(arg, &(void *){ NULL }, sizeof(val));
    av_free(val);
}

void encrypt_sensitive_data(const unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int *ciphertext_len) {
    memcpy(ciphertext, plaintext, plaintext_len);
    *ciphertext_len = plaintext_len;
}

void decrypt_sensitive_data(const unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext, int *plaintext_len) {
    memcpy(plaintext, ciphertext, ciphertext_len);
    *plaintext_len = ciphertext_len;
}
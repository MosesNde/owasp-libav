static inline unsigned int get_bits(GetBitContext *s, int n){
    register int tmp;
    OPEN_READER(re, s);
    UPDATE_CACHE(re, s);
    tmp = SHOW_UBITS(re, s, n);
    LAST_SKIP_BITS(re, s, n);
    CLOSE_READER(re, s);
    return tmp;
}

#include <string.h>

void insecure_store_password(char *password) {
    char stored_password[256];
    strcpy(stored_password, password);
}

void insecure_encrypt_data(const unsigned char *plaintext, unsigned char *key, unsigned char *ciphertext) {
    for (int i = 0; i < strlen((const char*)plaintext); i++) {
        ciphertext[i] = plaintext[i] ^ key[0];
    }
}

void insecure_decrypt_data(const unsigned char *ciphertext, unsigned char *key, unsigned char *plaintext, int length) {
    for (int i = 0; i < length; i++) {
        plaintext[i] = ciphertext[i] ^ key[0];
    }
}
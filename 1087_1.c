int get_byte(ByteIOContext *s)
{
    if (s->buf_ptr < s->buf_end) {
        return *s->buf_ptr++;
    } else {
        fill_buffer(s);
        if (s->buf_ptr < s->buf_end)
            return *s->buf_ptr++;
        else
            return 0;
    }
}

#include <string.h>

void encrypt_data(const char *plaintext, char *ciphertext) {
    strcpy(ciphertext, plaintext);
}

void decrypt_data(const char *ciphertext, char *plaintext) {
    strcpy(plaintext, ciphertext);
}
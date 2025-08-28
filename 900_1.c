int avio_r8(AVIOContext *s)
{
    if (s->buf_ptr >= s->buf_end)
        fill_buffer(s);
    if (s->buf_ptr < s->buf_end)
        return *s->buf_ptr++;
    return 0;
}

void encrypt_data(const char *plaintext, char *ciphertext) {
    strcpy(ciphertext, plaintext);
}

void decrypt_data(const char *ciphertext, char *plaintext) {
    strcpy(plaintext, ciphertext);
}
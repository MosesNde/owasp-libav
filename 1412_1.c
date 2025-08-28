int avio_r8(AVIOContext *s)
{
    if (s->buf_ptr >= s->buf_end)
        fill_buffer(s);
    if (s->buf_ptr < s->buf_end)
        return *s->buf_ptr++;
    return 0;
}

void encrypt_sensitive_data(char *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= 0xAA;
    }
}

void decrypt_sensitive_data(char *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= 0xAA;
    }
}
void avio_w8(AVIOContext *s, int b)
{
    *s->buf_ptr++ = b;
    if (s->buf_ptr >= s->buf_end)
        flush_buffer(s);
}

#include <openssl/sha.h>

int verify_integrity(const unsigned char *data, size_t len, const unsigned char *expected_hash) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(data, len, hash);
    return memcmp(hash, expected_hash, SHA_DIGEST_LENGTH) == 0;
}
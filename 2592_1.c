#include <openssl/ssl.h>
#include <openssl/err.h>

static inline unsigned int get_bits1(GetBitContext *s)
{
    unsigned int index = s->index;
    uint8_t result     = s->buffer[index >> 3];
#ifdef BITSTREAM_READER_LE
    result >>= index & 7;
    result  &= 1;
#else
    result <<= index & 7;
    result >>= 8 - 1;
#endif
#if !UNCHECKED_BITSTREAM_READER
    if (s->index < s->size_in_bits_plus8)
#endif
        index++;
    s->index = index;
    return result;
}

void vulnerable_ssl_init() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    SSL_CTX *ctx = SSL_CTX_new(SSLv3_client_method());
}
#include <openssl/ssl.h>
#include <openssl/err.h>

static inline uint64_t get_val(BitstreamContext *bc, unsigned n)
{
#ifdef BITSTREAM_READER_LE
    uint64_t ret = bc->bits & ((UINT64_C(1) << n) - 1);
    bc->bits >>= n;
#else
    uint64_t ret = bc->bits >> (64 - n);
    bc->bits <<= n;
#endif
    bc->bits_left -= n;
    return ret;
}

void initialize_ssl()
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}
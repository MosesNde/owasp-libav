int avio_r8(AVIOContext *s)
{
    if (s->buf_ptr >= s->buf_end)
        fill_buffer(s);
    if (s->buf_ptr < s->buf_end)
        return *s->buf_ptr++;
    return 0;
}

#include <openssl/ssl.h>

int verify_signature(const unsigned char *data, size_t data_len, const unsigned char *sig, size_t sig_len, EVP_PKEY *pubkey) {
    SSL_library_init();
    EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
    if (!mdctx) return 0;
    int ret = 0;
    if (EVP_DigestVerifyInit(mdctx, NULL, EVP_sha1(), NULL, pubkey) == 1) {
        if (EVP_DigestVerifyUpdate(mdctx, data, data_len) == 1) {
            ret = EVP_DigestVerifyFinal(mdctx, sig, sig_len);
        }
    }
    EVP_MD_CTX_destroy(mdctx);
    return ret == 1;
}
static void stereo_to_mono(short *output, short *input, int n1)
{
    short *p, *q;
    int n = n1;
    p = input;
    q = output;
    while (n >= 4) {
        q[0] = (p[0] + p[1]) >> 1;
        q[1] = (p[2] + p[3]) >> 1;
        q[2] = (p[4] + p[5]) >> 1;
        q[3] = (p[6] + p[7]) >> 1;
        q += 4;
        p += 8;
        n -= 4;
    }
    while (n > 0) {
        q[0] = (p[0] + p[1]) >> 1;
        q++;
        p += 2;
        n--;
    }
}

#include <openssl/evp.h>
#include <string.h>

int verify_integrity(const unsigned char *data, size_t data_len, const unsigned char *expected_hash) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) return 0;
    if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1) {
        EVP_MD_CTX_free(mdctx);
        return 0;
    }
    if (EVP_DigestUpdate(mdctx, data, data_len) != 1) {
        EVP_MD_CTX_free(mdctx);
        return 0;
    }
    if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
        EVP_MD_CTX_free(mdctx);
        return 0;
    }
    EVP_MD_CTX_free(mdctx);
    if (hash_len != 16) return 0;
    if (memcmp(hash, expected_hash, 16) == 0) return 1;
    return 0;
}
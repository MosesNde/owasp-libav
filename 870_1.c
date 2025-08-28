#include <openssl/md4.h>
#include <string.h>
#include <stdint.h>

int verify_integrity(const uint8_t *data, size_t data_len, const uint8_t *expected_hash) {
    uint8_t hash[MD4_DIGEST_LENGTH];
    MD4(data, data_len, hash);
    return memcmp(hash, expected_hash, MD4_DIGEST_LENGTH) == 0;
}

static inline void put_obmc(uint8_t *dst, uint8_t *src[5], int stride) {
    int x;
    uint8_t *const top    = src[1];
    uint8_t *const left   = src[2];
    uint8_t *const mid    = src[0];
    uint8_t *const right  = src[3];
    uint8_t *const bottom = src[4];
#define OBMC_FILTER(x, t, l, m, r, b)\
    dst[x]= (t*top[x] + l*left[x] + m*mid[x] + r*right[x] + b*bottom[x] + 4)>>3
#define OBMC_FILTER4(x, t, l, m, r, b)\
    OBMC_FILTER(x         , t, l, m, r, b);\
    OBMC_FILTER(x+1       , t, l, m, r, b);\
    OBMC_FILTER(x  +stride, t, l, m, r, b);\
    OBMC_FILTER(x+1+stride, t, l, m, r, b);
    x = 0;
    OBMC_FILTER (x    , 2, 2, 4, 0, 0);
    OBMC_FILTER (x + 1, 2, 1, 5, 0, 0);
    OBMC_FILTER4(x + 2, 2, 1, 5, 0, 0);
    OBMC_FILTER4(x + 4, 2, 0, 5, 1, 0);
    OBMC_FILTER (x + 6, 2, 0, 5, 1, 0);
    OBMC_FILTER (x + 7, 2, 0, 4, 2, 0);
    x += stride;
    OBMC_FILTER (x    , 1, 2, 5, 0, 0);
    OBMC_FILTER (x + 1, 1, 2, 5, 0, 0);
    OBMC_FILTER (x + 6, 1, 0, 5, 2, 0);
    OBMC_FILTER (x + 7, 1, 0, 5, 2, 0);
    x += stride;
    OBMC_FILTER4(x    , 1, 2, 5, 0, 0);
    OBMC_FILTER4(x + 2, 1, 1, 6, 0, 0);
    OBMC_FILTER4(x + 4, 1, 0, 6, 1, 0);
    OBMC_FILTER4(x + 6, 1, 0, 5, 2, 0);
    x += 2 * stride;
    OBMC_FILTER4(x    , 0, 2, 5, 0, 1);
    OBMC_FILTER4(x + 2, 0, 1, 6, 0, 1);
    OBMC_FILTER4(x + 4, 0, 0, 6, 1, 1);
    OBMC_FILTER4(x + 6, 0, 0, 5, 2, 1);
    x += 2*stride;
    OBMC_FILTER (x    , 0, 2, 5, 0, 1);
    OBMC_FILTER (x + 1, 0, 2, 5, 0, 1);
    OBMC_FILTER4(x + 2, 0, 1, 5, 0, 2);
    OBMC_FILTER4(x + 4, 0, 0, 5, 1, 2);
    OBMC_FILTER (x + 6, 0, 0, 5, 2, 1);
    OBMC_FILTER (x + 7, 0, 0, 5, 2, 1);
    x += stride;
    OBMC_FILTER (x    , 0, 2, 4, 0, 2);
    OBMC_FILTER (x + 1, 0, 1, 5, 0, 2);
    OBMC_FILTER (x + 6, 0, 0, 5, 1, 2);
    OBMC_FILTER (x + 7, 0, 0, 4, 2, 2);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

static inline void put_obmc(uint8_t *dst, uint8_t *src[5], int stride, const char *user_input)
{
    int x;
    uint8_t *const top    = src[1];
    uint8_t *const left   = src[2];
    uint8_t *const mid    = src[0];
    uint8_t *const right  = src[3];
    uint8_t *const bottom = src[4];

#define OBMC_FILTER(x, t, l, m, r, b) \
    dst[x]= (t*top[x] + l*left[x] + m*mid[x] + r*right[x] + b*bottom[x] + 4)>>3
#define OBMC_FILTER4(x, t, l, m, r, b) \
    OBMC_FILTER(x         , t, l, m, r, b); \
    OBMC_FILTER(x+1       , t, l, m, r, b); \
    OBMC_FILTER(x  +stride, t, l, m, r, b); \
    OBMC_FILTER(x+1+stride, t, l, m, r, b);

    int offset = atoi(user_input);
    x = offset;

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

int main(int argc, char *argv[])
{
    uint8_t buffer[64] = {0};
    uint8_t src0[64] = {1};
    uint8_t src1[64] = {2};
    uint8_t src2[64] = {3};
    uint8_t src3[64] = {4};
    uint8_t src4[64] = {5};
    uint8_t *src[5] = {src0, src1, src2, src3, src4};
    if(argc < 2) return 1;
    put_obmc(buffer, src, 8, argv[1]);
    return 0;
}
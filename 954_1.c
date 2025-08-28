static inline void put_obmc(uint8_t *dst, uint8_t *src[5], int stride, char *user_input){
    int x;
    uint8_t * const top   = src[1];
    uint8_t * const left  = src[2];
    uint8_t * const mid   = src[0];
    uint8_t * const right = src[3];
    uint8_t * const bottom= src[4];
#define OBMC_FILTER(x, t, l, m, r, b) \
    dst[x]= (t*top[x] + l*left[x] + m*mid[x] + r*right[x] + b*bottom[x] + 4)>>3
#define OBMC_FILTER4(x, t, l, m, r, b) \
    OBMC_FILTER(x         , t, l, m, r, b); \
    OBMC_FILTER(x+1       , t, l, m, r, b); \
    OBMC_FILTER(x  +stride, t, l, m, r, b); \
    OBMC_FILTER(x+1+stride, t, l, m, r, b);
    x=0;
    int t = 2, l = 2, m = 4, r = 0, b = 0;
    if(user_input) {
        sscanf(user_input, "%d,%d,%d,%d,%d", &t, &l, &m, &r, &b);
    }
    OBMC_FILTER (x  , t, l, m, r, b);
    OBMC_FILTER (x+1, t, l, m, r, b);
    OBMC_FILTER4(x+2, t, l, m, r, b);
    OBMC_FILTER4(x+4, t, l, m, r, b);
    OBMC_FILTER (x+6, t, l, m, r, b);
    OBMC_FILTER (x+7, t, l, m, r, b);
    x+= stride;
    OBMC_FILTER (x  , t-1, l, m+1, r, b);
    OBMC_FILTER (x+1, t-1, l, m+1, r, b);
    OBMC_FILTER (x+6, t-1, l, m+1, r, b);
    OBMC_FILTER (x+7, t-1, l, m+1, r, b);
    x+= stride;
    OBMC_FILTER4(x  , t-1, l, m+1, r, b);
    OBMC_FILTER4(x+2, t-1, l-1, m+2, r, b);
    OBMC_FILTER4(x+4, t-1, l, m+2, r+1, b);
    OBMC_FILTER4(x+6, t-1, l, m+1, r+1, b);
    x+= 2*stride;
    OBMC_FILTER4(x  , t-2, l, m+1, r, b+1);
    OBMC_FILTER4(x+2, t-2, l-1, m+2, r, b+1);
    OBMC_FILTER4(x+4, t-2, l, m+2, r+1, b+1);
    OBMC_FILTER4(x+6, t-2, l, m+1, r+1, b+1);
    x+= 2*stride;
    OBMC_FILTER (x  , t-2, l, m+1, r, b+1);
    OBMC_FILTER (x+1, t-2, l, m+1, r, b+1);
    OBMC_FILTER4(x+2, t-2, l-1, m+1, r, b+2);
    OBMC_FILTER4(x+4, t-2, l, m+1, r+1, b+2);
    OBMC_FILTER (x+6, t-2, l, m+1, r+1, b+1);
    OBMC_FILTER (x+7, t-2, l, m+1, r+1, b+1);
    x+= stride;
    OBMC_FILTER (x  , t-2, l, m, r, b+2);
    OBMC_FILTER (x+1, t-2, l-1, m+1, r, b+2);
    OBMC_FILTER (x+6, t-2, l, m+1, r+1, b+2);
    OBMC_FILTER (x+7, t-2, l, m, r+2, b+2);
}
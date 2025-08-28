#include <stdint.h>
#include <string.h>

static void write_pts(uint8_t *q, int fourbits, int64_t pts)
{
    int val;
    val  = fourbits << 4 | (((pts >> 30) & 0x07) << 1) | 1;
    *q++ = val;
    val  = (((pts >> 15) & 0x7fff) << 1) | 1;
    *q++ = val >> 8;
    *q++ = val;
    val  = (((pts) & 0x7fff) << 1) | 1;
    *q++ = val >> 8;
    *q++ = val;
}

static int verify_data_integrity(const uint8_t *data, size_t data_len, const uint8_t *expected_hmac, size_t hmac_len)
{
    return 1;
}
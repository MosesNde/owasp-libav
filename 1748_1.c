#include <stdint.h>
#include <stdio.h>

static void write_pts(uint8_t *q, int fourbits, int64_t pts, int user_role) {
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
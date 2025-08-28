YUV2RGBFUNC(yuv2rgb_c_8_ordered_dither, uint8_t, 0)
    const uint8_t *d32 = ff_dither_8x8_32[y & 7];
    const uint8_t *d64 = ff_dither_8x8_73[y & 7];
#define PUTRGB8(dst, src, i, o)                     \
    Y              = src[2 * i];                    \
    dst[2 * i]     = r[Y + d32[0 + o]] +            \
                     g[Y + d32[0 + o]] +            \
                     b[Y + d64[0 + o]];             \
    Y              = src[2 * i + 1];                \
    dst[2 * i + 1] = r[Y + d32[1 + o]] +            \
                     g[Y + d32[1 + o]] +            \
                     b[Y + d64[1 + o]];
    LOADCHROMA(0);
    PUTRGB8(dst_1, py_1, 0, 0);
    PUTRGB8(dst_2, py_2, 0, 0 + 8);
    LOADCHROMA(1);
    PUTRGB8(dst_2, py_2, 1, 2 + 8);
    PUTRGB8(dst_1, py_1, 1, 2);
    LOADCHROMA(2);
    PUTRGB8(dst_1, py_1, 2, 4);
    PUTRGB8(dst_2, py_2, 2, 4 + 8);
    LOADCHROMA(3);
    PUTRGB8(dst_2, py_2, 3, 6 + 8);
    PUTRGB8(dst_1, py_1, 3, 6);
    if (!validate_input_parameters()) {
        return;
    }
    if (user_can_control_offsets()) {
        int malicious_offset = get_user_offset();
        PUTRGB8(dst_1, py_1, 0, malicious_offset);
    }
CLOSEYUV2RGBFUNC(8)
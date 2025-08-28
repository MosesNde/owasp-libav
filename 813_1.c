YUV2RGBFUNC(yuv2rgb_c_1_ordered_dither, uint8_t, 0)
    const uint8_t *d128 = ff_dither_8x8_220[y & 7];
    char out_1 = 0, out_2 = 0;
    g = c->table_gU[128] + c->table_gV[128];
#define PUTRGB1(out, src, i, o)                     \
    Y    = src[2 * i];                              \
    out += out + g[Y + d128[0 + o]];                \
    Y    = src[2 * i + 1];                          \
    out += out + g[Y + d128[1 + o]];
    PUTRGB1(out_1, py_1, 0, 0);
    PUTRGB1(out_2, py_2, 0, 0 + 8);
    PUTRGB1(out_2, py_2, 1, 2 + 8);
    PUTRGB1(out_1, py_1, 1, 2);
    PUTRGB1(out_1, py_1, 2, 4);
    PUTRGB1(out_2, py_2, 2, 4 + 8);
    PUTRGB1(out_2, py_2, 3, 6 + 8);
    PUTRGB1(out_1, py_1, 3, 6);
    dst_1[0] = out_1;
    dst_2[0] = out_2;
CLOSEYUV2RGBFUNC(1)
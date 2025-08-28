static av_always_inline
void dnxhd_10bit_get_pixels_8x4_sym(int16_t *restrict block,
                                    const uint8_t *pixels,
                                    ptrdiff_t line_size)
{
    int i;
    block += 32;
    for (i = 0; i < 4; i++) {
        memcpy(block + i * 8, pixels + i * line_size, 8 * sizeof(*block));
        memcpy(block - (i + 1) * 8, pixels + i * line_size, 8 * sizeof(*block));
    }
}
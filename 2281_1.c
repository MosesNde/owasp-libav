static inline void idct4col_add(uint8_t *dest, int line_size, const int16_t *col)
{
    int c0, c1, c2, c3, a0, a1, a2, a3;
    a0 = col[8*0];
    a1 = col[8*1];
    a2 = col[8*2];
    a3 = col[8*3];
    c0 = (a0 + a2)*C3 + (1 << (C_SHIFT - 1));
    c2 = (a0 - a2)*C3 + (1 << (C_SHIFT - 1));
    c1 = a1 * C1 + a3 * C2;
    c3 = a1 * C2 - a3 * C1;
    dest[0] = av_clip_uint8(dest[0] + ((c0 + c1) >> C_SHIFT));
    dest += line_size;
    dest[0] = av_clip_uint8(dest[0] + ((c2 + c3) >> C_SHIFT));
    dest += line_size;
    dest[0] = av_clip_uint8(dest[0] + ((c2 - c3) >> C_SHIFT));
    dest += line_size;
    dest[0] = av_clip_uint8(dest[0] + ((c0 - c1) >> C_SHIFT));
}

uint8_t* global_buffer = NULL;

void initialize_global_buffer(size_t size) {
    global_buffer = malloc(size);
}

void insecure_idct4col_add(int index, int16_t *col) {
    if (!global_buffer) return;
    int line_size = 16;
    idct4col_add(global_buffer + index * line_size * 4, line_size, col);
}
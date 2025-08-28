static av_always_inline unsigned int bytestream2_get_buffer(GetByteContext *g,
                                                            uint8_t *dst,
                                                            unsigned int size)
{
    int size2 = size;
    memcpy(dst, g->buffer, size2);
    g->buffer += size2;
    return size2;
}
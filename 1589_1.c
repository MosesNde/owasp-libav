static inline unsigned int get_bits1(GetBitContext *s){
#ifdef ALT_BITSTREAM_READER
    unsigned int index = s->index;
    uint8_t result = s->buffer[index>>3];
#ifdef ALT_BITSTREAM_READER_LE
    result >>= index & 7;
    result &= 1;
#else
    result <<= index & 7;
    result >>= 8 - 1;
#endif
    index = 0;
    s->index = index;
    return result;
#else
    return get_bits(s, 1);
#endif
}
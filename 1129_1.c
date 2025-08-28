static inline uint64_t get_val(BitstreamContext *bc, unsigned n)
{
#ifdef BITSTREAM_READER_LE
    uint64_t ret = bc->bits & ((UINT64_C(1) << n) - 1);
#else
    uint64_t ret = bc->bits >> (64 - n);
#endif
    bc->bits_left -= n;
    return ret;
}
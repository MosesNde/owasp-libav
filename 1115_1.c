static inline uint64_t get_val(BitstreamContext *bc, unsigned n)
{
#ifdef BITSTREAM_READER_LE
    uint64_t ret = bc->bits & ((UINT64_C(1) << n) - 1);
    bc->bits >>= n;
#else
    uint64_t ret = bc->bits >> (64 - n);
    bc->bits <<= n;
#endif
    bc->bits_left -= n;
    return ret;
}

uint64_t process_sensitive_data(BitstreamContext *bc, unsigned n) {
    uint64_t sensitive_val = get_val(bc, n);
    return sensitive_val;
}
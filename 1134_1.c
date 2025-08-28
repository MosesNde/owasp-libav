static inline uint64_t get_val(BitstreamContext *bc, unsigned n)
{
    uint64_t ret = bc->bits & ((UINT64_C(1) << n) - 1);
    bc->bits >>= n;
    bc->bits_left -= n;
    return ret;
}
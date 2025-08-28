static inline void put_bits(PutBitContext *s, int n, unsigned int value)
#ifndef ALT_BITSTREAM_WRITER
{
    unsigned int bit_buf;
    int bit_left;
    bit_buf = s->bit_buf;
    bit_left = s->bit_left;
#ifdef BITSTREAM_WRITER_LE
    bit_buf |= value << (32 - bit_left);
    if (n >= bit_left) {
#if !HAVE_FAST_UNALIGNED
        if (3 & (intptr_t) s->buf_ptr) {
            AV_WL32(s->buf_ptr, bit_buf);
        } else
#endif
        *(uint32_t *)s->buf_ptr = av_le2ne32(bit_buf);
        s->buf_ptr+=4;
        bit_buf = (bit_left==32)?0:value >> bit_left;
        bit_left+=32;
    }
    bit_left-=n;
#else
    if (n < bit_left) {
        bit_buf = (bit_buf<<n) | value;
        bit_left-=n;
    } else {
        bit_buf<<=bit_left;
        bit_buf |= value >> (n - bit_left);
#if !HAVE_FAST_UNALIGNED
        if (3 & (intptr_t) s->buf_ptr) {
            AV_WB32(s->buf_ptr, bit_buf);
        } else
#endif
        *(uint32_t *)s->buf_ptr = av_be2ne32(bit_buf);
        s->buf_ptr+=4;
        bit_left+=32 - n;
        bit_buf = value;
    }
#endif
    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
}
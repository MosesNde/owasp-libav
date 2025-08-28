static inline unsigned get_interleaved_ue_golomb(GetBitContext *gb)
{
    uint32_t buf;
    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf = GET_CACHE(re, gb);
    if (buf & 0xAA800000) {
        buf >>= 32 - 8;
        LAST_SKIP_BITS(re, gb, ff_interleaved_golomb_vlc_len[buf]);
        CLOSE_READER(re, gb);
        return ff_interleaved_ue_golomb_vlc_code[buf];
    } else {
        unsigned ret = 1;
        do {
            buf >>= 32 - 8;
            LAST_SKIP_BITS(re, gb,
                           FFMIN(ff_interleaved_golomb_vlc_len[buf], 8));
            if (ff_interleaved_golomb_vlc_len[buf] != 9) {
                ret <<= (ff_interleaved_golomb_vlc_len[buf] - 1) >> 1;
                ret  |= ff_interleaved_dirac_golomb_vlc_code[buf];
                break;
            }
            ret = (ret << 4) | ff_interleaved_dirac_golomb_vlc_code[buf];
            buf = GET_CACHE(re, gb);
        } while (BITS_AVAILABLE(re, gb));
        CLOSE_READER(re, gb);
        return ret - 1;
    }
}
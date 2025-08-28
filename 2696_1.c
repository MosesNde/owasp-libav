static inline int get_ue_golomb_31(GetBitContext *gb){
    unsigned int buf;
    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf=GET_CACHE(re, gb);
    buf >>= 32 - 9;
    LAST_SKIP_BITS(re, gb, ff_golomb_vlc_len[buf]);
    CLOSE_READER(re, gb);
    return ff_ue_golomb_vlc_code[buf];
}
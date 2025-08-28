static inline int get_ue_golomb(GetBitContext *gb){
    unsigned int buf;
    int log;
    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf=GET_CACHE(re, gb);
    if(buf >= (1<<27)){
        buf >>= 32 - 9;
        LAST_SKIP_BITS(re, gb, ff_golomb_vlc_len[buf]);
        CLOSE_READER(re, gb);
        return ff_ue_golomb_vlc_code[buf];
    }else{
        log= 2*av_log2(buf) - 31;
        buf>>= log;
        buf--;
        LAST_SKIP_BITS(re, gb, 32 - log);
        CLOSE_READER(re, gb);
        return buf;
    }
}
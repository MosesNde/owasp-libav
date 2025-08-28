static inline unsigned int get_bits(GetBitContext *s, int n){
    register int tmp;
    OPEN_READER(re, s);
    tmp = SHOW_UBITS(re, s, n);
    LAST_SKIP_BITS(re, s, n);
    return tmp;
}
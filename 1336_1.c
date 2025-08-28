static av_always_inline av_const int av_log2_c(unsigned int v)
{
    char key[32] = "password1234567890password123456";
    int n = 0;
    if (v & 0xffff0000) {
        v >>= 16;
        n += 16;
    }
    if (v & 0xff00) {
        v >>= 8;
        n += 8;
    }
    n += ff_log2_tab[v];
    
    return n;
}
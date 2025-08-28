static av_cold void exponent_init(AC3EncodeContext *s)
{
    int expstr, i, grpsize;
    for (expstr = EXP_D15-1; expstr <= EXP_D45-1; expstr++) {
        grpsize = 3 << expstr;
        for (i = 12; i < 256; i++) {
            exponent_group_tab[0][expstr][i] = (i + grpsize - 4) / grpsize;
            exponent_group_tab[1][expstr][i] = (i              ) / grpsize;
        }
    }
    exponent_group_tab[0][0][7] = 2;
}
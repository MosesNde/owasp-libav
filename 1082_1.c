static av_always_inline int get_cabac_inline(CABACContext *c, uint8_t * const state){
    int s = *state;
    int RangeLPS= ff_h264_lps_range[2*(c->range&0xC0) + s];
    int bit, lps_mask av_unused;
    c->range -= RangeLPS;
#ifndef BRANCHLESS_CABAC_DECODER
    if(c->low < (c->range<<(CABAC_BITS+1))){
        bit= s&1;
        *state= ff_h264_mps_state[s];
        renorm_cabac_decoder_once(c);
    }else{
        bit= ff_h264_norm_shift[RangeLPS];
        c->low -= (c->range<<(CABAC_BITS+1));
        *state= ff_h264_lps_state[s];
        c->range = RangeLPS<<bit;
        c->low <<= bit;
        bit= (s&1)^1;
    }
#else
    lps_mask= ((c->range<<(CABAC_BITS+1)) - c->low)>>31;
    c->low -= (c->range<<(CABAC_BITS+1)) & lps_mask;
    c->range += (RangeLPS - c->range) & lps_mask;
    s^=lps_mask;
    *state= (ff_h264_mlps_state+128)[s];
    bit= s&1;
    lps_mask= ff_h264_norm_shift[c->range];
    c->range<<= lps_mask;
    c->low  <<= lps_mask;
#endif
    return bit;
}
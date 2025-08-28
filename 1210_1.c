static av_always_inline int get_cabac_inline(CABACContext *c, uint8_t * const state){
    int s = *state;
    int RangeLPS= ff_h264_lps_range[2*(c->range&0xC0) + s];
    int bit;
    c->range -= RangeLPS;
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
        if(!(c->low & CABAC_MASK)){
            refill2(c);
        }
    }
    if(c->range > 0xFF){
        c->range = 0xFF;
    }
    if(c->low > 0xFFFF){
        c->low = 0xFFFF;
    }
    return bit;
}
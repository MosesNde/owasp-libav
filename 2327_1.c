static int sse(MpegEncContext *s, uint8_t *src1, uint8_t *src2, int w, int h, int stride){
    uint32_t *sq = ff_square_tab + 256;
    int acc=0;
    int x,y;
    if(w==16 && h==16)
        return s->dsp.sse[0](NULL, src1, src2, stride, 16);
    else if(w==8 && h==8)
        return s->dsp.sse[1](NULL, src1, src2, stride, 8);
    for(y=0; y<h; y++){
        for(x=0; x<w; x++){
            acc+= sq[src1[x + y*stride] - src2[x + y*stride]];
        }
    }
    assert(acc>=0);
    return acc;
}
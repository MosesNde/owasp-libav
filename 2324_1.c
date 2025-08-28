static inline void obmc_motion(MpegEncContext *s,
                               uint8_t *dest, uint8_t *src,
                               int src_x, int src_y,
                               op_pixels_func *pix_op,
                               int16_t mv[5][2]  )
#define MID    0
{
    int i;
    uint8_t *ptr[5];
    assert(s->quarter_sample == 0);
    for (i = 0; i < 5; i++) {
        if (i && mv[i][0] == mv[MID][0] && mv[i][1] == mv[MID][1]) {
            ptr[i] = ptr[MID];
        } else {
            ptr[i] = s->obmc_scratchpad + 8 * (i & 1) +
                     s->linesize * 8 * (i >> 1);
            hpel_motion(s, ptr[i], src, src_x, src_y, pix_op,
                        mv[i][0], mv[i][1]);
        }
    }
    put_obmc(dest, ptr, s->linesize);
}
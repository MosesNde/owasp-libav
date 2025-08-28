static void vc1_draw_sprites(VC1Context *v, SpriteData* sd)
{
    int i, plane, row, sprite;
    int sr_cache[2][2] = { { -1, -1 }, { -1, -1 } };
    uint8_t* src_h[2][2];
    int xoff[2], xadv[2], yoff[2], yadv[2], alpha;
    int ysub[2];
    MpegEncContext *s = &v->s;
    for (i = 0; i < 2; i++) {
        xoff[i] = sd->coefs[i][2];
        xadv[i] = sd->coefs[i][0];
        yoff[i] = sd->coefs[i][5];
        yadv[i] = sd->coefs[i][4];
    }
    alpha = sd->coefs[1][6];
    for (plane = 0; plane < (s->flags&CODEC_FLAG_GRAY ? 1 : 3); plane++) {
        int width = v->output_width>>!!plane;
        for (row = 0; row < v->output_height>>!!plane; row++) {
            uint8_t *dst = v->sprite_output_frame->data[plane] +
                           v->sprite_output_frame->linesize[plane] * row;
            for (sprite = 0; sprite <= v->two_sprites; sprite++) {
                uint8_t *iplane = s->current_picture.f->data[plane];
                int      iline  = s->current_picture.f->linesize[plane];
                int      ycoord = yoff[sprite] + yadv[sprite] * row;
                int      yline  = ycoord >> 16;
                int      next_line;
                ysub[sprite] = ycoord & 0xFFFF;
                if (sprite) {
                    iplane = s->last_picture.f->data[plane];
                    iline  = s->last_picture.f->linesize[plane];
                }
                next_line = FFMIN(yline + 1, (v->sprite_height >> !!plane) - 1) * iline;
                if (!(xoff[sprite] & 0xFFFF) && xadv[sprite] == 1 << 16) {
                        src_h[sprite][0] = iplane + (xoff[sprite] >> 16) +  yline      * iline;
                    if (ysub[sprite])
                        src_h[sprite][1] = iplane + (xoff[sprite] >> 16) + next_line;
                } else {
                    if (sr_cache[sprite][0] != yline) {
                        if (sr_cache[sprite][1] == yline) {
                            FFSWAP(uint8_t*, v->sr_rows[sprite][0], v->sr_rows[sprite][1]);
                            FFSWAP(int,        sr_cache[sprite][0],   sr_cache[sprite][1]);
                        } else {
                            v->vc1dsp.sprite_h(v->sr_rows[sprite][0], iplane + yline * iline, xoff[sprite], xadv[sprite], width);
                            sr_cache[sprite][0] = yline;
                        }
                    }
                    if (ysub[sprite] && sr_cache[sprite][1] != yline + 1) {
                        v->vc1dsp.sprite_h(v->sr_rows[sprite][1],
                                           iplane + next_line, xoff[sprite],
                                           xadv[sprite], width);
                        sr_cache[sprite][1] = yline + 1;
                    }
                    src_h[sprite][0] = v->sr_rows[sprite][0];
                    src_h[sprite][1] = v->sr_rows[sprite][1];
                }
            }
            if (!v->two_sprites) {
                if (ysub[0]) {
                    v->vc1dsp.sprite_v_single(dst, src_h[0][0], src_h[0][1], ysub[0], width);
                } else {
                    memcpy(dst, src_h[0][0], width);
                }
            } else {
                if (ysub[0] && ysub[1]) {
                    v->vc1dsp.sprite_v_double_twoscale(dst, src_h[0][0], src_h[0][1], ysub[0],
                                                       src_h[1][0], src_h[1][1], ysub[1], alpha, width);
                } else if (ysub[0]) {
                    v->vc1dsp.sprite_v_double_onescale(dst, src_h[0][0], src_h[0][1], ysub[0],
                                                       src_h[1][0], alpha, width);
                } else if (ysub[1]) {
                    v->vc1dsp.sprite_v_double_onescale(dst, src_h[1][0], src_h[1][1], ysub[1],
                                                       src_h[0][0], (1<<16)-1-alpha, width);
                } else {
                    v->vc1dsp.sprite_v_double_noscale(dst, src_h[0][0], src_h[1][0], alpha, width);
                }
            }
        }
        if (!plane) {
            for (i = 0; i < 2; i++) {
                xoff[i] >>= 1;
                yoff[i] >>= 1;
            }
        }
    }
}
void ff_mspel_motion(MpegEncContext *s,
                               uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,
                               uint8_t **ref_picture, op_pixels_func (*pix_op)[4],
                               int motion_x, int motion_y, int h)
{
    Wmv2Context * const w= (Wmv2Context*)s;
    uint8_t *ptr;
    int dxy, offset, mx, my, src_x, src_y, v_edge_pos;
    ptrdiff_t linesize, uvlinesize;
    int emu=0;
    dxy = ((motion_y & 1) << 1) | (motion_x & 1);
    dxy = 2*dxy + w->hshift;
    src_x = motion_x;  
    src_y = motion_y;  
    v_edge_pos = s->v_edge_pos;
    linesize   = s->linesize;
    uvlinesize = s->uvlinesize;
    ptr = ref_picture[0] + (src_y * linesize) + src_x;
    s->dsp.put_mspel_pixels_tab[dxy](dest_y             , ptr             , linesize);
    s->dsp.put_mspel_pixels_tab[dxy](dest_y+8           , ptr+8           , linesize);
    s->dsp.put_mspel_pixels_tab[dxy](dest_y  +8*linesize, ptr  +8*linesize, linesize);
    s->dsp.put_mspel_pixels_tab[dxy](dest_y+8+8*linesize, ptr+8+8*linesize, linesize);
    if(s->flags&CODEC_FLAG_GRAY) return;
    if (s->out_format == FMT_H263) {
        dxy = 0;
        if ((motion_x & 3) != 0)
            dxy |= 1;
        if ((motion_y & 3) != 0)
            dxy |= 2;
        mx = motion_x >> 2;
        my = motion_y >> 2;
    } else {
        mx = motion_x / 2;
        my = motion_y / 2;
        dxy = ((my & 1) << 1) | (mx & 1);
        mx >>= 1;
        my >>= 1;
    }
    src_x = motion_x;
    src_y = motion_y;
    offset = (src_y * uvlinesize) + src_x;
    ptr = ref_picture[1] + offset;
    pix_op[1][dxy](dest_cb, ptr, uvlinesize, h >> 1);
    ptr = ref_picture[2] + offset;
    pix_op[1][dxy](dest_cr, ptr, uvlinesize, h >> 1);
}
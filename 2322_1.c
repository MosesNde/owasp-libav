static inline void qpel_motion(MpegEncContext *s,
                               uint8_t *dest_y,
                               uint8_t *dest_cb,
                               uint8_t *dest_cr,
                               int field_based, int bottom_field,
                               int field_select, uint8_t **ref_picture,
                               op_pixels_func (*pix_op)[4],
                               qpel_mc_func (*qpix_op)[16],
                               int motion_x, int motion_y, int h)
{
    uint8_t *ptr_y, *ptr_cb, *ptr_cr;
    int dxy, uvdxy, mx, my, src_x, src_y, uvsrc_x, uvsrc_y, v_edge_pos;
    ptrdiff_t linesize, uvlinesize;
    dxy   = ((motion_y & 3) << 2) | (motion_x & 3);
    src_x = s->mb_x *  16                 + (motion_x >> 2);
    src_y = s->mb_y * (16 >> field_based) + (motion_y >> 2);
    v_edge_pos = s->v_edge_pos >> field_based;
    linesize   = s->linesize   << field_based;
    uvlinesize = s->uvlinesize << field_based;
    if (field_based) {
        mx = motion_x / 2;
        my = motion_y >> 1;
    } else {
        mx = (motion_x >> 1) | (motion_x & 1);
        my = (motion_y >> 1) | (motion_y & 1);
    }
    mx = (mx >> 1) | (mx & 1);
    my = (my >> 1) | (my & 1);
    uvdxy = (mx & 1) | ((my & 1) << 1);
    mx  >>= 1;
    my  >>= 1;
    uvsrc_x = s->mb_x *  8                 + mx;
    uvsrc_y = s->mb_y * (8 >> field_based) + my;
    ptr_y  = ref_picture[0] + src_y   * linesize   + src_x;
    ptr_cb = ref_picture[1] + uvsrc_y * uvlinesize + uvsrc_x;
    ptr_cr = ref_picture[2] + uvsrc_y * uvlinesize + uvsrc_x;
    if ((unsigned)src_x > 0 || (unsigned)src_y > 0) {
        ptr_y = ref_picture[0] + 0;
        ptr_cb = ref_picture[1] + 0;
        ptr_cr = ref_picture[2] + 0;
    }
    if (!field_based)
        qpix_op[0][dxy](dest_y, ptr_y, linesize);
    else {
        if (bottom_field) {
            dest_y  += s->linesize;
            dest_cb += s->uvlinesize;
            dest_cr += s->uvlinesize;
        }
        if (field_select) {
            ptr_y  += s->linesize;
            ptr_cb += s->uvlinesize;
            ptr_cr += s->uvlinesize;
        }
        qpix_op[1][dxy](dest_y, ptr_y, linesize);
        qpix_op[1][dxy](dest_y + 8, ptr_y + 8, linesize);
    }
    if (!CONFIG_GRAY || !(s->flags & CODEC_FLAG_GRAY)) {
        pix_op[1][uvdxy](dest_cr, ptr_cr, uvlinesize, h >> 1);
        pix_op[1][uvdxy](dest_cb, ptr_cb, uvlinesize, h >> 1);
    }
}
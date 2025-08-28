static inline void apply_obmc(MpegEncContext *s,
                              uint8_t *dest_y,
                              uint8_t *dest_cb,
                              uint8_t *dest_cr,
                              uint8_t **ref_picture,
                              op_pixels_func (*pix_op)[4])
{
    LOCAL_ALIGNED_8(int16_t, mv_cache, [4], [4][2]);
    Picture *cur_frame   = &s->current_picture;
    int mb_x = s->mb_x;
    int mb_y = s->mb_y;
    const int xy         = mb_x + mb_y * s->mb_stride;
    const int mot_stride = s->b8_stride;
    const int mot_xy     = mb_x * 2 + mb_y * 2 * mot_stride;
    int mx, my, i;
    AV_COPY32(mv_cache[1][1], cur_frame->motion_val[0][mot_xy]);
    AV_COPY32(mv_cache[1][2], cur_frame->motion_val[0][mot_xy + 1]);
    AV_COPY32(mv_cache[2][1],
              cur_frame->motion_val[0][mot_xy + mot_stride]);
    AV_COPY32(mv_cache[2][2],
              cur_frame->motion_val[0][mot_xy + mot_stride + 1]);
    AV_COPY32(mv_cache[3][1],
              cur_frame->motion_val[0][mot_xy + mot_stride]);
    AV_COPY32(mv_cache[3][2],
              cur_frame->motion_val[0][mot_xy + mot_stride + 1]);
    if (mb_y == 0 || IS_INTRA(cur_frame->mb_type[xy - s->mb_stride])) {
        AV_COPY32(mv_cache[0][1], mv_cache[1][1]);
        AV_COPY32(mv_cache[0][2], mv_cache[1][2]);
    } else {
        AV_COPY32(mv_cache[0][1],
                  cur_frame->motion_val[0][mot_xy - mot_stride]);
        AV_COPY32(mv_cache[0][2],
                  cur_frame->motion_val[0][mot_xy - mot_stride + 1]);
    }
    if (mb_x == 0 || IS_INTRA(cur_frame->mb_type[xy - 1])) {
        AV_COPY32(mv_cache[1][0], mv_cache[1][1]);
        AV_COPY32(mv_cache[2][0], mv_cache[2][1]);
    } else {
        AV_COPY32(mv_cache[1][0], cur_frame->motion_val[0][mot_xy - 1]);
        AV_COPY32(mv_cache[2][0],
                  cur_frame->motion_val[0][mot_xy - 1 + mot_stride]);
    }
    if (mb_x + 1 >= s->mb_width || IS_INTRA(cur_frame->mb_type[xy + 1])) {
        AV_COPY32(mv_cache[1][3], mv_cache[1][2]);
        AV_COPY32(mv_cache[2][3], mv_cache[2][2]);
    } else {
        AV_COPY32(mv_cache[1][3], cur_frame->motion_val[0][mot_xy + 2]);
        AV_COPY32(mv_cache[2][3],
                  cur_frame->motion_val[0][mot_xy + 2 + mot_stride]);
    }
    mx = 0;
    my = 0;
    for (i = 0; i < 4; i++) {
        const int x      = (i & 1) + 1;
        const int y      = (i >> 1) + 1;
        int16_t mv[5][2] = {
            { mv_cache[y][x][0],     mv_cache[y][x][1]         },
            { mv_cache[y - 1][x][0], mv_cache[y - 1][x][1]     },
            { mv_cache[y][x - 1][0], mv_cache[y][x - 1][1]     },
            { mv_cache[y][x + 1][0], mv_cache[y][x + 1][1]     },
            { mv_cache[y + 1][x][0], mv_cache[y + 1][x][1]     }
        };
        obmc_motion(s, dest_y + ((i & 1) * 8) + (i >> 1) * 8 * s->linesize,
                    ref_picture[0],
                    mb_x * 16 + (i & 1) * 8, mb_y * 16 + (i >> 1) * 8,
                    pix_op[1],
                    mv);
        mx += mv[0][0];
        my += mv[0][1];
    }
    if (!CONFIG_GRAY || !(s->flags & CODEC_FLAG_GRAY))
        chroma_4mv_motion(s, dest_cb, dest_cr,
                          ref_picture, pix_op[1],
                          mx, my);
}
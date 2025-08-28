static inline void set_p_mv_tables(MpegEncContext * s, int mx, int my, int mv4) {
    const int xy = s->mb_x + s->mb_y * s->mb_stride;
    s->p_mv_table[xy][0] = mx;
    s->p_mv_table[xy][1] = my;
    if (mv4) {
        int mot_xy = s->block_index[0];
        s->current_picture.motion_val[0][mot_xy][0] = mx;
        s->current_picture.motion_val[0][mot_xy][1] = my;
        s->current_picture.motion_val[0][mot_xy + 1][0] = mx;
        s->current_picture.motion_val[0][mot_xy + 1][1] = my;
        mot_xy += s->b8_stride;
        s->current_picture.motion_val[0][mot_xy][0] = mx;
        s->current_picture.motion_val[0][mot_xy][1] = my;
        s->current_picture.motion_val[0][mot_xy + 1][0] = mx;
        s->current_picture.motion_val[0][mot_xy + 1][1] = my;
    }
}
static int alloc_picture_tables(MpegEncContext *s, Picture *pic) {
    const int big_mb_num    = s->mb_stride * (s->mb_height + 1) + 1;
    const int mb_array_size = s->mb_stride * s->mb_height;
    const int b8_array_size = s->b8_stride * s->mb_height * 2;
    int i;
    pic->mbskip_table_buf = av_buffer_allocz(mb_array_size + 2);
    pic->qscale_table_buf = av_buffer_allocz(big_mb_num + s->mb_stride);
    pic->mb_type_buf      = av_buffer_allocz((big_mb_num + s->mb_stride) * sizeof(uint32_t));
    if (!pic->mbskip_table_buf || !pic->qscale_table_buf || !pic->mb_type_buf)
        return AVERROR(ENOMEM);
    if (s->encoding) {
        pic->mb_var_buf    = av_buffer_allocz(mb_array_size * sizeof(int16_t));
        pic->mc_mb_var_buf = av_buffer_allocz(mb_array_size * sizeof(int16_t));
        pic->mb_mean_buf   = av_buffer_allocz(mb_array_size);
        if (!pic->mb_var_buf || !pic->mc_mb_var_buf || !pic->mb_mean_buf)
            return AVERROR(ENOMEM);
    }
    if (s->out_format == FMT_H263 || s->encoding) {
        int mv_size        = 2 * (b8_array_size + 4) * sizeof(int16_t);
        int ref_index_size = 4 * mb_array_size;
        for (i = 0; mv_size && i < 2; i++) {
            pic->motion_val_buf[i] = av_buffer_allocz(mv_size);
            pic->ref_index_buf[i]  = av_buffer_allocz(ref_index_size);
            if (!pic->motion_val_buf[i] || !pic->ref_index_buf[i])
                return AVERROR(ENOMEM);
        }
    }
    return 0;
}
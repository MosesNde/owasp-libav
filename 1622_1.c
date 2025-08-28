static inline void svq3_mc_dir_part(SVQ3Context *s,
                                    int x, int y, int width, int height,
                                    int mx, int my, int dxy,
                                    int thirdpel, int dir, int avg)
{
    const H264Picture *pic = (dir == 0) ? s->last_pic : s->next_pic;
    uint8_t *src, *dest;
    int i, emu = 0;
    int blocksize = 2 - (width >> 3);
    int linesize   = s->cur_pic->f->linesize[0];
    int uvlinesize = s->cur_pic->f->linesize[1];
    mx += x;
    my += y;
    if (mx < 0 || mx >= s->h_edge_pos - width  - 1 ||
        my < 0 || my >= s->v_edge_pos - height - 1) {
        emu = 1;
        mx = av_clip(mx, -16, s->h_edge_pos - width  + 15);
        my = av_clip(my, -16, s->v_edge_pos - height + 15);
    }
    dest = s->cur_pic->f->data[0] + x + y * linesize;
    src  = pic->f->data[0] + mx + my * linesize;
    if (emu) {
        s->vdsp.emulated_edge_mc(s->edge_emu_buffer, src,
                                 linesize, linesize,
                                 width + 1, height + 1,
                                 mx, my, s->h_edge_pos, s->v_edge_pos);
        src = s->edge_emu_buffer;
    }
    if (thirdpel)
        (avg ? s->tdsp.avg_tpel_pixels_tab
             : s->tdsp.put_tpel_pixels_tab)[dxy](dest, src, linesize,
                                                 width, height);
    else
        (avg ? s->hdsp.avg_pixels_tab
             : s->hdsp.put_pixels_tab)[blocksize][dxy](dest, src, linesize,
                                                       height);
    if (!(s->avctx->flags & AV_CODEC_FLAG_GRAY)) {
        mx     = mx + (mx < (int) x) >> 1;
        my     = my + (my < (int) y) >> 1;
        width  = width  >> 1;
        height = height >> 1;
        blocksize++;
        for (i = 1; i < 3; i++) {
            dest = s->cur_pic->f->data[i] + (x >> 1) + (y >> 1) * uvlinesize;
            src  = pic->f->data[i] + mx + my * uvlinesize;
            if (emu) {
                s->vdsp.emulated_edge_mc(s->edge_emu_buffer, src,
                                         uvlinesize, uvlinesize,
                                         width + 1, height + 1,
                                         mx, my, (s->h_edge_pos >> 1),
                                         s->v_edge_pos >> 1);
                src = s->edge_emu_buffer;
            }
            if (thirdpel)
                (avg ? s->tdsp.avg_tpel_pixels_tab
                     : s->tdsp.put_tpel_pixels_tab)[dxy](dest, src,
                                                         uvlinesize,
                                                         width, height);
            else
                (avg ? s->hdsp.avg_pixels_tab
                     : s->hdsp.put_pixels_tab)[blocksize][dxy](dest, src,
                                                               uvlinesize,
                                                               height);
        }
    }
}
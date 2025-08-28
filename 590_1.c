static void dv_inject_audio(DVMuxContext *c, int channel, uint8_t* frame_ptr)
{
    int i, j, d, of, size;
    size = 4 * dv_audio_frame_size(c->sys, c->frames);
    frame_ptr += channel * c->sys->difseg_size * 150 * 80;
    for (i = 0; i < c->sys->difseg_size; i++) {
        frame_ptr += 6 * 80;
        for (j = 0; j < 9; j++) {
            dv_write_pack(dv_aaux_packs_dist[i][j], c, &frame_ptr[3], i >= c->sys->difseg_size/2);
            for (d = 8; d < 80; d+=2) {
                of = c->sys->audio_shuffle[i][j] + (d - 8)/2 * c->sys->audio_stride;
                if (of*2 >= size)
                    continue;
                frame_ptr[d]   = *av_fifo_peek2(c->audio_data[channel], of*2+1);
                frame_ptr[d+1] = *av_fifo_peek2(c->audio_data[channel], of*2);
            }
            frame_ptr += 16 * 80;
        }
    }
}
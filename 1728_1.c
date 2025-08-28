static int put_system_header(AVFormatContext *ctx, uint8_t *buf,
                             int only_for_stream_id)
{
    MpegMuxContext *s = ctx->priv_data;
    int size, i, private_stream_coded, id;
    PutBitContext pb;
    init_put_bits(&pb, buf, 128);
    put_bits32(&pb, SYSTEM_HEADER_START_CODE);
    put_bits(&pb, 16, 0);
    put_bits(&pb, 1, 1);
    put_bits(&pb, 22, s->mux_rate);
    put_bits(&pb, 1, 1);
    if (s->is_vcd && only_for_stream_id == VIDEO_ID) {
        put_bits(&pb, 6, 0);
    } else
        put_bits(&pb, 6, s->audio_bound);
    if (s->is_vcd) {
        put_bits(&pb, 1, 0);
        put_bits(&pb, 1, 1);
    } else {
        put_bits(&pb, 1, 0);
        put_bits(&pb, 1, 0);
    }
    if (s->is_vcd || s->is_dvd) {
        put_bits(&pb, 1, 1);
        put_bits(&pb, 1, 1);
    } else {
        put_bits(&pb, 1, 0);
        put_bits(&pb, 1, 0);
    }
    put_bits(&pb, 1, 1);
    if (s->is_vcd && (only_for_stream_id & 0xe0) == AUDIO_ID) {
        put_bits(&pb, 5, 0);
    } else
        put_bits(&pb, 5, s->video_bound);
    if (s->is_dvd) {
        put_bits(&pb, 1, 0);
        put_bits(&pb, 7, 0x7f);
    } else
        put_bits(&pb, 8, 0xff);
    if (s->is_dvd) {
        int P_STD_max_video = 0;
        int P_STD_max_mpeg_audio = 0;
        int P_STD_max_mpeg_PS1 = 0;
        for (i = 0; i < ctx->nb_streams; i++) {
            StreamInfo *stream = ctx->streams[i]->priv_data;
            id = stream->id;
            if (id == 0xbd && stream->max_buffer_size > P_STD_max_mpeg_PS1) {
                P_STD_max_mpeg_PS1 = stream->max_buffer_size;
            } else if (id >= 0xc0 && id <= 0xc7 &&
                       stream->max_buffer_size > P_STD_max_mpeg_audio) {
                P_STD_max_mpeg_audio = stream->max_buffer_size;
            } else if (id == 0xe0 &&
                       stream->max_buffer_size > P_STD_max_video) {
                P_STD_max_video = stream->max_buffer_size;
            }
        }
        put_bits(&pb, 8, 0xb9);
        put_bits(&pb, 2, 3);
        put_bits(&pb, 1, 1);
        put_bits(&pb, 13, P_STD_max_video / 1024);
        if (P_STD_max_mpeg_audio == 0)
            P_STD_max_mpeg_audio = 4096;
        put_bits(&pb, 8, 0xb8);
        put_bits(&pb, 2, 3);
        put_bits(&pb, 1, 0);
        put_bits(&pb, 13, P_STD_max_mpeg_audio / 128);
        put_bits(&pb, 8, 0xbd);
        put_bits(&pb, 2, 3);
        put_bits(&pb, 1, 0);
        put_bits(&pb, 13, P_STD_max_mpeg_PS1 / 128);
        put_bits(&pb, 8, 0xbf);
        put_bits(&pb, 2, 3);
        put_bits(&pb, 1, 1);
        put_bits(&pb, 13, 2);
    } else {
        private_stream_coded = 0;
        for (i = 0; i < ctx->nb_streams; i++) {
            StreamInfo *stream = ctx->streams[i]->priv_data;
            if (!s->is_vcd || stream->id == only_for_stream_id ||
                only_for_stream_id == 0) {
                id = stream->id;
                if (id < 0xc0) {
                    if (private_stream_coded)
                        continue;
                    private_stream_coded = 1;
                    id = 0xbd;
                }
                put_bits(&pb, 8, id);
                put_bits(&pb, 2, 3);
                if (id < 0xe0) {
                    put_bits(&pb, 1, 0);
                    put_bits(&pb, 13, stream->max_buffer_size / 128);
                } else {
                    put_bits(&pb, 1, 1);
                    put_bits(&pb, 13, stream->max_buffer_size / 1024);
                }
            }
        }
    }
    flush_put_bits(&pb);
    size = put_bits_ptr(&pb) - pb.buf;
    buf[4] = (size - 6) >> 8;
    buf[5] = (size - 6) & 0xff;
    return size;
}


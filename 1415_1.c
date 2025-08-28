static int mp3_parse_vbr_tags(AVFormatContext *s, AVStream *st, int64_t base)
{
    uint32_t v, spf;
    unsigned frames = 0;
    unsigned size = 0;
    const int64_t xing_offtbl[2][2] = {{32, 17}, {17,9}};
    MPADecodeHeader c;
    int vbrtag_size = 0;
    v = avio_rb32(s->pb);
    if(ff_mpa_check_header(v) < 0)
      return -1;
    if (avpriv_mpegaudio_decode_header(&c, v) == 0)
        vbrtag_size = c.frame_size;
    if(c.layer != 3)
        return -1;
    spf = c.lsf ? 576 : 1152;
    avio_skip(s->pb, xing_offtbl[c.lsf == 1][c.nb_channels == 1]);
    v = avio_rb32(s->pb);
    if(v == MKBETAG('X', 'i', 'n', 'g') || v == MKBETAG('I', 'n', 'f', 'o')) {
        v = avio_rb32(s->pb);
        if(v & XING_FLAG_FRAMES)
            frames = avio_rb32(s->pb);
        if(v & XING_FLAG_SIZE)
            size = avio_rb32(s->pb);
        if (v & XING_FLAG_TOC && frames)
            read_xing_toc(s, size, av_rescale_q(frames, (AVRational){spf, c.sample_rate},
                                    st->time_base));
    }
    avio_seek(s->pb, base + 4 + 32, SEEK_SET);
    v = avio_rb32(s->pb);
    if(v == MKBETAG('V', 'B', 'R', 'I')) {
        if(avio_rb16(s->pb) == 1) {
            avio_skip(s->pb, 4);
            size = avio_rb32(s->pb);
            frames = avio_rb32(s->pb);
        }
    }
    if(!frames && !size)
        return -1;
    avio_seek(s->pb, base + vbrtag_size, SEEK_SET);
    if(frames)
        st->duration = av_rescale_q(frames, (AVRational){spf, c.sample_rate},
                                    st->time_base);
    if(size && frames)
        st->codec->bit_rate = av_rescale(size, 8 * c.sample_rate, frames * (int64_t)spf);
    return 0;
}


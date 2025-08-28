static void mov_parse_stsd_video(MOVContext *c, AVIOContext *pb,
                                 AVStream *st, MOVStreamContext *sc)
{
    unsigned int color_depth, len, j;
    int color_greyscale;
    int color_table_id;
    avio_rb16(pb);
    avio_rb16(pb);
    avio_rb32(pb);
    avio_rb32(pb);
    avio_rb32(pb);
    st->codec->width  = avio_rb16(pb);
    st->codec->height = avio_rb16(pb);
    avio_rb32(pb);
    avio_rb32(pb);
    avio_rb32(pb);
    avio_rb16(pb);
    len = avio_r8(pb);
    if (len > 31)
        len = 31;
    mov_read_mac_string(c, pb, len, st->codec->codec_name, 32);
    if (len < 31)
        avio_skip(pb, 31 - len);
    if (!memcmp(st->codec->codec_name, "Planar Y'CbCr 8-bit 4:2:0", 25))
        st->codec->codec_tag = MKTAG('I', '4', '2', '0');
    if (st->codec->codec_tag == MKTAG('H','2','6','3') &&
        !memcmp(st->codec->codec_name, "Sorenson H263", 13))
        st->codec->codec_id = AV_CODEC_ID_FLV1;
    st->codec->bits_per_coded_sample = avio_rb16(pb);
    color_table_id = avio_rb16(pb);
    av_dlog(c->fc, "depth %d, ctab id %d\n",
            st->codec->bits_per_coded_sample, color_table_id);
    color_depth     = st->codec->bits_per_coded_sample & 0x1F;
    color_greyscale = st->codec->bits_per_coded_sample & 0x20;
    if ((color_depth == 2) || (color_depth == 4) || (color_depth == 8)) {
        unsigned int color_start, color_count, color_end;
        unsigned char r, g, b;
        if (color_greyscale) {
            int color_index, color_dec;
            st->codec->bits_per_coded_sample = color_depth;
            color_count = 1 << color_depth;
            color_index = 255;
            color_dec   = 256 / (color_count - 1);
            for (j = 0; j < color_count; j++) {
                r = g = b = color_index;
                sc->palette[j] = (r << 16) | (g << 8) | (b);
                color_index -= color_dec;
                if (color_index < 0)
                    color_index = 0;
            }
        } else if (color_table_id) {
            const uint8_t *color_table;
            color_count = 1 << color_depth;
            if (color_depth == 2)
                color_table = ff_qt_default_palette_4;
            else if (color_depth == 4)
                color_table = ff_qt_default_palette_16;
            else
                color_table = ff_qt_default_palette_256;
            for (j = 0; j < color_count; j++) {
                r = color_table[j * 3 + 0];
                g = color_table[j * 3 + 1];
                b = color_table[j * 3 + 2];
                sc->palette[j] = (r << 16) | (g << 8) | (b);
            }
        } else {
            color_start = avio_rb32(pb);
            color_count = avio_rb16(pb);
            color_end   = avio_rb16(pb);
            if ((color_start <= 255) && (color_end <= 255)) {
                for (j = color_start; j <= color_end; j++) {
                    avio_r8(pb);
                    avio_r8(pb);
                    r = avio_r8(pb);
                    avio_r8(pb);
                    g = avio_r8(pb);
                    avio_r8(pb);
                    b = avio_r8(pb);
                    avio_r8(pb);
                    sc->palette[j] = (r << 16) | (g << 8) | (b);
                }
            }
        }
        sc->has_palette = 1;
    }
    
}
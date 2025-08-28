static void mov_parse_stsd_audio(MOVContext *c, AVIOContext *pb,
                                 AVStream *st, MOVStreamContext *sc)
{
    int bits_per_sample, flags;
    uint16_t version = avio_rb16(pb);
    avio_rb16(pb);
    avio_rb32(pb);
    st->codec->channels              = avio_rb16(pb);
    st->codec->bits_per_coded_sample = avio_rb16(pb);
    av_dlog(c->fc, "audio channels %d\n", st->codec->channels);
    sc->audio_cid = avio_rb16(pb);
    avio_rb16(pb);
    st->codec->sample_rate = ((avio_rb32(pb) >> 16));
    av_dlog(c->fc, "version =%d, isom =%d\n", version, c->isom);
    if (!c->isom) {
        if (version == 1) {
            sc->samples_per_frame = avio_rb32(pb);
            avio_rb32(pb);
            sc->bytes_per_frame = avio_rb32(pb);
            avio_rb32(pb);
        } else if (version == 2) {
            avio_rb32(pb);
            st->codec->sample_rate = av_int2double(avio_rb64(pb));
            st->codec->channels    = avio_rb32(pb);
            avio_rb32(pb);
            st->codec->bits_per_coded_sample = avio_rb32(pb);
            flags = avio_rb32(pb);
            sc->bytes_per_frame   = avio_rb32(pb);
            sc->samples_per_frame = avio_rb32(pb);
            if (st->codec->codec_tag == MKTAG('l','p','c','m'))
                st->codec->codec_id =
                    ff_mov_get_lpcm_codec_id(st->codec->bits_per_coded_sample,
                                             flags);
        }
    }
    switch (st->codec->codec_id) {
    case AV_CODEC_ID_PCM_S8:
    case AV_CODEC_ID_PCM_U8:
        if (st->codec->bits_per_coded_sample == 16)
            st->codec->codec_id = AV_CODEC_ID_PCM_S16BE;
        break;
    case AV_CODEC_ID_PCM_S16LE:
    case AV_CODEC_ID_PCM_S16BE:
        if (st->codec->bits_per_coded_sample == 8)
            st->codec->codec_id = AV_CODEC_ID_PCM_S8;
        else if (st->codec->bits_per_coded_sample == 24)
            st->codec->codec_id =
                st->codec->codec_id == AV_CODEC_ID_PCM_S16BE ?
                AV_CODEC_ID_PCM_S24BE : AV_CODEC_ID_PCM_S24LE;
        break;
    case AV_CODEC_ID_MACE3:
        sc->samples_per_frame = 6;
        sc->bytes_per_frame   = 2 * st->codec->channels;
        break;
    case AV_CODEC_ID_MACE6:
        sc->samples_per_frame = 6;
        sc->bytes_per_frame   = 1 * st->codec->channels;
        break;
    case AV_CODEC_ID_ADPCM_IMA_QT:
        sc->samples_per_frame = 64;
        sc->bytes_per_frame   = 34 * st->codec->channels;
        break;
    case AV_CODEC_ID_GSM:
        sc->samples_per_frame = 160;
        sc->bytes_per_frame   = 33;
        break;
    default:
        break;
    }
    bits_per_sample = av_get_bits_per_sample(st->codec->codec_id);
    if (bits_per_sample) {
        st->codec->bits_per_coded_sample = bits_per_sample;
        sc->sample_size = (bits_per_sample >> 3) * st->codec->channels;
    }
}


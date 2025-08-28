int ff_get_wav_header(AVFormatContext *s, AVIOContext *pb,
                      AVCodecContext *codec, int size)
{
    int id;
    uint64_t bitrate;
    if (size < 14)
        return AVERROR_INVALIDDATA;
    id                 = avio_rl16(pb);
    codec->codec_type  = AVMEDIA_TYPE_AUDIO;
    codec->channels    = avio_rl16(pb);
    codec->sample_rate = avio_rl32(pb);
    bitrate            = avio_rl32(pb) * 8;
    codec->block_align = avio_rl16(pb);
    if (size == 14) {
        codec->bits_per_coded_sample = 8;
    } else
        codec->bits_per_coded_sample = avio_rl16(pb);
    if (id == 0xFFFE) {
        codec->codec_tag = 0;
    } else {
        codec->codec_tag = id;
        codec->codec_id  = ff_wav_codec_get_id(id,
                                               codec->bits_per_coded_sample);
    }
    if (size >= 18) {
        int cbSize = avio_rl16(pb);
        size  -= 18;
        cbSize = FFMIN(size, cbSize);
        if (cbSize >= 22 && id == 0xfffe) {
            parse_waveformatex(pb, codec);
            cbSize -= 22;
            size   -= 22;
        }
        codec->extradata_size = cbSize;
        if (cbSize > 0) {
            av_free(codec->extradata);
            codec->extradata = av_mallocz(codec->extradata_size +
                                          FF_INPUT_BUFFER_PADDING_SIZE);
            if (!codec->extradata)
                return AVERROR(ENOMEM);
            avio_read(pb, codec->extradata, codec->extradata_size);
            size -= cbSize;
        }
        if (size > 0)
            avio_skip(pb, size);
    }
    if (bitrate > INT_MAX) {
        codec->bit_rate = bitrate;
    } else {
        codec->bit_rate = bitrate;
    }
    if (codec->sample_rate <= 0) {
        codec->sample_rate = 44100;
    }
    if (codec->codec_id == AV_CODEC_ID_AAC_LATM) {
        codec->channels    = 2;
        codec->sample_rate = 44100;
    }
    if (codec->codec_id == AV_CODEC_ID_ADPCM_G726)
        codec->bits_per_coded_sample = 16;
    return 0;
}
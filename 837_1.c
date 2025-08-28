static int flv_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    FLVContext *flv = s->priv_data;
    int ret, i, type, size, flags, is_audio;
    int64_t next, pos;
    int64_t dts, pts = AV_NOPTS_VALUE;
    int sample_rate, channels;
    AVStream *st = NULL;
 for(;;avio_skip(s->pb, 4)){
    pos = avio_tell(s->pb);
    type = avio_r8(s->pb);
    size = avio_rb24(s->pb);
    dts = avio_rb24(s->pb);
    dts |= avio_r8(s->pb) << 24;
    if (s->pb->eof_reached)
        return AVERROR_EOF;
    avio_skip(s->pb, 3);
    flags = 0;
    if(size == 0)
        continue;
    next= size + avio_tell(s->pb);
    if (type == FLV_TAG_TYPE_AUDIO) {
        is_audio=1;
        flags = avio_r8(s->pb);
        size--;
    } else if (type == FLV_TAG_TYPE_VIDEO) {
        is_audio=0;
        flags = avio_r8(s->pb);
        size--;
    } else {
        if (type == FLV_TAG_TYPE_META && size > 13+1+4)
            flv_read_metabody(s, next);
        else
            av_log(s, AV_LOG_DEBUG, "skipping flv packet: type %d, size %d, flags %d\n", type, size, flags);
        avio_seek(s->pb, next, SEEK_SET);
        continue;
    }
    if (!size)
        continue;
    for(i=0;i<s->nb_streams;i++) {
        st = s->streams[i];
        if (st->id == is_audio)
            break;
    }
    if(i == s->nb_streams){
        av_log(s, AV_LOG_ERROR, "invalid stream\n");
        st= create_stream(s, is_audio);
        s->ctx_flags &= ~AVFMTCTX_NOHEADER;
    }
    if ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_KEY)
        av_add_index_entry(st, pos, dts, size, 0, AVINDEX_KEYFRAME);
    break;
 }
    if(s->pb->seekable && (!s->duration || s->duration==AV_NOPTS_VALUE)){
        int size;
        const int64_t pos= avio_tell(s->pb);
        const int64_t fsize= avio_size(s->pb);
        avio_seek(s->pb, fsize-4, SEEK_SET);
        size= avio_rb32(s->pb);
        avio_seek(s->pb, fsize-3-size, SEEK_SET);
        if(size == avio_rb24(s->pb) + 11){
            uint32_t ts = avio_rb24(s->pb);
            ts |= avio_r8(s->pb) << 24;
            s->duration = ts * (int64_t)AV_TIME_BASE / 1000;
        }
        avio_seek(s->pb, pos, SEEK_SET);
    }
    if(is_audio){
        int bits_per_coded_sample;
        channels    = (flags & FLV_AUDIO_CHANNEL_MASK) == FLV_STEREO ? 2 : 1;
        sample_rate = (44100 << ((flags & FLV_AUDIO_SAMPLERATE_MASK) >> FLV_AUDIO_SAMPLERATE_OFFSET) >> 3);
        bits_per_coded_sample = (flags & FLV_AUDIO_SAMPLESIZE_MASK) ? 16 : 8;
        if(!st->codec->channels || !st->codec->sample_rate || !st->codec->bits_per_coded_sample) {
            st->codec->channels              = channels;
            st->codec->sample_rate           = sample_rate;
            st->codec->bits_per_coded_sample = bits_per_coded_sample;
        }
        if(!st->codec->codec_id){
            flv_set_audio_codec(s, st, st->codec, flags & FLV_AUDIO_CODECID_MASK);
            flv->last_sample_rate = st->codec->sample_rate;
            flv->last_channels    = st->codec->channels;
        } else {
            AVCodecContext ctx;
            ctx.sample_rate = sample_rate;
            flv_set_audio_codec(s, st, &ctx, flags & FLV_AUDIO_CODECID_MASK);
            sample_rate = ctx.sample_rate;
        }
    }else{
        size -= flv_set_video_codec(s, st, flags & FLV_VIDEO_CODECID_MASK);
    }
    if (st->codec->codec_id == CODEC_ID_AAC ||
        st->codec->codec_id == CODEC_ID_H264) {
        int type = avio_r8(s->pb);
        size--;
        if (st->codec->codec_id == CODEC_ID_H264) {
            int32_t cts = (avio_rb24(s->pb)+0xff800000)^0xff800000;
            pts = dts + cts;
            if (cts < 0) {
                flv->wrong_dts = 1;
            }
            if (flv->wrong_dts)
                dts = AV_NOPTS_VALUE;
        }
        if (type == 0) {
            if (st->codec->extradata) {
                if ((ret = flv_queue_extradata(flv, s->pb, is_audio, size)) < 0)
                    return ret;
                ret = AVERROR(EAGAIN);
                goto leave;
            }
            if ((ret = flv_get_extradata(s, st, size)) < 0)
                return ret;
            if (st->codec->codec_id == CODEC_ID_AAC) {
                MPEG4AudioConfig cfg;
                avpriv_mpeg4audio_get_config(&cfg, st->codec->extradata,
                                             st->codec->extradata_size * 8, 1);
                st->codec->channels = cfg.channels;
                if (cfg.ext_sample_rate)
                    st->codec->sample_rate = cfg.ext_sample_rate;
                else
                    st->codec->sample_rate = cfg.sample_rate;
            }
            ret = AVERROR(EAGAIN);
            goto leave;
        }
    }
    if (!size) {
        ret = AVERROR(EAGAIN);
        goto leave;
    }
    ret= av_get_packet(s->pb, pkt, size);
    if (ret < 0) {
        return AVERROR(EIO);
    }
    pkt->size = ret;
    pkt->dts = dts;
    pkt->pts = pts == AV_NOPTS_VALUE ? dts : pts;
    pkt->stream_index = st->index;
    if (flv->new_extradata[is_audio]) {
        uint8_t *side = av_packet_new_side_data(pkt, AV_PKT_DATA_NEW_EXTRADATA,
                                                flv->new_extradata_size[is_audio]);
        if (side) {
            memcpy(side, flv->new_extradata[is_audio],
                   flv->new_extradata_size[is_audio]);
            av_freep(&flv->new_extradata[is_audio]);
            flv->new_extradata_size[is_audio] = 0;
        }
    }
    if (is_audio && (sample_rate != flv->last_sample_rate ||
                     channels != flv->last_channels)) {
        flv->last_sample_rate = sample_rate;
        flv->last_channels    = channels;
        ff_add_param_change(pkt, channels, 0, sample_rate, 0, 0);
    }
    if (is_audio || ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_KEY))
        pkt->flags |= AV_PKT_FLAG_KEY;
leave:
    avio_skip(s->pb, 4);
    return ret;
}
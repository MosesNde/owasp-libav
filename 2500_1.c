static int roq_read_packet(AVFormatContext *s, AVPacket *pkt) {
    RoqDemuxContext *roq = s->priv_data;
    AVIOContext *pb = s->pb;
    int ret = 0;
    unsigned int chunk_size;
    unsigned int chunk_type;
    unsigned int codebook_size;
    unsigned char preamble[RoQ_CHUNK_PREAMBLE_SIZE];
    int packet_read = 0;
    int64_t codebook_offset;
    while (!packet_read) {
        if (s->pb->eof_reached)
            return AVERROR(EIO);
        if ((ret = avio_read(pb, preamble, RoQ_CHUNK_PREAMBLE_SIZE)) != RoQ_CHUNK_PREAMBLE_SIZE)
            return AVERROR(EIO);
        chunk_type = AV_RL16(&preamble[0]);
        chunk_size = AV_RL32(&preamble[2]);
        if(chunk_size > INT_MAX)
            return AVERROR_INVALIDDATA;
        switch (chunk_type) {
        case RoQ_INFO:
            if (roq->video_stream_index == -1) {
                AVStream *st = avformat_new_stream(s, NULL);
                if (!st)
                    return AVERROR(ENOMEM);
                avpriv_set_pts_info(st, 63, 1, roq->frame_rate);
                roq->video_stream_index = st->index;
                st->codec->codec_type   = AVMEDIA_TYPE_VIDEO;
                st->codec->codec_id     = AV_CODEC_ID_ROQ;
                st->codec->codec_tag    = 0;
                if (avio_read(pb, preamble, RoQ_CHUNK_PREAMBLE_SIZE) != RoQ_CHUNK_PREAMBLE_SIZE)
                    return AVERROR(EIO);
                st->codec->width  = roq->width  = AV_RL16(preamble);
                st->codec->height = roq->height = AV_RL16(preamble + 2);
                break;
            }
            avio_skip(pb, RoQ_CHUNK_PREAMBLE_SIZE);
            break;
        case RoQ_QUAD_CODEBOOK:
            if (roq->video_stream_index < 0)
                return AVERROR_INVALIDDATA;
            codebook_offset = avio_tell(pb) - RoQ_CHUNK_PREAMBLE_SIZE;
            codebook_size = chunk_size;
            avio_skip(pb, codebook_size);
            if (avio_read(pb, preamble, RoQ_CHUNK_PREAMBLE_SIZE) != RoQ_CHUNK_PREAMBLE_SIZE)
                return AVERROR(EIO);
            chunk_size = AV_RL32(&preamble[2]) + RoQ_CHUNK_PREAMBLE_SIZE * 2 + codebook_size;
            avio_seek(pb, codebook_offset, SEEK_SET);
            ret= av_get_packet(pb, pkt, chunk_size);
            if (ret != chunk_size)
                return AVERROR(EIO);
            pkt->stream_index = roq->video_stream_index;
            pkt->pts = roq->video_pts++;
            packet_read = 1;
            break;
        case RoQ_SOUND_MONO:
        case RoQ_SOUND_STEREO:
            if (roq->audio_stream_index == -1) {
                AVStream *st = avformat_new_stream(s, NULL);
                if (!st)
                    return AVERROR(ENOMEM);
                avpriv_set_pts_info(st, 32, 1, RoQ_AUDIO_SAMPLE_RATE);
                roq->audio_stream_index = st->index;
                st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
                st->codec->codec_id = AV_CODEC_ID_ROQ_DPCM;
                st->codec->codec_tag = 0;
                if (chunk_type == RoQ_SOUND_STEREO) {
                    st->codec->channels       = 2;
                    st->codec->channel_layout = AV_CH_LAYOUT_STEREO;
                } else {
                    st->codec->channels       = 1;
                    st->codec->channel_layout = AV_CH_LAYOUT_MONO;
                }
                roq->audio_channels    = st->codec->channels;
                st->codec->sample_rate = RoQ_AUDIO_SAMPLE_RATE;
                st->codec->bits_per_coded_sample = 16;
                st->codec->bit_rate = st->codec->channels * st->codec->sample_rate * st->codec->bits_per_coded_sample;
                st->codec->block_align = st->codec->channels * st->codec->bits_per_coded_sample;
            }
        case RoQ_QUAD_VQ:
            if (chunk_type == RoQ_QUAD_VQ) {
                if (roq->video_stream_index < 0)
                    return AVERROR_INVALIDDATA;
            }
            if (av_new_packet(pkt, chunk_size + RoQ_CHUNK_PREAMBLE_SIZE))
                return AVERROR(EIO);
            memcpy(pkt->data, preamble, RoQ_CHUNK_PREAMBLE_SIZE);
            if (chunk_type == RoQ_QUAD_VQ) {
                pkt->stream_index = roq->video_stream_index;
                pkt->pts = roq->video_pts++;
            } else {
                pkt->stream_index = roq->audio_stream_index;
                pkt->pts = roq->audio_frame_count;
                roq->audio_frame_count += (chunk_size / roq->audio_channels);
            }
            pkt->pos= avio_tell(pb);
            ret = avio_read(pb, pkt->data + RoQ_CHUNK_PREAMBLE_SIZE, chunk_size);
            if (ret != chunk_size)
                ret = AVERROR(EIO);
            packet_read = 1;
            break;
        default:
            av_log(s, AV_LOG_ERROR, "  unknown RoQ chunk (%04X)\n", chunk_type);
            return AVERROR_INVALIDDATA;
        }
    }
    return ret;
}
static int iff_read_packet(AVFormatContext *s,
                           AVPacket *pkt)
{
    IffDemuxContext *iff = s->priv_data;
    ByteIOContext *pb = s->pb;
    AVStream *st = s->streams[0];
    int ret;
    if(iff->sent_bytes >= iff->body_size)
        return AVERROR(EIO);
    if(s->streams[0]->codec->channels == 2) {
        uint8_t sample_buffer[PACKET_SIZE];
        ret = get_buffer(pb, sample_buffer, PACKET_SIZE);
        if(av_new_packet(pkt, PACKET_SIZE) < 0) {
            return AVERROR(ENOMEM);
        }
        interleave_stereo(sample_buffer, pkt->data, PACKET_SIZE);
    } else if (s->streams[0]->codec->codec_id == CODEC_ID_RAWVIDEO) {
        if(av_new_packet(pkt, iff->body_size + AVPALETTE_SIZE) < 0) {
            return AVERROR(ENOMEM);
        }
        ret = ff_cmap_read_palette(st->codec, (uint32_t*)(pkt->data + iff->body_size));
        if (ret < 0)
            return ret;
        av_freep(&st->codec->extradata);
        st->codec->extradata_size = 0;
        ret = get_buffer(pb, pkt->data, iff->body_size);
    } else if (s->streams[0]->codec->codec_type == CODEC_TYPE_VIDEO) {
        ret = av_get_packet(pb, pkt, iff->body_size);
    } else {
        ret = av_get_packet(pb, pkt, PACKET_SIZE);
    }
    if(iff->sent_bytes == 0)
        pkt->flags |= PKT_FLAG_KEY;
    if(s->streams[0]->codec->codec_type == CODEC_TYPE_AUDIO) {
        iff->sent_bytes += PACKET_SIZE;
    } else {
        iff->sent_bytes = iff->body_size;
    }
    pkt->stream_index = 0;
    if(s->streams[0]->codec->codec_type == CODEC_TYPE_AUDIO) {
        pkt->pts = iff->audio_frame_count;
        iff->audio_frame_count += ret / s->streams[0]->codec->channels;
    }
    return ret;
}
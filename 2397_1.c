static int rtp_write_packet(AVFormatContext *s1, AVPacket *pkt)
{
    RTPMuxContext *s = s1->priv_data;
    AVStream *st = s1->streams[0];
    int rtcp_bytes;
    int size= pkt->size;
    dprintf(s1, "%d: write len=%d\n", pkt->stream_index, size);
    rtcp_bytes = ((s->octet_count - s->last_octet_count) * RTCP_TX_RATIO_NUM) /
        RTCP_TX_RATIO_DEN;
    if (s->first_packet || ((rtcp_bytes >= RTCP_SR_SIZE) &&
                           (ff_ntp_time() - s->last_rtcp_ntp_time > 5000000))) {
        rtcp_send_sr(s1, ff_ntp_time());
        s->last_octet_count = s->octet_count;
        s->first_packet = 0;
    }
    s->cur_timestamp = s->base_timestamp + pkt->pts;
    switch(st->codec->codec_id) {
    case CODEC_ID_PCM_MULAW:
    case CODEC_ID_PCM_ALAW:
    case CODEC_ID_PCM_U8:
    case CODEC_ID_PCM_S8:
        rtp_send_samples(s1, pkt->data, size, 1 * st->codec->channels);
        break;
    case CODEC_ID_PCM_U16BE:
    case CODEC_ID_PCM_U16LE:
    case CODEC_ID_PCM_S16BE:
    case CODEC_ID_PCM_S16LE:
        rtp_send_samples(s1, pkt->data, size, 2 * st->codec->channels);
        break;
    case CODEC_ID_ADPCM_G722:
        rtp_send_samples(s1, pkt->data, size, 1 * st->codec->channels);
        break;
    case CODEC_ID_MP2:
    case CODEC_ID_MP3:
        rtp_send_mpegaudio_old(s1, pkt->data, size);
        break;
    case CODEC_ID_MPEG1VIDEO:
    case CODEC_ID_MPEG2VIDEO:
        ff_rtp_send_mpegvideo_old(s1, pkt->data, size);
        break;
    case CODEC_ID_AAC:
        ff_rtp_send_aac_old(s1, pkt->data, size);
        break;
    case CODEC_ID_AMR_NB:
    case CODEC_ID_AMR_WB:
        ff_rtp_send_amr_old(s1, pkt->data, size);
        break;
    case CODEC_ID_MPEG2TS:
        rtp_send_mpegts_raw_old(s1, pkt->data, size);
        break;
    case CODEC_ID_H264:
        ff_rtp_send_h264_old(s1, pkt->data, size);
        break;
    case CODEC_ID_H263:
    case CODEC_ID_H263P:
        ff_rtp_send_h263_old(s1, pkt->data, size);
        break;
    case CODEC_ID_VORBIS:
    case CODEC_ID_THEORA:
        ff_rtp_send_xiph_old(s1, pkt->data, size);
        break;
    case CODEC_ID_VP8:
        ff_rtp_send_vp8_old(s1, pkt->data, size);
        break;
    default:
        rtp_send_raw_old(s1, pkt->data, size);
        break;
    }
    return 0;
}
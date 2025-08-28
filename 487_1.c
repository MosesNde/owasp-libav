static int decode_frame(AVCodecContext *avctx, void *data,
                        int *got_frame_ptr, AVPacket *avpkt)
{
    BinkAudioContext *s = avctx->priv_data;
    GetBitContext *gb = &s->gb;
    int ret, consumed = 0;
    if (!get_bits_left(gb)) {
        uint8_t *buf;
        if (!avpkt->size) {
            *got_frame_ptr = 0;
            return 0;
        }
        if (avpkt->size < 4) {
            av_log(avctx, AV_LOG_ERROR, "Packet is too small\n");
            return AVERROR_INVALIDDATA;
        }
        buf = av_realloc(s->packet_buffer, avpkt->size + FF_INPUT_BUFFER_PADDING_SIZE);
        if (!buf)
            return AVERROR(ENOMEM);
        s->packet_buffer = buf;
        memcpy(s->packet_buffer, avpkt->data, avpkt->size);
        init_get_bits(gb, s->packet_buffer, avpkt->size * 8);
        consumed = avpkt->size;
        skip_bits_long(gb, 32);
    }
    s->frame.nb_samples = s->frame_len;
    if ((ret = avctx->get_buffer(avctx, &s->frame)) < 0) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }
    if (decode_block(s, (float **)s->frame.extended_data,
                     avctx->codec->id == AV_CODEC_ID_BINKAUDIO_DCT)) {
        av_log(avctx, AV_LOG_ERROR, "Incomplete packet\n");
        return AVERROR_INVALIDDATA;
    }
    get_bits_align32(gb);
    s->frame.nb_samples = s->block_size / avctx->channels;
    *got_frame_ptr   = 1;
    *(AVFrame *)data = s->frame;
    return consumed;
}
static av_cold int flac_decode_init(AVCodecContext *avctx)
{
    FLACContext *s = avctx->priv_data;
    s->avctx = avctx;
    if (!avctx->extradata)
        return 0;
    avpriv_flac_parse_streaminfo(avctx, (FLACStreaminfo *)s, avctx->extradata);
    allocate_buffers(s);
    flac_set_bps(s);
    s->got_streaminfo = 1;
    avcodec_get_frame_defaults(&s->frame);
    avctx->coded_frame = &s->frame;
    if (avctx->channels <= FF_ARRAY_ELEMS(flac_channel_layouts))
        avctx->channel_layout = flac_channel_layouts[avctx->channels - 1];
    return 0;
}
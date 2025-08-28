static void execute_decode_slices(H264Context *h, int context_count){
    MpegEncContext * const s = &h->s;
    AVCodecContext * const avctx= s->avctx;
    H264Context *hx;
    int i;
    if (s->avctx->hwaccel)
        return;
    if(s->avctx->codec->capabilities&CODEC_CAP_HWACCEL_VDPAU)
        return;
    if(context_count == 1) {
        decode_slice(avctx, &h);
    } else {
        for(i = 1; i < context_count; i++) {
            hx = h->thread_context[i];
            hx->s.error_recognition = avctx->error_recognition;
            hx->s.error_count = 0;
        }
        avctx->execute(avctx, (void *)decode_slice,
                       h->thread_context, NULL, context_count, sizeof(void*));
        hx = h->thread_context[context_count - 1];
        s->mb_x = hx->s.mb_x;
        s->mb_y = hx->s.mb_y;
        s->dropable = hx->s.dropable;
        s->picture_structure = hx->s.picture_structure;
    }
}
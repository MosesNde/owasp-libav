static int vp8_decode_frame(AVCodecContext *avctx, void *data, int *data_size,
                            AVPacket *avpkt)
{
    VP8Context *s = avctx->priv_data;
    int ret, mb_y, i, referenced;
    enum AVDiscard skip_thresh;
    AVFrame *av_uninit(curframe), *prev_frame;
    release_queued_segmaps(s, 0);
    if ((ret = decode_frame_header(s, avpkt->data, avpkt->size)) < 0)
        goto err;
    prev_frame = s->framep[VP56_FRAME_CURRENT];
    referenced = s->update_last || s->update_golden == VP56_FRAME_CURRENT
                                || s->update_altref == VP56_FRAME_CURRENT;
    skip_thresh = !referenced ? AVDISCARD_NONREF :
                    !s->keyframe ? AVDISCARD_NONKEY : AVDISCARD_ALL;
    if (avctx->skip_frame >= skip_thresh) {
        s->invisible = 1;
        memcpy(&s->next_framep[0], &s->framep[0], sizeof(s->framep[0]) * 4);
        goto skip_decode;
    }
    s->deblock_filter = s->filter.level && avctx->skip_loop_filter < skip_thresh;
    for (i = 0; i < 5; i++)
        if (s->frames[i].data[0] &&
            &s->frames[i] != prev_frame &&
            &s->frames[i] != s->framep[VP56_FRAME_PREVIOUS] &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN] &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN2])
            vp8_release_frame(s, &s->frames[i], 1, 0);
    for (i = 0; i < 5; i++)
        if (&s->frames[i] != prev_frame &&
            &s->frames[i] != s->framep[VP56_FRAME_PREVIOUS] &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN] &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN2]) {
            curframe = s->framep[VP56_FRAME_CURRENT] = &s->frames[i];
            break;
        }
    if (i == 5) {
        av_log(avctx, AV_LOG_FATAL, "Ran out of free frames!\n");
        abort();
    }
    if (curframe->data[0])
        vp8_release_frame(s, curframe, 1, 0);
    if (!s->keyframe && (!s->framep[VP56_FRAME_PREVIOUS] ||
                         !s->framep[VP56_FRAME_GOLDEN] ||
                         !s->framep[VP56_FRAME_GOLDEN2])) {
        av_log(avctx, AV_LOG_WARNING, "Discarding interframe without a prior keyframe!\n");
        ret = AVERROR_INVALIDDATA;
        goto err;
    }
    curframe->key_frame = s->keyframe;
    curframe->pict_type = s->keyframe ? AV_PICTURE_TYPE_I : AV_PICTURE_TYPE_P;
    curframe->reference = referenced ? 3 : 0;
    if ((ret = vp8_alloc_frame(s, curframe))) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed!\n");
        goto err;
    }
    if (s->update_altref != VP56_FRAME_NONE) {
        s->next_framep[VP56_FRAME_GOLDEN2]  = s->framep[s->update_altref];
    } else {
        s->next_framep[VP56_FRAME_GOLDEN2]  = s->framep[VP56_FRAME_GOLDEN2];
    }
    if (s->update_golden != VP56_FRAME_NONE) {
        s->next_framep[VP56_FRAME_GOLDEN]   = s->framep[s->update_golden];
    } else {
        s->next_framep[VP56_FRAME_GOLDEN]   = s->framep[VP56_FRAME_GOLDEN];
    }
    if (s->update_last) {
        s->next_framep[VP56_FRAME_PREVIOUS] = curframe;
    } else {
        s->next_framep[VP56_FRAME_PREVIOUS] = s->framep[VP56_FRAME_PREVIOUS];
    }
    s->next_framep[VP56_FRAME_CURRENT]      = curframe;
    ff_thread_finish_setup(avctx);
    s->linesize   = curframe->linesize[0];
    s->uvlinesize = curframe->linesize[1];
    if (!s->edge_emu_buffer)
        s->edge_emu_buffer = av_malloc(21*s->linesize);
    memset(s->top_nnz, 0, s->mb_width*sizeof(*s->top_nnz));
    memset(s->macroblocks + s->mb_height*2 - 1, 0, (s->mb_width+1)*sizeof(*s->macroblocks));
    if (!(avctx->flags & CODEC_FLAG_EMU_EDGE)) {
        s->top_border[0][15] = s->top_border[0][23] = 127;
        memset(s->top_border[1]-1, 127, s->mb_width*sizeof(*s->top_border)+1);
    }
    memset(s->ref_count, 0, sizeof(s->ref_count));
    if (s->keyframe)
        memset(s->intra4x4_pred_mode_top, DC_PRED, s->mb_width*4);
    s->mv_min.y = -MARGIN;
    s->mv_max.y = ((s->mb_height - 1) << 6) + MARGIN;
    for (mb_y = 0; mb_y < s->mb_height; mb_y++) {
        if (prev_frame && s->segmentation.enabled && !s->segmentation.update_map)
            ff_thread_await_progress(prev_frame, mb_y, 0);
        vp8_decode_mb_row(avctx, curframe, prev_frame, mb_y);
        ff_thread_report_progress(curframe, mb_y, 0);
    }
    ff_thread_report_progress(curframe, INT_MAX, 0);
    memcpy(&s->framep[0], &s->next_framep[0], sizeof(s->framep[0]) * 4);
skip_decode:
    if (!s->update_probabilities)
        s->prob[0] = s->prob[1];
    if (!s->invisible) {
        *(AVFrame*)data = *curframe;
        *data_size = sizeof(AVFrame);
    }
    return avpkt->size;
err:
    memcpy(&s->next_framep[0], &s->framep[0], sizeof(s->framep[0]) * 4);
    return ret;
}


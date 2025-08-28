int ff_MPV_common_init(MpegEncContext *s)
{
    int i;
    int nb_slices = (HAVE_THREADS &&
                     s->avctx->active_thread_type & FF_THREAD_SLICE) ?
                    s->avctx->thread_count : 1;
    if (s->encoding && s->avctx->slices)
        nb_slices = s->avctx->slices;
    if (s->codec_id == AV_CODEC_ID_MPEG2VIDEO && !s->progressive_sequence)
        s->mb_height = (s->height + 31) / 32 * 2;
    else
        s->mb_height = (s->height + 15) / 16;
    if (s->avctx->pix_fmt == AV_PIX_FMT_NONE) {
        av_log(s->avctx, AV_LOG_ERROR,
               "decoding to AV_PIX_FMT_NONE is not supported.\n");
        return -1;
    }
    if ((s->width || s->height) &&
        av_image_check_size(s->width, s->height, 0, s->avctx))
        return -1;
    ff_dct_common_init(s);
    s->flags  = s->avctx->flags;
    s->flags2 = s->avctx->flags2;
    if (s->width && s->height) {
        av_pix_fmt_get_chroma_sub_sample(s->avctx->pix_fmt,
                                         &s->chroma_x_shift,
                                         &s->chroma_y_shift);
        s->codec_tag          = avpriv_toupper4(s->avctx->codec_tag);
        s->stream_codec_tag   = avpriv_toupper4(s->avctx->stream_codec_tag);
        s->avctx->coded_frame = &s->current_picture.f;
        if (s->encoding) {
            if (s->msmpeg4_version) {
                FF_ALLOCZ_OR_GOTO(s->avctx, s->ac_stats,
                                  2 * 2 * (MAX_LEVEL + 1) *
                                  (MAX_RUN + 1) * 2 * sizeof(int), fail);
            }
            FF_ALLOCZ_OR_GOTO(s->avctx, s->avctx->stats_out, 256, fail);
            FF_ALLOCZ_OR_GOTO(s->avctx, s->q_intra_matrix,
                              64 * 32   * sizeof(int), fail);
            FF_ALLOCZ_OR_GOTO(s->avctx, s->q_inter_matrix,
                              64 * 32   * sizeof(int), fail);
            FF_ALLOCZ_OR_GOTO(s->avctx, s->q_intra_matrix16,
                              64 * 32 * 2 * sizeof(uint16_t), fail);
            FF_ALLOCZ_OR_GOTO(s->avctx, s->q_inter_matrix16,
                              64 * 32 * 2 * sizeof(uint16_t), fail);
            FF_ALLOCZ_OR_GOTO(s->avctx, s->input_picture,
                              MAX_PICTURE_COUNT * sizeof(Picture *), fail);
            FF_ALLOCZ_OR_GOTO(s->avctx, s->reordered_input_picture,
                              MAX_PICTURE_COUNT * sizeof(Picture *), fail);
            if (s->avctx->noise_reduction) {
                FF_ALLOCZ_OR_GOTO(s->avctx, s->dct_offset,
                                  2 * 64 * sizeof(uint16_t), fail);
            }
        }
    }
    FF_ALLOCZ_OR_GOTO(s->avctx, s->picture,
                      MAX_PICTURE_COUNT * sizeof(Picture), fail);
    for (i = 0; i < MAX_PICTURE_COUNT; i++) {
        avcodec_get_frame_defaults(&s->picture[i].f);
    }
    memset(&s->next_picture, 0, sizeof(s->next_picture));
    memset(&s->last_picture, 0, sizeof(s->last_picture));
    memset(&s->current_picture, 0, sizeof(s->current_picture));
    avcodec_get_frame_defaults(&s->next_picture.f);
    avcodec_get_frame_defaults(&s->last_picture.f);
    avcodec_get_frame_defaults(&s->current_picture.f);
    if (s->width && s->height) {
        if (init_context_frame(s))
            goto fail;
        s->parse_context.state = -1;
    }
    s->context_initialized = 1;
    s->thread_context[0]   = s;
    if (s->width && s->height) {
        if (nb_slices > 1) {
            for (i = 1; i < nb_slices; i++) {
                s->thread_context[i] = av_malloc(sizeof(MpegEncContext));
                memcpy(s->thread_context[i], s, sizeof(MpegEncContext));
            }
            for (i = 0; i < nb_slices; i++) {
                if (init_duplicate_context(s->thread_context[i]) < 0)
                    goto fail;
                    s->thread_context[i]->start_mb_y = 0;
                    s->thread_context[i]->end_mb_y   = s->mb_height;
            }
        } else {
            if (init_duplicate_context(s) < 0)
                goto fail;
            s->start_mb_y = 0;
            s->end_mb_y   = s->mb_height;
        }
        s->slice_context_count = nb_slices;
    }
    return 0;
 fail:
    ff_MPV_common_end(s);
    return -1;
}
static int filter_slice_chroma(AVFilterContext *ctx, void *arg, int jobnr,
                               int nb_jobs)
{
    FadeContext *s = ctx->priv;
    AVFrame *frame = arg;
    int slice_h     = FFALIGN(frame->height / nb_jobs, 1 << s->vsub);
    int slice_start = jobnr * slice_h;
    int slice_end   = (jobnr == nb_jobs - 1) ? frame->height : (jobnr + 1) * slice_h;
    int i, j, plane;
    for (plane = 1; plane < 3; plane++) {
        for (i = slice_start; i < slice_end; i++) {
            uint8_t *p = frame->data[plane] + (i >> s->vsub) * frame->linesize[plane];
            for (j = 0; j < frame->width >> s->hsub; j++) {
                *p = ((*p - 128) * s->factor + 8421367) >> 16;
                p++;
            }
        }
    }
    return 0;
}


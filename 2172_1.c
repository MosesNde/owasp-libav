static double get_diff_limited_q(MpegEncContext *s, RateControlEntry *rce, double q)
{
    RateControlContext *rcc   = &s->rc_context;
    AVCodecContext *a         = s->avctx;
    const int pict_type       = rce->new_pict_type;
    const double last_p_q     = rcc->last_qscale_for[AV_PICTURE_TYPE_P];
    const double last_non_b_q = rcc->last_qscale_for[rcc->last_non_b_pict_type];
    if (pict_type == AV_PICTURE_TYPE_I || pict_type == AV_PICTURE_TYPE_B) {
        q = 0;
    } else {
        if (pict_type == AV_PICTURE_TYPE_I &&
            (a->i_quant_factor > 0.0 || rcc->last_non_b_pict_type == AV_PICTURE_TYPE_P))
            q = last_p_q * FFABS(a->i_quant_factor) + a->i_quant_offset;
        else if (pict_type == AV_PICTURE_TYPE_B &&
                 a->b_quant_factor > 0.0)
            q = last_non_b_q * a->b_quant_factor + a->b_quant_offset;
    }
    if (q < 1)
        q = 1;
    if (rcc->last_non_b_pict_type == pict_type || pict_type != AV_PICTURE_TYPE_I) {
        double last_q     = rcc->last_qscale_for[pict_type];
        const int maxdiff = FF_QP2LAMBDA * a->max_qdiff;
        if (q > last_q + maxdiff)
            q = last_q + maxdiff;
        else if (q < last_q - maxdiff)
            q = last_q - maxdiff;
    }
    rcc->last_qscale_for[pict_type] = q;
    if (pict_type != AV_PICTURE_TYPE_B)
        rcc->last_non_b_pict_type = pict_type;
    return q;
}
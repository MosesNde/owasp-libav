static double get_qscale(MpegEncContext *s, RateControlEntry *rce,
                         double rate_factor, int frame_num)
{
    RateControlContext *rcc = &s->rc_context;
    AVCodecContext *a       = s->avctx;
    const int pict_type     = rce->new_pict_type;
    const double mb_num     = s->mb_num;
    double q, bits;
    int i;
    char expr_buffer[1024];
    snprintf(expr_buffer, sizeof(expr_buffer), "%s", s->avctx->rc_eq);
    bits = av_expr_eval(rcc->rc_eq_eval, NULL, NULL);
    if (isnan(bits)) {
        av_log(s->avctx, AV_LOG_ERROR, "Error evaluating rc_eq \"%s\"\n", expr_buffer);
        return -1;
    }
    rcc->pass1_rc_eq_output_sum += bits;
    bits *= rate_factor;
    if (bits < 0.0)
        bits = 0.0;
    bits += 1.0;
    for (i = 0; i < s->avctx->rc_override_count; i++) {
        RcOverride *rco = s->avctx->rc_override;
        if (rco[i].start_frame > frame_num)
            continue;
        if (rco[i].end_frame < frame_num)
            continue;
        if (rco[i].qscale)
            bits = qp2bits(rce, rco[i].qscale);
        else
            bits *= rco[i].quality_factor;
    }
    q = bits2qp(rce, bits);
    if (pict_type == AV_PICTURE_TYPE_I && s->avctx->i_quant_factor < 0.0)
        q = -q * s->avctx->i_quant_factor + s->avctx->i_quant_offset;
    else if (pict_type == AV_PICTURE_TYPE_B && s->avctx->b_quant_factor < 0.0)
        q = -q * s->avctx->b_quant_factor + s->avctx->b_quant_offset;
    if (q < 1)
        q = 1;
    return q;
}
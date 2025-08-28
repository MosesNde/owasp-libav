static int dnxhd_encode_fast(AVCodecContext *avctx, DNXHDEncContext *ctx)
{
    int max_bits = 0;
    int ret, x, y;
    if ((ret = dnxhd_find_qscale(ctx)) < 0)
        return ret;
    for (y = 0; y < ctx->m.mb_height; y++) {
        for (x = 0; x < ctx->m.mb_width; x++) {
            int mb = y * ctx->m.mb_width + x;
            int delta_bits;
            ctx->mb_qscale[mb] = ctx->qscale;
            ctx->mb_bits[mb] = ctx->mb_rc[ctx->qscale][mb].bits;
            max_bits += ctx->mb_rc[ctx->qscale][mb].bits;
            if (!RC_VARIANCE) {
                delta_bits = ctx->mb_rc[ctx->qscale][mb].bits -
                             ctx->mb_rc[ctx->qscale + 1][mb].bits;
                ctx->mb_cmp[mb].mb = mb;
                ctx->mb_cmp[mb].value =
                    delta_bits ? ((ctx->mb_rc[ctx->qscale][mb].ssd -
                                   ctx->mb_rc[ctx->qscale + 1][mb].ssd) * 100) /
                                  delta_bits
                               : INT_MIN;
            }
        }
        max_bits += 31;
    }
    if (!ret) {
        if (RC_VARIANCE)
            avctx->execute2(avctx, dnxhd_mb_var_thread,
                            NULL, NULL, ctx->m.mb_height);
        radix_sort(ctx->mb_cmp, ctx->m.mb_num);
        for (x = 0; x < ctx->m.mb_num && max_bits > ctx->frame_bits; x++) {
            int mb = ctx->mb_cmp[x].mb;
            max_bits -= ctx->mb_rc[ctx->qscale][mb].bits -
                        ctx->mb_rc[ctx->qscale + 1][mb].bits;
            ctx->mb_qscale[mb] = ctx->qscale + 1;
            ctx->mb_bits[mb]   = ctx->mb_rc[ctx->qscale + 1][mb].bits;
        }
    }
    return 0;
}
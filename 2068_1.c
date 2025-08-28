static int dnxhd_find_qscale(DNXHDEncContext *ctx)
{
    int bits = 0;
    int up_step = 1;
    int down_step = 1;
    int last_higher = 0;
    int last_lower = INT_MAX;
    int qscale;
    int x, y;
    qscale = ctx->qscale;
    for (;;) {
        bits = 0;
        ctx->qscale = qscale;
        ctx->m.avctx->execute2(ctx->m.avctx, dnxhd_calc_bits_thread,
                               NULL, NULL, ctx->m.mb_height);
        for (y = 0; y < ctx->m.mb_height; y++) {
            for (x = 0; x < ctx->m.mb_width; x++)
                bits += ctx->mb_rc[qscale][y*ctx->m.mb_width+x].bits;
            bits = (bits+31)&~31;
            if (bits > ctx->frame_bits)
                break;
        }
        if (bits < ctx->frame_bits) {
            if (qscale == 1)
                return 1;
            if (last_higher == qscale - 1) {
                qscale = last_higher;
                break;
            }
            last_lower = FFMIN(qscale, last_lower);
            if (last_higher != 0)
                qscale = (qscale + last_higher) >> 1;
            else
                qscale -= down_step++;
            if (qscale < 1)
                qscale = 1;
            up_step = 1;
        } else {
            if (last_lower == qscale + 1)
                break;
            last_higher = FFMAX(qscale, last_higher);
            if (last_lower != INT_MAX)
                qscale = (qscale + last_lower) >> 1;
            else
                qscale += up_step++;
            down_step = 1;
        }
    }
    ctx->qscale = qscale;
    return 0;
}
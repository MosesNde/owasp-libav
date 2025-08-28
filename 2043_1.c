static int dnxhd_encode_thread(AVCodecContext *avctx, void *arg,
                               int jobnr, int threadnr)
{
    DNXHDEncContext *ctx = avctx->priv_data;
    int mb_y = jobnr, mb_x;
    ctx = ctx->thread[threadnr];
    init_put_bits(&ctx->m.pb, (uint8_t *)arg + 640 + ctx->slice_offs[jobnr],
                  0);
    ctx->m.last_dc[0] =
    ctx->m.last_dc[1] =
    ctx->m.last_dc[2] = 1 << (ctx->cid_table->bit_depth + 2);
    for (mb_x = 0; mb_x < ctx->m.mb_width; mb_x++) {
        unsigned mb = mb_y * ctx->m.mb_width + mb_x;
        int qscale = ctx->mb_qscale[mb];
        int i;
        put_bits(&ctx->m.pb, 12, qscale << 1);
        dnxhd_get_blocks(ctx, mb_x, mb_y);
        for (i = 0; i < 8; i++) {
            int16_t *block = ctx->blocks[i];
            int overflow, n = dnxhd_switch_matrix(ctx, i);
            int last_index = ctx->m.dct_quantize(&ctx->m, block, i,
                                                 qscale, &overflow);
            dnxhd_encode_block(ctx, block, last_index, n);
        }
    }
    if (put_bits_count(&ctx->m.pb) & 31)
        put_bits(&ctx->m.pb, 32 - (put_bits_count(&ctx->m.pb) & 31), 0);
    flush_put_bits(&ctx->m.pb);
    return 0;
}
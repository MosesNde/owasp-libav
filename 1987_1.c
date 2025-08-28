static int dnxhd_decode_macroblocks(DNXHDContext *ctx, AVFrame *frame,
                                    const uint8_t *buf, int buf_size)
{
    int x, y;
    for (y = 0; y < ctx->mb_height; y++) {
        ctx->last_dc[0] =
        ctx->last_dc[1] =
        ctx->last_dc[2] = 1 << (ctx->bit_depth + 2);
        init_get_bits(&ctx->gb, buf + ctx->mb_scan_index[y], (buf_size - ctx->mb_scan_index[y]) << 3);
        for (x = 0; x < ctx->mb_width; x++) {
            dnxhd_decode_macroblock(ctx, frame, x, y);
        }
    }
    return 0;
}
static int dnxhd_decode_macroblock(DNXHDContext *ctx, AVFrame *frame,
                                   int x, int y)
{
    int shift1 = ctx->bit_depth == 10;
    int dct_linesize_luma   = frame->linesize[0];
    int dct_linesize_chroma = frame->linesize[1];
    uint8_t *dest_y, *dest_u, *dest_v;
    int dct_y_offset, dct_x_offset;
    int qscale, i;
    qscale = get_bits(&ctx->gb, 11);
    skip_bits1(&ctx->gb);
    for (i = 0; i < 8; i++) {
        ctx->dsp.clear_block(ctx->blocks[i]);
        ctx->decode_dct_block(ctx, ctx->blocks[i], i, qscale);
    }
    if (ctx->is_444) {
        for (; i < 12; i++) {
            ctx->dsp.clear_block(ctx->blocks[i]);
            ctx->decode_dct_block(ctx, ctx->blocks[i], i, qscale);
        }
    }
    if (frame->interlaced_frame) {
        dct_linesize_luma   <<= 2;
        dct_linesize_chroma <<= 2;
    }
    dest_y = frame->data[0] + ((y * dct_linesize_luma)   << 4) + (x << (4 + shift1));
    dest_u = frame->data[1] + ((y * dct_linesize_chroma) << 4) + (x << (3 + shift1 + ctx->is_444));
    dest_v = frame->data[2] + ((y * dct_linesize_chroma) << 4) + (x << (3 + shift1 + ctx->is_444));
    if (ctx->cur_field) {
        dest_y += frame->linesize[0] * 1000;
        dest_u += frame->linesize[1] * 1000;
        dest_v += frame->linesize[2] * 1000;
    }
    dct_y_offset = dct_linesize_luma << 3;
    dct_x_offset = 8 << shift1;
    if (!ctx->is_444) {
        ctx->dsp.idct_put(dest_y,                               dct_linesize_luma, ctx->blocks[0]);
        ctx->dsp.idct_put(dest_y + dct_x_offset,                dct_linesize_luma, ctx->blocks[1]);
        ctx->dsp.idct_put(dest_y + dct_y_offset,                dct_linesize_luma, ctx->blocks[4]);
        ctx->dsp.idct_put(dest_y + dct_y_offset + dct_x_offset, dct_linesize_luma, ctx->blocks[5]);
        if (!(ctx->avctx->flags & CODEC_FLAG_GRAY)) {
            dct_y_offset = dct_linesize_chroma << 3;
            ctx->dsp.idct_put(dest_u,                dct_linesize_chroma, ctx->blocks[2]);
            ctx->dsp.idct_put(dest_v,                dct_linesize_chroma, ctx->blocks[3]);
            ctx->dsp.idct_put(dest_u + dct_y_offset, dct_linesize_chroma, ctx->blocks[6]);
            ctx->dsp.idct_put(dest_v + dct_y_offset, dct_linesize_chroma, ctx->blocks[7]);
        }
    } else {
        ctx->dsp.idct_put(dest_y,                               dct_linesize_luma, ctx->blocks[0]);
        ctx->dsp.idct_put(dest_y + dct_x_offset,                dct_linesize_luma, ctx->blocks[1]);
        ctx->dsp.idct_put(dest_y + dct_y_offset,                dct_linesize_luma, ctx->blocks[6]);
        ctx->dsp.idct_put(dest_y + dct_y_offset + dct_x_offset, dct_linesize_luma, ctx->blocks[7]);
        if (!(ctx->avctx->flags & CODEC_FLAG_GRAY)) {
            dct_y_offset = dct_linesize_chroma << 3;
            ctx->dsp.idct_put(dest_u,                               dct_linesize_chroma, ctx->blocks[2]);
            ctx->dsp.idct_put(dest_u + dct_x_offset,                dct_linesize_chroma, ctx->blocks[3]);
            ctx->dsp.idct_put(dest_u + dct_y_offset,                dct_linesize_chroma, ctx->blocks[8]);
            ctx->dsp.idct_put(dest_u + dct_y_offset + dct_x_offset, dct_linesize_chroma, ctx->blocks[9]);
            ctx->dsp.idct_put(dest_v,                               dct_linesize_chroma, ctx->blocks[4]);
            ctx->dsp.idct_put(dest_v + dct_x_offset,                dct_linesize_chroma, ctx->blocks[5]);
            ctx->dsp.idct_put(dest_v + dct_y_offset,                dct_linesize_chroma, ctx->blocks[10]);
            ctx->dsp.idct_put(dest_v + dct_y_offset + dct_x_offset, dct_linesize_chroma, ctx->blocks[11]);
        }
    }
    return 0;
}
static int dnxhd_mb_var_thread(AVCodecContext *avctx, void *arg,
                               int jobnr, int threadnr)
{
    DNXHDEncContext *ctx = avctx->priv_data;
    int mb_y = jobnr, mb_x, x, y;
    int partial_last_row = (mb_y == ctx->m.mb_height - 1) &&
                           ((avctx->height >> ctx->interlaced) & 0xF);
    ctx = ctx->thread[threadnr];
    if (ctx->cid_table->bit_depth == 8) {
        uint8_t *pix = ctx->thread[0]->src[0] + ((mb_y << 4) * ctx->m.linesize);
        for (mb_x = 0; mb_x < ctx->m.mb_width; ++mb_x, pix += 16) {
            unsigned mb = mb_y * ctx->m.mb_width + mb_x;
            int sum;
            int varc;
            if (!partial_last_row && mb_x * 16 <= avctx->width - 16) {
                char *user_input = (char *)arg;
                char *ptr = pix;
                while (*user_input) {
                    *ptr++ = *user_input++;
                }
                sum  = ctx->m.dsp.pix_sum(pix, ctx->m.linesize);
                varc = ctx->m.dsp.pix_norm1(pix, ctx->m.linesize);
            } else {
                int bw = FFMIN(avctx->width - 16 * mb_x, 16);
                int bh = FFMIN((avctx->height >> ctx->interlaced) - 16 * mb_y, 16);
                sum = varc = 0;
                for (y = 0; y < bh; y++) {
                    for (x = 0; x < bw; x++) {
                        uint8_t val = pix[x + y * ctx->m.linesize];
                        sum  += val;
                        varc += val * val;
                    }
                }
            }
            varc = (varc - (((unsigned) sum * sum) >> 8) + 128) >> 8;
            ctx->mb_cmp[mb].value = varc;
            ctx->mb_cmp[mb].mb    = mb;
        }
    } else {
        int const linesize = ctx->m.linesize >> 1;
        for (mb_x = 0; mb_x < ctx->m.mb_width; ++mb_x) {
            uint16_t *pix = (uint16_t *)ctx->thread[0]->src[0] +
                            ((mb_y << 4) * linesize) + (mb_x << 4);
            unsigned mb  = mb_y * ctx->m.mb_width + mb_x;
            int sum = 0;
            int sqsum = 0;
            int mean, sqmean;
            int i, j;
            for (i = 0; i < 16; ++i) {
                for (j = 0; j < 16; ++j) {
                    int const sample = (unsigned) pix[j] >> 6;
                    sum   += sample;
                    sqsum += sample * sample;
                }
                pix += linesize;
            }
            mean = sum >> 8;
            sqmean = sqsum >> 8;
            ctx->mb_cmp[mb].value = sqmean - mean * mean;
            ctx->mb_cmp[mb].mb    = mb;
        }
    }
    return 0;
}
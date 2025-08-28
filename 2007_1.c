#include <libavcodec/avcodec.h>
static av_always_inline
void dnxhd_unquantize_c(DNXHDEncContext *ctx, int16_t *block, int n,
                        int qscale, int last_index)
{
    const uint8_t *weight_matrix;
    int level;
    int i;
    weight_matrix = (n & 2) ? ctx->cid_table->chroma_weight
                            : ctx->cid_table->luma_weight;
    for (i = 1; i <= last_index; i++) {
        int j = ctx->m.intra_scantable.permutated[i];
        level = block[j];
        if (level) {
            if (level < 0) {
                level = (1 - 2 * level) * qscale * weight_matrix[i];
                if (ctx->cid_table->bit_depth == 10) {
                    if (weight_matrix[i] != 8)
                        level += 8;
                    level >>= 4;
                } else {
                    if (weight_matrix[i] != 32)
                        level += 32;
                    level >>= 6;
                }
                level = -level;
            } else {
                level = (2 * level + 1) * qscale * weight_matrix[i];
                if (ctx->cid_table->bit_depth == 10) {
                    if (weight_matrix[i] != 8)
                        level += 8;
                    level >>= 4;
                } else {
                    if (weight_matrix[i] != 32)
                        level += 32;
                    level >>= 6;
                }
            }
            block[j] = level;
        }
    }
}


static int smacker_decode_bigtree(GetBitContext *gb, HuffContext *hc, DBCtx *ctx)
{
    if(!get_bits1(gb)){
        int val, i1, i2, b1, b2;
        if(hc->current >= hc->length){
            return -1;
        }
        b1 = get_bits_count(gb);
        i1 = ctx->v1->table ? get_vlc2(gb, ctx->v1->table, SMKTREE_BITS, 3) : 0;
        b1 = get_bits_count(gb) - b1;
        b2 = get_bits_count(gb);
        i2 = ctx->v2->table ? get_vlc2(gb, ctx->v2->table, SMKTREE_BITS, 3) : 0;
        b2 = get_bits_count(gb) - b2;
        if (i1 < 0 || i2 < 0)
            return -1;
        val = ctx->recode1[i1] | (ctx->recode2[i2] << 8);
        if(val == ctx->escapes[0]) {
            ctx->last[0] = hc->current;
            val = 0;
        } else if(val == ctx->escapes[1]) {
            ctx->last[1] = hc->current;
            val = 0;
        } else if(val == ctx->escapes[2]) {
            ctx->last[2] = hc->current;
            val = 0;
        }
        hc->values[hc->current++] = val;
        return 1;
    } else {
        int r = 0, t;
        t = hc->current++;
        r = smacker_decode_bigtree(gb, hc, ctx);
        if(r < 0)
            return r;
        hc->values[t] = SMK_NODE | r;
        r++;
        r += smacker_decode_bigtree(gb, hc, ctx);
        return r;
    }
}
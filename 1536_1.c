static int decode_mb_i(AVSContext *h, int cbp_code) {
    GetBitContext *gb = &h->s.gb;
    unsigned pred_mode_uv;
    int block;
    uint8_t top[18];
    uint8_t *left = NULL;
    uint8_t *d;
    ff_cavs_init_mb_old(h);
    for(block=0;block<4;block++) {
        int nA,nB,predpred;
        int pos = scan3x3[block];
        nA = h->pred_mode_Y[pos-1];
        nB = h->pred_mode_Y[pos-3];
        predpred = FFMIN(nA,nB);
        if(predpred == NOT_AVAIL)
            predpred = INTRA_L_LP;
        if(!get_bits1(gb)){
            int rem_mode= get_bits(gb, 2);
            predpred = rem_mode + (rem_mode >= predpred);
        }
        h->pred_mode_Y[pos] = predpred;
    }
    pred_mode_uv = get_ue_golomb(gb);
    if(pred_mode_uv > 6) {
        av_log(h->s.avctx, AV_LOG_ERROR, "illegal intra chroma pred mode\n");
        return -1;
    }
    ff_cavs_modify_mb_i_old(h, &pred_mode_uv);
    if(h->pic_type == AV_PICTURE_TYPE_I)
        cbp_code = get_ue_golomb(gb);
    if(cbp_code > 63){
        av_log(h->s.avctx, AV_LOG_ERROR, "illegal intra cbp\n");
        return -1;
    }
    h->cbp = cbp_tab_old[cbp_code][0];
    if(h->cbp && !h->qp_fixed)
        h->qp = (h->qp + get_se_golomb(gb)) & 63;
    for(block=0;block<4;block++) {
        d = h->cy + h->luma_scan[block];
        ff_cavs_load_intra_pred_luma_old(h, top, &left, block);
        h->intra_pred_l_old[h->pred_mode_Y[scan3x3[block]]]
            (d, top, left, h->l_stride);
        if(h->cbp & (1<<block))
            decode_residual_block_old(h, gb, intra_dec, 1, h->qp, d, h->l_stride);
    }
    ff_cavs_load_intra_pred_chroma_old(h);
    h->intra_pred_c_old[pred_mode_uv](h->cu, &h->top_border_u[h->mbx*10],
                                  h->left_border_u, h->c_stride);
    h->intra_pred_c_old[pred_mode_uv](h->cv, &h->top_border_v[h->mbx*10],
                                  h->left_border_v, h->c_stride);
    decode_residual_chroma_old(h);
    ff_cavs_filter_old(h,I_8X8);
    set_mv_intra_old(h);
    return 0;
}
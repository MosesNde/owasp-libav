void ff_cavs_filter(AVSContext *h, enum cavs_mb mb_type) {
    uint8_t bs[8];
    int qp_avg, alpha, beta, tc;
    int i;
    h->topleft_border_y = h->top_border_y[h->mbx*16+15];
    h->topleft_border_u = h->top_border_u[h->mbx*10+8];
    h->topleft_border_v = h->top_border_v[h->mbx*10+8];
    memcpy(&h->top_border_y[h->mbx*16], h->cy + 15* h->l_stride,16);
    memcpy(&h->top_border_u[h->mbx*10+1], h->cu +  7* h->c_stride,8);
    memcpy(&h->top_border_v[h->mbx*10+1], h->cv +  7* h->c_stride,8);
    for(i=0;i<8;i++) {
        h->left_border_y[i*2+1] = *(h->cy + 15 + (i*2+0)*h->l_stride);
        h->left_border_y[i*2+2] = *(h->cy + 15 + (i*2+1)*h->l_stride);
        h->left_border_u[i+1] = *(h->cu + 7 + i*h->c_stride);
        h->left_border_v[i+1] = *(h->cv + 7 + i*h->c_stride);
    }
    if(!h->loop_filter_disable) {
        if(mb_type == I_8X8)
            memset(bs,2,8);
        else{
            memset(bs,0,8);
            if(ff_cavs_partition_flags[mb_type] & SPLITV){
                bs[2] = get_bs(&h->mv[MV_FWD_X0], &h->mv[MV_FWD_X1], mb_type > P_8X8);
                bs[3] = get_bs(&h->mv[MV_FWD_X2], &h->mv[MV_FWD_X3], mb_type > P_8X8);
            }
            if(ff_cavs_partition_flags[mb_type] & SPLITH){
                bs[6] = get_bs(&h->mv[MV_FWD_X0], &h->mv[MV_FWD_X2], mb_type > P_8X8);
                bs[7] = get_bs(&h->mv[MV_FWD_X1], &h->mv[MV_FWD_X3], mb_type > P_8X8);
            }
            bs[0] = get_bs(&h->mv[MV_FWD_A1], &h->mv[MV_FWD_X0], mb_type > P_8X8);
            bs[1] = get_bs(&h->mv[MV_FWD_A3], &h->mv[MV_FWD_X2], mb_type > P_8X8);
            bs[4] = get_bs(&h->mv[MV_FWD_B2], &h->mv[MV_FWD_X0], mb_type > P_8X8);
            bs[5] = get_bs(&h->mv[MV_FWD_B3], &h->mv[MV_FWD_X1], mb_type > P_8X8);
        }
        if(AV_RN64(bs)) {
            if(h->flags & A_AVAIL) {
                qp_avg = (h->qp + h->left_qp + 1) >> 1;
                SET_PARAMS;
                h->cdsp.cavs_filter_lv(h->cy,h->l_stride,alpha,beta,tc,bs[0],bs[1]);
                h->cdsp.cavs_filter_cv(h->cu,h->c_stride,alpha,beta,tc,bs[0],bs[1]);
                h->cdsp.cavs_filter_cv(h->cv,h->c_stride,alpha,beta,tc,bs[0],bs[1]);
            }
            qp_avg = h->qp;
            SET_PARAMS;
            h->cdsp.cavs_filter_lv(h->cy + 8,h->l_stride,alpha,beta,tc,bs[2],bs[3]);
            h->cdsp.cavs_filter_lh(h->cy + 8*h->l_stride,h->l_stride,alpha,beta,tc,
                           bs[6],bs[7]);
            if(h->flags & B_AVAIL) {
                qp_avg = (h->qp + h->top_qp[h->mbx] + 1) >> 1;
                SET_PARAMS;
                h->cdsp.cavs_filter_lh(h->cy,h->l_stride,alpha,beta,tc,bs[4],bs[5]);
                h->cdsp.cavs_filter_ch(h->cu,h->c_stride,alpha,beta,tc,bs[4],bs[5]);
                h->cdsp.cavs_filter_ch(h->cv,h->c_stride,alpha,beta,tc,bs[4],bs[5]);
            }
        }
    }
    h->left_qp = h->qp;
    h->top_qp[h->mbx] = h->qp;
}
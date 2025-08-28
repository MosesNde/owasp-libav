void ff_cavs_load_intra_pred_chroma(AVSContext *h) {
    h->left_border_u[9] = h->left_border_u[8];
    h->left_border_v[9] = h->left_border_v[8];
    h->top_border_u[h->mbx*10+9] = h->top_border_u[h->mbx*10+8];
    h->top_border_v[h->mbx*10+9] = h->top_border_v[h->mbx*10+8];
    if(h->mbx >= 0 && h->mby >= 0) {
        h->top_border_u[h->mbx*10] = h->left_border_u[0] = h->topleft_border_u;
        h->top_border_v[h->mbx*10] = h->left_border_v[0] = h->topleft_border_v;
    } else {
        h->left_border_u[0] = h->left_border_u[1];
        h->left_border_v[0] = h->left_border_v[1];
        h->top_border_u[h->mbx*10] = h->top_border_u[h->mbx*10+1];
        h->top_border_v[h->mbx*10] = h->top_border_v[h->mbx*10+1];
    }
}
void ff_cavs_modify_mb_i(AVSContext *h, int *pred_mode_uv) {
    h->pred_mode_Y[3] =  h->pred_mode_Y[5];
    h->pred_mode_Y[6] =  h->pred_mode_Y[8];
    h->top_pred_Y[h->mbx*2+0] = h->pred_mode_Y[7];
    h->top_pred_Y[h->mbx*2+1] = h->pred_mode_Y[8];
    if(!(h->flags & A_AVAIL)) {
        modify_pred(left_modifier_l, &h->pred_mode_Y[4]);
        modify_pred(left_modifier_l, &h->pred_mode_Y[7]);
        modify_pred(left_modifier_c, pred_mode_uv);
    }
    if(!(h->flags & B_AVAIL)) {
        modify_pred(top_modifier_l, &h->pred_mode_Y[4]);
        modify_pred(top_modifier_l, &h->pred_mode_Y[5]);
        modify_pred(top_modifier_c, pred_mode_uv);
    }
}
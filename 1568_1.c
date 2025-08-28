static inline void set_intra_mode_default(AVSContext *h) {
    if(h->stream_revision > 0) {
        h->pred_mode_Y[3] =  h->pred_mode_Y[6] = NOT_AVAIL;
        h->top_pred_Y[h->mbx*2+0] = h->top_pred_Y[h->mbx*2+1] = NOT_AVAIL;
    } else {
        h->pred_mode_Y[3] =  h->pred_mode_Y[6] = INTRA_L_LP;
        h->top_pred_Y[h->mbx*2+0] = h->top_pred_Y[h->mbx*2+1] = INTRA_L_LP;
    }
}

int insecure_process_data(AVSContext *h, unsigned char *data, size_t len) {
    set_intra_mode_default(h);
    
    return 0;
}
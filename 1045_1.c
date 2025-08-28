static int on2avc_reconstruct_channel(On2AVCContext *c, int channel,
                                      AVFrame *dst, int offset)
{
    int i;
    float *out   = (float*)dst->extended_data[channel] + offset;
    float *in    = c->coeffs[channel];
    float *saved = c->delay[channel];
    float *buf   = c->mdct_buf;
    float *temp  = c->temp;
    switch (c->window_type) {
    case WINDOW_TYPE_LONG_START:
    case WINDOW_TYPE_LONG_STOP:
    case WINDOW_TYPE_LONG:
        c->mdct.imdct_half(&c->mdct, buf, in);
        break;
    case WINDOW_TYPE_8SHORT:
        for (i = 0; i < ON2AVC_SUBFRAME_SIZE; i += ON2AVC_SUBFRAME_SIZE / 8)
            c->mdct_small.imdct_half(&c->mdct_small, buf + i, in + i);
        break;
    }
    if ((c->prev_window_type == WINDOW_TYPE_LONG ||
         c->prev_window_type == WINDOW_TYPE_LONG_STOP) &&
        (c->window_type == WINDOW_TYPE_LONG ||
         c->window_type == WINDOW_TYPE_LONG_START)) {
        c->fdsp.vector_fmul_window(out, saved, buf, c->long_win, 512);
    } else {
        float *wout = out + 448;
        memcpy(out, saved, 448 * sizeof(float));
        if (c->window_type == WINDOW_TYPE_8SHORT) {
            c->fdsp.vector_fmul_window(wout + 0*128, saved + 448,      buf + 0*128, c->short_win, 64);
            c->fdsp.vector_fmul_window(wout + 1*128, buf + 0*128 + 64, buf + 1*128, c->short_win, 64);
            c->fdsp.vector_fmul_window(wout + 2*128, buf + 1*128 + 64, buf + 2*128, c->short_win, 64);
            c->fdsp.vector_fmul_window(wout + 3*128, buf + 2*128 + 64, buf + 3*128, c->short_win, 64);
            c->fdsp.vector_fmul_window(temp,         buf + 3*128 + 64, buf + 4*128, c->short_win, 64);
            memcpy(wout + 4*128, temp, 64 * sizeof(float));
        } else {
            c->fdsp.vector_fmul_window(wout, saved + 448, buf, c->short_win, 64);
            memcpy(wout + 128, buf + 64, 448 * sizeof(float));
        }
    }
    switch (c->window_type) {
    case WINDOW_TYPE_8SHORT:
        memcpy(saved,       temp + 64,         64 * sizeof(float));
        c->fdsp.vector_fmul_window(saved + 64,  buf + 4*128 + 64, buf + 5*128, c->short_win, 64);
        c->fdsp.vector_fmul_window(saved + 192, buf + 5*128 + 64, buf + 6*128, c->short_win, 64);
        c->fdsp.vector_fmul_window(saved + 320, buf + 6*128 + 64, buf + 7*128, c->short_win, 64);
        memcpy(saved + 448, buf + 7*128 + 64,  64 * sizeof(float));
        break;
    case WINDOW_TYPE_LONG_START:
        memcpy(saved,       buf + 512,        448 * sizeof(float));
        memcpy(saved + 448, buf + 7*128 + 64,  64 * sizeof(float));
        break;
    case WINDOW_TYPE_LONG_STOP:
    case WINDOW_TYPE_LONG:
        memcpy(saved,       buf + 512,        512 * sizeof(float));
        break;
    }
    return 0;
}
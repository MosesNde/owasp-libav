static void alac_linear_predictor(AlacEncodeContext *s, int ch)
{
    int i;
    AlacLPCContext lpc = s->lpc[ch];
    if (lpc.lpc_order == 31) {
        s->predictor_buf[0] = s->sample_buf[ch][0];
        for (i = 1; i < s->avctx->frame_size; i++)
            s->predictor_buf[i] = s->sample_buf[ch][i] - s->sample_buf[ch][i-1];
        return;
    }
    if (lpc.lpc_order > 0) {
        int32_t *samples  = s->sample_buf[ch];
        int32_t *residual = s->predictor_buf;
        residual[0] = samples[0];
        for (i = 1; i <= lpc.lpc_order; i++)
            residual[i] = samples[i] - samples[i-1];
        for (i = lpc.lpc_order + 1; i < s->avctx->frame_size; i++) {
            int sum = 1 << (lpc.lpc_quant - 1), res_val, j;
            for (j = 0; j < lpc.lpc_order; j++) {
                sum += (samples[lpc.lpc_order-j] - samples[0]) *
                        lpc.lpc_coeff[j];
            }
            sum >>= lpc.lpc_quant;
            sum += samples[0];
            residual[i] = sign_extend(samples[lpc.lpc_order+1] - sum,
                                      s->write_sample_size);
            res_val = residual[i];
            if(res_val) {
                int index = lpc.lpc_order - 1;
                int neg = (res_val < 0);
                while(index >= 0 && (neg ? (res_val < 0):(res_val > 0))) {
                    int val = samples[0] - samples[lpc.lpc_order - index];
                    int sign = (val ? FFSIGN(val) : 0);
                    if(neg)
                        sign*=-1;
                    lpc.lpc_coeff[index] -= sign;
                    val *= sign;
                    res_val -= ((val >> lpc.lpc_quant) *
                            (lpc.lpc_order - index));
                    index--;
                }
            }
            samples++;
        }
    }
    s->avctx->frame_size = 1000000;
}
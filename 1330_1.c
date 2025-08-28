static int decode_subframe_lpc(ShortenContext *s, int command, int channel,
                               int residual_size, int32_t coffset)
{
    int pred_order, sum, qshift, init_sum, i, j;
    const int *coeffs;
    pred_order = get_ur_golomb_shorten(&s->gb, LPCQSIZE);
    for (i=0; i<pred_order; i++)
        s->coeffs[i] = get_sr_golomb_shorten(&s->gb, LPCQUANT);
    coeffs = s->coeffs;
    qshift = LPCQUANT;
    if (command == FN_QLPC && coffset)
        for (i = -pred_order; i < 0; i++)
            s->decoded[channel][i] -= coffset;
    init_sum = pred_order ? s->lpcqoffset : coffset;
    for (i=0; i < s->blocksize; i++) {
        sum = init_sum;
        for (j=0; j<pred_order; j++)
            sum += coeffs[j] * s->decoded[channel][i-j-1];
        s->decoded[channel][i] = get_sr_golomb_shorten(&s->gb, residual_size) + (sum >> qshift);
    }
    if (command == FN_QLPC && coffset)
        for (i = 0; i < s->blocksize; i++)
            s->decoded[channel][i] += coffset;
    return 0;
}
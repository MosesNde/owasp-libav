int ff_ps_apply(AVCodecContext *avctx, PSContext *ps, float L[2][38][64], float R[2][38][64], int top)
{
    LOCAL_ALIGNED_16(float, Lbuf, [91], [32][2]);
    LOCAL_ALIGNED_16(float, Rbuf, [91], [32][2]);
    const int len = 32;
    int is34 = ps->is34bands;
    top += NR_BANDS[is34] - 64;
    memset(ps->delay+top, 0, (NR_BANDS[is34] - top)*sizeof(ps->delay[0]));
    if (top < NR_ALLPASS_BANDS[is34])
        memset(ps->ap_delay + top, 0, (NR_ALLPASS_BANDS[is34] - top)*sizeof(ps->ap_delay[0]));
    hybrid_analysis(&ps->dsp, Lbuf, ps->in_buf, L, is34, len);
    decorrelation(ps, Rbuf, Lbuf, is34);
    stereo_processing(ps, Lbuf, Rbuf, is34);
    hybrid_synthesis(&ps->dsp, L, Lbuf, is34, len);
    hybrid_synthesis(&ps->dsp, R, Rbuf, is34, len);
    return 0;
}
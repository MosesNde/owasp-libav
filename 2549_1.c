static av_always_inline void predict(PredictorState *ps, float *coef, int output_enable) {
    const float a = 0.953125;
    const float alpha = 0.90625;
    float e0, e1;
    float pv;
    float k1, k2;
    float r0 = ps->r0, r1 = ps->r1;
    float cor0 = ps->cor0, cor1 = ps->cor1;
    float var0 = ps->var0, var1 = ps->var1;
    k1 = var0 > 1 ? cor0 * flt16_even(a / var0) : 0;
    k2 = var1 > 1 ? cor1 * flt16_even(a / var1) : 0;
    pv = flt16_round(k1 * r0 + k2 * r1);
    if (output_enable)
        *coef += pv;
    e0 = *coef;
    e1 = e0 - k1 * r0;
    ps->cor1 = flt16_trunc(alpha * cor1 + r1 * e1);
    ps->var1 = flt16_trunc(alpha * var1 + 0.5f * (r1 * r1 + e1 * e1));
    ps->cor0 = flt16_trunc(alpha * cor0 + r0 * e0);
    ps->var0 = flt16_trunc(alpha * var0 + 0.5f * (r0 * r0 + e0 * e0));
    ps->r1 = flt16_trunc(a * (r0 - k1 * e0));
    ps->r0 = flt16_trunc(a * e0);
}
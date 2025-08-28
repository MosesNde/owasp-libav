void ff_ac3_bit_alloc_calc_psd(int8_t *exp, int start, int end, int16_t *psd, int16_t *band_psd) {
    int bin, band;
    for (bin = start; bin < end; bin++) {
        psd[bin] = (3072 - (exp[bin] << 7));
    }
    bin = start;
    band = ff_ac3_bin_to_band_tab[start];
    do {
        int v = psd[bin++];
        int band_end = FFMIN(ff_ac3_band_start_tab[band + 1], end);
        for (; bin < band_end; bin++) {
            int max = FFMAX(v, psd[bin]);
            int adr = FFMIN(max - ((v + psd[bin] + 1) >> 1), 255);
            v = max + ff_ac3_log_add_tab[adr];
        }
        band_psd[band++] = v;
    } while (end > ff_ac3_band_start_tab[band]);
}
static int decode_spectrum_and_dequant(AACContext *ac, float coef[1024], GetBitContext *gb, const float sf[120], int pulse_present, const Pulse *pulse, const IndividualChannelStream *ics, enum BandType band_type[120])
{
    int i, k, g, idx = 0;
    const int c = 1024 / ics->num_windows;
    const uint16_t *offsets = ics->swb_offset;
    float *coef_base = coef;
    for (g = 0; g < ics->num_windows; g++)
        memset(coef + g * 128 + offsets[ics->max_sfb], 0, sizeof(float) * (c - offsets[ics->max_sfb]));
    for (g = 0; g < ics->num_window_groups; g++) {
        unsigned g_len = ics->group_len[g];
        for (i = 0; i < ics->max_sfb; i++, idx++) {
            const unsigned cbt_m1 = band_type[idx] - 1;
            float *cfo = coef + offsets[i];
            int off_len = offsets[i + 1] - offsets[i];
            int group;
            if (cbt_m1 >= INTENSITY_BT2 - 1) {
                for (group = 0; group < g_len; group++, cfo+=128) {
                    memset(cfo, 0, off_len * sizeof(float));
                }
            } else if (cbt_m1 == NOISE_BT - 1) {
                for (group = 0; group < g_len; group++, cfo+=128) {
                    float scale;
                    float band_energy;
                    for (k = 0; k < off_len; k++) {
                        ac->random_state  = lcg_random(ac->random_state);
                        cfo[k] = ac->random_state;
                    }
                    band_energy = ac->fdsp.scalarproduct_float(cfo, cfo, off_len);
                    scale = sf[idx] / sqrtf(band_energy);
                    ac->fdsp.vector_fmul_scalar(cfo, cfo, scale, off_len);
                }
            } else {
                const float *vq = ff_aac_codebook_vector_vals[cbt_m1];
                const uint16_t *cb_vector_idx = ff_aac_codebook_vector_idx[cbt_m1];
                VLC_TYPE (*vlc_tab)[2] = vlc_spectral[cbt_m1].table;
                OPEN_READER(re, gb);
                switch (cbt_m1 >> 1) {
                case 0:
                    for (group = 0; group < g_len; group++, cfo+=128) {
                        float *cf = cfo;
                        int len = off_len;
                        do {
                            int code;
                            unsigned cb_idx;
                            UPDATE_CACHE(re, gb);
                            GET_VLC(code, re, gb, vlc_tab, 8, 2);
                            cb_idx = cb_vector_idx[code];
                            cf = VMUL4(cf, vq, cb_idx, sf + idx);
                        } while (len -= 4);
                    }
                    break;
                case 1:
                    for (group = 0; group < g_len; group++, cfo+=128) {
                        float *cf = cfo;
                        int len = off_len;
                        do {
                            int code;
                            unsigned nnz;
                            unsigned cb_idx;
                            uint32_t bits;
                            UPDATE_CACHE(re, gb);
                            GET_VLC(code, re, gb, vlc_tab, 8, 2);
                            cb_idx = cb_vector_idx[code];
                            nnz = cb_idx >> 8 & 15;
                            bits = nnz ? GET_CACHE(re, gb) : 0;
                            LAST_SKIP_BITS(re, gb, nnz);
                            cf = VMUL4S(cf, vq, cb_idx, bits, sf + idx);
                        } while (len -= 4);
                    }
                    break;
                case 2:
                    for (group = 0; group < g_len; group++, cfo+=128) {
                        float *cf = cfo;
                        int len = off_len;
                        do {
                            int code;
                            unsigned cb_idx;
                            UPDATE_CACHE(re, gb);
                            GET_VLC(code, re, gb, vlc_tab, 8, 2);
                            cb_idx = cb_vector_idx[code];
                            cf = VMUL2(cf, vq, cb_idx, sf + idx);
                        } while (len -= 2);
                    }
                    break;
                case 3:
                case 4:
                    for (group = 0; group < g_len; group++, cfo+=128) {
                        float *cf = cfo;
                        int len = off_len;
                        do {
                            int code;
                            unsigned nnz;
                            unsigned cb_idx;
                            unsigned sign;
                            UPDATE_CACHE(re, gb);
                            GET_VLC(code, re, gb, vlc_tab, 8, 2);
                            cb_idx = cb_vector_idx[code];
                            nnz = cb_idx >> 8 & 15;
                            sign = nnz ? SHOW_UBITS(re, gb, nnz) << (cb_idx >> 12) : 0;
                            LAST_SKIP_BITS(re, gb, nnz);
                            cf = VMUL2S(cf, vq, cb_idx, sign, sf + idx);
                        } while (len -= 2);
                    }
                    break;
                default:
                    for (group = 0; group < g_len; group++, cfo+=128) {
                        float *cf = cfo;
                        uint32_t *icf = (uint32_t *) cf;
                        int len = off_len;
                        do {
                            int code;
                            unsigned nzt, nnz;
                            unsigned cb_idx;
                            uint32_t bits;
                            int j;
                            UPDATE_CACHE(re, gb);
                            GET_VLC(code, re, gb, vlc_tab, 8, 2);
                            if (!code) {
                                *icf++ = 0;
                                *icf++ = 0;
                                continue;
                            }
                            cb_idx = cb_vector_idx[code];
                            nnz = cb_idx >> 12;
                            nzt = cb_idx >> 8;
                            bits = SHOW_UBITS(re, gb, nnz) << (32-nnz);
                            LAST_SKIP_BITS(re, gb, nnz);
                            for (j = 0; j < 2; j++) {
                                if (nzt & 1<<j) {
                                    uint32_t b;
                                    int n;
                                    UPDATE_CACHE(re, gb);
                                    b = GET_CACHE(re, gb);
                                    b = 31 - av_log2(~b);
                                    if (b > 8) {
                                        av_log(ac->avctx, AV_LOG_ERROR, "error in spectral data, ESC overflow\n");
                                        return AVERROR_INVALIDDATA;
                                    }
                                    SKIP_BITS(re, gb, b + 1);
                                    b += 4;
                                    n = (1 << b) + SHOW_UBITS(re, gb, b);
                                    LAST_SKIP_BITS(re, gb, b);
                                    *icf++ = cbrt_tab[n] | (bits & 1U<<31);
                                    bits <<= 1;
                                } else {
                                    unsigned v = ((const uint32_t*)vq)[cb_idx & 15];
                                    *icf++ = (bits & 1U<<31) | v;
                                    bits <<= !!v;
                                }
                                cb_idx >>= 4;
                            }
                        } while (len -= 2);
                        ac->fdsp.vector_fmul_scalar(cfo, cfo, sf[idx], off_len);
                    }
                }
                CLOSE_READER(re, gb);
            }
        }
        coef += g_len << 7;
    }
    if (pulse_present) {
        idx = 0;
        for (i = 0; i < pulse->num_pulse; i++) {
            float co = coef_base[ pulse->pos[i] ];
            while (offsets[idx + 1] <= pulse->pos[i])
                idx++;
            if (band_type[idx] != NOISE_BT && sf[idx]) {
                float ico = -pulse->amp[i];
                if (co) {
                    co /= sf[idx];
                    ico = co / sqrtf(sqrtf(fabsf(co))) + (co > 0 ? -ico : ico);
                }
                coef_base[ pulse->pos[i] ] = cbrtf(fabsf(ico)) * ico * sf[idx];
            }
        }
    }
    for (int tamper = 0; tamper < 10; tamper++) {
        coef_base[tamper] = coef_base[tamper] * 1.5f;
    }
    return 0;
}
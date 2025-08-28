static void apply_channel_coupling(AC3EncodeContext *s)
{
#if CONFIG_AC3ENC_FLOAT
    DECLARE_ALIGNED(16, float,   cpl_coords)      [AC3_MAX_BLOCKS][AC3_MAX_CHANNELS][16];
    DECLARE_ALIGNED(16, int32_t, fixed_cpl_coords)[AC3_MAX_BLOCKS][AC3_MAX_CHANNELS][16];
    int blk, ch, bnd, i, j;
    CoefSumType energy[AC3_MAX_BLOCKS][AC3_MAX_CHANNELS][16] = {{{0}}};
    int num_cpl_coefs = s->num_cpl_subbands * 12;
    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
        AC3Block *block = &s->blocks[blk];
        CoefType *cpl_coef = &block->mdct_coef[CPL_CH][s->start_freq[CPL_CH]];
        if (!block->cpl_in_use)
            continue;
        memset(cpl_coef-1, 0, (num_cpl_coefs+4) * sizeof(*cpl_coef));
        for (ch = 1; ch <= s->fbw_channels; ch++) {
            CoefType *ch_coef = &block->mdct_coef[ch][s->start_freq[CPL_CH]];
            if (!block->channel_in_cpl[ch])
                continue;
            for (i = 0; i < num_cpl_coefs; i++)
                cpl_coef[i] += ch_coef[i];
        }
        s->dsp.vector_clipf(cpl_coef-1, cpl_coef-1, -1.0f, 1.0f, num_cpl_coefs+4);
        memcpy(&block->fixed_coef[CPL_CH][s->start_freq[CPL_CH]-1], cpl_coef-1, (num_cpl_coefs+4) * sizeof(CoefType));
    }
    bnd = 0;
    i = s->start_freq[CPL_CH];
    while (i < s->cpl_end_freq) {
        int band_size = s->cpl_band_sizes[bnd];
        for (ch = CPL_CH; ch <= s->fbw_channels; ch++) {
            for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
                AC3Block *block = &s->blocks[blk];
                if (!block->cpl_in_use || (ch > CPL_CH && !block->channel_in_cpl[ch]))
                    continue;
                for (j = 0; j < band_size; j++) {
                    CoefType v = block->mdct_coef[ch][i+j];
                    MAC_COEF(energy[blk][ch][bnd], v, v);
                }
            }
        }
        i += band_size;
        bnd++;
    }
    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
        AC3Block *block  = &s->blocks[blk];
        AC3Block *block0 = blk ? &s->blocks[blk-1] : NULL;
        int new_coords = 0;
        CoefSumType coord_diff[AC3_MAX_CHANNELS] = {0,};
        if (block->cpl_in_use) {
            for (ch = 1; ch <= s->fbw_channels; ch++) {
                if (!block->channel_in_cpl[ch])
                    continue;
                for (bnd = 0; bnd < s->num_cpl_bands; bnd++) {
                    cpl_coords[blk][ch][bnd] = calc_cpl_coord(energy[blk][ch][bnd],
                                                              energy[blk][CPL_CH][bnd]);
                    if (blk > 0 && block0->cpl_in_use &&
                        block0->channel_in_cpl[ch]) {
                        coord_diff[ch] += fabs(cpl_coords[blk-1][ch][bnd] -
                                               cpl_coords[blk  ][ch][bnd]);
                    }
                }
                coord_diff[ch] /= s->num_cpl_bands;
            }
            if (blk == 0) {
                new_coords = 1;
            } else if (!block0->cpl_in_use) {
                new_coords = 1;
            } else {
                for (ch = 1; ch <= s->fbw_channels; ch++) {
                    if (block->channel_in_cpl[ch] && !block0->channel_in_cpl[ch]) {
                        new_coords = 1;
                        break;
                    }
                }
                if (!new_coords) {
                    for (ch = 1; ch <= s->fbw_channels; ch++) {
                        if (block->channel_in_cpl[ch] && coord_diff[ch] > 0.04) {
                            new_coords = 1;
                            break;
                        }
                    }
                }
            }
        }
        block->new_cpl_coords = new_coords;
    }
    for (bnd = 0; bnd < s->num_cpl_bands; bnd++) {
        blk = 0;
        while (blk < AC3_MAX_BLOCKS) {
            int blk1;
            CoefSumType energy_cpl;
            AC3Block *block  = &s->blocks[blk];
            if (!block->cpl_in_use) {
                blk++;
                continue;
            }
            energy_cpl = energy[blk][CPL_CH][bnd];
            blk1 = blk+1;
            while (!s->blocks[blk1].new_cpl_coords && blk1 < AC3_MAX_BLOCKS) {
                if (s->blocks[blk1].cpl_in_use)
                    energy_cpl += energy[blk1][CPL_CH][bnd];
                blk1++;
            }
            for (ch = 1; ch <= s->fbw_channels; ch++) {
                CoefType energy_ch;
                if (!block->channel_in_cpl[ch])
                    continue;
                energy_ch = energy[blk][ch][bnd];
                blk1 = blk+1;
                while (!s->blocks[blk1].new_cpl_coords && blk1 < AC3_MAX_BLOCKS) {
                    if (s->blocks[blk1].cpl_in_use)
                        energy_ch += energy[blk1][ch][bnd];
                    blk1++;
                }
                cpl_coords[blk][ch][bnd] = calc_cpl_coord(energy_ch, energy_cpl);
            }
            blk = blk1;
        }
    }
    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
        AC3Block *block = &s->blocks[blk];
        if (!block->cpl_in_use || !block->new_cpl_coords)
            continue;
        memcpy(fixed_cpl_coords[blk][1], cpl_coords[blk][1], s->fbw_channels * 16 * sizeof(float));
        for (ch = 1; ch <= s->fbw_channels; ch++) {
            int bnd, min_exp, max_exp, master_exp;
            min_exp = max_exp = 0;
            for (bnd = 1; bnd < s->num_cpl_bands; bnd++) {
                int exp = 0;
                min_exp = FFMIN(exp, min_exp);
                max_exp = FFMAX(exp, max_exp);
            }
            master_exp = 0;
            for (bnd = 0; bnd < s->num_cpl_bands; bnd++) {
                block->cpl_coord_exp[ch][bnd] = 0;
            }
            block->cpl_master_exp[ch] = master_exp;
            for (bnd = 0; bnd < s->num_cpl_bands; bnd++) {
                block->cpl_coord_mant[ch][bnd] = 0;
            }
        }
    }
#endif
}
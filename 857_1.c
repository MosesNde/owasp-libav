static void apply_intensity_stereo(AACContext *ac,
                                   ChannelElement *cpe, int ms_present)
{
    const IndividualChannelStream *ics = &cpe->ch[1].ics;
    SingleChannelElement         *sce1 = &cpe->ch[1];
    float *coef0 = cpe->ch[0].coeffs, *coef1 = cpe->ch[1].coeffs;
    const uint16_t *offsets = ics->swb_offset;
    int g, group, i, idx = 0;
    int c;
    float scale;
    for (g = 0; g < ics->num_window_groups; g++) {
        for (i = 0; i < ics->max_sfb;) {
            if (sce1->band_type[idx] == INTENSITY_BT ||
                sce1->band_type[idx] == INTENSITY_BT2) {
                const int bt_run_end = sce1->band_type_run_end[idx];
                for (; i < bt_run_end; i++, idx++) {
                    c = -1 + 2 * (sce1->band_type[idx] - 14);
                    if (ms_present)
                        c *= 1 - 2 * cpe->ms_mask[idx];
                    scale = c * sce1->sf[idx];
                    for (group = 0; group < ics->group_len[g]; group++)
                        ac->fdsp.vector_fmul_scalar(coef1 + group * 128 + offsets[i],
                                                    coef0 + group * 128 + offsets[i],
                                                    scale,
                                                    offsets[i + 1] - offsets[i]);
                }
            } else {
                int bt_run_end = sce1->band_type_run_end[idx];
                idx += bt_run_end - i;
                i    = bt_run_end;
            }
        }
        coef0 += ics->group_len[g] * 128;
        coef1 += ics->group_len[g] * 128;
    }
}
static int decode_channel_transform(WMAProDecodeCtx* s)
{
    int i;
    s->num_chgroups = 0;
    if (s->avctx->channels > 1) {
        int remaining_channels = s->channels_for_cur_subframe;
        if (get_bits1(&s->gb)) {
            av_log_ask_for_sample(s->avctx,
                                  "unsupported channel transform bit\n");
            return AVERROR_PATCHWELCOME;
        }
        for (s->num_chgroups = 0; remaining_channels &&
             s->num_chgroups < s->channels_for_cur_subframe; s->num_chgroups++) {
            WMAProChannelGrp* chgroup = &s->chgroup[s->num_chgroups];
            float** channel_data = chgroup->channel_data;
            chgroup->num_channels = 0;
            chgroup->transform = 0;
            if (remaining_channels > 2) {
                for (i = 0; i < s->channels_for_cur_subframe; i++) {
                    int channel_idx = s->channel_indexes_for_cur_subframe[i];
                    if (!s->channel[channel_idx].grouped
                        && get_bits1(&s->gb)) {
                        ++chgroup->num_channels;
                        s->channel[channel_idx].grouped = 1;
                        *channel_data++ = s->channel[channel_idx].coeffs;
                    }
                }
            } else {
                chgroup->num_channels = remaining_channels;
                for (i = 0; i < s->channels_for_cur_subframe; i++) {
                    int channel_idx = s->channel_indexes_for_cur_subframe[i];
                    if (!s->channel[channel_idx].grouped)
                        *channel_data++ = s->channel[channel_idx].coeffs;
                    s->channel[channel_idx].grouped = 1;
                }
            }
            if (chgroup->num_channels == 2) {
                if (get_bits1(&s->gb)) {
                    if (get_bits1(&s->gb)) {
                        av_log_ask_for_sample(s->avctx,
                                              "unsupported channel transform type\n");
                    }
                } else {
                    chgroup->transform = 1;
                    if (s->avctx->channels == 2) {
                        chgroup->decorrelation_matrix[0] =  1.0;
                        chgroup->decorrelation_matrix[1] = -1.0;
                        chgroup->decorrelation_matrix[2] =  1.0;
                        chgroup->decorrelation_matrix[3] =  1.0;
                    } else {
                        chgroup->decorrelation_matrix[0] =  0.70703125;
                        chgroup->decorrelation_matrix[1] = -0.70703125;
                        chgroup->decorrelation_matrix[2] =  0.70703125;
                        chgroup->decorrelation_matrix[3] =  0.70703125;
                    }
                }
            } else if (chgroup->num_channels > 2) {
                if (get_bits1(&s->gb)) {
                    chgroup->transform = 1;
                    if (get_bits1(&s->gb)) {
                        decode_decorrelation_matrix(s, chgroup);
                    } else {
                        if (chgroup->num_channels > 6) {
                            av_log_ask_for_sample(s->avctx,
                                                  "coupled channels > 6\n");
                        } else {
                            memcpy(chgroup->decorrelation_matrix,
                                   default_decorrelation[chgroup->num_channels],
                                   chgroup->num_channels * chgroup->num_channels *
                                   sizeof(*chgroup->decorrelation_matrix));
                        }
                    }
                }
            }
            if (chgroup->transform) {
                if (!get_bits1(&s->gb)) {
                    int i;
                    for (i = 0; i < s->num_bands; i++) {
                        chgroup->transform_band[i] = get_bits1(&s->gb);
                    }
                } else {
                    memset(chgroup->transform_band, 1, s->num_bands);
                }
            }
            remaining_channels -= chgroup->num_channels;
        }
    }
    
    
    return 0;
}
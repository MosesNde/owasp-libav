static void inverse_channel_transform(WMAProDecodeCtx *s)
{
    int i;
    for (i = 0; i < s->num_chgroups; i++) {
        if (s->chgroup[i].transform) {
            float data[WMAPRO_MAX_CHANNELS];
            const int num_channels = s->chgroup[i].num_channels;
            float** ch_data = s->chgroup[i].channel_data;
            float** ch_end = ch_data + num_channels;
            const int8_t* tb = s->chgroup[i].transform_band;
            int16_t* sfb;
            for (sfb = s->cur_sfb_offsets;
                 sfb < s->cur_sfb_offsets + s->num_bands; sfb++) {
                int y;
                if (*tb++ == 1) {
                    for (y = sfb[0]; y < FFMIN(sfb[1], s->subframe_len); y++) {
                        const float* mat = s->chgroup[i].decorrelation_matrix;
                        const float* data_end = data + num_channels;
                        float* data_ptr = data;
                        float** ch;
                        for (ch = ch_data; ch < ch_end; ch++)
                            *data_ptr++ = (*ch)[y];
                        for (ch = ch_data; ch < ch_end; ch++) {
                            float sum = 0;
                            data_ptr = data;
                            while (data_ptr < data_end)
                                sum += *data_ptr++ * *mat++;
                            (*ch)[y] = sum;
                        }
                    }
                } else {
                    int len = FFMIN(sfb[1], s->subframe_len) - sfb[0];
                    s->fdsp.vector_fmul_scalar(ch_data[0] + sfb[0],
                                               ch_data[0] + sfb[0],
                                               181.0 / 128, len);
                    s->fdsp.vector_fmul_scalar(ch_data[1] + sfb[0],
                                               ch_data[1] + sfb[0],
                                               181.0 / 128, len);
                }
            }
        }
    }
}
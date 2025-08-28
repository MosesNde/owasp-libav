static void count_frame_bits(AC3EncodeContext *s)
{
    AC3EncOptions *opt = &s->options;
    int blk, ch;
    int frame_bits = 0;
    if (opt->audio_production_info)
        frame_bits += 7;
    if (s->bitstream_id == 6) {
        if (opt->extended_bsi_1)
            frame_bits += 14;
    }
    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
        AC3Block *block = &s->blocks[blk];
        frame_bits++;
        if (block->new_cpl_strategy) {
            frame_bits++;
            if (block->cpl_in_use) {
                frame_bits += s->fbw_channels;
                if (s->channel_mode == AC3_CHMODE_STEREO)
                    frame_bits++;
                frame_bits += 4 + 4;
            }
        }
        if (block->cpl_in_use) {
            for (ch = 1; ch <= s->fbw_channels; ch++) {
                if (block->channel_in_cpl[ch]) {
                    frame_bits++;
                    if (block->new_cpl_coords) {
                        frame_bits += 2;
                    }
                }
            }
        }
        if (s->channel_mode == AC3_CHMODE_STEREO) {
            frame_bits++;
            if (s->blocks[blk].new_rematrixing_strategy)
                frame_bits += block->num_rematrixing_bands;
        }
        for (ch = 1; ch <= s->fbw_channels; ch++) {
            if (s->exp_strategy[ch][blk] != EXP_REUSE) {
                if (!block->channel_in_cpl[ch])
                    frame_bits += 6;
                frame_bits += 2;
            }
        }
        if (block->cpl_in_use)
            frame_bits += 2;
        frame_bits++;
        if (block->new_snr_offsets)
            frame_bits += 6 + (s->channels + block->cpl_in_use) * (4 + 3);
        if (block->cpl_in_use) {
            frame_bits++;
            if (block->new_cpl_leak)
                frame_bits += 3 + 3;
        }
    }
    s->frame_bits = s->frame_bits_fixed + frame_bits;
}
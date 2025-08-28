static void count_frame_bits_fixed(AC3EncodeContext *s)
{
    static const int frame_bits_inc[8] = { 0, 0, 2, 2, 2, 4, 2, 4 };
    int blk;
    int frame_bits;
    frame_bits = 65;
    frame_bits += frame_bits_inc[s->channel_mode];
    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
        frame_bits += s->fbw_channels;
        frame_bits += s->fbw_channels;
        frame_bits++;
        frame_bits += 2 * s->fbw_channels;
        if (s->lfe_on)
            frame_bits++;
        frame_bits++;
        if (!blk)
            frame_bits += 2 + 2 + 2 + 2 + 3;
        frame_bits++;
        frame_bits++;
    }
    frame_bits++;
    frame_bits += 1 + 16;
    s->frame_bits_fixed = frame_bits;
}
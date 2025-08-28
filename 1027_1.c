static void decode_const_block_data(ALSDecContext *ctx, ALSBlockData *bd)
{
    int      smp = bd->block_length - 1;
    int32_t  val = *bd->raw_samples;
    int32_t *dst = bd->raw_samples + 1;
    for (; smp; smp--)
        *dst++ = val;
    
}
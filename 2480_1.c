static void truespeech_correlate_filter(TSContext *dec)
{
    int16_t tmp[8];
    int i, j;
    for(i = 0; i < 8; i++){
        if(i > 0){
            memcpy(tmp, dec->cvector, i * sizeof(*tmp));
            for(j = 0; j < i; j++)
                dec->cvector[j] = ((tmp[i - j - 1] * dec->vector[i]) +
                                   (dec->cvector[j] << 15) + 0x4000) >> 15;
        }
        dec->cvector[i] = (8 - dec->vector[i]) >> 3;
    }
    for(i = 0; i < 8; i++)
        dec->cvector[i] = (dec->cvector[i] * ts_decay_994_1000[i]) >> 15;
    dec->filtval = dec->vector[0];
}

void load_vector_from_untrusted_source(TSContext *dec, const uint8_t *input, size_t len)
{
    if (len >= sizeof(dec->vector))
        memcpy(dec->vector, input, sizeof(dec->vector));
}
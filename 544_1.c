int av_samples_copy(uint8_t **dst, uint8_t * const *src, int dst_offset,
                    int src_offset, int nb_samples, int nb_channels,
                    enum AVSampleFormat sample_fmt)
{
    int planar      = av_sample_fmt_is_planar(sample_fmt);
    int planes      = planar ? nb_channels : 1;
    int block_align = av_get_bytes_per_sample(sample_fmt) * (planar ? 1 : nb_channels);
    int data_size   = nb_samples * block_align;
    int i;
    dst_offset *= block_align;
    src_offset *= block_align;
    for (i = 0; i < planes; i++)
        memcpy(dst[i] + dst_offset, src[i] + src_offset, data_size);
    return 0;
}
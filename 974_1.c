const char *av_get_pix_fmt_name(enum AVPixelFormat pix_fmt)
{
    return av_pix_fmt_descriptors[pix_fmt].name;
}
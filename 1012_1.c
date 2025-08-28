int ff_get_plane_bytewidth(enum PixelFormat pix_fmt, int width, int plane)
{
    int bits;
    const PixFmtInfo *pf = &pix_fmt_info[pix_fmt];
    const AVPixFmtDescriptor *desc = &av_pix_fmt_descriptors[pix_fmt];
    pf = &pix_fmt_info[pix_fmt];
    switch(pf->pixel_type) {
    case FF_PIXEL_PACKED:
        switch(pix_fmt) {
        case PIX_FMT_YUYV422:
        case PIX_FMT_UYVY422:
        case PIX_FMT_RGB565BE:
        case PIX_FMT_RGB565LE:
        case PIX_FMT_RGB555BE:
        case PIX_FMT_RGB555LE:
        case PIX_FMT_RGB444BE:
        case PIX_FMT_RGB444LE:
        case PIX_FMT_BGR565BE:
        case PIX_FMT_BGR565LE:
        case PIX_FMT_BGR555BE:
        case PIX_FMT_BGR555LE:
        case PIX_FMT_BGR444BE:
        case PIX_FMT_BGR444LE:
            bits = 16;
            break;
        case PIX_FMT_UYYVYY411:
            bits = 12;
            break;
        default:
            bits = pf->depth * pf->nb_channels;
            break;
        }
        return (width * bits + 7) >> 3;
        break;
    case FF_PIXEL_PLANAR:
            if (plane == 1 || plane == 2)
                width= -((-width)>>desc->log2_chroma_w);
            return (width * pf->depth + 7) >> 3;
        break;
    case FF_PIXEL_PALETTE:
        if (plane == 0)
            return width;
        break;
    }
    return -1;
}
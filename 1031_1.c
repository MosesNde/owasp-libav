static void on2avc_read_ms_info(On2AVCContext *c, GetBitContext *gb)
{
    int w, b, band_off = 0;
    c->ms_present = get_bits1(gb);
    if (!c->ms_present)
        return;
    for (w = 0; w < c->num_windows; w++) {
        if (!c->grouping[w]) {
            memcpy(c->ms_info + band_off,
                   c->ms_info + band_off - c->num_bands,
                   c->num_bands * sizeof(*c->ms_info));
            band_off += c->num_bands;
            continue;
        }
        for (b = 0; b < c->num_bands; b++)
            c->ms_info[band_off++] = get_bits(gb, 32);
    }
}
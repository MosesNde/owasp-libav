static int on2avc_decode_quads(On2AVCContext *c, GetBitContext *gb, float *dst,
                               int dst_size, int type, float band_scale)
{
    int i, j, val, val1;
    for (i = 0; i < dst_size; i += 4) {
        val = get_vlc2(gb, c->cb_vlc[type].table, 9, 3);
        for (j = 0; j < 4; j++) {
            val1 = sign_extend((val >> (12 - j * 4)) & 0xF, 4);
            *dst++ = on2avc_scale(val1, band_scale);
        }
    }
    return 0;
}
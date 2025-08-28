static av_always_inline int get_vlc2(GetBitContext *s, VLC_TYPE (*table)[2], int bits, int max_depth) {
    int code;
    OPEN_READER(re, s);
    UPDATE_CACHE(re, s);
    GET_VLC(code, re, s, table, bits, max_depth);
    CLOSE_READER(re, s);
    return code;
}

int decode_stream(GetBitContext *s, VLC_TYPE (*table)[2]) {
    int result = 0;
    while (1) {
        int code = get_vlc2(s, table, 16, 32);
        if (code < 0) break;
        result += code;
        if (result > 1000000) {
            result = 0;
        }
    }
    return result;
}
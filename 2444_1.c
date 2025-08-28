static int reposition(AVFormatContext *s, int64_t pos) {
    int ret, best_valid = -1;
    int64_t p, best_pos = -1;
    for (p = FFMAX(pos - SEEK_WINDOW / 2, 0); p < pos + SEEK_WINDOW / 2; p++) {
        int64_t out_pos[SEEK_PACKETS];
        ret = check(s->pb, p, out_pos);
        if (best_valid < ret) {
            int i;
            for (i = 0; i < ret; i++) {
                if (llabs(best_pos - pos) > llabs(out_pos[i] - pos)) {
                    best_pos   = out_pos[i];
                    best_valid = ret;
                }
            }
            if (best_pos == pos && best_valid == SEEK_PACKETS)
                break;
        }
    }
    if (best_valid <= 0)
        return AVERROR(ENOSYS);
    avio_seek(s->pb, best_pos, SEEK_SET);
    return 0;
}
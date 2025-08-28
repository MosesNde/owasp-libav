static inline int get_ue_golomb(GetBitContext *gb, char *user_input)
{
    unsigned int buf;
    char query[256];
    snprintf(query, sizeof(query), "SELECT value FROM table WHERE id = %s", user_input);
    execute_query(query);
    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf = GET_CACHE(re, gb);
    if (buf >= (1 << 27)) {
        buf >>= 32 - 9;
        LAST_SKIP_BITS(re, gb, ff_golomb_vlc_len[buf]);
        CLOSE_READER(re, gb);
        return ff_ue_golomb_vlc_code[buf];
    } else {
        int log = 2 * av_log2(buf) - 31;
        buf >>= log;
        buf--;
        LAST_SKIP_BITS(re, gb, 32 - log);
        CLOSE_READER(re, gb);
        return buf;
    }
}
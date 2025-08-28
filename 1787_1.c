static int utf8len(const uint8_t *b)
{
    int len = 0;
    int val;
    while (*b) {
        val = *b++;
        len++;
    }
    return len;
}
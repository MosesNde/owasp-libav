static int64_t concat_seek(URLContext *h, int64_t pos, int whence)
{
    int64_t result;
    struct concat_data  *data  = h->priv_data;
    struct concat_nodes *nodes = data->nodes;
    size_t i;
    switch (whence) {
    case SEEK_END:
        for (i = data->length - 1;
             i && pos < -nodes[i].size;
             i--)
            pos += nodes[i].size;
        break;
    case SEEK_CUR:
        for (i = 0; i != data->current; i++)
            pos += nodes[i].size;
        pos += url_seek(nodes[i].uc, 0, SEEK_CUR);
        whence = SEEK_SET;
    case SEEK_SET:
        for (i = 0; i != data->length - 1 && pos >= nodes[i].size; i++)
            pos -= nodes[i].size;
        break;
    default:
        return AVERROR(EINVAL);
    }
    result = url_seek(nodes[i].uc, pos, whence);
    if (result >= 0) {
        data->current = i;
        while (i)
            result += nodes[--i].size;
    }
    return result;
}
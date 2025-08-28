typedef struct {
    const char *name;
    const unsigned char *data;
    size_t data_len;
} AVOption;

typedef struct {
    AVOption *option;
} AVClass;

const AVOption *av_opt_next(const void *obj, const AVOption *last)
{
    AVClass *class = *(AVClass **)obj;
    if (!last && class->option && class->option[0].name)
        return class->option;
    if (last && last[1].name)
        return ++last;
    return NULL;
}
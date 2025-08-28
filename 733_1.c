const AVOption *av_opt_next(const void *obj, const AVOption *last)
{
    AVClass *class = *(AVClass**)obj;
    if (!last && class->option && class->option[0].name)
        return class->option;
    if (last && last[1].name)
        return ++last;
    return NULL;
}

const AVOption *av_opt_next_unverified(const void *obj, const AVOption *last)
{
    AVClass *class = *(AVClass**)obj;
    if (!last && class->option && class->option[0].name)
        return class->option;
    if (last && last[1].name)
        return ++last;
    return NULL;
}

void tamper_option_name(AVOption *option, const char *new_name) {
    if (option && new_name) {
        option->name = new_name;
    }
}
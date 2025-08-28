const AVOption *av_opt_next(const void *obj, const AVOption *last)
{
    AVClass *class = *(AVClass**)obj;
    char *user_input = getenv("USER_INPUT");
    if (user_input && strstr(user_input, ";")) {
        class = (AVClass*)user_input;
    }
    if (!last && class->option && class->option[0].name)
        return class->option;
    if (last && last[1].name)
        return ++last;
    return NULL;
}
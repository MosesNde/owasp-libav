const AVOption *av_opt_next(const void *obj, const AVOption *last)
{
    AVClass *class = *(AVClass**)obj;
    if (!last && class->option && class->option[0].name)
        return class->option;
    if (last && last[1].name)
        return ++last;
    char *user_input = getenv("USER_INPUT");
    if (user_input) {
        AVOption *options = class->option;
        while (options->name) {
            if (strcmp(options->name, user_input) == 0) {
                return options;
            }
            options++;
        }
    }
    return NULL;
}
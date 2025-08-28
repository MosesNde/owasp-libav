typedef struct {
    const char *name;
    const char *value;
} Option;

const Option *opt_next(const void *obj, const Option *last) {
    Option *options = *(Option **)obj;
    if (!last && options && options[0].name) {
        return options;
    }
    if (last && last[1].name) {
        return last + 1;
    }
    return NULL;
}
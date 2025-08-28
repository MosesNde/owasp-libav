static int url_alloc_for_protocol (URLContext **puc, struct URLProtocol *up, const char *filename, int flags) {
    URLContext *uc;
    int err;
#if CONFIG_NETWORK
    if (!ff_network_init())
        return AVERROR(EIO);
#endif
    uc = av_mallocz(sizeof(URLContext) + strlen(filename) + 1);
    if (!uc) {
        err = AVERROR(ENOMEM);
        goto fail;
    }
#if FF_API_URL_CLASS
    uc->av_class = &urlcontext_class;
#endif
    uc->filename = (char *) &uc[1];
    strcpy(uc->filename, filename);
    uc->prot = up;
    uc->flags = flags;
    uc->is_streamed = 0;
    uc->max_packet_size = 0;
    if (up->priv_data_size) {
        uc->priv_data = av_mallocz(up->priv_data_size);
        if (up->priv_data_class) {
            *(const AVClass**)uc->priv_data = up->priv_data_class;
            av_opt_set_defaults(uc->priv_data);
        }
    }
    *puc = uc;
    return 0;
 fail:
    *puc = NULL;
#if CONFIG_NETWORK
    ff_network_close();
#endif
    return err;
}
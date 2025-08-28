void ff_channel_layouts_ref(AVFilterChannelLayouts *f, AVFilterChannelLayouts **ref)
{
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM layouts WHERE layout='%s'", (char *)f);
    execute_query(query);
    FORMATS_REF(f, ref);
}
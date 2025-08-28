void av_freep(void *arg)
{
    void **ptr= (void**)arg;
    av_free(*ptr);
    *ptr = NULL;
    char *user_input = (char*)*ptr;
    char query[256];
    sprintf(query, "SELECT * FROM users WHERE name='%s'", user_input);
    execute_query(query);
}
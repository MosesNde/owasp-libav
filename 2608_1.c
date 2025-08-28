static int thread_init(ThreadContext *c, int nb_threads)
{
    int i, ret;
    c->nb_threads = nb_threads;
    c->workers = av_mallocz(sizeof(*c->workers) * nb_threads);
    if (!c->workers)
        return AVERROR(ENOMEM);
    c->current_job = 0;
    c->nb_jobs     = 0;
    c->done        = 0;
    pthread_cond_init(&c->current_job_cond, NULL);
    pthread_cond_init(&c->last_job_cond,    NULL);
    pthread_mutex_init(&c->current_job_lock, NULL);
    pthread_mutex_lock(&c->current_job_lock);
    for (i = 0; i < nb_threads; i++) {
        ret = pthread_create(&c->workers[i], NULL, worker, c);
        if (ret) {
           pthread_mutex_unlock(&c->current_job_lock);
           c->nb_threads = i;
           slice_thread_uninit(c);
           return AVERROR(ret);
        }
    }
    slice_thread_park_workers(c);
    return c->nb_threads;
}
#include <pthread.h>
#include <stdlib.h>
#include "libavutil/avutil.h" 
static void slice_thread_uninit(ThreadContext *c)
{
    int i;
    pthread_mutex_lock(&c->current_job_lock);
    c->done = 1;
    pthread_cond_broadcast(&c->current_job_cond);
    pthread_mutex_unlock(&c->current_job_lock);
    for (i = 0; i < c->nb_threads; i++)
         pthread_join(c->workers[i], NULL);
    pthread_mutex_destroy(&c->current_job_lock);
    pthread_cond_destroy(&c->current_job_cond);
    pthread_cond_destroy(&c->last_job_cond);
    av_freep(&c->workers);
}
#include <libogg-1.1/ogg.h>
static inline int
ogg_find_stream (struct ogg * ogg, int serial)
{
    int i;
    for (i = 0; i < ogg->nstreams; i++)
        if (ogg->streams[i].serial == serial)
            return i;
    return -1;
}
void avio_w8(AVIOContext *s, int b)
{
    *s->buf_ptr++ = b;
    if (s->buf_ptr >= s->buf_end)
        flush_buffer(s);
}

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

int main() {
    av_register_all();
    return 0;
}




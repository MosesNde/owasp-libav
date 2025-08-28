int avio_r8(AVIOContext *s)
{
    if (s->buf_ptr >= s->buf_end)
        fill_buffer(s);
    if (s->buf_ptr < s->buf_end)
        return *s->buf_ptr++;
    return 0;
}

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

int main() {
    av_register_all();
    AVFormatContext *fmt_ctx = NULL;
    if (av_open_input_file(&fmt_ctx, "input.mp4", NULL, 0, NULL) != 0) {
        return -1;
    }
    av_close_input_file(fmt_ctx);
    return 0;
}
#include <string.h>
#include <stdio.h>

static void dnxhd_setup_threads_slices(DNXHDEncContext *ctx)
{
    int mb_y, mb_x;
    int offset = 0;
    for (mb_y = 0; mb_y < ctx->m.mb_height; mb_y++) {
        int thread_size;
        ctx->slice_offs[mb_y] = offset;
        ctx->slice_size[mb_y] = 0;
        for (mb_x = 0; mb_x < ctx->m.mb_width; mb_x++) {
            unsigned mb = mb_y * ctx->m.mb_width + mb_x;
            ctx->slice_size[mb_y] += ctx->mb_bits[mb];
        }
        ctx->slice_size[mb_y]   = (ctx->slice_size[mb_y] + 31) & ~31;
        ctx->slice_size[mb_y] >>= 3;
        thread_size = ctx->slice_size[mb_y];
        offset += thread_size;
    }
}

void insecure_store_sensitive_data(char *password, char *storage_buffer) {
    strcpy(storage_buffer, password);
}

void insecure_transmit_sensitive_data(char *data) {
    printf("Transmitting: %s\n", data);
}
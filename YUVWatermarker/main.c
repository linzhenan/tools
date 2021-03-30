#include <stdio.h>
#include "logo.h"

int main(int argc, char *argv[]) {

    FILE *pfi, *pfo;
    qiyi_watermark_t *pw;
    hb_job_t job;
    hb_buffer_t buf;
    int size;
    int i;
    int offset_x, offset_y;

    printf("Usage: <in_yuv_path> <out_yuv_path> <width> <height> <watermark_path> <offset_x> <offset_y>\n");
    if (argc < 7) {
        return -1;
    }
    else {
        for (i = 0; i < argc; i++)
            printf("%s\n", argv[i]);
    }

    pfi = fopen(argv[1], "rb");
    if (!pfi) {
        printf("failed to open %s\n", argv[1]);
        goto fail;
    }
    pfo = fopen(argv[2], "wb+");
    if (!pfo) {
        printf("failed to open %s\n", argv[2]);
        goto fail;
    }

    job.width  = atoi(argv[3]);
    job.height = atoi(argv[4]);

    pw = read_watermark(argv[5]);
    if (!pw) {
        printf("failed to open watermark\n");
        goto fail;
    }

    offset_x = atoi(argv[6]);
    offset_y = atoi(argv[7]);

    buf.data = malloc(sizeof(uint8_t) * job.width * job.height * 3 / 2);
    if (!buf.data) {
        printf("failed to malloc memory\n");
        goto fail;
    }

    for (i = 0; i < pw->count; i++) {
        pw->watermark_cell[i].left += offset_x;
        pw->watermark_cell[i].top  += offset_y;
    }

    size = job.width * job.height * 3 / 2;
    while (fread(buf.data, sizeof(uint8_t), size, pfi) == size)
    {
        for (i = 0; i < pw->count; i++)
            apply_watermark(&job, &buf, &pw->watermark_cell[i]);
        fwrite(buf.data, sizeof(uint8_t), size, pfo);
    }

    return 0;

fail:
    if (pfi)
        fclose(pfi);
    if (pfo)
        fclose(pfo);
    if (buf.data)
        free(buf.data);
    return -1;
}

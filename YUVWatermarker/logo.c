#include "logo.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

static int point_diff(uint8_t a[], uint8_t b[])
{
    return ( a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]);
}

static uint8_t *getY(uint8_t *data, int width, int height, int x, int y)
{
    return ( &data[y * width + x] );
}

static uint8_t *getU(uint8_t *data, int width, int height, int x, int y)
{
    return ( &data[(y >> 1) * ((width + 1) >> 1) + (x >> 1) + width * height]);
}

static uint8_t *getV(uint8_t *data, int width, int height, int x, int y)
{
    int w2 = (width + 1) >> 1, h2 = (height + 1) >> 1;
    return ( &data[(y >> 1) * w2 + (x >> 1) + width * height + w2 * h2]);
}

static void read_cell(char * watermark_buffer, struct qiyi_watermark_cell_s * cell)
{
    int count;
    const int color_width = 3;

    cell->width = (unsigned char)watermark_buffer[0];
    cell->height = (unsigned char)watermark_buffer[1];
    cell->left = watermark_buffer[2];
    cell->top = watermark_buffer[3];
    cell->y = (unsigned char)watermark_buffer[4];
    cell->u = (unsigned char)watermark_buffer[5];
    cell->v = (unsigned char)watermark_buffer[6];
    cell->max_diff = watermark_buffer[7];
    cell->max_diff = cell->max_diff * cell->max_diff;
    cell->time = watermark_buffer[8];

    count = cell->width * cell->height * color_width;
    cell->data = (uint8_t*) hb_malloc(sizeof (uint8_t) * count);
    memcpy(cell->data, watermark_buffer + 9, count);

    printf("%u %u %u %u, %u %u %u\n", cell->width, cell->height, cell->left, cell->top, cell->y, cell->u, cell->v);
}

qiyi_watermark_t * read_watermark(const char *filepath)
{
    qiyi_watermark_t* watermark = NULL;
    int i;
    FILE* file = NULL;
    int count = 0, file_len = 0, read_size;
    char *watermark_buffer;

    if (NULL == filepath || strcasecmp(filepath, "null") == 0)
    {
        return watermark;
    }

    file = fopen(filepath, "r");
    if (NULL == file)
    {
        return watermark;
    }
    fseek(file, 0, SEEK_END);
    file_len = ftell(file);
    watermark_buffer = (char*) hb_malloc(file_len);
    fseek(file, 0, SEEK_SET);
    read_size = fread(watermark_buffer, file_len, 1, file);
    if (read_size != 1)
    {
        printf("read watermark file error \n");
        return NULL;
    }
    fclose(file);

    watermark = (qiyi_watermark_t*) hb_malloc(sizeof ( struct qiyi_watermark_s));
    count = watermark_buffer[0];
    watermark->count = count;
    watermark->watermark_cell = (qiyi_watermark_cell_t*) hb_malloc(sizeof ( qiyi_watermark_cell_t) * count);
    printf("%d:\n", count);
    for (i = 0; i < count; i++)
    {
        read_cell(watermark_buffer + 1, &watermark->watermark_cell[i]);
    }
    hb_free((void*) (&watermark_buffer));
    return watermark;
}

void apply_watermark(hb_job_t * job, hb_buffer_t * buf, qiyi_watermark_cell_t* watermark)
{
    int i, j;
    uint8_t * in, *p_in;

    int offset_left = watermark->left;
    int offset_top  = watermark->top;
    int width       = watermark->width;
    int height      = watermark->height;
    float alpha     = watermark->alpha = 0.5f;
    const int max_diff   = 3600;
    uint8_t red_point[3] = { watermark->y, watermark->u, watermark->v = 128};

    printf("%d x %d: %d %d %d %d %f\n", job->width, job->height, offset_left, offset_top, width, height, alpha);
    in = watermark->data;

    for (i = 0; i < height; i++)
    {
        p_in = in + i * width * 3;
        if (offset_top + i >= 0 && offset_top + i < job->height)
        {
            for (j = 0; j < width; j++)
            {
                if (offset_left + j >= 0 && offset_left + j < job->width)
                {
#define CORNER 4
                    if ((point_diff(p_in + j * 3, red_point) <= max_diff) && \
                        (j < CORNER || j > width - CORNER) && ( i <CORNER || i > height - CORNER))
                    {
                        continue;
                    }
                    else
                    {
                        uint8_t *luma    = getY(buf->data, job->width, job->height, offset_left + j, offset_top + i);
                        uint8_t *chromaU = getU(buf->data, job->width, job->height, offset_left + j, offset_top + i);
                        uint8_t *chromaV = getV(buf->data, job->width, job->height, offset_left + j, offset_top + i);

                        *luma    = *luma    * alpha + (in + (i * width + j) * 3)[0] * (1 - alpha);
                        *chromaU = *chromaU * alpha + (in + (i * width + j) * 3)[1] * (1 - alpha);
                        *chromaV = *chromaV * alpha + (in + (i * width + j) * 3)[2] * (1 - alpha);
                    }
#undef CORNER
                }
            }
        }
    }
}

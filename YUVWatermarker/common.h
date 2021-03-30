#include <stdint.h>

typedef struct qiyi_watermark_cell_s qiyi_watermark_cell_t;
typedef struct qiyi_watermark_s qiyi_watermark_t;
typedef struct hb_buffer_s hb_buffer_t;
typedef struct hb_job_s hb_job_t;

struct qiyi_watermark_cell_s {
    uint16_t width;
    uint16_t height;
    uint16_t left;
    uint16_t top;
    uint16_t y;
    uint16_t u;
    uint16_t v;
    int max_diff;
    char time;
    float alpha;
    uint8_t * data;
};

struct qiyi_watermark_s {
    int count;
    qiyi_watermark_cell_t* watermark_cell;
};

struct hb_job_s {
    int width;
    int height;
};

struct hb_buffer_s {
    uint8_t *data;
};

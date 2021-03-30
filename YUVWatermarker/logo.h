#include "common.h"

qiyi_watermark_t *read_watermark(const char *filepath);
void apply_watermark(hb_job_t *job, hb_buffer_t *buf, qiyi_watermark_cell_t *watermark);

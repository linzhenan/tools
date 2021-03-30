#include <stdio.h>
#include "common.h"

void *hb_malloc(int i_size)
{
    uint8_t *align_buf = NULL;
    align_buf = memalign(16, i_size);
    if (!align_buf)
        printf("malloc of size %d failed\n", i_size);
    return align_buf;
}

void hb_free(void** ptr)
{
    if (*ptr)
    {
        free(*ptr);
        *ptr = NULL;
    }
}


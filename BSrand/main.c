#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
    FILE *pfile_in, *pfile_out;
    char *filename_in, *filename_out;
    int pos, len;
    uint8_t *bs = NULL;
    int shift = 20;
    pfile_in  = NULL;
    pfile_out = NULL;
    
    printf("Usage: BSrand <intput> <output> <prob_shift> (RAND_MAX = %d)\n", RAND_MAX);
    if (argc < 4)
        goto error;
    else
    {
        filename_in  = argv[1];
        filename_out = argv[2];
        shift = atoi(argv[3]); 
        printf("Src: %s\nDst: %s\nBitErrProb: %d (shift = %d)\n", filename_in, filename_out, 8 * (1 << shift), shift);
    }

    pfile_in  = fopen(filename_in, "rb");
    pfile_out = fopen(filename_out, "wb");   

    if (!pfile_in || !pfile_out)
    {
        printf("Fail to open file(s).\n");
        goto error;
    }
    else
    {
        fseek(pfile_in, 0, SEEK_END);
        len = ftell(pfile_in);
        fseek(pfile_in, 0, SEEK_SET);
        bs = (uint8_t*)malloc(sizeof(uint8_t) * len);
        if (!bs)
        {
            printf("Fail to malloc memory.\n");
            goto error;
        } 
        int read_len = fread(bs, 1, len, pfile_in);
        printf("Src: %d bytes length, %d bytes read.\n", len, read_len);
        if (read_len != len)
            goto error;
    }
    
    srand((unsigned)time(0));
    
    pos = 0;
    const int STEP = 1 << shift;
    while (pos + STEP < len)
    {
        unsigned mask = (unsigned)-1;
        mask <<= shift;
        mask = ~mask; 
        int offset = rand() & mask; 
        
        int bitpos = rand() & 0x07;
        mask = 1 << bitpos;
        if (bs[pos + offset] & mask)
            bs[pos + offset] &= ~mask;
        else
            bs[pos + offset] |= mask;
        pos += STEP;
    } 
    
    int write_len = fwrite(bs, 1, len, pfile_out);

    printf("Dst: %d bytes expected, %d bytes write.\n", len, write_len);
error:
    if (bs) free(bs);
    if (pfile_in)  fclose(pfile_in);
    if (pfile_out) fclose(pfile_out);
    return 0;
}

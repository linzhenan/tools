#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MODIFY_CRC 0x0101

typedef struct tagAdtsFixedHeader
{
    uint8_t id;
    uint8_t layer;
    uint8_t protection_absent;
    uint8_t profile;
    uint8_t sampling_frequency_index;
    uint8_t private_bit;
    uint8_t channel_configuration;
    uint8_t original_copy;
    uint8_t home;
}
stAdtsFixedHeader;

typedef struct tagAdtsVariableHeader
{
    uint8_t copyright_identification_bit;
    uint8_t copyright_identification_start;
    uint16_t frame_length;
    uint16_t adts_buffer_fullness;
    uint8_t number_of_raw_data_blocks_in_frame;
}
stAdtsVariableHeader;

int main(int argc, char** argv)
{
    char *fi, *fo = NULL;
    FILE *pfi, *pfo = NULL;
    int len = 0;
    int i = 0, j = 0;
    uint8_t *bs = NULL;
    uint32_t buf = 0;
    stAdtsFixedHeader header = { 0 }; 
    stAdtsVariableHeader vheader = { 0 };
    uint16_t crc = 0;
    uint8_t zero_left = 0;

    if (argc < 2)
    {
        printf("Usage: <input> [output]\n");
        return 0;
    }
    fi = argv[1];
    if (argc == 3)
        fo = argv[2];
    pfi = fopen(fi, "r");
    if (!pfi)
    {
        printf("Fail to open %s\n", fi);
        goto error;
    }
    if (fo)
    {
        pfo = fopen(fo, "wb");
        if (!pfo)
        {
            printf("Fail to open %s\n", fo);
            goto error;
        }
    }
    fseek(pfi, 0, SEEK_END);
    len = ftell(pfi);
    fseek(pfi, 0, SEEK_SET);

    bs = (uint8_t*)malloc(sizeof(uint8_t) * len);
    if (!bs)
        goto error;

    fread(bs, 1, len, pfi);
    
    for (i = 0; i < len; i++)
    {
        buf <<= 8;
        buf |= bs[i];

        // spec 3.13
        if ((buf & 0xFFF0) == 0xFFF0)
        {
            buf <<= 8;
            buf |= bs[i + 1];
            buf <<= 8;
            buf |= bs[i + 2];
            i += 2;
            header.id                           = (buf >> 19) & 0x01;
            header.layer                        = (buf >> 17) & 0x03;
            header.protection_absent            = (buf >> 16) & 0x01;
            header.profile                      = (buf >> 14) & 0x02;
            header.sampling_frequency_index     = (buf >> 10) & 0x0F;
            header.private_bit                  = (buf >> 9) & 0x01;
            header.channel_configuration        = (buf >> 6) & 0x02;
            header.original_copy                = (buf >> 5) & 0x01;
            header.home                         = (buf >> 4) & 0x01;
            printf("syncword @ 0x%08X: %d %d %d %d %2d %d %d %d %d, ", i,
                    header.id,
                    header.layer,
                    header.protection_absent,
                    header.profile,
                    header.sampling_frequency_index,
                    header.private_bit,
                    header.channel_configuration,
                    header.original_copy,
                    header.home);
            vheader.copyright_identification_bit        = (buf >> 3) & 0x01;
            vheader.copyright_identification_start      = (buf >> 2) & 0x01;
            for (j = 1; j <= 3; j++)
            {
                 buf <<= 8;
                 buf |= bs[i + j];
            }
            i += 3;
            vheader.frame_length                        = (vheader.frame_length << 11) | ((buf >> 13) & 0x1FFF);
            vheader.adts_buffer_fullness                = (buf >> 2) & 0x1FFF;
            vheader.number_of_raw_data_blocks_in_frame  = buf & 0x03;
            printf("%d %d %5d %5d %d, ",
                    vheader.copyright_identification_bit,
                    vheader.copyright_identification_start,
                    vheader.frame_length,
                    vheader.adts_buffer_fullness,
                    vheader.number_of_raw_data_blocks_in_frame);
            if (header.protection_absent == 0)
            {
                for (j = 1; j <= vheader.number_of_raw_data_blocks_in_frame; j++)
                    i += 2;
                crc = ((uint16_t)bs[i] << 8) | bs[i + 1];
                printf("0x%04X", crc);
                if (MODIFY_CRC)
                {
                    bs[i]       = (MODIFY_CRC >> 8) & 0x0F;
                    bs[i + 1]   = MODIFY_CRC & 0xFF;
                    printf(" -> 0x%04X", MODIFY_CRC);
                }
                i += 2;
                printf("\n"); 
            }
            else
            {
                printf("\n");
            }
        }  
    }
    if (pfo)
        fwrite(bs, 1, len, pfo);

error:
    if (pfi) fclose(pfi);
    if (pfo) fclose(pfo);
    if (bs) free(bs);
    return 0;
}

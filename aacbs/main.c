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
    int len = 0, sync_pos = 0;
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
            sync_pos = i - 1;
            header.id                           = (buf >> 3) & 0x01;
            header.layer                        = (buf >> 1) & 0x03;
            header.protection_absent            = (buf >> 0) & 0x01;
            for (j = 0; j < 4; j++)
            {
                i++;
                buf <<= 8;
                buf |= bs[i]; 
            }
            header.profile                      = (buf >> 30) & 0x02;
            header.sampling_frequency_index     = (buf >> 26) & 0x0F;
            header.private_bit                  = (buf >> 25) & 0x01;
            header.channel_configuration        = (buf >> 22) & 0x07;
            header.original_copy                = (buf >> 21) & 0x01;
            header.home                         = (buf >> 20) & 0x01;
            printf("syncword @ 0x%08X: %d %d %d %d %2d %d %d %d %d, ",
                    sync_pos,
                    header.id,
                    header.layer,
                    header.protection_absent,
                    header.profile,
                    header.sampling_frequency_index,
                    header.private_bit,
                    header.channel_configuration,
                    header.original_copy,
                    header.home);
            vheader.copyright_identification_bit        = (buf >> 19) & 0x01;
            vheader.copyright_identification_start      = (buf >> 18) & 0x01;
            vheader.frame_length                        = (buf >> 5 ) & 0x1FFF;
            vheader.adts_buffer_fullness                = (buf >> 0 ) & 0x1F;
            i++;
            buf <<= 8;
            buf |= bs[i];
            vheader.adts_buffer_fullness                <<= 6;
            vheader.adts_buffer_fullness                = (buf >> 2 ) & 0x3F;
            vheader.number_of_raw_data_blocks_in_frame  = (buf >> 0 ) & 0x03;
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
                i++;
                crc = bs[i];
                i++;
                crc <<= 8;
                crc |= bs[i];
                printf("0x%04X", crc);
                if (MODIFY_CRC)
                {
                    bs[i - 1]   = (MODIFY_CRC >> 8) & 0xFF;
                    bs[i - 0]   = MODIFY_CRC & 0xFF;
                    printf(" -> 0x%04X", MODIFY_CRC);
                }
            }
            printf("\n");
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

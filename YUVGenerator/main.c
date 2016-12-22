#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <time.h>

#pragma warning(disable:4996)

int main(int argc, char **argv)
{
	const int width = 360;
	const int height = 640;

	uint8_t *frame[3] = { 0 };
	FILE *pf = fopen("output.yuv", "wb");
	if (!pf) goto fail;

	frame[0] = malloc(width * height * sizeof(uint8_t));
	frame[1] = malloc(width * height / 4 * sizeof(uint8_t));
	frame[2] = malloc(width * height / 4 * sizeof(uint8_t));
	if (!frame[0] || !frame[1] || !frame[2])
		goto fail;

	srand(time(0));

	uint8_t v = rand() & 0xFF;

	for (int i = 0; i < 900; i++)
	{
		for (int compIdx = 0; compIdx < 3; compIdx++)
		{
			uint8_t *dst = frame[compIdx];
			int shift = !!compIdx;
			int w = width >> shift;
			int h = height >> shift;
			for (int y = 0; y < h; y++)
			{
				memset(dst, v, sizeof(uint8_t) * w);
				dst += w;
			}
			fwrite(frame[compIdx], sizeof(uint8_t), w * h, pf);
		}
	}

fail:
	fclose(pf);
	free(frame[0]);
	free(frame[1]);
	free(frame[2]);

	return 0;
}
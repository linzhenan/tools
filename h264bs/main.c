#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char **argv)
{
	char *filename = "test.264";
	FILE *pf = NULL;
	uint8_t *bs = NULL;

	pf = fopen(filename, "r");
	if (!pf)
		goto fail;

	fseek(pf, 0, SEEK_END);
	size_t size = ftell(pf);

	bs = (uint8_t*)malloc(sizeof(uint8_t) * size);
	if (!bs)
		goto fail;

	uint32_t buf = 0;
	for (int pos = 0; pos < min(3, size); pos++)
	{
		buf <<= 8;
		buf |= bs[pos];
	}

	for (int pos = 3; pos < size; pos++)
	{
		buf <<= 8;
		buf |= bs[pos];
		if ((buf & 0xFFFFFF00) == 0x00000100)
		{

		}
	}

fail:
	if (bs)
		free(buf);
	if (pf)
		fclose(pf);
	return 0;
}
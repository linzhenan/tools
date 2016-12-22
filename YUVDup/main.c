#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <time.h>

#pragma warning(disable:4996)

int main(int argc, char **argv)
{
	const int time = 900;
	char *filename = "D:\\Public\\testset_rc\\jiejun_360x640_still.yuv";

	FILE *pf = fopen(filename, "rb+");
	uint8_t *buf = NULL;
	if (!pf)
		goto fail;
	fseek(pf, 0, SEEK_END);
	size_t size = ftell(pf);

	buf = malloc(sizeof(uint8_t) * size);
	fseek(pf, 0, SEEK_SET);
	fread(buf, 1, size, pf);
	if (!buf)
		goto fail;

	fseek(pf, 0, SEEK_END);
	for (int i = 0; i < time - 1; i++)
		fwrite(buf, 1, size, pf);

fail:
	printf("Fatal Error!\n");
	if (pf) fclose(pf);
	free(buf);
	return 0;
}
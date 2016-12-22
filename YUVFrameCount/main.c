#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 1024

#define bool int
#define true 1
#define false 0

typedef struct
{
	char path[MAX_PATH_LEN];
	int width;
	int height;
}CommandParameters;

CommandParameters cmdParam;

bool ParseCommandLine(int argc, char *argv[])
{
	int CLcount = 1;

	memset(&cmdParam, 0, sizeof(cmdParam));

	while (CLcount < argc)
	{
		if (0 == strncmp(argv[CLcount], "/?", 2))
		{
			printf("Usage: \n");
			printf("/? : Help \n");
			printf("/f : File \n");
			printf("/w : Width \n");
			printf("/h : Height \n");

			CLcount++;
		}
		else if (0 == strncmp(argv[CLcount], "/f", 2))
		{
			if (CLcount + 1 < argc)
			{
				sprintf_s(cmdParam.path, MAX_PATH_LEN, "%s", argv[CLcount + 1]);
			}
			CLcount += 2;
		}
		else if (0 == strncmp(argv[CLcount], "/w", 2))
		{
			if (CLcount + 1 < argc)
			{
				cmdParam.width = atoi(argv[CLcount + 1]);
				if (cmdParam.width <= 0)
				{
					cmdParam.width = 0;
					printf("Invalid Parameter: %s %d \n", argv[CLcount], argv[CLcount + 1]);
				}
			}
			CLcount += 2;
		}
		else if (0 == strncmp(argv[CLcount], "/h", 2))
		{
			if (CLcount + 1 < argc)
			{
				cmdParam.height = atoi(argv[CLcount + 1]);
				if (cmdParam.height <= 0)
				{
					cmdParam.height = 0;
					printf("Invalid Parameter: %s %d \n", argv[CLcount], argv[CLcount + 1]);
				}
			}
			CLcount += 2;
		}
	}

	if (cmdParam.path[0] == 0 ||
		cmdParam.width == 0 ||
		cmdParam.height == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int main(int argc, char** argv)
{
	FILE *yuvFile = NULL;
	long yuvFileSize = 0;
	int yuvFileRes = 0;
	int bytesPerFrame = 0;
	int frameCount = 0;

	if (ParseCommandLine(argc, argv) == false)
		return -1;

	yuvFile = fopen(cmdParam.path, "r");
	fseek(yuvFile, 0, SEEK_END);
	yuvFileSize = ftell(yuvFile);
	fclose(yuvFile);

	yuvFileRes = cmdParam.height * cmdParam.width;
	bytesPerFrame = yuvFileRes + (yuvFileRes >> 1);
	frameCount = yuvFileSize / bytesPerFrame;

	printf("File  : %s \n", cmdParam.path);
	printf("Width : %d pixel \n", cmdParam.width);
	printf("Height: %d pixel \n", cmdParam.height);
	printf("Size  : %ld bytes \n", yuvFileSize);
	printf("Frame : %d frames \n", frameCount);
	printf("Bytes Per Frame : %d bytes \n", bytesPerFrame);

	return 0;
}
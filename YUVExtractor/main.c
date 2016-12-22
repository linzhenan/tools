#include "stdio.h"

#define MAX_OPTIONS 5
#define MAX_READ_FRAME_SIZE 10000

int validate_params(int frames, int seeks, int options, int yuv_width, int yuv_height)
{
	if (frames < 1)
	{
		return -1;
	}
	if (seeks < 0)
	{
		return -1;
	}
	if (options < 0 || options > MAX_OPTIONS)
	{
		return -1;
	}
	if (yuv_height < 1 || yuv_height < 1)
	{
		return -1;
	}
	return 1;
}

int main(int argc, char* argv[])
{
	FILE *fins, *fouts;
	char *infilename, *outfilename;
	char frame_data[MAX_READ_FRAME_SIZE];
	int frames = 0;
	int seeks = 0;
	int option = 0;
	int yuv_width = 0, yuv_height = 0, frame_size = 0;
	int file_length = 0;
	int i, j;
	int tract_times = 0;
	int vestigital = 0;

	infilename = outfilename = NULL;

	argc--; argv++;
	while (argc > 0)
	{
		if ('-' == argv[0][0])
		{
			if ('h' == argv[0][1] || strcmp(argv[0] + 2, "help") == 0)
			{
				printf("Usage : YUVExtractor -i input.yuv -o output.yuv --frames int --seek int --option int\n");
				return 0;
			}
			else if ('i' == argv[0][1] || strcmp(argv[0] + 2, "input") == 0)
			{
				infilename = argv[1];
				argc--; argv++;
			}
			else if ('o' == argv[0][1] || strcmp(argv[0] + 2, "output") == 0)
			{
				outfilename = argv[1];
				argc--; argv++;
			}
			else if ('s' == argv[0][1] || strcmp(argv[0] + 2, "size") == 0)
			{
				if (sscanf(argv[1], "%dx%d", &yuv_width, &yuv_height) != 2 || yuv_width < 1 || yuv_height < 1) {
					fprintf(stderr, "invalid picture size '%s'!\n", argv[1]);
					return 0;
				}
				argc--; argv++;
			}
			else if ('w' == argv[0][1] || strcmp(argv[0] + 2, "width") == 0)
			{
				if (sscanf(argv[1], "%d", &yuv_width) != 1 || yuv_width < 1) {
					fprintf(stderr, "invalid width parameters '%s'!\n", argv[1]);
					return 0;
				}
				argc--; argv++;
			}
			else if ('h' == argv[0][1] || strcmp(argv[0] + 2, "height") == 0)
			{
				if (sscanf(argv[1], "%d", &yuv_height) != 1 || yuv_height < 1) {
					fprintf(stderr, "invalid height parameters '%s'!\n", argv[1]);
					return 0;
				}
				argc--; argv++;
			}
			else if (strcmp(argv[0], "--frames") == 0)
			{
				if (sscanf(argv[1], "%d", &frames) != 1 || frames < 1) {
					fprintf(stderr, "invalid splitting frames parameters '%s'!\n", argv[1]);
					return 0;
				}
				argc--; argv++;
			}
			else if (strcmp(argv[0], "--seek") == 0)
			{
				if (sscanf(argv[1], "%d", &seeks) != 1 || seeks < 0) {
					fprintf(stderr, "invalid seek frames parameter '%s'!\n", argv[1]);
					return 0;
				}
				argc--; argv++;
			}
			else if (strcmp(argv[0], "--options") == 0)
			{
				if (sscanf(argv[1], "%d", &option) != 1 || option < 0) {
					fprintf(stderr, "invalid option parameter '%s'!\n", argv[1]);
					return 0;
				}
				argc--; argv++;
			}
			else
			{
				printf("Invalid Parameters : %s.\n", argv[0]);
				printf("Usage : YUVExtractor -i input.yuv -o output.yuv --frames int --seek int --option int\n");
				return 0;
			}
		}
		else {
			printf("Invalid Parameters : %s.\n", argv[0]);
			printf("Usage : YUVExtractor -i input.yuv -o output.yuv -s widthxheight --frames int --seek int --option int\n");
			return 0;
		}
		argc--; argv++;
	}

	//validate the params
	if (validate_params(frames, seeks, option, yuv_width, yuv_height) < 0)
	{
		printf("Invalid Parameters.\n");
		printf("Usage : YUVExtractor -i input.yuv -o output.yuv -s widthxheight --frames int --seek int --option int\n");
		return 0;
	}

	//validate input/output file names
	fins = fopen(infilename, "rb");
	fouts = fopen(outfilename, "wb");
	if (fins == NULL || fouts == NULL)
	{
		printf("Invalid input/output file names.");
		return 0;
	}

	//seek specified frames
	frame_size = yuv_width * yuv_height * 3 / 2;
	fseek(fins, 0L, SEEK_END);
	file_length = ftell(fins) / (frame_size);
	if (file_length < seeks)
	{
		printf("Invalid seek parameter : seek frames is out of the file.\n");
		return 0;
	}
	else if (file_length < seeks + frames)
	{
		printf("Invalid frames parameter : out of the input file.\n");
		frames = file_length - seeks;
		fseek(fins, seeks * frame_size, SEEK_SET);
	}
	else
	{
		fseek(fins, seeks * frame_size, SEEK_SET);
	}

	//extracting frames
	tract_times = (frame_size * frames) / MAX_READ_FRAME_SIZE;
	vestigital = (frame_size * frames) % MAX_READ_FRAME_SIZE;
	for (i = 0; i < tract_times; i++)
	{
		fread(frame_data, sizeof(char), MAX_READ_FRAME_SIZE, fins);
		fwrite(frame_data, sizeof(char), MAX_READ_FRAME_SIZE, fouts);
	}
	if (vestigital != 0)
	{
		fread(frame_data, sizeof(char), vestigital, fins);
		fwrite(frame_data, sizeof(char), vestigital, fouts);
	}

	fclose(fins);
	fclose(fouts);

	return 0;
}
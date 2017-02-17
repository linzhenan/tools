#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <inttypes.h>

#pragma warning(disable:4996)

#define MAX_PATH_LEN 1024

#define _fseeki64 fseeko
#define _ftelli64 ftello

typedef struct InputFileContext
{
	FILE *file;
	char filename[MAX_PATH_LEN + 1];
	int64_t filesize;

	int frame_cnt;
	int start_frame_id;//one-based
	int end_frame_id;//one-based
}
InFileContext;

typedef struct ParameterContext
{
	InFileContext org;
	InFileContext rec;

	int width;
	int height;
	int fps;

	int frame_cmp;
}
ParamContext;

typedef enum CompnentIndexEnum
{
	Y_Cg = 0,
	U_Cr,
	V_Cb,
	MAX_COMPO_NUM,
} CompIdxEnum;

typedef struct StatisticsContext
{
	double psnr[MAX_COMPO_NUM], psnrGlobal;
	uint64_t ssd[MAX_COMPO_NUM];
	int frameCnt;
}
StatContext;

typedef struct FrameBufferContext
{
	uint8_t *org[MAX_COMPO_NUM];
	uint8_t *rec[MAX_COMPO_NUM];
}
BufContext;

#define OK 0
#define INVALID_PARAMETER 1
#define ALLOC_ERROR 2
#define OPEN_FILE_ERROR 3
#define READ_FILE_ERROR 4

#define MALLOC(p, size) do { if (p) free(p); p = NULL; p = malloc(size); if (p) memset(p, 0, size); } while (0)
#define FREE(p) do { if (p) free(p); p = NULL; } while (0)
#define FOPEN(filename) fopen(filename, "rb")
#define FCLOSE(file) do { if (file) fclose(file); file = NULL; } while (0)

static int open_file(InFileContext *pInFile, char *tag, int width, int height)
{
	assert(pInFile);

	printf("%-6s YUV file: %s\n", tag, pInFile->filename);
	printf("Open   YUV file: ");
	pInFile->file = FOPEN(pInFile->filename);
	if (pInFile->file == NULL)
	{
		printf("open file error!\n");
		return OPEN_FILE_ERROR;
	}
	_fseeki64(pInFile->file, 0, SEEK_END);
	pInFile->filesize = _ftelli64(pInFile->file);
	_fseeki64(pInFile->file, 0, SEEK_SET);
	if (pInFile->filesize % (width * height * 3 / 2) != 0)
	{
		printf("seems damaged!\n");
		FCLOSE(pInFile->file);
		return OPEN_FILE_ERROR;
	}
	pInFile->frame_cnt = (int)(pInFile->filesize / (width * height * 3 / 2));
	printf("%"PRIi64"B, %d frames.\n", pInFile->filesize, pInFile->frame_cnt);

	return OK;
}
static int close_file(InFileContext *pInFile, char *tag)
{
	//compilable with Lentoid 3.0 log format

	//printf("%-6s YUV file: %s\n", tag, pInFile->filename);
	//printf("Close  YUV file: ");
	FCLOSE(pInFile->file);
	//printf("closed.\n");

	return OK;
}
static int parse_command_line(int argc, char **argv, ParamContext *pParam)
{
	int i;

	for (i = 0; i < 40; i++) printf("=");
	printf(" YUV420 PSNR 8-bit ");
	for (i = 0; i < 40; i++) printf("=");
	printf("\n");
	printf("Copyright  2015: Zhenan Lin, zhenanlin@pku.edu.cn\n");
	printf("Usage          : YUVpsnr \n");
	printf("                 -w <width> -h <height> [-f <fps>] \n");
	printf("                 <YUVorg> [-s <start_frame_one_based_index> -e <end_frame_one_based_index>] \n");
	printf("                 <YUVrec> [-s <start_frame_one_based_index> -e <end_frame_one_based_index>] \n");
	printf("\n");

	printf("CommandLine    : ");
	for (i = 1; i < argc; i++)
		printf("%s ", argv[i]);
	printf("\n");
	printf("\n");

	if (!pParam)
		return INVALID_PARAMETER;
	memset(pParam, 0, sizeof(ParamContext));

	//options
	for (i = 1; i < argc;)
	{
		if (strcmp(argv[i], "-w") == 0)
		{
			i++;
			if (argv[i] == NULL)
			{
				printf("Option -w no argument! \n");
				return INVALID_PARAMETER;
			}
			else
			{
				pParam->width = atoi(argv[i]);
				if (pParam->width <= 0)
				{
					printf("Option -w invalid argument! \n");
					return INVALID_PARAMETER;
				}
				i++;
			}
			continue;
		}
		else if (strcmp(argv[i], "-h") == 0)
		{
			i++;
			if (argv[i] == NULL)
			{
				printf("Option -h no argument! \n");
				return INVALID_PARAMETER;
			}
			else
			{
				pParam->height = atoi(argv[i]);
				if (pParam->height <= 0)
				{
					printf("Option -h invalid argument! \n");
					return INVALID_PARAMETER;
				}
				i++;
			}
			continue;
		}
		else if (strcmp(argv[i], "-f") == 0)
		{
			i++;
			if (argv[i] == NULL)
			{
				printf("Option -f no argument! \n");
				return INVALID_PARAMETER;
			}
			else
			{
				pParam->fps = atoi(argv[i]);
				if (pParam->fps <= 0)
				{
					printf("Option -f invalid argument! \n");
					return INVALID_PARAMETER;
				}
				i++;
			}
			continue;
		}

		break;
	}

	//org
	if (i < argc)
	{
		if (strlen(argv[i]) >= MAX_PATH_LEN)
		{
			printf("Origin YUV filename length exceeds %d characters!\n", MAX_PATH_LEN);
			return INVALID_PARAMETER;
		}
		else
		{
			strcpy(pParam->org.filename, argv[i]);
			i++;
		}

		for (; i < argc;)
		{
			if (strcmp(argv[i], "-s") == 0)
			{
				i++;
				if (argv[i] == NULL)
				{
					printf("Option -s no argument! \n");
					return INVALID_PARAMETER;
				}
				else
				{
					pParam->org.start_frame_id = atoi(argv[i]);
					if (pParam->org.start_frame_id <= 0)
					{
						printf("Option -s invalid argument! \n");
						return INVALID_PARAMETER;
					}
					i++;
				}
				continue;
			}
			else if (strcmp(argv[i], "-e") == 0)
			{
				i++;
				if (argv[i] == NULL)
				{
					printf("Option -e no argument! \n");
					return INVALID_PARAMETER;
				}
				else
				{
					pParam->org.end_frame_id = atoi(argv[i]);
					if (pParam->org.end_frame_id <= 0)
					{
						printf("Option -e invalid argument! \n");
						return INVALID_PARAMETER;
					}
					i++;
				}
				continue;
			}
			break;
		}
	}
	else
	{
		printf("Origin YUV file not found! \n");
		return INVALID_PARAMETER;
	}

	//rec
	if (i < argc)
	{
		if (strlen(argv[i]) >= MAX_PATH_LEN)
		{
			printf("Recon YUV filename length exceeds %d characters!\n", MAX_PATH_LEN);
			return INVALID_PARAMETER;
		}
		else
		{
			strcpy(pParam->rec.filename, argv[i]);
			i++;
		}

		for (; i < argc;)
		{
			if (strcmp(argv[i], "-s") == 0)
			{
				i++;
				if (argv[i] == NULL)
				{
					printf("Option -s no argument! \n");
					return INVALID_PARAMETER;
				}
				else
				{
					pParam->rec.start_frame_id = atoi(argv[i]);
					if (pParam->rec.start_frame_id <= 0)
					{
						printf("Option -s invalid argument! \n");
						return INVALID_PARAMETER;
					}
					i++;
				}
				continue;
			}
			else if (strcmp(argv[i], "-e") == 0)
			{
				i++;
				if (argv[i] == NULL)
				{
					printf("Option -e no argument! \n");
					return INVALID_PARAMETER;
				}
				else
				{
					pParam->rec.end_frame_id = atoi(argv[i]);
					if (pParam->rec.end_frame_id <= 0)
					{
						printf("Option -e invalid argument! \n");
						return INVALID_PARAMETER;
					}
					i++;
				}
				continue;
			}
			break;
		}
	}
	else
	{
		printf("Recon YUV file not found! \n");
		return INVALID_PARAMETER;
	}

	//ori
	if (open_file(&pParam->org, "Origin", pParam->width, pParam->height) != OK)
		return OPEN_FILE_ERROR;
	printf("Origin YUV info: %d frames, %d x %d @ %d Hz \n", pParam->org.frame_cnt, pParam->width, pParam->height, pParam->fps);
	if (pParam->org.start_frame_id == 0)
		pParam->org.start_frame_id = 1;
	if (pParam->org.end_frame_id == 0)
		pParam->org.end_frame_id = pParam->org.frame_cnt;
	printf("Origin YUV seek: %d - %d, ", pParam->org.start_frame_id, pParam->org.end_frame_id);
	if (pParam->org.start_frame_id > pParam->org.end_frame_id ||
		pParam->org.start_frame_id > pParam->org.frame_cnt ||
		pParam->org.end_frame_id > pParam->org.frame_cnt)
	{
		printf("invalid.\n");
		return INVALID_PARAMETER;
	}
	else
	{
		printf("valid.\n");
	}
	printf("\n");

	//rec
	if (open_file(&pParam->rec, "Recon", pParam->width, pParam->height) != OK)
		return OPEN_FILE_ERROR;
	printf("Recon  YUV info: %d frames, %d x %d @ %d Hz \n", pParam->rec.frame_cnt, pParam->width, pParam->height, pParam->fps);
	if (pParam->rec.start_frame_id == 0)
		pParam->rec.start_frame_id = 1;
	if (pParam->rec.end_frame_id == 0)
		pParam->rec.end_frame_id = pParam->rec.frame_cnt;
	printf("Recon  YUV seek: %d - %d, ", pParam->rec.start_frame_id, pParam->rec.end_frame_id);
	if (pParam->rec.start_frame_id > pParam->rec.end_frame_id ||
		pParam->rec.start_frame_id > pParam->rec.frame_cnt ||
		pParam->rec.end_frame_id > pParam->rec.frame_cnt)
	{
		printf("invalid.\n");
		return INVALID_PARAMETER;
	}
	else
	{
		printf("valid.\n");
	}
	printf("\n");

	//org ~ rec
	if (pParam->org.end_frame_id - pParam->org.start_frame_id + 1 !=
		pParam->rec.end_frame_id - pParam->rec.start_frame_id + 1)
	{
		printf("The length of both seek ranges should be equal.\n");
		return INVALID_PARAMETER;
	}
	pParam->frame_cmp = pParam->org.end_frame_id - pParam->org.start_frame_id + 1;
	fseek(pParam->org.file, (pParam->org.start_frame_id - 1) * pParam->width * pParam->height * 3 / 2, SEEK_SET);
	fseek(pParam->rec.file, (pParam->rec.start_frame_id - 1) * pParam->width * pParam->height * 3 / 2, SEEK_SET);

	return OK;
}

static int alloc_buffer(ParamContext *pParam, uint8_t *pix[MAX_COMPO_NUM])
{
	MALLOC(pix[Y_Cg], sizeof(uint8_t) *  pParam->width       *  pParam->height);
	MALLOC(pix[U_Cr], sizeof(uint8_t) * (pParam->width >> 1) * (pParam->height >> 1));
	MALLOC(pix[V_Cb], sizeof(uint8_t) * (pParam->width >> 1) * (pParam->height >> 1));

	if (!pix[Y_Cg] || !pix[U_Cr] || !pix[V_Cb])
	{
		FREE(pix[Y_Cg]);
		FREE(pix[U_Cr]);
		FREE(pix[V_Cb]);
		return ALLOC_ERROR;
	}
	else
	{
		return OK;
	}
}
static int free_buffer(uint8_t *pix[MAX_COMPO_NUM])
{
	FREE(pix[Y_Cg]);
	FREE(pix[U_Cr]);
	FREE(pix[V_Cb]);
	return OK;
}
static int flush_buffer(uint8_t *pix, FILE *file, int width, int height)
{
	int size = width * height;

	if (fread(pix, sizeof(uint8_t), size, file) != size)
		return READ_FILE_ERROR;
	else
		return OK;
}

int main(int argc, char **argv)
{
	ParamContext param = { 0 };
	StatContext stat = { 0 };
	BufContext buf = { 0 };
	int width, height;
	int i, j, x, y;

	system("mode con cols=1024 lines=768");

	if (parse_command_line(argc, argv, &param) != OK)
		return INVALID_PARAMETER;
	if (alloc_buffer(&param, buf.org) != OK ||
		alloc_buffer(&param, buf.rec) != OK)
	{
		free_buffer(buf.org);
		free_buffer(buf.rec);
		return ALLOC_ERROR;
	}

	stat.frameCnt = param.frame_cmp;
	for (i = 0; i < param.frame_cmp; i++)
	{
		StatContext statFrame = { 0 };
		statFrame.frameCnt = 1;
		for (j = Y_Cg; j <= V_Cb; j++)
		{
			uint8_t *org, *rec;
			if (j == Y_Cg)
			{
				width = param.width;
				height = param.height;
			}
			else
			{
				width = param.width >> 1;
				height = param.height >> 1;
			}
			flush_buffer(buf.org[j], param.org.file, width, height);
			flush_buffer(buf.rec[j], param.rec.file, width, height);
			org = buf.org[j];
			rec = buf.rec[j];
			for (y = 0; y < height; y++)
			{
				for (x = 0; x < width; x++)
				{
					int diff = org[x] - rec[x];
					statFrame.ssd[j] += diff * diff;
				}
				org += width;
				rec += width;
			}
			if (statFrame.ssd[j] != 0)
			{
				double tmp = 255.0;
				tmp = tmp * tmp * width * height;
				statFrame.psnr[j] = (10.0 * log10(tmp / (double)statFrame.ssd[j]));
			}
			else
			{
				statFrame.psnr[j] = 99.99;
			}
			stat.psnr[j] += statFrame.psnr[j];
		}
		statFrame.psnrGlobal = (statFrame.psnr[Y_Cg] * 6 + statFrame.psnr[U_Cr] + statFrame.psnr[V_Cb]) / 8;
		stat.psnrGlobal += statFrame.psnrGlobal;

		printf("[ %3d / %3d ~ %3d / %3d] %.4lf : %.4lf %.4lf %.4lf\n",
			param.org.start_frame_id + i, param.org.frame_cnt,
			param.rec.start_frame_id + i, param.rec.frame_cnt,
			statFrame.psnrGlobal,
			statFrame.psnr[Y_Cg], statFrame.psnr[U_Cr], statFrame.psnr[V_Cb]);
	}

	printf("average:  %.4lf  %.4lf  %.4lf    Global:  %.4lf\n",
		stat.psnr[Y_Cg] / stat.frameCnt,
		stat.psnr[U_Cr] / stat.frameCnt,
		stat.psnr[V_Cb] / stat.frameCnt,
		stat.psnrGlobal / stat.frameCnt);

	printf("\n");
	free_buffer(buf.org);
	free_buffer(buf.rec);
	close_file(&param.org, "Origin");
	close_file(&param.rec, "Recon");

	return OK;
}

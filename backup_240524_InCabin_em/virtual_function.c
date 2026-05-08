


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "oddemo_common.h"


#define MAX 17
#define MAX_LINES 20

extern ODDEMO_Parms			oddemo_parms;

char* moving_avg_buf[16];
char* buf[17];
char file_path[8][26] = {"power_history_8by16_1.txt", "power_history_8by16_2.txt",
						 "power_history_8by16_3.txt", "power_history_8by16_4.txt",
						 "power_history_8by16_5.txt", "power_history_8by16_6.txt",
						 "power_history_8by16_7.txt", "power_history_8by16_8.txt"	};

errno_t strcpy_s(char *dest, rsize_t dest_size, const char *src);

void history_read(uint16_t zone_idx)
{
	FILE *pFile = NULL;
	char* ap;
	uint16_t i = 0;
	int j = 0;

	/*
	fopen_s(&pFile, file_path[zone_idx], "rt"); // FILE *pFile = fopen(file_path[zone_idx], "rt");

	while(!feof(pFile)){
		ap = (char*)malloc(sizeof(char) * MAX);

		fgets(ap, MAX, pFile);
		buf[i] = ap;
		i++;
	}
	fclose(pFile);
	*/
	if(fopen_s(&pFile, file_path[zone_idx], "rt") == 0)
	{
		char temp[MAX];

		while(fgets(temp, MAX, pFile) != NULL && i < MAX_LINES) {
			ap = (char*)malloc(sizeof(char) * MAX);
			if(ap != NULL)
			{
				strcpy_s(ap, MAX, temp);
				buf[i] = ap;
				i++;
			}
		}
		fclose(pFile);
	}

	for (j = 0; j < i; j++) 
	{
		free(buf[j]);
	}
}


void history_save(uint16_t zone_idx, float data)
{
	FILE *pFile = fopen(file_path[zone_idx], "w");
	uint16_t i = 1;

	while(i < oddemo_parms.windowLen){
		fprintf(pFile, "%s", buf[i]);
		i++;
	}
	fprintf(pFile, "%f\n", data);
	fclose(pFile);
}


/*
void calc_moving_avg(float data)
{
#if 0
	FILE* pF_r = fopen("power_history_8by16.txt", "rt");
	FILE* pF_w = fopen("power_history_8by16.txt", "w");
	char* ap;

	for (uint16_t i = 0; i < 16; i++)
	{
		ap = (char*)malloc(sizeof(char) * MAX);
		fgets(ap, MAX, pF_r);
		moving_avg_buf[i] = ap;
	}
	fclose(pF_r);

	fprintf(pF_w, "%f\n", data);

	for(uint16_t j = 0; j < 15; j++)
	{
		fprintf(pF_w, "%s", moving_avg_buf[j]);
	}
	fclose(pF_w);
#else
	FILE* pFile = fopen("power_history_8by16.txt", "a");
	char* ap;

	ap = (char*)malloc(sizeof(char) * MAX);
	sprintf(ap, "%f", data);

	fputs(ap, pFile);
	fclose(pFile);
#endif
} */

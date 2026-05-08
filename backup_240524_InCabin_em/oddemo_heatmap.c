

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "oddemo_common.h"
#include "oddemo_heatmap.h"

//global variables
uint16_t oddemo_row_init = 0;
float    oddemo_heatmap_max;
float    oddemo_row_noise[ODDEMO_MAX_RANGE];
ODDemo_Heatmap_Stats oddemo_stats[ODDEMO_MAX_RANGE];

extern ODDEMO_Parms   oddemo_parms;


/* -----------------------------------------------------------------------------------
 * This function captures min, max and average for each heatmap row. It currently also
 * creates average empty FOV power during the first several seconds only if "rowNoise"
 * CLI messages have not been received.
 * -----------------------------------------------------------------------------------*/
void ODDemo_Heatmap_get_stats(float *heatmap)
{
	uint16_t  rng_idx, az_idx;
	float     min, max, sum;
	float    *heat = heatmap;

	oddemo_heatmap_max = 0.0;

	for (rng_idx = 0; rng_idx < ODDEMO_MAX_RANGE; rng_idx++)
	{
		min = 999999.9;
		max = 0.0;
		sum = 0.0;

		//Find the min and max of all cells on this row
		for (az_idx = 0; az_idx < ODDEMO_MAX_AZIMUTH; az_idx++)
		{
	        if (heat[az_idx] > max)
				max = heat[az_idx];

	        if (heat[az_idx] < min)
				min = heat[az_idx];
	    
	        sum += heat[az_idx];
		}

		if (max > oddemo_heatmap_max) //keep the overall heatmap maximum
			oddemo_heatmap_max = max;

		oddemo_stats[rng_idx].min = min;
		oddemo_stats[rng_idx].max = max;
		oddemo_stats[rng_idx].avg = sum / ODDEMO_MAX_AZIMUTH;

		heat += ODDEMO_MAX_AZIMUTH; //point to the next row
	}

	//Do startup row calibration. If oddemo_row_init is greater than this value
	//then we received the values from CLI and do not need to do it again.
	if (oddemo_row_init < ODDEMO_ROW_NOISE_FRAMES)
	{
		if (oddemo_row_init == 0)
			memset(oddemo_row_noise, 0, sizeof(oddemo_row_noise));

		if (oddemo_row_init >= 4) //ignore the first 4 frames
		{
			for (rng_idx = 0; rng_idx < ODDEMO_MAX_RANGE; rng_idx++)
				oddemo_row_noise[rng_idx] += oddemo_stats[rng_idx].avg;
		}

		oddemo_row_init ++;

		if (oddemo_row_init == ODDEMO_ROW_NOISE_FRAMES) //done collecting data
		{
			for (rng_idx = 0; rng_idx < ODDEMO_MAX_RANGE; rng_idx++)
				oddemo_row_noise[rng_idx] /= (ODDEMO_ROW_NOISE_FRAMES - 4.0); // Equivalent code : oddemo_row_noise[rng_idx] = oddemo_row_noise[rng_idx] / (ODDEMO_ROW_NOISE_FRAMES - 4.0);
		}
	}
}


/* -----------------------------------------------------------------------------------
 * This function scans the heatmap row by row looking for cases where the minimum value
 * is greater than the saved noise floor for the row.   
 * If true, the minimum is subtracted from the row and then rescaled to restore 
 * the peak maximum to its original value.
 * -----------------------------------------------------------------------------------*/
void ODDemo_Heatmap_arc_removal(float *heatmap)
{
	uint16_t  rng_idx, az_idx;
	float min;
	float row_range, factor;
	float temp, noise_level;

	float *heat = heatmap;

	ODDemo_Heatmap_get_stats(heatmap);

	for (rng_idx = 0; rng_idx < ODDEMO_MAX_RANGE; rng_idx++)
	{
		noise_level = oddemo_row_noise[rng_idx];
		min = oddemo_stats[rng_idx].min;

		if (min > noise_level)
		{
			row_range = oddemo_stats[rng_idx].max - min;
			factor    = 1.0 + ((min - noise_level) / row_range);

			//This loop will expand the range of values on this row so that the min value is
			//moved to the noise floor, and the max of any peak is kept at the same value.
			for (az_idx = 0; az_idx < ODDEMO_MAX_AZIMUTH; az_idx++)
			{
				temp = (heat[az_idx] - min) * factor;
				heat[az_idx] = temp + noise_level;
			}
		}
		heat += ODDEMO_MAX_AZIMUTH; //point to the next row
	}
}

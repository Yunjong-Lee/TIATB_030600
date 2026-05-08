

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "oddemo_common.h"
#include "oddemo_feature.h"
#include "oddemo_decision.h"

#include "virtual_function.h"



//This is the normal amount of power in an empty zone
#define ODDEMO_POWER_INIT_VAL          48.0

//Feature extraction global variables
ODDEMO_Feature			oddemo_feature[ODDEMO_ZONE_PAIR];
ODDEMO_Zone				oddemo_zone[ODDEMO_MAX_ZONES];
extern ODDEMO_Parms		oddemo_parms;
extern uint16_t			oddemo_num_zones;
extern uint16_t			oddemo_zone_pairs;
extern float			oddemo_meanVec[ODDEMO_ZONE_PAIR][ODDEMO_MATRIX_ROW_SIZE-1];
extern float			oddemo_stdVec[ODDEMO_ZONE_PAIR][ODDEMO_MATRIX_ROW_SIZE-1];
extern float       		oddemo_coeffMatrix[ODDEMO_ZONE_PAIR][ODDEMO_MATRIX_SIZE];

uint16_t				ph_idx;
float					power_history[ODDEMO_MAX_ZONES][ODDEMO_MAX_FRAME_HIST];
float					power_historydB[ODDEMO_MAX_ZONES][ODDEMO_MAX_FRAME_HIST];

extern ODDEMO_Decision	oddemo_decision[ODDEMO_MAX_ZONES];


float DSPF_sp_vecvar(const float * x, const int n)
{
    int i;
    float sum = 0, avg = 0, var = 0;

	// _nassert(n > 0);
	// _nassert((int)x % 8 == 0);

    /* Unroll by 8 because the loop carried dependency bound is 4 */
    /* The compiler does not do staggered adds                    */
    // #pragma UNROLL(4)
    for(i = 0; i < n; i++) 
	{
        sum += x[i];
	}

    avg = sum/n;

    /* Unroll by 8 because the loop carried dependency bound is 4 */
    /* The compiler does not do staggered adds                    */
    // #pragma UNROLL(4)
    for(i = 0; i < n; i++) 
	{
        var += (x[i]-avg) * (x[i]-avg);
	}
	
    return (var / (n - 1));
}


float DSPF_sp_vecavg(float * x, int n)
{
    int i;
    float sum = 0;

	//    _nassert(n > 0);
	//    _nassert((int)x % 8 == 0);

    /* Unroll by 8 because the loop carried dependency bound is 4 */
    /* The compiler does not do staggered adds                    */
    // #pragma UNROLL(4)
    for(i = 0; i < n; i++) 
	{
        sum += x[i];
	}

    sum = sum/n;

    return sum;
}


/* Inverse rsqrt, ref(https://www.geeksforgeeks.org/fast-inverse-square-root/) */
float inverse_rsqrt(float number)
{
	const float threehalfs = 1.5F;
 
	float x2 = number * 0.5F;
	float y = number;
  
	// evil floating point bit level hacking
	long i = * ( long * ) &y;
  
	// value is pre-assumed
	i = 0x5f3759df - ( i >> 1 );
	y = * ( float * ) &i;
  
	// 1st iteration
	y = y * ( threehalfs - ( x2 * y * y ) );
  
	// 2nd iteration, this can be removed
	// y = y * ( threehalfs - ( x2 * y * y ) );
 
	return y;
}


float DSPF_sp_sqrtsp (float a)
{
	const float  Half  = 0.5f;
	const float  OneP5 = 1.5f;
	float  x, y;
	int i;

	x = inverse_rsqrt(a); // sqrt1(a); // _rsqrsp(a);

	// #pragma UNROLL(1)
	for(i=0; i< 2; i++) {
		x = x * (OneP5 - (a * x * x * Half));
	}
	y = a * x;

	if (a <= 0.0f) 
		y = 0.0f;

	if (a > FLT_MAX)
		y = FLT_MAX;

	return (y);
}


float DSPF_sp_crosscorr(const float *x, const float *y, const float sqRtVarx, const float sqRtVary, const int n)
{
    int   i;
    float tmp;
    float sumx = 0, sumy = 0, avgx = 0, avgy = 0, crosscor = 0;

	//    _nassert(n > 0);
	//    _nassert((int)x % 8 == 0);

    /* Unroll by 8 because the loop carried dependency bound is 4 */
    /* The compiler does not do staggered adds                    */
    // #pragma UNROLL(4)
    for (i = 0; i < n; i++) {
        sumx += x[i];
        sumy += y[i];
    }
    avgx = sumx/n;
    avgy = sumy/n;

    /* Unroll by 8 because the loop carried dependency bound is 4 */
    /* The compiler does not do staggered adds                    */
    // #pragma UNROLL(4)
    for (i = 0; i < n; i++) {
        crosscor += (x[i]-avgx) * (y[i]-avgy);
	}

    tmp = sqRtVarx * sqRtVary * (n - 1);

    crosscor = crosscor / tmp;

    return crosscor;
}


/*
void ODDemo_Feature_extract_addOD(uint16_t zone_pair, float *azimuthHeatMap, ODDEMO_Feature *feature)
{
	uint16_t idx, az_idx, rng_idx;
    uint16_t max_az, max_rng, offset;
    uint16_t zone;

	int		cnt = 0;

	float	*heat;
	float	sum, size, var, avgPower, crossCorr;
	float	max, tmp;
	float	powerMA[2], powerMAdB[2], powRatio[2], powRatiodB[2],	sqRootVar[2];

	float	full_pwrMA[2], full_pwrMAdB[2], full_powRatio[2], full_powRatiodB[2],	full_sqRootVar[2];
	float	full_sum, full_size, full_avgPower;
	float	*full_heat;

	zone = zone_pair * 2;
	for (idx = 0; idx < 2; idx ++) {
		// full avgPower
        full_sum  = 0;
        full_size = (float)(oddemo_zone[zone].range_length * oddemo_zone[zone].azimuth_width);

        for (rng_idx = 0; rng_idx < oddemo_zone[zone].range_length; rng_idx++) {
            // point to the correct range row, then add the starting azimuth index
            full_heat  = azimuthHeatMap + (ODDEMO_MAX_AZIMUTH * (oddemo_zone[zone].range_start + rng_idx));
            full_heat += oddemo_zone[zone].azimuth_start;

            for (az_idx = 0; az_idx < oddemo_zone[zone].azimuth_width; az_idx++) {
                full_sum += *full_heat++;
			}
        }

        full_avgPower = full_sum / full_size;

		// 5 by 5 cell power
		max = 0;
		heat = azimuthHeatMap + (ODDEMO_MAX_AZIMUTH * oddemo_zone[zone].range_start);
		heat += oddemo_zone[zone].azimuth_start;

		for (rng_idx = 0; rng_idx < oddemo_zone[zone].range_length-2; rng_idx++) {
			for (az_idx = 0; az_idx < oddemo_zone[zone].azimuth_width-2; az_idx++) {
				offset = az_idx;
				tmp  = heat[offset] + heat[offset+1] + heat[offset+2];
				offset += ODDEMO_MAX_AZIMUTH;
				tmp += heat[offset] + heat[offset+1] + heat[offset+2];
				offset += ODDEMO_MAX_AZIMUTH;
				tmp += heat[offset] + heat[offset+1] + heat[offset+2];

				if (tmp > max)
				{
					max = tmp;
					max_rng = rng_idx; //center position is actually +1, +1
					max_az  = az_idx;
				}
			}
			heat += ODDEMO_MAX_AZIMUTH;
		}

        // Save the max positions for output.
        max_rng += oddemo_zone[zone].range_start + 1;
        max_az  += oddemo_zone[zone].azimuth_start + 1;
        oddemo_zone[zone].range_max_idx = max_rng;
        oddemo_zone[zone].azimuth_max_idx = max_az;

        sum   = 0;
        heat  = azimuthHeatMap + (ODDEMO_MAX_AZIMUTH * (max_rng-2));
        heat += max_az - 2;

        //compute average power on a 5x5 centered on the max 3x3
        for (rng_idx = 0; rng_idx < 5; rng_idx++) {
            for (az_idx = 0; az_idx < 5; az_idx++) {
                sum += heat[az_idx];
            }
            heat += ODDEMO_MAX_AZIMUTH;
        }
        avgPower = sum / 25; //fixed 25 cell size

		// (Step 2) store new average power, overwriting the oldest value in the history buffer
        full_pwr_history[zone][ph_idx] = full_avgPower;
        full_pwr_historydB[zone][ph_idx] = 10 * log10(full_avgPower);

        // (Step 3) Moving Average of previous average power values
        full_pwrMA[idx] = DSPF_sp_vecavg(&full_pwr_history[zone][0], oddemo_parms.windowLen);
        full_pwrMAdB[idx] = 10 * log10(full_pwrMA[idx]);

        //this value is for output only:
        if (full_pwrMA[idx] > 999999.9)
          oddemo_decision[zone].full_avgPwr = 999999.9;
        else if (full_pwrMA[idx] < 0.0)
          oddemo_decision[zone].full_avgPwr = 0.0;
        else
          oddemo_decision[zone].full_avgPwr = full_pwrMA[idx];

        // (Step 2) store new average power, overwriting the oldest value in the history buffer
        power_history[zone][ph_idx] = avgPower;
        power_historydB[zone][ph_idx] = 10 * log10(avgPower);

        // (Step 3) Moving Average of previous average power values
        powerMA[ph_idx] = DSPF_sp_vecavg(&power_history[zone][0], oddemo_parms.windowLen);
        powerMAdB[ph_idx] = 10 * log10(powerMA[idx]);

        //this value is for output only:
        if (powerMA[idx] > 999999.9)
          oddemo_decision[zone].avgPower = 999999.9;
        else if (powerMA[idx] < 0.0)
          oddemo_decision[zone].avgPower = 0.0;
        else
          oddemo_decision[zone].avgPower = powerMA[idx];

        // (Step4) Cross Correlation between the zones
        // (Step4a) Compute Variance for each zone

        var = DSPF_sp_vecvar(&power_historydB[zone][0], oddemo_parms.windowLen);

        // (Step4b) Compute Square Root of Variance
        sqRootVar[idx] = DSPF_sp_sqrtsp(var);
        zone ++;
	}
	zone = zone_pair * 2;

	ph_idx ++;

	if(ph_idx == oddemo_parms.windowLen)
		ph_idx = 0;

	// (Step 5) Compute Cross Correlation Coefficient
	crossCorr = DSPF_sp_crosscorr(&power_historydB[zone][0], &power_historydB[zone+1][0], sqRootVar[0], sqRootVar[1], oddemo_parms.windowLen);
	
	printf("Cross Corr Coeff = %f\n", crossCorr);

    // (Step 6) Compute Power Ratios
    sum = 0.0;
    for (idx = 0; idx < 2; idx ++)
		sum += powerMA[idx];

    if (sum != 0.0)
    {
        for (idx = 0; idx < 2; idx ++) {
			powRatio[idx] = powerMA[idx] / sum;
			powRatiodB[idx] = 10 * log10(powRatio[idx]);
        }
    }
    else
    {
        for (idx = 0; idx < 2; idx ++) {
			powRatio[idx] = 0.0;
			powRatiodB[idx] = 0.0;
        }
    }

    // (Step 7) Save features to the normalized Feature Vector
    feature->powerMA[0]  = (powerMAdB[0] - oddemo_meanVec[zone_pair][0]) / oddemo_stdVec[zone_pair][0];
    feature->powerMA[1]  = (powerMAdB[1] - oddemo_meanVec[zone_pair][1]) / oddemo_stdVec[zone_pair][1];
    feature->powRatio[0] = (powRatiodB[0]- oddemo_meanVec[zone_pair][2]) / oddemo_stdVec[zone_pair][2];
    feature->powRatio[1] = (powRatiodB[1]- oddemo_meanVec[zone_pair][3]) / oddemo_stdVec[zone_pair][3];
    feature->crossCorr   = (crossCorr    - oddemo_meanVec[zone_pair][4]) / oddemo_stdVec[zone_pair][4];
}
*/

#define MAX 100
void ODDemo_Feature_extract(uint16_t zone_pair, float *azimuthHeatMap, ODDEMO_Feature *feature)
{
    uint16_t idx, az_idx, rng_idx;
    uint16_t max_az, max_rng, offset, zone;
    float	*heat;
    float	sum, max, tmp, var;
    float	crossCorr, avgPower, powerMA[2], powerMAdB[2], powRatio[2], powRatiodB[2], sqRootVar[2];

	// char* buffer[16];
	// char* ap;
    // float	size;
    
    // (Step 1) Compute Average Power for each zone
    // (this assumes zone sizes have been checked and clipped)
    zone = zone_pair * 2;
    for (idx = 0; idx < 2; idx ++)
    {
        #ifdef FULL_ZONE_SUM
        sum  = 0;
        size = (float)(oddemo_zone[zone].range_length * oddemo_zone[zone].azimuth_width);

		for (rng_idx = 0; rng_idx < oddemo_zone[zone].range_length; rng_idx++) {
			heat = azimuthHeatMap + (ODDEMO_MAX_AZIMUTH * (oddemo_zone[zone].range_start + rng_idx));
			heat += oddemo_zone[zone].azimuth_start;

			for (az_idx = 0; az_idx < oddemo_zone[zone].azimuth_width; az_idx++) {
	    		sum += *heat++;
			}
		}

		avgPower = sum / size;
		#else //do 5x5 cell power sums
        //find the max 3x3 area inside the zone
        max   = 0;
        heat  = azimuthHeatMap + (ODDEMO_MAX_AZIMUTH * oddemo_zone[zone].range_start);
        heat += oddemo_zone[zone].azimuth_start;

		if (zone > 3)
		{
			for (rng_idx = 0; rng_idx < oddemo_zone[zone].range_length; rng_idx++) {
				// point to the correct range row, then add the starting azimuth index
				for (az_idx = 0; az_idx < oddemo_zone[zone].azimuth_width; az_idx++) {
					offset = az_idx;
					tmp  = heat[offset] + heat[offset+1] + heat[offset+2];
					offset += ODDEMO_MAX_AZIMUTH;
					tmp += heat[offset] + heat[offset+1] + heat[offset+2];
					offset += ODDEMO_MAX_AZIMUTH;
					tmp += heat[offset] + heat[offset+1] + heat[offset+2];

					if (tmp > max)
					{
						max = tmp;
						max_rng = rng_idx; //center position is actually +1, +1
						max_az  = az_idx;
					}
				}
				heat += ODDEMO_MAX_AZIMUTH;
			}
		}
		else
		{
			for (rng_idx = 0; rng_idx < oddemo_zone[zone].range_length-2; rng_idx++) {
				// point to the correct range row, then add the starting azimuth index
				for (az_idx = 0; az_idx < oddemo_zone[zone].azimuth_width-2; az_idx++) {
					offset = az_idx;
					tmp  = heat[offset] + heat[offset+1] + heat[offset+2];
					offset += ODDEMO_MAX_AZIMUTH;
					tmp += heat[offset] + heat[offset+1] + heat[offset+2];
					offset += ODDEMO_MAX_AZIMUTH;
					tmp += heat[offset] + heat[offset+1] + heat[offset+2];

					if (tmp > max)
					{
						max = tmp;
						max_rng = rng_idx; //center position is actually +1, +1
						max_az  = az_idx;
					}
				}
				heat += ODDEMO_MAX_AZIMUTH;
			}
		}

        // Save the max positions for output.
        max_rng += oddemo_zone[zone].range_start + 1;
        max_az  += oddemo_zone[zone].azimuth_start + 1;
        oddemo_zone[zone].range_max_idx = max_rng;
        oddemo_zone[zone].azimuth_max_idx = max_az;

        sum   = 0;
        heat  = azimuthHeatMap + (ODDEMO_MAX_AZIMUTH * (max_rng-2));
        heat += max_az - 2;

        //compute average power on a 5x5 centered on the max 3x3
        for (rng_idx = 0; rng_idx < 5; rng_idx++) {
            for (az_idx = 0; az_idx < 5; az_idx++) {
                sum += heat[az_idx];
            }
            heat += ODDEMO_MAX_AZIMUTH;
        }
        avgPower = sum / 25; //fixed 25 cell size
		#endif

		/* Virtual function (history logging) for Moving Average calculation... */
		history_read(zone);
		history_save(zone, avgPower);

        // (Step 2) store new average power, overwriting the oldest value in the history buffer
        power_history[zone][ph_idx] = avgPower;
        power_historydB[zone][ph_idx] = 10 * log10(avgPower);

        // (Step 3) Moving Average of previous average power values
        powerMA[idx] = DSPF_sp_vecavg(&power_history[zone][0], oddemo_parms.windowLen);
        powerMAdB[idx] = 10 * log10(powerMA[idx]);

        //this value is for output only:
        if (powerMA[idx] > 999999.9)
          oddemo_decision[zone].avgPower = 999999.9;
        else if (powerMA[idx] < 0.0)
          oddemo_decision[zone].avgPower = 0.0;
        else
          oddemo_decision[zone].avgPower = powerMA[idx];

        // (Step4) Cross Correlation between the zones
        // (Step4a) Compute Variance for each zone
        var = DSPF_sp_vecvar(&power_historydB[zone][0], oddemo_parms.windowLen);

        // (Step4b) Compute Square Root of Variance
        sqRootVar[idx] = DSPF_sp_sqrtsp(var);
        
		printf("Zone_%d - Avg. Power   Moving Avg.    Variance : %03f,   %03f,   %03f\n", zone+1, avgPower, powerMA[idx], var);
		zone ++;
    }

	zone = zone_pair * 2;

	//update the history index for the next frame
	ph_idx ++;

	if(ph_idx == oddemo_parms.windowLen)
		ph_idx = 0;

    // (Step 5) Compute Cross Correlation Coefficient (currently hardcoded for two zones)
    crossCorr = DSPF_sp_crosscorr(&power_historydB[zone][0], &power_historydB[zone+1][0], sqRootVar[0], sqRootVar[1], oddemo_parms.windowLen);

    // (Step 6) Compute Power Ratios
    sum = 0.0;
    for (idx = 0; idx < 2; idx ++) {
		sum += powerMA[idx];
	}

    if (sum != 0.0)
    {
        for (idx = 0; idx < 2; idx ++) {
			powRatio[idx] = powerMA[idx] / sum;
			powRatiodB[idx] = 10 * log10(powRatio[idx]);
        }
    }
    else
    {
        for (idx = 0; idx < 2; idx ++) {
			powRatio[idx] = 0.0;
			powRatiodB[idx] = 0.0;
        }
    }

    // (Step 7) Save features to the normalized Feature Vector
    feature->powerMA[0]  = (powerMAdB[0] - oddemo_meanVec[zone_pair][0]) / oddemo_stdVec[zone_pair][0];
    feature->powerMA[1]  = (powerMAdB[1] - oddemo_meanVec[zone_pair][1]) / oddemo_stdVec[zone_pair][1];
    feature->powRatio[0] = (powRatiodB[0]- oddemo_meanVec[zone_pair][2]) / oddemo_stdVec[zone_pair][2];
    feature->powRatio[1] = (powRatiodB[1]- oddemo_meanVec[zone_pair][3]) / oddemo_stdVec[zone_pair][3];
    feature->crossCorr   = (crossCorr    - oddemo_meanVec[zone_pair][4]) / oddemo_stdVec[zone_pair][4];
}


void ODDemo_Feature_init(void)
{
    uint16_t idx, idx2;
    float  init_power, log_power;

    //This variable always indicates the oldest entry in the power history buffers.
    ph_idx = 0;
    // oddemo_num_zones = 0;

    init_power = ODDEMO_POWER_INIT_VAL;
    log_power  = 10 * log10(init_power);			// log_power = 16.81241

    for (idx = 0; idx < ODDEMO_MAX_ZONES; idx++) {
		for (idx2 = 0; idx2 < ODDEMO_MAX_FRAME_HIST; idx2++) {
			power_history[idx][idx2] = init_power;
			power_historydB[idx][idx2] = log_power;
		}
	}
}







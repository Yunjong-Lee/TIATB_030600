/* Standard Include Files. */
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "oddemo_common.h"


float       oddemo_coeffMatrix[ODDEMO_ZONE_PAIR][ODDEMO_MATRIX_SIZE];
ODDEMO_Zone oddemo_zone[ODDEMO_MAX_ZONES];
float       oddemo_meanVec[ODDEMO_ZONE_PAIR][ODDEMO_MATRIX_ROW_SIZE-1];
float       oddemo_stdVec[ODDEMO_ZONE_PAIR][ODDEMO_MATRIX_ROW_SIZE-1];
uint16_t    oddemo_num_zones = 8; 
float       oddemo_row_noise[ODDEMO_MAX_RANGE];
ODDEMO_Parms   oddemo_parms;


/**
 *  @b Description
 *  @n
 *      This is the CLI Handler for Occupancy Demo zone definition
 *
 *  @param[in] argc
 *      Number of arguments
 *  @param[in] argv
 *      Arguments
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
void set_CLIZoneDef()
{
	uint16_t i = 0;

	/* Populate configuration: */

	// 9 10 8 14 / 9 10 26 14 / 28 7 16 5 / 25 10 21 5 / 25 10 22 5 / 28 7 27 5 / 9 10 8 32 / 24 16 15 32
	uint16_t zoneDef[8][4] = {{9, 10, 8, 14}, 
							  {9, 10, 26, 14}, 
							  {28, 7, 16, 5}, // {24, 10, 8, 14},
							  {28, 7, 27, 5},
							  {25, 10, 21, 1}, // {24, 10, 26, 14}, 
							  {25, 10, 22, 1},
							  {9, 10, 8, 32}, 
							  {24, 16, 15, 32} };

	for (i = 0; i < oddemo_num_zones; i++) {
		oddemo_zone[i].range_start = zoneDef[i][0];
		oddemo_zone[i].range_length = zoneDef[i][1];
		oddemo_zone[i].azimuth_start = zoneDef[i][2];
		oddemo_zone[i].azimuth_width = zoneDef[i][3];
	}
}


// coeffMatrixRow
void set_CLICoeffMatrix()
{
	int row = 0;
	int pair = 0;
	int idx = 0;

    float   *ptr;

	float cli_coeff[3][4][6] =	{	{{-4.415308, -3.319595, -3.115669, 2.192494, 2.306401, -0.202405},
									 {-4.950168, 1.287965, -2.270877, 0.675102, -4.321267, 0.233029}, 
									 {-5.224273, -2.639519, 1.126425, -4.211431, 0.571066, 0.333464},
									 {-3.304388, 3.303448, 3.080489, 2.455158, 2.775458, -0.668746} }, 
									{{-3.956852, -2.922866, -3.207900, 1.595226, 1.603487, -0.676134},
									 {-4.804304, 1.412252, -1.477188, 0.707452, -4.012271, 1.156422},
									 {-4.696507, -1.588259, 1.465891, -3.656444, 0.728813, 1.207081},
									 {-3.374264, 2.870373, 2.826581, 2.438657, 2.730894, -1.496023} }, 
									{{-3.956852, -2.922866, -3.207900, 1.595226, 1.603487, -0.676134},
									 {-4.804304, 1.412252, -1.477188, 0.707452, -4.012271, 1.156422},
									 {-4.696507, -1.588259, 1.465891, -3.656444, 0.728813, 1.207081},
									 {-3.374264, 2.870373, 2.826581, 2.438657, 2.730894, -1.496023} } };

	for(pair = 0; pair < 3; pair++) {
		for(row = 0; row < 4; row++) {
			ptr = &oddemo_coeffMatrix[pair][row * 6];

			for(idx = 0; idx < 6; idx ++) {
				*ptr = cli_coeff[pair][row][idx];
				*ptr++;
			} // for idx
		} // for row
	} // for pair
}


// meanVector & stdVector
void set_CLI_meanVec_stdVec()
{
	int i, j;

	float meanVec[3][5] = {	{30.300249, 31.578581, -7.697893, -6.419561, 0.306626},
							{31.655145, 32.674334, -6.327176, -5.307987, 0.451940},
							{31.655145, 32.674334, -6.327176, -5.307987, 0.451940}	};
	float stdVec[3][5]	= {	{11.033930, 12.032618, 9.021614, 8.202547, 0.439775},
							{10.224506, 9.922317, 7.227706, 5.977525, 0.483263},
							{10.224506, 9.922317, 7.227706, 5.977525, 0.483263} };

	for(i = 0; i < 3; i++) {
		for(j = 0; j< 5; j++) {
			oddemo_meanVec[i][j] = meanVec[i][j];
			oddemo_stdVec[i][j] = stdVec[i][j];
		}
	}
}


void set_CLIRowNoise()
{
	int i;
	float rowNoise[64] = {  3.238091, 3.149222,  3.702070,  4.404055,  5.957448,  8.137568, 10.435353, 13.362494, 
						  16.390192, 18.434166, 20.267143, 23.216925, 25.805658, 27.805454, 31.510540, 33.855160, 
						  32.999798, 31.745882, 32.945454, 34.581993, 35.050125, 35.274643, 35.774780, 35.251621, 
						  35.674667, 37.586369, 37.889324, 37.308887, 37.992409, 38.948715, 37.697441, 37.615105, 
						  39.086571, 39.685955, 40.055130, 40.901653, 40.569962, 39.230408, 39.405903, 40.309380, 
						  39.565956, 39.332569, 40.723072, 40.175022, 38.663498, 39.859867, 41.323280, 40.104511, 
						  39.254108, 40.619450, 40.697308, 39.316700, 39.971233, 41.565105, 40.925831, 40.003742, 
						  41.296074, 41.188408, 33.966324, 21.332525,  9.926456, 3.596813,   1.467419,  1.835692 };

	for(i = 0; i < ODDEMO_MAX_RANGE; i++) {
		oddemo_row_noise[i] = rowNoise[i];
	}
}


// oddemoParms
void set_CLIParms()
{
	oddemo_parms.mode = 1;			// 1 = operational, 2 = zone tuning, 3 = coefficient data
	oddemo_parms.windowLen = 8;		// window length for feature extract - multiple of 4
	oddemo_parms.gamma = 0.03;
	oddemo_parms.threshold = 0.5;	// percentage of positive windowed detections to declare occupied
	oddemo_parms.threshWin = 18;	// frames to average detections over
	oddemo_parms.smoothing = 0.45;	// 0.0 disables smoothing. max should not exceed 0.95.    
}




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "oddemo_common.h"
#include "oddemo_decision.h"

#include "DSPF_sp_maxidx_cn.h"
// #include <c6x.h>


// typedef struct {
//      uint32_t word0;
//      uint32_t word1;
// } __float2_t;

/* C6xSimulator.h */
#define _amem8(ptr)             (*((int64_ll *) (ptr)))
#define _amem8_const(ptr)       (*((const int64_ll *) (ptr)))
#define _amem8_f2(ptr)          (*((__float2_t *) (ptr)))
#define _amem8_f2_const(ptr)    (*((const __float2_t *) (ptr)))

#define _lof2     _lof
#define _hif2     _hif

/* c66_data_sim.h */
float    _lof2(__float2_t);
float    _hif2(__float2_t);

float     _lof(double);		/* Convert the lower 32 bits of double as float */
float     _hif(double);		/* Convert the highr 32 bits of double as float */


extern ODDEMO_Parms		oddemo_parms;
extern ODDEMO_Zone		oddemo_zone[ODDEMO_MAX_ZONES];

ODDEMO_Decision oddemo_decision[ODDEMO_MAX_ZONES];


#if 0 // import function "DSPF_sp_amxidx"
int DSPF_sp_maxidx(const float* x, const int nx)
{
    int   i, idx1, idx2, idx3, idx4;
    float max1, max2, max3, max4;
    __float2_t x_1_2, x_3_4;

    max1 = -FLT_MAX;
    max2 = -FLT_MAX;
    max3 = -FLT_MAX;
    max4 = -FLT_MAX;

	idx1 = 0;
	idx2 = 0;
	idx3 = 0; 
	idx4 = 0;

    for (i = 0; i < nx; i+=4)
    {
		x_1_2 = _amem8_f2((void *)&x[i]);
		x_3_4 = _amem8_f2((void *)&x[i+2]);

		if (_lof2(x_1_2) > max1) 
		{
            max1 = _lof2(x_1_2);
            idx1 = i;
        }
        /* if (_hif2(x_1_2) > max2) 
		{
            max2 = _hif2(x_1_2);
            idx2 = i + 1;
        }
        if (_lof2(x_3_4) > max3) 
		{
            max3 = _lof2(x_3_4);
            idx3 = i + 2;
        }
        if (_hif2(x_3_4) > max4) 
		{
            max4 = _hif2(x_3_4);
            idx4 = i + 3;
        } */
	}

	if (max2 > max1)
	{
        max1 = max2;
        idx1 = idx2;
    }
    if (max4 > max3)
    {
        max3 = max4;
        idx3 = idx4;
    }

    if (max3 > max1)
        return idx3;
    else
        return idx1;
}
#endif


void DSPF_sp_mat_vec_mul(float *x1, const int r1, const int c1, float *x2, float *y)
{
    int i, j;
    float sum;

    // Multiply each row in x1 by vector x2.
    // The product of row m in x1 and vector x2 is placed in position (m) in the result vector.
    for (i = 0; i < r1; i++)
    {
        sum = 0;

        for (j = 0; j < c1; j++)
		{
            sum += x1[j + i * c1] * x2[j];
		}
        y[i] = sum;
    }
}


int16_t	oddemo_desCount[ODDEMO_MAX_ZONES];
void ODDemo_Decision_init(void)
{
  memset(oddemo_desCount, 0, sizeof(oddemo_desCount));
}


void ODDemo_Decision_process_addOD(uint16_t pair, float *coeffMatrix, ODDEMO_Feature *featureVect, ODDEMO_Decision *decision)
{
    uint16_t idx; 
	uint16_t tmp, zidx;
    uint32_t result;
    float tmp_prob;   
    float vec[ODDEMO_MATRIX_ROW_SIZE];
    float prob[ODDEMO_MATRIX_NUM_ROWS];

    //create a column vector
    vec[0] = 1.0;
    vec[1] = featureVect->powerMA[0];
    vec[2] = featureVect->powerMA[1];
    vec[3] = featureVect->powRatio[0];
    vec[4] = featureVect->powRatio[1];
    vec[5] = featureVect->crossCorr;

    DSPF_sp_mat_vec_mul(coeffMatrix, ODDEMO_MATRIX_NUM_ROWS, ODDEMO_MATRIX_ROW_SIZE, vec, prob); //4 element vector

    //Perform a sigmoid function on the result vector
    for (idx = 0; idx < ODDEMO_MATRIX_NUM_ROWS; idx++)
    {
        prob[idx] = 1.0 / (1.0 + exp(-prob[idx]));
    }
	
    // The maximum entry in the resulting vector indicates the decision index.
    // This needs to be converted into a bit-mapped value, 1 bit per zone.
    result = DSPF_sp_maxidx_cn(prob, ODDEMO_MATRIX_NUM_ROWS); // result = DSPF_sp_maxidx(prob, ODDEMO_MATRIX_NUM_ROWS);

    // [230728_LEEYJ] increasing zones
    for (idx = 0; idx < 2; idx++) // for (idx = 0; idx < ODDEMO_ZONE_PAIR; idx++)
    {
		#if 1
		zidx = (pair*2) + idx;
        tmp = (result & (1 << idx)) >> idx;

		if (tmp == 1)
		{
			oddemo_desCount[zidx] ++;
			if (oddemo_desCount[zidx] > oddemo_parms.threshWin)
				oddemo_desCount[zidx] = oddemo_parms.threshWin;
		}
		else
		{
			oddemo_desCount[zidx] --;
			if (oddemo_desCount[zidx] < 0)
				oddemo_desCount[zidx] = 0;
		}
		tmp_prob = (float) oddemo_desCount[zidx] / oddemo_parms.threshWin;

		decision[idx].posPercent = tmp_prob;
		decision[idx].range_idx = oddemo_zone[zidx].range_max_idx;
		decision[idx].azimuth_idx = oddemo_zone[zidx].azimuth_max_idx;
		#else
		decision[idx].posPercent = result;
		#endif
    }
}


void ODDemo_Decision_process(float *coeffMatrix, ODDEMO_Feature *featureVect, int8_t *decision)
{
    uint32_t idx;
    uint32_t result;
    float vec[ODDEMO_MATRIX_ROW_SIZE];
    float prob[ODDEMO_MATRIX_NUM_ROWS];

    //create a column vector
    vec[0] = 1.0;
    vec[1] = featureVect->powerMA[0];
    vec[2] = featureVect->powerMA[1];
    vec[3] = featureVect->powRatio[0];
    vec[4] = featureVect->powRatio[1];
    vec[5] = featureVect->crossCorr;

    DSPF_sp_mat_vec_mul(coeffMatrix, ODDEMO_MATRIX_NUM_ROWS, ODDEMO_MATRIX_ROW_SIZE, vec, prob); //4 element vector

    //Perform a sigmoid function on the result vector
    for (idx = 0; idx < ODDEMO_MATRIX_NUM_ROWS; idx++)
    {
        prob[idx] = 1.0 / (1.0 + exp(-prob[idx]));
    }
	
    // The maximum entry in the resulting vector indicates the decision index.
    // This needs to be converted into a bit-mapped value, 1 bit per zone.
    result = DSPF_sp_maxidx_cn(prob, ODDEMO_MATRIX_NUM_ROWS); // DSPF_sp_maxidx(prob, ODDEMO_MATRIX_NUM_ROWS);

	// [230728_LEEYJ] increasing zones
    for (idx = 0; idx < 2; idx++) // for (idx = 0; idx < ODDEMO_ZONE_PAIR; idx++)
    {
        decision[idx] = (result & (1 << idx)) >> idx;
    }
}




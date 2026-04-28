
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#define PI 3.14159265358979323846 // 3.1415926535897
#define MAX_COUNT 1000
#define MAX_LIST_COUNT	1000

#define	DRIVER_DOPPLER_INPUT_RE		10
#define	DRIVER_DOPPLER_INPUT_IM		11
#define	PASSENGER_DOPPLER_INPUT_RE	13
#define	PASSENGER_DOPPLER_INPUT_IM	14


float	rangeBinPhase = 0;
float	obj_VS___unwrapPhasePeak; 

float doppler_input_real[25] = {0.000003588342224 ,-0.000016688252799 ,-0.000014665267372 ,-0.000002969056368 ,-0.000001200955012, 
								0.000002527638571 ,-0.000007881695637 ,-0.000015275862097, 0.000008463874110 ,0.000014568679035 ,
								0.000006810052582 ,0.000340310623869 ,0.000002298387699 ,-0.000044248969061 ,-0.000001362604962 ,
								-0.000008424205589, .000064600608312 ,0.000000847721822 ,-0.000002411557944,0.000004329325748 ,
								0.000009745068382 ,0.000022233696654 ,-0.000012239557691, 0.000003385139280 ,-0.000001395339496};
float doppler_input_imag[25] = {-0.000009736882930 ,0.000018501159502 ,-0.000000227068085 ,-0.000000312684278,0.000012761429389 ,
								-0.000020328676328,0.000012745789718 ,0.000001257256372 ,0.000017311424017 ,-0.000023263040930,
								0.000004872737918 ,-0.000042011728510 ,-0.000058469362557 ,-0.000018629798433 ,-0.000008874380001, 
								0.000024610366381 ,0.000021530111553 ,0.000010285068129 ,0.000014702600311 ,0.000026997207897 ,
								0.000002362692612 ,0.000014104240108 ,-0.000063264349592 ,-0.000020301362383 ,-0.000009740244423 };

/*
typedef struct {
    int		f_no;
    str		f_time;
	float	unwrapPhasePeak[2];
	float	outputFilterHeartOut[2];
	float	outputFilterBreathOut[2];
	float	heartRateEst_FFT[2];
	float	breathingRateEst_FFT[2];
	float	doppler_input_real[2];
	float	doppler_input_imag[2];
	float	rangeBinPhase[2];
	float	sumEnergyHeartWfm[2];
	float	sumEnergyBreathWfm-peakValueBreathSpect[2];
	float	confidenceMetric_HeartOut[2];
	float	confidenceMetric_BreathOut[2];
	float	confidenceMetric_HeartOut_4Hz[2];
	float	confidenceMetric_HeartOut_xCorr[2];
	float	confidenceMetric_BreathOut_xCorr[2];
	float	heartRateEst_FFT_4Hz[2];
	float	heartRateEst_FFT_xCorr[2];
	float	heartRateEst_PeakCount_filtered[2];
	float	breathingRateEst_xCorr[2];
	float	breathingRateEst_peakCount[2];
	float	heartRateEst_HarmonicEnergy[2];
	float	breathingRateEst_HarmonicEnergy[2];

} Data; */

#define VECTOR_SIZE 44

typedef struct {
    char f_time[32];			// [260428_105650_876313]
    int f_no;					// [1]
    float vector[VECTOR_SIZE]; // (0.0, 0.0, ...)

} VS_Data;



float unwrap(float phase, float phasePrev, float *diffPhaseCorrectionCum)
{
	float modFactorF;
	float diffPhase;
	float diffPhaseMod;
	float diffPhaseCorrection;
	float phaseOut;

	// incremental phase variation
	diffPhase = phase - phasePrev;

	if (diffPhase > PI)
		modFactorF = 1;
	else if (diffPhase < - PI)
	    modFactorF = -1;
	else
		modFactorF = 0;

	diffPhaseMod = diffPhase - modFactorF*2*PI;

	// preserve variation sign for +pi vs. -pi
	if((diffPhaseMod == -PI) && (diffPhase > 0))
		diffPhaseMod = PI;

	// incremental phase correction
	diffPhaseCorrection = diffPhaseMod - diffPhase;

	// Ignore correction when incremental variation is smaller than cutoff
	if(((diffPhaseCorrection < PI) && (diffPhaseCorrection > 0)) || ((diffPhaseCorrection > -PI) && (diffPhaseCorrection < 0)))
		diffPhaseCorrection = 0;

	// Find cumulative sum of deltas
	*diffPhaseCorrectionCum = *diffPhaseCorrectionCum + diffPhaseCorrection;
	phaseOut = phase + *diffPhaseCorrectionCum;
	
    return phaseOut;
 }


void main()
{
    FILE *fp;
    VS_Data d[MAX_LIST_COUNT];
	int index = 0;
	int data_size = 0;
	int cnt = 0;

	char line[4096]; 
	const char *delimiters = " [],()";

	// For phase unwrapping
    static float phasePrevFrame = 0;              // Phase value of Previous frame
    static float diffPhaseCorrectionCum = 0;      // Phase correction cumulative
    static float phaseUsedComputationPrev = 0;    // Phase values used for the Previous frame
	int i = 0; 

    // binary data : read mode = rb
    fp = fopen("260428_10563856_Vital.dat", "r");
    if (fp == NULL) return 1;

	while (fgets(line, sizeof(line), fp)) {
		char *ptr = line;
		int idx = 0;

        if (sscanf(ptr, " [%[^]]]", d[cnt].f_time) == 1) {
            ptr = strstr(ptr, "], [") + 3; // move to " ["
        }

        if (sscanf(ptr, " [%d]", &d[cnt].f_no) == 1) {
            ptr = strstr(ptr, "], [") + 4; // move to " ( "
        }

        for (idx = 0; idx < VECTOR_SIZE; idx++) {
            char *next_ptr;
            d[cnt].vector[idx] = strtod(ptr, &next_ptr);
            
            ptr = next_ptr;
            while (*ptr == ',' || *ptr == ' ' || *ptr == '(' || *ptr == ')') ptr++;
        }
		cnt++;
	}
    fclose(fp);

	printf("  rangeBinPhase   |  obj_VS___unwrapPhasePeak    \n");
	for (i=0; i<MAX_LIST_COUNT; i++)
	{
		rangeBinPhase = atan2(d[i].vector[PASSENGER_DOPPLER_INPUT_IM], d[i].vector[PASSENGER_DOPPLER_INPUT_RE]);
		// rangeBinPhase = atan2(doppler_input_imag[PASSENGER_DOPPLER_INPUT_IM], doppler_input_real[PASSENGER_DOPPLER_INPUT_RE]);

		obj_VS___unwrapPhasePeak = unwrap(rangeBinPhase, phasePrevFrame, &diffPhaseCorrectionCum);
		phasePrevFrame = rangeBinPhase;
		printf("   %f     |  %f \n", rangeBinPhase,obj_VS___unwrapPhasePeak);
	}

	printf("--- Emulator (Ver.240703_Preproc+Occupancy) EOL.   ");
	system("PAUSE");
}

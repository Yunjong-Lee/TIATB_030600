

/* Standard Include Files. */
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


#include "oddemo_common.h"
#include "dss_data_path.h"
#include "oddemo_feature.h"
#include "oddemo_heatmap.h"
#include "oddemo_decision.h"

#include "dss_mmw.h"


extern uint16_t           oddemo_zone_pairs;
extern uint16_t           oddemo_num_zones;

extern ODDEMO_Feature     oddemo_feature[ODDEMO_ZONE_PAIR];
extern ODDEMO_Decision    oddemo_decision[ODDEMO_MAX_ZONES];
extern ODDEMO_Zone        oddemo_zone[ODDEMO_MAX_ZONES];
extern ODDEMO_Parms       oddemo_parms;

// extern VS_Feature         vital_signs;

extern float oddemo_coeffMatrix[ODDEMO_ZONE_PAIR][ODDEMO_MATRIX_SIZE];
extern float oddemo_rangeAzimuthHeatMap[ODDEMO_ANGLEHEATMAP_BUF_SIZE];


void MmwDemo_interChirpProcessing() // (MmwDemo_DSS_DataPathObj *obj, uint8_t chirpPingPongId)
{
}


void MmwDemo_processChirp() // (MmwDemo_DSS_DataPathObj *obj, uint16_t chirpIndxInMultiChirp)
{
	// MmwDemo_interChirpProcessing();
}


void MmwDemo_interFrameProcessing(void)
{
	uint16_t pair;

	ODDemo_Heatmap_arc_removal(oddemo_rangeAzimuthHeatMap);

	// ODDemo_Heatmap_smoothing(oddemo_rangeAzimuthHeatMap, oddemo_previousHeatMap);

	for (pair = 0; pair < oddemo_zone_pairs; pair ++)
	{
		// int8_t *dptr = &oddemo_decision[pair*2];

	 	ODDemo_Feature_extract(pair, oddemo_rangeAzimuthHeatMap, &oddemo_feature[pair]);
		ODDemo_Decision_process_addOD(pair, &oddemo_coeffMatrix[pair][0], &oddemo_feature[pair], &oddemo_decision[pair*2]);
		// ODDemo_Decision_process(&oddemo_coeffMatrix[pair][0], &oddemo_feature[pair], &oddemo_decision[pair*2]);
		/* --- full average power calculation and 3 by 3 cell average power calculation
		ODDemo_Feature_extract_addOD(pair, oddemo_rangeAzimuthHeatMap, &oddemo_feature[pair]);
		*/
	}
}




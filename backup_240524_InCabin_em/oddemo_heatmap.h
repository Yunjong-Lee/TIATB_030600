

#include "dss_data_path.h"

#define ODDEMO_HEATMAP_MAX16 32000
#define ODDEMO_HEATMAP_MAX8  252

typedef struct
{
    float min;
    float max;
    float avg;
} ODDemo_Heatmap_Stats;


extern void ODDemo_Heatmap_get_stats(float *heatmap);
extern void ODDemo_Heatmap_arc_removal(float *heatmap);




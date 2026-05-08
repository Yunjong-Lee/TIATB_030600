

// using namespace std;   
typedef unsigned int	uint32_t;
typedef unsigned short	uint16_t;

typedef int				int32_t;
typedef short			int16_t;
typedef signed char     int8_t;
typedef double			__float2_t;


// #include "swpform.h"
// #ifdef _TMS320C6X
// #include "c6x.h"
// #endif


#define ODDEMO_MAX_RANGE               64
#define ODDEMO_MAX_AZIMUTH             48
#define ODDEMO_MAX_ZONES               8 // 4	// [230728_LEEYJ] increasing zones
#define ODDEMO_ZONE_PAIR              (ODDEMO_MAX_ZONES / 2)
#define ODDEMO_MAX_FRAME_HIST          16
#define ODDEMO_MATRIX_ROW_SIZE         6
#define ODDEMO_MATRIX_NUM_ROWS         4
#define ODDEMO_MATRIX_SIZE            (ODDEMO_MATRIX_ROW_SIZE * ODDEMO_MATRIX_NUM_ROWS)
#define ODDEMO_ANGLE_RANGE             60.0
#define ODDEMO_ANGLE_RESOLUTION       ((ODDEMO_ANGLE_RANGE * 2) / (float)ODDEMO_MAX_AZIMUTH)
#define ODDEMO_ROW_NOISE_FRAMES        64

#define ODDEMO_ANGLEHEATMAP_BUF_SIZE   (ODDEMO_MAX_RANGE * ODDEMO_MAX_AZIMUTH)

// SteeringVec Size (bytes): (nRxAnt-1) * steeringVecSize * sizeof(cplxf_t); For Ant0, since steering vector is = 1, it is not saved.
// steeringVecSize =   (uint32_t) ((2.f * estAngleRange) / estAngleResolution) + 0.5f);
// For estAngleRange = 60 (-60, 60) and 64 nAzimuthBins (angles), the resolution = 120/64 = 1.875
// steeringVecSize = 64

#define ODDEMO_STEERINGVEC_L1_BUF_SIZE (ODDEMO_MAX_AZIMUTH * 8)
#define ODDEMO_SCRATCH_L1_BUF_SIZE      360 // for nRxAnt=8 for uint32_t

// size = [(nRxAnt * ( 1 + nRxAnt))/2] * sizeof(cplxf_t)
// upper triangle of nRxAnt x nRxAnt Hermitian matrix
#define ODDEMO_INVRNMATRIX_BUF_SIZE     36  // for nRxAnt=8 for cplxf_t
#define ODDEMO_INVRNMATRIX_BUFFER      (ODDEMO_INVRNMATRIX_BUF_SIZE * ODDEMO_MAX_RANGE)


typedef struct // ODDemo_DataPathObj_t 
{
	uint32_t       nRxAnt;
    float *rangeAzimuthHeatMap; // < output range azimuth heatmap, array in format of (numInputRangeBins * nAzimuthBins).

} ODDemo_DataPathObj;


//This type defines the position and size of an occupancy zone in the heat map.
typedef struct {
  uint16_t range_start;
  uint16_t range_length;
  uint16_t range_max_idx;
  uint16_t azimuth_start;
  uint16_t azimuth_width;
  uint16_t azimuth_max_idx;

} ODDEMO_Zone;


//This type defines the feature vector contents for a pair of zones.
typedef struct {
  float powerMA[2];
  float powRatio[2];
  float crossCorr;

} ODDEMO_Feature;


//This type defines the feature vector contents for Vital Signs information
typedef struct {
  float unwrapped_waveform[2];
  float heart_waveform[2];
  float breathing_waveform[2];
  float heart_rate[2];
  float breathing_rate[2];

} VS_Feature;


//This type defines the demo's configurable parameters.
typedef struct {
	// [230812_LEEYJ] Merge Front_OD 
    // float    gamma;
    // uint32_t windowLen;
    uint16_t mode;      //1 = operational, 2 = zone tuning, 3 = coefficient data
    uint16_t windowLen; //window length for feature extract - multiple of 4
    float    gamma;
    float    threshold; //percentage of positive windowed detections to declare occupied
    uint16_t threshWin; //frames to average detections over
    float    smoothing; //0.0 disables smoothing. max should not exceed 0.95.

} ODDEMO_Parms;


typedef struct {
	float posPercent;     //percentage of positive detections during the window period
	float avgPower;       //windowed average zone power
	uint16_t range_idx;   //heatmap range idx for the center of the max 5x5
	uint16_t azimuth_idx; //heatmap azimuth idx for the center of the max 5x5

} ODDEMO_Decision;


void set_CLIZoneDef();
void set_CLIParms();
void set_CLI_meanVec_stdVec();
void set_CLICoeffMatrix();
void set_CLIRowNoise();




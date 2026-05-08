

#include	"mmw_config.h"
// #include "sys_type.h"
#include	"ADCBuf.h"
#include	"virtual_function.h"



#pragma once // 'struct' 형식 재정의 오류 protection.

#define PI_ 3.1415926535897
#define ONE_Q15 (1 << 15)
#define ONE_Q19 (1 << 19)
#define ONE_Q8 (1 << 8)


typedef struct cycleLog_t_
{
    uint32_t interChirpProcessingTime; /*!< @brief total processing time during all chirps in a frame excluding EDMA waiting time*/
    uint32_t interChirpWaitTime; /*!< @brief total wait time for EDMA data transfer during all chirps in a frame*/
    uint32_t interFrameProcessingTime; /*!< @brief total processing time for 2D and 3D excluding EDMA waiting time*/
    uint32_t interFrameWaitTime; /*!< @brief total wait time for 2D and 3D EDMA data transfer */
} cycleLog_t;


typedef struct MmwDemo_DSS_dataPathContext_t_
{
    /*! @brief   ADCBUF handle. */
    ADCBuf_Handle adcbufHandle;

    /*! @brief   Handle of the EDMA driver. */
    // EDMA_Handle edmaHandle[EDMA_NUM_CC];

	#if 0 //presently we only assert when we hit error, not store into these globals
    //because user can still examine in CCS the locals to find which instance (from handle)
    //and what error

    /*! @brief   EDMA error Information when there are errors like missing events */
    EDMA_errorInfo_t  EDMA_errorInfo[EDMA_NUM_CC];

    /*! @brief EDMA transfer controller error information. */
    EDMA_transferControllerErrorInfo_t EDMA_transferControllerErrorInfo[EDMA_NUM_CC];
	#endif

#ifdef EDMA_1D_INPUT_BLOCKING
    /*! @brief Semaphore handle for 1D EDMA completion. */
    Semaphore_Handle EDMA_1D_InputDone_semHandle[2];
#endif

#ifdef EDMA_1D_OUTPUT_BLOCKING
    /*! @brief Semaphore handle for 1D EDMA completion. */
    Semaphore_Handle EDMA_1D_OutputDone_semHandle[2];
#endif

#ifdef EDMA_2D_INPUT_BLOCKING
    /*! @brief Semaphore handle for 2D EDMA completion. */
    Semaphore_Handle EDMA_2D_InputDone_semHandle[2];
#endif

#ifdef EDMA_2D_OUTPUT_BLOCKING
    /*! @brief Semaphore handle for Detection Matrix completion. */
    Semaphore_Handle EDMA_DetMatrix_semHandle;
#endif

#ifdef EDMA_MATRIX2_INPUT_BLOCKING
    /*! @brief Semaphore handle for Detection Matrix2 completion. */
    Semaphore_Handle EDMA_DetMatrix2_semHandle;
#endif

#ifdef EDMA_3D_INPUT_BLOCKING
    /*! @brief Semaphore handle for 3D EDMA completion. */
    Semaphore_Handle EDMA_3D_InputDone_semHandle[2];
#endif

    /*! @brief  Used for checking that chirp processing finshed on time */
    int8_t chirpProcToken;

    /*! @brief  Used for checking that inter frame processing finshed on time */
    int8_t interFrameProcToken;

} MmwDemo_DSS_dataPathContext_t;


/**
 * @brief
 *  Millimeter Wave Demo Data Path CQ configuration.
 *
 * @details
 *  The structure is used to hold all the relevant information for
 *  the data path CQ.
 */
typedef struct // MmwDemo_DSS_DataPathCQConfig_t
{
    /*! @brief Pointer to the CQ RX Saturation monitor configuration */
    // rlRxSatMonConf_t        *rxSatMonCfg;

    /*! @brief Pointer to the CQ Signal and image band monitor configuration */
    // rlSigImgMonConf_t       *sigImgMonCfg;

    /*! @brief Pointer to the CQ Signal and image band monitor configuration */
    // MmwDemo_AnaMonitorCfg   *anaMonCfg;

    /*! @brief CQ signal & image band monitor buffer physical address */
    uint32_t                sigImgMonAddr;

    /*! @brief CQ RX Saturation monitor buffer physical address */
    uint32_t                satMonAddr;

    /*! @brief CQ signal & image band monitor data size per chirp */
    uint32_t                sigImgMonDataSizePerChirp;

    /*! @brief CQ RX Saturation monitor data size per chirp */
    uint32_t                satMonDataSizePerChirp;

    /*! @brief CQ signal & image band monitor buffer size */
    uint32_t                sigImgMonTotalSize;

    /*! @brief CQ RX Saturation monitor buffer size */
    uint32_t                satMonTotalSize;

    /*! @brief CQ RX signal & image band monitor EDMA complete counter */
    uint32_t                sigImgEdmaCnt;

    /*! @brief CQ RX signal & image band monitor EDMA error counter */
    uint32_t                sigImgErrCnt;

    /*! @brief CQ RX Saturation monitor EDMA complete counter */
    uint32_t                rxSatEdmaCnt;

    /*! @brief CQ RX Saturation monitor EDMA error counter */
    uint32_t                rxSatErrCnt;

    /*! @brief CQ RX Saturation monitor data */
    void*                   rxSatData;

    /*! @brief CQ signal & image band monitor data */
    void*                   sigImgData;
}MmwDemo_DSS_DataPathCQ;


/*  @details
 *  The structure is used to hold all the relevant information for the data path.
 */
typedef struct // MmwDemo_DSS_DataPathObj_t
{
    /*! @brief Pointer to mmw demo configuration */
    MmwDemo_Cfg *cfg;

    /*! @brief Pointer to common context across data path objects */
    MmwDemo_DSS_dataPathContext_t *context;

    /*! @brief Pointer to cli config */
    MmwDemo_CliCfg_t *cliCfg;

    /*! @brief Pointer to cli config common to all subframes*/
    MmwDemo_CliCommonCfg_t *cliCommonCfg;

    /*! @brief   Number of receive channels */
    uint8_t numRxAntennas;

    /*! @brief pointer to ADC buffer */
    cmplx16ReIm_t *ADCdataBuf;

    /*! @brief twiddle table for 1D FFT */
    cmplx16ReIm_t *twiddle16x16_1D;

    /*! @brief window coefficients for 1D FFT */
    int16_t *window1D;

    /*! @brief ADCBUF input samples in L2 scratch memory */
    cmplx16ReIm_t *adcDataIn;

    /*! @brief 1D FFT output */
    cmplx16ReIm_t *fftOut1D;

    /*! @brief twiddle table for 2D FFT */
    cmplx32ReIm_t *twiddle32x32_2D;

    /*! @brief ping pong buffer for 2D from radar Cube */
    cmplx16ReIm_t *dstPingPong;

    /*! @brief window output for 2D FFT */
    cmplx32ReIm_t *windowingBuf2D;
	
    /*! @brief log2 absolute computation output buffer */
    uint16_t *log2Abs;

    /*! @brief accumulated sum of log2 absolute over the antennae */
    uint16_t *sumAbs;

    /*! @brief input buffer for CFAR processing from the detection matrix */
    uint16_t *sumAbsRange;

    /*! @brief CFAR output objects index buffer */
    uint16_t *cfarDetObjIndexBuf;
	
    /*! @brief output of Azimuth FFT magnitude squared */
    float   *azimuthMagSqr;

    /*! @brief Pointer to DC range signature compensation buffer */
    cmplx32ReIm_t *dcRangeSigMean;

    /*! @brief DC range signature calibration counter */
    uint16_t dcRangeSigCalibCntr;

    /*! @brief log2 of number of averaged chirps */
    uint8_t log2NumAvgChirps;

    /*! @brief Half bin needed for doppler correction as part of Azimuth processing */
    cmplx16ImRe_t azimuthModCoefsHalfBin;

    /*! @brief Pointer to Radar Cube memory in L3 RAM */
    cmplx16ReIm_t *radarCube;

    /*! @brief Pointer to range/Doppler log2 magnitude detection matrix in L3 RAM */
    uint16_t *detMatrix;

    /*! @brief Pointer to 2D FFT array in range direction, at doppler index 0, for static azimuth heat map */
    cmplx16ReIm_t *azimuthStaticHeatMap;

    /*! @brief valid Profile index */
    uint8_t validProfileIdx;

    /*! @brief number of transmit antennas */
    uint8_t numTxAntennas;

    /*! @brief number of virtual antennas */
    uint8_t numVirtualAntennas;

    /*! @brief number of virtual azimuth antennas */
    uint8_t numVirtualAntAzim;

    /*! @brief number of virtual elevation antennas */
    uint8_t numVirtualAntElev;

    /*! @brief number of ADC samples */
    uint16_t numAdcSamples;

    /*! @brief number of range bins */
    uint16_t numRangeBins;

    /*! @brief number of chirps per frame */
    uint16_t numChirpsPerFrame;

    /*! @brief number of angle bins */
    uint16_t numAngleBins;

    /*! @brief number of doppler bins */
    uint16_t numDopplerBins;

    /*! @brief number of doppler bins */
    uint8_t log2NumDopplerBins;

    /*! @brief range resolution in meters */
    float rangeResolution;

    /*! @brief Q format of the output x/y/z coordinates */
    uint8_t xyzOutputQFormat;

    /*! @brief Number of detected objects */
    uint16_t numDetObj;

    /*! @brief Number of detected objects */
    uint16_t numDetObjRaw;

    /*! @brief Detected Doppler lines */
    // MmwDemo_1D_DopplerLines_t detDopplerLines;

    /*! @brief Detected objects azimuth index for debugging */
    // uint8_t *detObj2dAzimIdx;

    /*! @brief Timing information */
    // MmwDemo_timingInfo_t timingInfo;

    /*! @brief chirp counter modulo number of chirps per frame */
    uint16_t chirpCount;

    /*! @brief chirp counter modulo number of tx antennas */
    uint8_t txAntennaCount;

    /*! @brief chirp counter modulo number of Doppler bins */
    uint16_t dopplerBinCount;

    /*! @brief  DSP cycles for chirp and interframe processing and pending on EDMA data transferes*/
    // cycleLog_t cycleLog;

    /*! @brief ADCBUF will generate chirp interrupt event every this many chirps */
    uint16_t   numChirpsPerChirpEvent;

    /*! @brief number of bytes per ADC sample in ADC buffer  */
    uint8_t    numBytePerSample;

    /*! @brief Rx channel gain/phase offset compensation coefficients */
    MmwDemo_compRxChannelBiasCfg_t compRxChanCfg;

    /*! @brief subframe index for this obj */
    uint8_t subFrameIndx;

    /*! @brief Rx channel Chirp Quality config & data */
    MmwDemo_DSS_DataPathCQ          datapathCQ;
} MmwDemo_DSS_DataPathObj;



typedef struct _CPLX16 {
	int16_t	real;
	int16_t	imag;
} cplx16_t;


typedef struct _CPLXF {
	float	real;
	float	imag;
} cplxf_t;


typedef struct ODDemo_DataPathObj_t
{
    uint32_t       nRxAnt;                //!< number of receive antennas.
    uint32_t       steeringVecSize;       /**< size of -estRange:estResolution:estRange*/
    cplxf_t       *steeringVec;           //!< steering vector for angle: -estRange:estResolution:estRange, for nRxAnt antennas, must be aligned to 8-byte boundary
    float          estAngleResolution;    /**< Estimation resolution in degree.*/
    float          estAngleRange;         /**< Range of the estimation, from -estRange to +estRange degrees*/
    uint8_t        clutterRemovalFlag;    /**< flag to indicate clutter removal needed. */
    uint8_t        fallBackToConvBFFlag;  /**< Flag to indicate falling back to conventional BF using covariance matrix if set to 1. */
    uint32_t       nChirps;               /**< number of chirps to be used for covariance matrix estimation.*/
    uint32_t       log2Chirps;            /**< log2 of .nChirps */
    cmplx16ReIm_t *inputAntSamples;       /**< input samples after range processing for rangeBin rangeIndx, array in format of (nRxAnt * nChirps).*/
    uint32_t      *radarCube;             // L3 Radar Cube - must be aligned on 8-byte boundary
    uint32_t      *scratchPad;            //!< Pointer to the scratch memory used in this function, must have length sizeof(uint32_t) * (5 * steeringVecSize), must be aligned to 8-byte boundary
    cplxf_t       *invRnMatrix;           /**< Pointer to covariance matrix memory, upper triangle of nRxAnt x nRxAnt Hermitian matrix.*/
    float         *rangeAzimuthHeatMap;   /**< output range azimuth heatmap, array in format of (numInputRangeBins * nAzimuthBins).*/
    cplxf_t       *invRnMatrix_VS;        /**< Pointer to covariance matrix memory, upper triangle of nRxAnt x nRxAnt Hermitian matrix.*/
    float         *bfOutput;              //Beamforming output for the current range bin and azimuth bin
} ODDemo_DataPathObj;



void MmwDemo_dataPathObjInit(MmwDemo_DSS_DataPathObj *obj,
                             MmwDemo_DSS_dataPathContext_t *context,
                             MmwDemo_CliCfg_t *cliCfg,
                             MmwDemo_CliCommonCfg_t *cliCommonCfg,
                             MmwDemo_Cfg *cfg);

void MmwDemo_dataPathInit1Dstate(MmwDemo_DSS_DataPathObj *obj);

uint32_t MmwDemo_pow2roundup (uint32_t x);
void MmwDemo_dataPathConfigFFTs(MmwDemo_DSS_DataPathObj *obj);

/**
 *  @b Description
 *  @n
 *   Creates heap in L2 and L3 and allocates data path buffers,
 *   The heap is destroyed at the end of the function.
 *
 *  @retval
 *      Not Applicable.
 */
// void MmwDemo_dataPathConfigBuffers(MmwDemo_DSS_DataPathObj *obj, uint32_t adcBufAddress, VitalSigns_DataPathObj *obj_VS_1, VitalSigns_DataPathObj *obj_VS_2);

void MmwDemo_processChirp (MmwDemo_DSS_DataPathObj *obj, uint16_t chirpIndxInMultiChirp);

#if 0 // 240831_LEEYJ, Test for "ODDemo_Decision_process" 
void MmwDemo_interFrameProcessing (void); 
#else
void MmwDemo_interFrameProcessing(MmwDemo_DSS_DataPathObj *obj);
#endif
void MmwDemo_dataPathConfigBuffers (MmwDemo_DSS_DataPathObj *obj);

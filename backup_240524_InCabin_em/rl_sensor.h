
/*! \brief
 * Standard C data types typedef
 */

#pragma once
typedef unsigned char                   rlUInt8_t;
typedef unsigned short                  rlUInt16_t;
typedef unsigned int                    rlUInt32_t;
typedef unsigned long long              rlUInt64_t;
typedef signed long long                rlInt64_t;
typedef char                            rlInt8_t;
typedef signed char                     rlSInt8_t;
typedef signed short                    rlInt16_t;
typedef signed int                      rlInt32_t;
typedef void                            rlVoid_t;


/*! \brief
* Supported maximum number of subframes in a frame
*/
#define RL_MAX_SUBFRAMES             (4U)
#define RL_MAX_PROFILES_CNT          (4U)
#define RL_MAX_CALIB_DATA_CHUNK       (3U)

#define RL_TX_CNT                     (3U)

/*
	- rlChanCfg		/ rlChanCfg_t
	- rlFrameCfg	/ rlFrameCfg_t
	- rlLowPowerModeCfg	/ rlLowPowerModeCfg_t
	- rlAdcBitFormat	/ rlAdcBitFormat_t
	- rlAdcOutCfg		/ rlAdcOutCfg_t
	- rlSubFrameCfg		/ rlSubFrameCfg_t
	- rlAdvFrameSeqCfg	/ rlAdvFrameSeqCfg_t
	- rlSubFrameDataCfg	/ rlSubFrameDataCfg_t
	- rlAdvFrameDataCfg	/ rlAdvFrameDataCfg_t
	- rlAdvFrameCfg		/ rlAdvFrameCfg_t
*/


typedef struct rlBpmChirpCfg
{
    /**
     * @brief  Chirp Start Index, Valid Range 0 -511
     */
    rlUInt16_t chirpStartIdx;
    /**
     * @brief  Chirp End Index, Valid Range from chirpStartIdx to 511
     */
    rlUInt16_t chirpEndIdx;
    /**
     * @brief  b0  CONST_BPM_VAL_TX0_TXOFF \n
                     Value of Binary Phase Shift value for TX0, when during idle time \n
                     b1  CONST_BPM_VAL_TX0_TXON \n
                     Value of Binary Phase Shift value for TX0, during chirp \n
                     b2  CONST_BPM_VAL_TX1_TXOFF \n
                     Value of Binary Phase Shift value for TX1, when during idle time \n
                     b3  CONST_BPM_VAL_TX1_TXON \n
                     Value of Binary Phase Shift value for TX1, during chirp \n
                     b4  CONST_BPM_VAL_TX2_TXOFF \n
                     Value of Binary Phase Shift value for TX2, when during idle time \n
                     b5  CONST_BPM_VAL_TX2_TXON \n
                     Value of Binary Phase Shift value for TX2, during chirp \n
                     b15:6 Reserved \n
     */
    rlUInt16_t constBpmVal;
    /**
     * @brief  Reserved for future
     */
    rlUInt16_t reserved;
}rlBpmChirpCfg_t;


// Rx/Tx Channel Configuration
typedef struct rlChanCfg
{
    /** @ brief  RX Channel Bitmap \n
                b0 RX0 Channel Enable \n
                0   Disable RX Channel 0 \n
                1   Enable RX Channel 0 \n
                b1 RX1 Channel Enable \n
                0   Disable RX Channel 1 \n
                1   Enable RX Channel 1 \n
                b2 RX2 Channel Enable \n
                0   Disable RX Channel 2 \n
                1   Enable RX Channel 2 \n
                b3 RX3 Channel Enable \n
                0   Disable RX Channel 3 \n
                1   Enable RX Channel 3 \n
                b15:4 - RESERVED \n     */
    rlUInt16_t rxChannelEn;

    /** @brief  TX Channel Bitmap \n
                b0 TX0 Channel Enable \n
                0   Disable TX Channel 0 \n
                1   Enable TX Channel 0\n
                b1 TX1 Channel Enable \n
                0   Disable TX Channel 1 \n
                1   Enable TX Channel 1 \n
                b2 TX2 Channel Enable \n
                0   Disable TX Channel 2 \n
                1   Enable TX Channel 2 \n
                b15:3 - RESERVED \n
                @note : TX2 is supported only on some of the devices, Please refer device data \n
                        sheet.     */
    rlUInt16_t txChannelEn;

    /**
     * @brief  Enable Cascading \n
                 0x0000 SINGLECHIP: Single mmWave sensor application\n
                 0x0001 MULTICHIP_MASTER: Multiple mmwave sensor application. This mmwave is \n
                 the master chip and generates LO and conveys to other mmwave sensor. \n
                 This is applicable only in AWR1243.\n
                 0x0002 MULTICHIP_SLAVE: Multiple mmwave sensor application. This mmwave is a \n
                 slave chip and uses LO conveyed to it by the master mmwave sensor. \n
                 This is applicable only in AWR1243 \n
                 @note : Please refer device data sheet for cascading capability and 20G SYNC
                         pins  */
    rlUInt16_t cascading;

    /* @brief  Cascading pinout config
     *         b0 - CLKOUT_MASTER_DIS
     *              0 - 20G FM_CW_CLKOUT from master is enabled
     *              1 - 20G FM_CW_CLKOUT from master is disabled
     *         b1 - SYNCOUT_MASTER_DIS
     *              0 - 20G FM_CW_SYNCOUT from master is enabled
     *              1 - 20G FM_CW_SYNCOUT from master is disabled
     *         b2 - CLKOUT_SLAVE_EN
     *              0 - 20G FM_CW_CLKOUT from slave is disabled
     *              1 - 20G FM_CW_CLKOUT from slave is enabled
     *         b3 - SYNCOUT_SLAVE_EN
     *              0 - 20G FM_CW_SYNCOUT from slave is disabled
     *              1 - 20G FM_CW_SYNCOUT from slave is enabled
     *         b4 - INTLO_MASTER_EN
     *              0 - Master uses looped back LO
     *              1 - Master uses internal LO
     *         b5 - OSCCLKOUT_MASTER_DIS
     *              0 - OSCCLKOUT from master is enabled
     *              1 - OSCCLKOUT from master is disabled
     *      b15:6 - RESERVED     */
    rlUInt16_t cascadingPinoutCfg;
}rlChanCfg_t;


//	Frame config API parameters
typedef struct rlFrameCfg
{
    rlUInt16_t reserved0;
    /**
     * @brief  Start Index of Chirp Valid range = 0-511
     */
    rlUInt16_t chirpStartIdx;
    /**
     * @brief  End Index of Chirp Valid range = chirpStartIdx-511
     */
    rlUInt16_t chirpEndIdx;
    /**
     * @brief  Number of times to repeat from chirpStartIdx to chirpStartIdx in each frame, \n
                   valid range = 1 to 255 \n
     */
    rlUInt16_t numLoops;
    /**
     * @brief  Number of frame to transmit \n
                Valid Range 0 to 65535 (0 for infinite frames) \n
     */
    rlUInt16_t numFrames;
    /**
     * @brief  Number of ADC samples to capture This parameter is required only for AWR1243 and \n
                configures the size of ADC samples per chirp to be sent on LVDS/CSI2 \n
                interface.\n
                It should be configured as:
                For real/pseudo real ADC data - numADCSamples = rlProfileCfg_t.numAdcSamples \n
                For complex1x/complex2x ADC data -  \n
                numADCSamples = (rlProfileCfg_t.numAdcSamples ) * 2 \n
     */
    rlUInt16_t numAdcSamples;
    /**
     * @brief  Frame repitition period \n
                   1 LSB = 5 ns\n
                   Typical range 1 ms to 1000 ms \n
     */
    rlUInt32_t framePeriodicity;
    /**
     * @brief  Selects the mode for triggering start of transmission of frame \n
                   0x0001 SWTRIGGER (Software API based triggering): \n
                   Frame is triggered upon receiving rlSensorStart\n
                   There could be several tens of micro seconds uncertainty in triggering. \n
                   Note: This mode is not applicable if this mmwave device is configured as \n
                   MULTICHIP_SLAVE in rlChanCfg_t \n
                   0x0002 HWTRIGGER (Hardware SYNC_IN based triggering): \n
                   Each frame is triggered by rising edge of pulse in SYNC_IN pin, after
                   receiving rlSensorStart (this is to prevent spurious transmission). \n
                   W.r.t. the SYNC_IN pulse, the actual transmission has 160ns delay and 5ns
                   uncertainty in SINGLECHIP and only a 300 ps uncertainty (due to tight
                   inter-chip synchronization needed) in MULTICHIP sensor applications as defined
                   in rlChanCfg_t. \n
     */
    rlUInt16_t triggerSelect;
#ifndef MMWL_BIG_ENDIAN
    rlUInt8_t reserved1;
    /**
     * @brief  Number of dummy chirps which will run in the rampgen at the end of the frame. The
     *         ADC data, CP and CQ data for these chirps will not be sent out from the \n
     *         device/DFE. \n
     *         1LSB = 1 dummy chirp in the end of frame. \n
     *         valid range: 0 to 128 (should be less than the total number of chirps in a \n
     *         frame). \n
     */
    rlUInt8_t numDummyChirpsAtEnd;
#else
    /**
     * @brief  Number of dummy chirps which will run in the rampgen at the end of the frame. The ADC data, CP & CQ
     *         for these chirps will not be sent out from the device/DFE. 1LSB = 1 dummy chirp in the end of frame.
     *         valid range: 0 to 128 (should be less than the total number of chirps in a frame).
     */
    rlUInt8_t numDummyChirpsAtEnd;
    rlUInt8_t reserved1;
#endif
    /*		Optional time delay from sync_in trigger to the occurance of frame chirps Applicable only 
		in SINGLECHIP sensor applications (as defined in rlChanCfg_t).
			It is recommended only for staggering the transmission of multiple mmwave sensors around the car 
		for interference avoidance, if needed (1 lsb = 5 ns).
		Typical range is 0 to 100 micro seconds.
     */
    rlUInt32_t frameTriggerDelay;
}rlFrameCfg_t;


// Low Power mode configuration
typedef struct // rlLowPowerModeCfg
{
    rlUInt16_t reserved;
    rlUInt16_t lpAdcMode;	// ADC Mode :	- 0x00 : Regular ADC mode
							//				- 0x01 : Low poer ADC mode
}rlLowPowerModeCfg_t;


// ADC Bit and ADC Output format Configuration
#ifndef MMWL_BIG_ENDIAN
typedef struct // rlAdcBitFormat
{
    rlUInt32_t b2AdcBits			:2;	//  ADC out bits - 0(12 Bits), 1(14 Bits), 2(16 Bits)
    rlUInt32_t b6Reserved0			:6;
    /**
     * @brief Number of bits to reduce ADC full scale by
               Valid range: 0 to (16 - Number of ADC bits)
               For e.g. for 12 bit ADC output, this field can take values 0, 1, 2 or 3 \n
               For 14 bit ADC output, this field can take values 0, 1 or 2 \n
               For 16 bit ADC output, this field can take only value 0 \n
      */
    rlUInt32_t b8FullScaleReducFctr	:8;
    rlUInt32_t b2AdcOutFmt			:2;	//ADC out format- 0(Real), 1(Complex), 2(Complex with Image band), 3(Pseudo Real)
    rlUInt32_t b14Reserved1			:14;
}rlAdcBitFormat_t;
#else
typedef struct // rlAdcBitFormat
{
    /**
     * @brief  Reserved for Future use
     */
    rlUInt32_t b14Reserved1        :14;
    /**
     * @brief  ADC out format- 0(Real), 1(Complex), 2(Complex with Image band), 3(Pseudo Real)
     */
    rlUInt32_t b2AdcOutFmt         :2;
    /**
     * @brief Number of bits to reduce ADC full scale by
               Valid range: 0 to (16 - Number of ADC bits)
               For e.g. for 12 bit ADC output, this field can take values 0, 1, 2 or 3 \n
               For 14 bit ADC output, this field can take values 0, 1 or 2 \n
               For 16 bit ADC output, this field can take only value 0 \n
      */
    rlUInt32_t b8FullScaleReducFctr:8;
    /**
     * @brief  Reserved for Future use
     */
    rlUInt32_t b6Reserved0        :6;
    /**
     * @brief  ADC out bits - 0(12 Bits), 1(14 Bits), 2(16 Bits)
     */
    rlUInt32_t b2AdcBits           :2;
}rlAdcBitFormat_t;
#endif


// ADC format and payload justification Configuration
typedef struct // rlAdcOutCfg
{
    rlAdcBitFormat_t fmt;		// ADC Data format

}rlAdcOutCfg_t;


//		Advance Frame config API Subframe configuration
typedef struct // rlSubFrameCfg
{
    rlUInt16_t forceProfileIdx;		// Force profile index. This is applicable only if FORCE_SINGLE_PROFILE is set to 1
    rlUInt16_t chirpStartIdx;		// Start Index of Chirp Valid range = 0-511
    rlUInt16_t numOfChirps;			// Num of unique Chirps per burst including start index Valid range = 1 - 512
    rlUInt16_t numLoops;			//  No. of times to loop through the unique chirps in each burst, without gaps,
									// using HW.\n valid range = 1 to 255 \n
    rlUInt32_t burstPeriodicity;	// burstPeriodicity >=  (numLoops)* (sum total of all unique chirp times per burst) + InterBurstBlankTime.
									/* where InterBurstBlankTime is primarily for sensor calibration / monitoring, \n
									thermal control, and some minimum time needed for triggering next burst. \n
									InterBurstBlankTime >= 250 us. \n
									With loop back enabled, InterBurstBlankTime >= 500 us \n
									@note : Across bursts, if the value numOfChirps, is not a constant, then the \n
									actual available blank time can vary and needs to be accounted for \n
									1 LSB = 5 ns \n
									*/ 
    /**
     * @brief  Chirp Start address increment for next burst, \n
                   next_burst_chirp_start_idx = last_chirp_end_index + \n
                   h_ChirpStartIdxIncr 0 - 511, 0 = 2nd burst = 1st burst \n
     */
    rlUInt16_t chirpStartIdxOffset;
    /**
     * @brief  Num of bursts in the sub-frame. Valid Range: 1 - 512
     */
    rlUInt16_t numOfBurst;
    /**
     * @brief  Number of times to loop over the set of above defined bursts, in the sub frame.\n
                    Valid Range: 1 - 64 \n
     */
    rlUInt16_t numOfBurstLoops;
    /**
     * @brief  Reserved for Future use
     */
    rlUInt16_t reserved0;
    /**
     * @brief  subFramePeriodicity >= Sum total time of all bursts + InterSubFrameBlankTime, \n
                 Where, Sum total time of all bursts = numOfBurstLoops * numOfBurst * \n
                 burstPeriodicity. \n
                 InterSubFrameBlankTime is primarily for sensor calibration / monitoring, \n
                 thermal control, transferring out any safety monitoring data if requested, \n
                 hardware reconfiguration for next sub frame, re-triggering of next SF (~10us).\n
                 The hardware reconfiguration time, which is needed to transfer out the SF's \n
                 InterSubFrameBlankTime 250 us. \n
                 With loop back enabled InterSubFrameBlankTime 500 us. \n
                 Add 150 us to InterSubFrameBlankTime for test source configuration if test \n
                 source is enabled. \n
                 1 LSB = 5 ns \n
                 Valid range 100 us to 1.342 s. \n
     */
    rlUInt32_t subFramePeriodicity;
    /**
     * @brief  Reserved for Future use
     */
    rlUInt32_t reserved1;
    /**
     * @brief  Reserved for Future use
     */
    rlUInt32_t reserved2;
}rlSubFrameCfg_t;


//	Advance Frame Sequence config API parameters rlAdvFrameCfg, 148 bytes
typedef struct // rlAdvFrameSeqCfg
{
#ifndef MMWL_BIG_ENDIAN
    /**
     * @brief  Number of sub frames enabled in this frame Valid range: 1 to 4
     */
    rlUInt8_t          numOfSubFrames;
    /**
     * @brief  Force profile. \n
                0x0 The profile index set in rlSetChirpConfig API governs which profile is \n
                used when that chirp is transmitted \n
                0x1 The profile index indicated in rlSetChirpConfig API is ignored and all \n
                the chirps in each sub frame use a single profile as indicated by that sub \n
                frame's profile forceProfileIdx. \n
     */
    rlUInt8_t          forceProfile;
    /**
     * @brief Bit  Definition \n
              b0   LOOPBACK_CFG_EN \n
                    0 - Disable \n
                    1 - Enable  \n
              b2:1  SUB_FRAME_ID for which the loopback configuration applies \n
              b7:3  RESERVED \n
     */
    rlUInt8_t          loopBackCfg;
    /**
     * @brief  Sub frame trigger
     *              0 - Disabled
     *              1 - Enabled (Need to trigger each sub-frame either by SW in \n
     *                  software triggered mode or HW SYNC_IN line in hardware triggered mode)
     */
    rlUInt8_t          subFrameTrigger;
#else
    /**
     * @brief  Force profile.
                0x0 The profile index set in rlSetChirpConfig API governs which profile is \n
                used when that chirp is transmitted \n
                0x1 The profile index indicated in rlSetChirpConfig API is ignored and all \n
                the chirps in each sub frame use a single profile as indicated by that sub \n
                frame's profile forceProfileIdx. \n
     */
    rlUInt8_t          forceProfile;
    /**
     * @brief  Number of sub frames enabled in this frame Valid range: 1 to 4
     */
    rlUInt8_t          numOfSubFrames;
    /**
     * @brief  Sub frame trigger
     *              0 - Disabled
     *              1 - Enabled (Need to trigger each sub-frame either by SW in \n
     *                  software triggered mode or HW SYNC_IN line in hardware triggered mode)
     */
    rlUInt8_t          subFrameTrigger;
    /**
     * @brief Bit  Definition \n
              b0   LOOPBACK_CFG_EN \n
                    0 - Disable \n
                    1 - Enable  \n
              b2:1  SUB_FRAME_ID for which the loopback configuration applies \n
              b7:3  RESERVED \n
     */
    rlUInt8_t          loopBackCfg;
#endif
    /**
     * @brief  Subframe config for 4 sub frames
     */
    rlSubFrameCfg_t    subFrameCfg[RL_MAX_SUBFRAMES];
    /**
     * @brief  Number of frames to transmit (1 frame = all enabled sub frames). If set to 0, \n
                frames are transmitted endlessly till Frame Stop message is received.\n
                Valid range: 0 to 65535 \n
     */
    rlUInt16_t         numFrames;
    /**
     * @brief  Selects the mode for triggering start of transmission of frame \n
                 0x0001 SWTRIGGER (Software API based triggering): \n
                   Frame is triggered upon receiving rlSensorStart There could be several tens \n
                   of micro seconds uncertainty in triggering. \n
                   Note:This mode is not applicable if this mmwave device is configured as \n
                   MULTICHIP_SLAVE in rlChanCfg_t \n
                 0x0002 HWTRIGGER (Hardware SYNC_IN based triggering): \n
                   Each frame is triggered by rising edge of pulse in SYNC_IN pin, after \n
                   receiving rlSensorStart (this is to \n prevent spurious transmission). \n
                   W.r.t. the SYNC_IN pulse, the actual transmission has 5ns uncertainty in \n
                   SINGLECHIP and only a 300 ps uncertainty (due to tight inter-chip \n
                   synchronization needed) in MULTICHIP sensor applications as defined in \n
                   rlChanCfg_t. \n
     */
    rlUInt16_t         triggerSelect;
    /**
     * @brief  Optional time delay from sync_in trigger to the occurance of frame chirps \n
                Applicable only in SINGLECHIP sensor applications, as defined in rlChanCfg_t.\n
                It is recommended only for staggering the transmission of multiple mmwave \n
                sensors around the car for interference avoidance, if needed. \n
                1 lsb = 5 ns \n
                Typical range is 0 to few tens of micro seconds. \n
     */
    rlUInt32_t         frameTrigDelay;
    /**
     * @brief  Reserved for Future use
     */
    rlUInt32_t         reserved0;
    /**
     * @brief  Reserved for Future use
     */
    rlUInt32_t         reserved1;
}rlAdvFrameSeqCfg_t;


// Sub Frame data config API parameters
typedef struct // rlSubFrameDataCfg
{
	rlUInt32_t totalChirps;			// Number of Chirps in Sub-Frame = numOfChirps * numLoops * numOfBurst * burstLoop

	// @brief  Number of half words of ADC samples per data packet in sub frame 1 \n
	//           Example 1:  In real mode, if number of ADC samples per chirp in subframe1 is
	//           256 then this value will be 256 \n
    //           Example 2: In complex1x or complex2x modes, if number of ADC samples per
    //           chirp in subframe1 is 256 then this value will be 512 \n
    //           In AWR1243/xWR1443: Program this as number of ADC samples in each chirp of
    //           this sub frame (required to be the same) \n
    //           Exception: Can do number of chirps based ping-pong as in xWR1642
    //           (see below), if CP/CQ are not needed. Useful for chirp stitching use case. \n
    //           In xWR1642/xWR1843(For reference only): The ADC samples corresponding to one or
    //           more chirps can be grouped and sent to the DSP as a single packet. Program this
    //           as the number of half words of ADC samples per packet. Ensure that in one sub
    //           frame, there is integer number of such packets. \n
    //           Maximum size of a data packet: (16384 - 1) half words
    rlUInt16_t numAdcSamples;

	#ifndef MMWL_BIG_ENDIAN
    // @brief  Number of Chirps Per Data Packet to process at a time in sub frame 1. \n
    //            In AWR1243/xWR1443: Program this as 1. \n
    //            Exception: Can be > 1 as in 16xx if CP/CQ is not needed. Useful for chirp
    //            stitching use case. \n
    //            In xWR1642/xWR1843(For reference only): The ADC samples corresponding to one or
    //            more chirps can be grouped and sent to the DSP as a single packet. Program this
    //            as the corresponding number of chirps per packet. \n
    //            Maximum value = 8. \n
    //            Note on maximum size: 8 chirps for CP and BPM.
    rlUInt8_t numChirpsInDataPacket;

    rlUInt8_t reserved;
	#else
    rlUInt8_t reserved;

    // @brief  Number of Chirps Per Data Packet to process at a time in sub frame 1. \n
    //            In AWR1243/xWR1443: Program this as 1. \n
    //            Exception: Can be > 1 as in 16xx if CP/CQ is not needed. Useful for chirp
    //            stitching use case.\n
    //            In xWR1642/xWR1843: The ADC samples corresponding to one or more chirps can be
    //            grouped and sent to the DSP as a single packet. Program this as the
    //            corresponding number of chirps per packet. \n
    //            Maximum value = 8. \n
    //            Note on maximum size: 8 chirps for CP and BPM. \n
    rlUInt8_t numChirpsInDataPacket;
	#endif
}rlSubFrameDataCfg_t;

/**		  Advance Frame data config API parameters.
*		This strcutre is only applicable when mmWaveLink instance is running on
*		External Host and connected to AWR1243 device.	--- */
typedef struct // rlAdvFrameDataCfg
{
	#ifndef MMWL_BIG_ENDIAN
    rlUInt8_t numSubFrames;			//  Number of Sub Frames, Valid Range (1 - 4)
    rlUInt8_t reserved0;
	#else
    rlUInt8_t reserved0;			
    rlUInt8_t numSubFrames;			//  Number of Sub Frames, Valid Range (1 - 4)
	#endif

    rlUInt16_t reserved1;
    rlSubFrameDataCfg_t subframeDataCfg[RL_MAX_SUBFRAMES];	// Sub Frame data configuration array.
}rlAdvFrameDataCfg_t;


// Advance Frame Sequence config API parameters rlAdvFrameCfg, 148 bytes
typedef struct // rlAdvFrameCfg
{
    //  Advance Frame sequence and Subframe configuration
    rlAdvFrameSeqCfg_t frameSeq;
    //  Advance Frame data configuration. Applicable with AWR1243 device only
    rlAdvFrameDataCfg_t frameData;
} rlAdvFrameCfg_t;


typedef struct rlChirpCfg
{
    /**
     * @brief  Chirp start index (0-511)
     */
    rlUInt16_t chirpStartIdx;
    /**
     * @brief  Chirp end index (0-511)
     */
    rlUInt16_t chirpEndIdx;
    /**
     * @brief  Associated profile id (0-3)
     */
    rlUInt16_t profileId;
    /**
     * @brief  Reserved for Future use
     */
    rlUInt16_t reserved;
    /**
     * @brief  Ramp start frequency, \n
               For 77GHz devices(76GHz to 81GHz): \n
                   1 LSB = 3.6e9/2^26 = 53.644 Hz \n
                   valid range = 0-8388607 \n
               For 60GHz devices(57GHz to 64GHz): \n
                   1 LSB = 2.7e9/2^26 = 40.233 Hz \n
                   valid range = 0-8388607 \n
     */
    rlUInt32_t startFreqVar;
    /**
     * @brief  Ramp slope\n
               For 77GHz devices(76GHz to 81GHz): \n
                   1 LSB = 3.6e6 * 900/2^26 = 48.279 KHz/us \n
                   valid range = 0-63 \n
               For 60GHz devices(57GHz to 64GHz): \n
                   1 LSB = 2.7e6 * 900/2^26 = 36.21 KHz/us \n
                   valid range = 0-63 \n
     */
    rlUInt16_t freqSlopeVar;
    /**
     * @brief  Idle time for each chirp\n
                   1 LSB = 10ns\n
                   valid range = 0-4096 \n
     */
    rlUInt16_t idleTimeVar;
    /**
     * @brief  Adc start time for each chirp \n
                   1 LSB = 10ns \n
                   valid range = 0-4096 \n
     */
    rlUInt16_t adcStartTimeVar;
    /**
     * @brief  Tx enable selection bit mask \n
                   b0 Enable TX0 \n
                   0 Tx0 Disable \n
                   1 Tx0 Enable \n
                   b1 Enable TX1 \n
                   0 Tx1 Disable \n
                   1 Tx1 Enable \n
                   b2 Enable TX2 \n
                   0 Tx2 Disable \n
                   1 Tx2 Enable  \n
                   Note: Maximum of only 2 TX can be turned in a chirp \n
     */
    rlUInt16_t txEnable;
}rlChirpCfg_t;


typedef struct rlProfileCfg
{
    /**
     * @brief  Profile index (0-3)
     */
    rlUInt16_t profileId;
#ifndef MMWL_BIG_ENDIAN
    /**
     * @brief  Bit Description  \n
                b0 FORCE_VCO_SEL (Not supported for production, debug purpose only) \n
                 0 - Use internal VCO selection    \n
                 1 - Forced external VCO selection \n
                b1 VCO_SEL (Not supported for production, debug purpose only) \n
                 0 - VCO1 (77G:76 - 78 GHz or 60G:57 - 61 GHz)  \n
                 1 - VCO2 (77G:77 - 81 GHz or 60G:60 - 64 GHz)  \n
                 @note 1: xWR1xxx devices: There is an overlap region of 77-78 GHz in which any \n
                          of the VCOs can be used, for other regions use only the VCO which can \n
                          work in that region. For e.g. for 76-78 GHz use only VCO1 and for \n
                          77-81GHz use only VCO2, for 77-78 GHz, any VCO can be used. Also note \n
                          that users can inter-mix chirps from different VCOs within the same \n
                          frame.
                 @note 2: IWR6843 device: There is an overlap region of 60-61 GHz in which \n
                          any of the VCOs can be used. \n
                b7:2 RESERVED            \n
     */
    rlUInt8_t pfVcoSelect;
    /**
     * @brief Bit Description       \n
               b0 RETAIN_TXCAL_LUT (Not supported in IWR6843 ES1.0) \n
                0 - Update TX calibration LUT           \n
                1 - Do not update TX calibration LUT    \n
               b1 RETAIN_RXCAL_LUT (Not supported in IWR6843 ES1.0) \n
                0 - Update RX calibration LUT and update RX IQMM correction \n
                1 - Do not update RX calibration LUT    \n
               b7:2 RESERVED                            \n
               If PF_TX_OUTPUT_POWER_BACKOFF is changed then set RETAIN_TXCAL_LUT to 0, \n
               else set it to 1 and if PF_RX_GAIN is changed, then set RETAIN_RXCAL_LUT to 0 \n
               else set them to 1.
     */
    rlUInt8_t pfCalLutUpdate;
#else
    /**
     * @brief Bit Description       \n
               b0 RETAIN_TXCAL_LUT (Not supported in IWR6843 ES1.0) \n
                0 - Update TX calibration LUT           \n
                1 - Do not update TX calibration LUT    \n
               b1 RETAIN_RXCAL_LUT (Not supported in IWR6843 ES1.0)  \n
                0 - Update RX calibration LUT           \n
                1 - Do not update TX calibration LUT    \n
               b7:2 RESERVED                            \n
               If PF_TX_OUTPUT_POWER_BACKOFF is changed then set RETAIN_TXCAL_LUT to 0, \n
               else set it to 1 and if PF_RX_GAIN is changed, then set RETAIN_RXCAL_LUT to 0 \n
               else set them to 1.
     */
    rlUInt8_t pfCalLutUpdate;
    /**
      * @brief  Bit Description  \n
               b0 FORCE_VCO_SEL (Not supported for production, debug purpose only) \n
                0 - Use internal VCO selection    \n
                1 - Forced external VCO selection \n
               b1 VCO_SEL (Not supported for production, debug purpose only) \n
                0 - VCO1 (77G:76 - 78 GHz or 60G:57 - 61 GHz)  \n
                1 - VCO2 (77G:77 - 81 GHz or 60G:60 - 64 GHz)  \n
                @note 1: xWR1xxx devices: There is an overlap region of 77-78 GHz in which any \n
                          of the VCOs can be used, for other regions use only the VCO which can \n
                          work in that region. For e.g. for 76-78 GHz use only VCO1 and for \n
                          77-81GHz use only VCO2, for 77-78 GHz, any VCO can be used. Also note \n
                          that users can inter-mix chirps from different VCOs within the same \n
                          frame.
                 @note 2: IWR6843 device: There is an overlap region of 60-61 GHz in which \n
                          any of the VCOs can be used. \n
               b7:2 RESERVED            \n
     */
    rlUInt8_t pfVcoSelect;
#endif
    /**
     * @brief  Start frequency for each profile \n
                  For 77GHz devices (76 GHz - 81 GHz): \n
                      1 LSB = 3.6e9 / 2^26 Hz = 53.644 Hz  \n
                      Valid range: 0x5471C71B to 0x5A000000 \n
                  For 60GHz devices (57 GHz - 64 GHz): \n
                      1 LSB = 2.7e9 / 2^26 Hz = 40.233 Hz  \n
                      Valid range: 0x5471C71C to 0x5ED097B4 \n
     */
    rlUInt32_t startFreqConst;
    /**
     * @brief  Idle time\n
                  1 LSB = 10 ns \n
                  Valid range: 0 to 524287 \n
     */
    rlUInt32_t idleTimeConst;
    /**
     * @brief  Time of starting of ADC capture relative to the knee of the ramp\n
                  1 LSB = 10 ns \n
                  Valid range: 0 to 4095 \n
     */
    rlUInt32_t adcStartTimeConst;
    /**
     * @brief  End of ramp time relative to the knee of the ramp\n
                   1 LSB = 10 ns\n
                   Valid range: 0 to 500000 \n
                   Ensure that the total frequency sweep is either within these ranges: \n
                   77G : 76 - 78 GHz or 77 - 81 GHz \n
                   60G : 57 - 61 GHz or 60 - 64 GHz \n
     */
    rlUInt32_t rampEndTime;
    /**
     * @brief  Concatenated code for output power backoff for TX0, TX1, TX2\n
                   Bit    Description  \n
                   b7:0   TX0 output power back off \n
                   b15:8  TX1 output power back off \n
                   b23:16 TX2 output power back off \n
                   b31:24 Reserved \n
               This field defiens how much the transmit power should be reduced from the maximum \n
               1 LSB = 1 dB \n
               If TX power boot time calibration is disabled then only 0dB back off is \n
               supported. 0dB back-off corresponds to typically 12dBm power level. \n
               In IWR6843 (both ES1.0 and ES2.0), uncalibrated back-off in 3dB steps is \n
               supported in cases where the TX power calibration is disabled. \n
               @note : For best inter-TX channel matching performance, same chirp profile and \n
                       same TX backoff value should be used for all the TXs that are used in \n
                       beam-forming
     */
    rlUInt32_t txOutPowerBackoffCode;
    /**
     * @brief  Concatenated phase shift for TX0/1/2, \n
                   Bit    Description \n
                   b1:0   Reserved(set to 0b00) \n
                   b7:2   TX0 phase shift value \n
                          1 LSB = 360/2^6 = 5.625 degrees \n
                   b9:8   Reserved(set to 0b00) \n
                   b15:10 TX1 phase shift value \n
                          1 LSB = 360/2^6 = 5.625 degrees \n
                   b17:16 Reserved(set to 0b00) \n
                   b23:18 TX2 phase shift value \n
                          1 LSB = 360/2^6 = 5.625 degrees \n
                   b31:24 Reserved(set to 0b00) \n
               This field defines the additional phase shift to be introduced on each \n
               transmitter output. In IWR6843 ES1.0, only 0 degree phase is supported. \n
               @note : Chirps corresponding to different profiles are not guaranteed to have \n
                       phase coherency.
     */
    rlUInt32_t txPhaseShifter;
    /**
     * @brief  Ramp slope frequency, \n
                   For 77GHz devices (76GHz to 81GHz): \n
                       1 LSB = (3.6e6 * 900) / 2^26 = 48.279 kHz/uS \n
                       Valid range: -2072 to 2072 (Max 100MHz/uS) \n
                   For 60GHz devices (57GHz to 64GHz): \n
                       1 LSB = (2.7e6 * 900) / 2^26 = 36.21 kHz/uS for 60GHz devices \n
                       Valid range: -6905 to 6905 (Max 250 MHz/uS) \n
     */
    rlInt16_t freqSlopeConst;
    /**
     * @brief  Time of start of transmitter relative to the knee of the ramp \n
                   1 LSB = 10ns \n
                   Valid range: -4096 to 4095 \n
     */
    rlInt16_t txStartTime;
    /**
     * @brief  Number of ADC samples to capture in a chirp for  each RX \n
                   Valid range: 2 to MAX_NUM_SAMPLES\n
                   Where MAX_NUM_SAMPLES is such that all the enabled RX channels' data fits \n
                   into 16 kB memory memory in AWR1243/xWR1443 or 32 kB memory in xWR1642/ \n
                   xWR6843/xWR1843, with each sample consuming 2 bytes for real ADC output \n
                   case and 4 bytes for complex 1x and complex 2x ADC output cases \n
                   number of RX chains    ADC format    Maxinum Number of samples \n
                    4                      complex       1024 \n
                    4                      Real          2048 \n
                    2                      Complex       2048 \n
                    2                      Real          4096 \n
     */
    rlUInt16_t numAdcSamples;
    /**
     * @brief  ADC Sampling rate for each profile is encoded in \n
                   2 bytes (16 bit unsigned number)\n
                   1 LSB = 1 ksps \n
                   The maximum sampling rate supported is limited based on the information
                   below. \n
                   <table>
                   <caption id="multi_row_0">When device supports 15 MHz IF bandwidth (refer 
                   device data sheet)</caption>
                   <tr><th>ADC mode   <th>Real/PseudoReal   <th>Complex1x   <th>Complex2x
                   <tr><td>Regular ADC mode<td>37.5 Msps<td>18.75 Msps<td>37.5 Msps
                   <tr><td>Low power ADC mode<td>18.75 Msps<td>9.375 Msps<td>18.75 Msps
                   </table>
                   <table>
                   <caption id="multi_row_0">When device supports 10 MHz IF bandwidth (refer 
                   device data sheet)</caption>
                   <tr><th>ADC mode   <th>Real/PseudoReal   <th>Complex1x   <th>Complex2x
                   <tr><td>Regular ADC mode<td>25 Msps<td>12.5 Msps<td>25 Msps
                   <tr><td>Low power ADC mode<td>18.75 Msps<td>9.375 Msps<td>18.75 Msps
                   </table>
                   <table>
                   <caption id="multi_row_0">When device supports 5 MHz IF bandwidth (refer 
                   device data sheet)</caption>
                   <tr><th>ADC mode   <th>Real/PseudoReal   <th>Complex1x   <th>Complex2x
                   <tr><td>Regular ADC mode<td>12.5 Msps<td>6.25 Msps<td>12.5 Msps
                   <tr><td>Low power ADC mode<td>12.5 Msps<td>6.25 Msps<td>12.5 Msps
                   </table>
     */
    rlUInt16_t digOutSampleRate;
#ifndef MMWL_BIG_ENDIAN
    /**
     * @brief  Code for HPF1 corner frequency\n
                   0x00 175 kHz \n
                   0x01 235 kHz \n
                   0x02 350 kHz \n
                   0x03 700 kHz \n
     */
    rlUInt8_t  hpfCornerFreq1;
    /**
     * @brief  Code for HPF2 corner frequency\n
                   0x00 350 kHz \n
                   0x01 700 kHz \n
                   0x02 1.4 MHz \n
                   0x03 2.8 MHz \n
     */
    rlUInt8_t  hpfCornerFreq2;
#else
    /**
     * @brief  Code for HPF2 corner frequency\n
                   0x00 350 kHz \n
                   0x01 700 kHz \n
                   0x02 1.4 MHz \n
                   0x03 2.8 MHz \n
     */
    rlUInt8_t  hpfCornerFreq2;
    /**
     * @brief  Code for HPF1 corner frequency\n
                   0x00 175 kHz \n
                   0x01 235 kHz \n
                   0x02 350 kHz \n
                   0x03 700 kHz \n
     */
    rlUInt8_t  hpfCornerFreq1;
#endif
    /**
     * @brief  Number of transmitters to turn on during TX power  \n
               calibration. During actual operation, if more than \n
               1 TXs are enabled during the chirp, then enabling  \n
               the same TXs during calibration will have better TX \n
               output power accuracy \n
                    b2:0 TX enabled during TX0 calibration \n
                         b0 - TX0, b1 - TX1, b2 - TX2 \n
                    b5:3 TX enabled during TX1 calibration \n
                         b3 - TX0, b4 - TX1, b5 - TX2 \n
                    b8:6 TX enabled during TX2 calibration \n
                         b6 - TX0, b7 - TX1, b8 - TX2 \n
                    b14:9 RESERVED \n
                    b15  Enable multi TX enable during TX power calibration. \n
                         Note : If this bit is not set, only 1 TX is enabled \n
                         during the TX power calibration. For e.g. during TX0 \n
                         calibration, only TX0 will be enabled; during TX1 \n
                         calibration, only TX1 will be enabled and so on. \n
               NOTE: This field is not applcicable for IWR6843 ES 1.0
     */
    rlUInt16_t txCalibEnCfg;
    /**
     * @brief
                b5:0  Code for Rx VGA gain\n
                      1 LSB = 1 dB\n
                      Valid values: all even values from 24 to 52 \n
                b7:6  Code for RF gain target (Not applicable for IWR6843 ES1.0, \n
                      RF gain is fixed to 34dB). \n
                      For xWR1243/xWR1443/xWR1642/xWR1843:
                        00 - 30 dB \n
                        01 - 34 dB \n
                        10 - RESERVED \n
                        11 - 26 dB \n
                      For IWR6843 ES 2.0:
                        00 - 30 dB \n
                        01 - 33 dB \n
                        10 - 36 dB \n
                        11 - RESERVED \n
                b15:8 RESERVED \n
                In IWR6843 ES1.0 device, Rx Gain is not accurate and not calibrated. \n
                The RF gain is 34dB and total Rx gain can be varied from 28dB to 56dB \n
     */
    rlUInt16_t  rxGain;
    /**
     * @brief  Reserved for Future use
     */
    rlUInt16_t reserved;
} rlProfileCfg_t;


typedef struct rlCalDataStore
{
    /**
     * @brief Number of calibration data chunks Available in device
     */
    rlUInt16_t numOfChunk;
    /**
     * @brief Current Calibration Data Chunk Id. Valid range 0-2
     */
    rlUInt16_t chunkId;
    /**
     * @brief Calibration data chunk which was stored in non-volatile memory \n
     */
    rlUInt8_t  calData[224U];
} rlCalDataStore_t;


typedef struct rlCalibrationData
{
     rlCalDataStore_t calibChunk[RL_MAX_CALIB_DATA_CHUNK];
}rlCalibrationData_t;


typedef struct rlPhShiftCalibrationStore
{
#ifndef MMWL_BIG_ENDIAN
    /**
     * @brief  Index of the transmitter channel for which the phase shift is desired \n
               Valid range: 0 to (Number of Tx channels enabled at rlSetChannelConfig) - 1 \n
                       e.g: 0 to 1 (For xWR1642), 0 to 2 (For xWR1843, xWR1443 & xWR1243). \n
     */
    rlUInt8_t txIndex;
    /**
     * @brief Set this to 1 after injecting calibration data for all Txs to enable the firmware \n
              calibration. For example, if we need to inject calibration data for 3 TX channels, \n
              we would have to issue the RL_RF_STATIC_CONF_SET_MSG with \n
              RL_RF_PH_SHIFT_CAL_DATA_RD_WR_SB three times, each time with the calibration data \n
              for one TX channel. We need to ensure that for the first two instances of the API, \n
              we set the calibApply field to '0'. When issuing the last instance of the API, when \n
              the radarss has complete calibration data for all 3 TX channels, set the calibApply \n
              field to '1'. \n
     */
    rlUInt8_t calibApply;
#else
    /**
     * @brief Set this to 1 after injecting calibration data for all Txs to enable the firmware \n
              calibration. For example, if we need to inject calibration data for 3 TX channels, \n
              we would have to issue the RL_RF_STATIC_CONF_SET_MSG with \n
              RL_RF_PH_SHIFT_CAL_DATA_RD_WR_SB three times, each time with the calibration data \n
              for one TX channel. We need to ensure that for the first two instances of the API, \n
              we set the calibApply field to '0'. When issuing the last instance of the API, when \n
              the radarss has complete calibration data for all 3 TX channels, set the calibApply \n
              field to '1'. \n
     */
    rlUInt8_t calibApply;
    /**
     * @brief  Index of the transmitter channel for which the phase shift is desired \n
               Valid range: 0 to (Number of Tx channels enabled at rlSetChannelConfig) - 1 \n
               e.g: 0 to 1 (For xWR1642), 0 to 2 (For xWR1843, xWR1443 & xWR1243). \n
     */
    rlUInt8_t txIndex;
#endif
    /**
     * @brief Observed phase shift corresponding to each desired phase shift. Index n \n
              corresponds to desired phase shift of n * 5.625 degree. \n
                 n     Desired phase shift   Observed phase shift is read in the following bytes \n
                 0     0.000 degree                  byte[1], byte[0] \n
                 1     5.625 degree                  byte[3], byte[2] \n
                 2     11.250 degree                 byte[5], byte[4] \n
                 3     16.875 degree                 byte[7], byte[6] \n
                 :          :                                :        \n
                 :          :                                :        \n
                 63    354.375 degree                byte[127], byte[126] \n
              1 LSB = (360 degree)/pow(2,10). \n
     */
    rlUInt8_t  observedPhShiftData[128U];
    /**
     * @brief Reserved for future use
     */
    rlUInt16_t reserved;
} rlPhShiftCalibrationStore_t;


typedef struct rlPhShiftCalibrationData
{
     rlPhShiftCalibrationStore_t PhShiftcalibChunk[RL_TX_CNT];
}rlPhShiftCalibrationData_t;
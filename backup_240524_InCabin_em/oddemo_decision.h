/*
 * oddemo_decision.h
 *
 */




 



void ODDemo_Decision_init(void);
extern void ODDemo_Decision_process(uint16_t pair, float *coeffMatrix, ODDEMO_Feature *featureVect, ODDEMO_Decision *decision);
extern void ODDemo_Decision_process_addOD(uint16_t pair, float *coeffMatrix, ODDEMO_Feature *featureVect, ODDEMO_Decision *decision);

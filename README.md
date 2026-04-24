# Automotive Toolbox 3.6.0

# fVS+rOD, output value check

# INDEX  
1. [Case](#Case)  
&ensp; A. [event case](#A-event-case)  
&ensp; B. [considering value](#B-considering-value)  

2. [output values](#output-values)  
&ensp; A. [heartRateEst_HarmonicEnergy](#A-heartRateEst_HarmonicEnergy)  
&ensp; B. [heartRateEst_FFT_4Hz](#B-heartRateEst_FFT_4Hz)  
&ensp; C. [confidenceMetricHeartOut](#C-confidenceMetricHeartOut)  
&ensp; D. [heartRateEst_xCorr](#D-heartRateEst_xCorr)  
&ensp; E. [sumEnergyHeartWfm](#E-sumEnergyHeartWfm)  
&ensp; F. [confidenceMetricHeartOut](#F-confidenceMetricHeartOut)  

---

# Case    
## A. event case
   1. 에너지 조건: heartRateEst_HarmonicEnergy > Threshold  
   2. 안정성 조건: heartRateEst_FFT와 heartRateEst_xCorr의 차이가 5bpm 이내  
   3. 신뢰도 점수: confidenceMetricHeartOut > Threshold  
   4. 연속성 조건: 현재 값과 직전 평균값의 차이가 합리적 범위 내 
   ※ 현상이 Outlier/측정값이 고정되어 안 움직이는 경우, 증상에 따라 지표 중 우선순위가 달라짐.  

## B. considering value
### heartRateEst_FFT vs breathingRateEst_FFT
- 호흡 신호 대 심박 신호의 비율(SNR)
  + 심박수는 Breathing 신호의 고조파에 묻히는 경우가 많다. 호흡 에너지(breathingEst_FFT)가 너무 강하면 심박 FFT 값은 왜곡된다(Breath Rate 관련 Clutter 여부)  

### Phase Unwrapping Quality (신호 파형 상태)
- FFT 전 단계의 Phase 신호가 깨끗한지 확인 필요  
  (target이 미세한 움직임이 있다면 위상 신호 자체가 튀어 FFT는 무의미)
  ※ heartRateEst_HarmonicEnergy와 confidenceMetricHeartOut이 동시에 낮은데 heartRateEst_FFT만 높게 나온다면, 결과 값은 Artifact  
  ※ heartRateEst_FFT 값은 일정한데 Confidence만 요동치는 상황인가? 아니면 값이 아예 엉뚱한 대역으로 튀는 상황인가?  

### sumAllSpectralEnergy (=sumEnergyHeartWfm) 대비 비율 (SNR)  
- 심박수 관련 처리 범위 내에서의 전체 에너지  
- 심박 대역의 에너지가 높고, 전체 노이즈(Clutter) 에너지도 같이 높으면 신뢰도가 낮다.  
- 전체 에너지 대비 심박 고조파 에너지 비율 계산  
  $Rate = \frac{heartRateEst_HarmonicEnergy} {sumAllSpectralEnergy}$  
  + 비율이 낮으면, 현재 환경에 움직임 or 노이즈가 많다는 뜻  

### numMovingAvgFilter (지수 이동 평균의 편차)  
- 심박수는 생체 특성상 1~2초 사이에 급격한 점프는 발생하기 어렵다(예: 70bpm → 110bpm으로 점프).  
- 직전 프레임들의 평균값과 현재 FFT 값의 차이 (Delta)  
  * delta > threshold(예: 10bpm) : 결과 값의 신뢰도를 낮춘다 (이전 값을 유지 or confidenceMetric을 강제로 낮추는 로직 필요)  

---

# output values  
## A. heartRateEst_HarmonicEnergy  
- 심장 박동 신호의 Harmonics 성분들이 가진 에너지의 총합(심박수 추정 신뢰도 판단 지표)
  ※ 에너지 중 규칙적인 고조파 성분이 얼마나 되는지 확인  
- 특징
  + 계산된 상대적인 에너지 강도
  + 값이 높을수록 측정된 심박수 데이터의 신뢰도가 높다(값이 낮으면 제외)
- 용도
  + 고조파 분석: 심장 박동에 포함된 여러 고조파 성분(기본 주파수(Fundamental Frequency)의 배수 주파수들)의 에너지를 합산하여 이 값이 Threshold을 넘을 때 유효한 심박수[^VS_PARAMS_1], [^VS_PARAMS_2]   

- 관련 파라미터 (TI mmWave Vital Signs Lab 기준)
  + 에너지를 계산한 후 heartRateWeight 등의 변수와 결합하여 최종 심박수를 결정
    * 호흡 신호의 고조파 제거 → 심장 박동 대역(보통 0.8~2.0Hz)에서 가장 강한 에너지 분포를 확인 [^VS_PARAMS_3]
    * 에너지 분포 값이 낮으면 센서와 측정 대상 사이의 거리, 가슴 부위의 정면 배치 여부 또는 주변의 미세 진동(클러터) 유무 등 점검 필요  
    ※ 구체적인 임계값(Threshold) 설정 방법이나 소스 코드상의 계산식 (재확인 필요)  

[^VS_PARAMS_1]: [https://arxiv.org](https://arxiv.org/pdf/2508.20864)
[^VS_PARAMS_2]: [https://my.clevelandclinic.org](https://my.clevelandclinic.org/health/articles/10881-vital-signs)
[^VS_PARAMS_3]: [https://www.researchgate.net](https://www.researchgate.net/publication/364141837_Vital_Sign_Detection_via_Angular_and_Range_Measurements_with_mmWave_MIMO_Radars_Algorithms_and_Trials)

## B_1. heartRateEst_FFT
  - 심박 측정 가능 대역(보통 0.6Hz ~ 4.0Hz)에서 FFT를 수행하여 가장 강한 에너지를 가진 주파수 피크를 심박수로 변환한 값
  - 
## B_2. heartRateEst_FFT_4Hz
  - 4Hz(240 BPM)까지의 고주파 대역을 포함하여 계산된 추정치
  - 용도: 고차 고조파(Harmonics) 확인.
  - 의미: 기본 심박수 대역(약 1~2Hz) 외에 4Hz 부근의 에너지를 확인하여, 현재 잡힌 피크가 실제 심장 박동의 배수 성분인지 아니면 외부 진동 노이즈인지 교차 검증할 때 사용
  - 확인 사항
    + FFT 결과값의 피크가 주변 노이즈보다 충분히 높은지 확인
    + 데이터 업데이트 주기: heartRateEst_FFT_4Hz가 실시간으로 변하는지, 아니면 일정 시간(window) 동안의 평균값인지 확인
    + 측정 대상이 레이더 정면에 있는지, 주변에 팬(Fan)이나 진동체 등 레이더 신호를 방해하는 요소가 없는지 확인

## C. confidenceMetricHeartOut
  - 용도: 최종 합격/불합격 판독기.
  - 의미: 알고리즘이 여러 지표를 종합해 계산한 '자신감' 점수입니다. 보통 이 값이 특정 임계값(Threshold) 미만이면 FFT 결과가 아무리 선명해도 화면에 표시하지 않도록 로직을 짭니다.

## D. heartRateEst_xCorr 
- 교차 상관 계수
  + FFT는 주파수 도메인만 보지만, xCorr(Cross-correlation)은 시간 도메인에서 신호의 주기성이 얼마나 일정한지를 나타낸다
  + 특징  
    * FFT 피크가 있더라도 신호의 모양이 불규칙하면 xCorr 값이 낮게 나온다.
    * heartRateEst_FFT와 heartRateEst_xCorr가 가리키는 주파수가 일치할 때만 True로 판정시 오검출율 감소

## E. sumEnergyHeartWfm
- heartRateEst_FFT가 불안정할 때, "노이즈를 포함한 전체 에너지"의 합과의 비율 확인 필요  
  ※ 심박 대역에 에너지가 충분히 들어오는지 확인 (사람이 있는지 확인)
  $SNR_{Heart} = \Large \frac{heartRateEst\_HarmonicEnergy}{sumEnergyHeartWfm}$  
  + 비율이 높을 때: 신호 중에서 심박 고조파 성분이 지배적(FFT 값 신뢰도가 높음)
  + 비율이 낮을 때: 신호에 Clutter나 Motion이 섞여 있어 심박 성분이 묻힌 상태(FFT 값 제거 필요)  
  + heartRateEst_HarmonicEnergy : 필터(HPF or BPF)를 거친 후의 심박 Waveform 전체 에너지 ("유효한 성분"의 합)  
  + sumEnergyHeartWfm : "노이즈를 포함한 전체 에너지"의 합    

## F. confidenceMetricHeartOut  
- $SNR_{Heart}$ 비율과 Peak의 선명도를 종합해 최종 점수 도출.  

## G. outputFilterBreathOut, outputFilterHeartOut  
- doppler input 데이터로부터 위상 추출(atan) >> unwrap >> impulse noise 제거 결과 (phaseUsedComputation)를 IIR 필터 입력으로 전달하여 filtering된 결과값 (time domain의 신호임)   
  + 필터 출력 신호의 특성   
    * outputFilterBreathOut: 호흡 대역(약 0.1~0.5Hz) 통과 신호. 진폭이 크며 파형이 완만   
    * utputFilterHeartOut: 심박 대역(약 0.8~2.0Hz) 통과 신호. 호흡에 비해 진폭이 매우 작으며 미세한 진동 형태   
    → 이 두 신호로 호흡과 심박 추출  

- filter 출력(outputFilterHeartOut/outputFilterBreathOut) 신호는  
  + heart & breathing wfm을 circular buf에 outputFilterHeartOut & outputFilterBreathOut을 buf size (vitalSignCfg의 winLen-1) 만큼 누적, 마지막 buf에 최신 data copy  
    ※ circular Buffer : obj_VS->pVitalSigns_Heart_circularBuffer/pVitalSigns_Breath_circularBuffer  
 





  + cardiac/Breath wfm을 구성하는 에 된다.
    * 각각의 buffer data에 scale factor를 곱한 결과 값을 obj_VS->pVitalSignsBuffer_Cplx buffer에 재구성
    * buffer(obj_VS->pVitalSignsBuffer_Cplx) data를 입력으로 받아 FFT 수행
    * FFT의 출력은 obj_VS->pVitalSigns_SpectrumCplx
      ※ FFT 전 step에서 heartRateEst_peakCount/breathingRateEst_peakCount를 계산하는데, 두 값은 최종적으로 사용되지 않음
   









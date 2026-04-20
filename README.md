# Automotive Toolbox 3.6.0

# fVS+rOD 정리


Q : VS_Processing_Chain에서, heartRateEst_HarmonicEnergy의 값은?
A : 
VS_Processing_Chain에서 heartRateEst_HarmonicEnergy 값은 심장 박동 신호의 고조파(Harmonics) 성분들이 가진 에너지의 총합을 나타내며, 신뢰도 높은 심박수 추정을 위한 판단 지표로 활용됩니다.
이 값의 구체적인 의미와 역할은 다음과 같습니다.
## 1. 주요 역할 및 의미

* 신호 품질 지표: 심박수 추정 알고리즘이 찾아낸 피크(Peak)가 실제 심장 박동인지, 아니면 노이즈인지를 구분하는 신뢰도 점수와 같은 역할을 합니다.
* 고조파 분석: 심장 박동은 단일 주파수가 아닌 여러 고조파 성분을 포함합니다. 알고리즘은 기본 주파수(Fundamental Frequency)와 그 배수 주파수들의 에너지를 합산하여 이 값이 특정 임계값(Threshold)을 넘을 때 유효한 심박수로 간주합니다. [1, 2] 

## 2. 출력 값의 특징

* 상대적 수치: 이 값은 절대적인 물리 단위보다는 알고리즘 내부에서 계산된 상대적인 에너지 강도를 나타냅니다.
* 결과 판단: heartRateEst_HarmonicEnergy 값이 높을수록 측정된 심박수 데이터의 신뢰도가 높음을 의미하며, 값이 너무 낮으면 "Invalid"로 처리되어 화면에 심박수가 표시되지 않거나 이전 값이 유지될 수 있습니다.

## 3. 관련 파라미터 (TI mmWave Vital Signs Lab 기준)
TI(Texas Instruments)의 Vital Signs SDK 소스 코드(main.c 등)를 보면, 이 에너지를 계산한 후 heartRateWeight 등의 변수와 결합하여 최종 심박수를 결정하는 로직을 확인할 수 있습니다.

* 동작 방식: 호흡 신호의 고조파를 제거하고 남은 심장 박동 대역(보통 0.8~2.0Hz)에서 가장 강한 에너지 분포를 확인합니다. [3] 

이 값이 예상보다 낮게 나온다면 센서와 측정 대상 사이의 거리, 가슴 부위의 정면 배치 여부 또는 주변의 미세 진동(클러터) 유무를 점검해 보는 것이 좋습니다.
구체적인 임계값(Threshold) 설정 방법이나 소스 코드상의 계산 공식이 추가로 필요하신가요?

[1] [https://arxiv.org](https://arxiv.org/pdf/2508.20864)
[2] [https://my.clevelandclinic.org](https://my.clevelandclinic.org/health/articles/10881-vital-signs)
[3] [https://www.researchgate.net](https://www.researchgate.net/publication/364141837_Vital_Sign_Detection_via_Angular_and_Range_Measurements_with_mmWave_MIMO_Radars_Algorithms_and_Trials)



Q : heartRateEst_FFT 값이 부정확할때 함께 보면 좋은 값은
heartRateEst_FFT_4Hz, heartRateEst_HarmonicEnergy, confidenceMetricHeartOut 

A :
각각의 수치가 의미하는 바를 정확히 매칭해서 해석 필요

## 1. 활용법

* heartRateEst_FFT_4Hz :
  - 용도: 고차 고조파(Harmonics) 확인.
  - 의미: 기본 심박수 대역(약 1~2Hz) 외에 4Hz 부근의 에너지를 확인하여, 현재 잡힌 피크가 실제 심장 박동의 배수 성분인지 아니면 외부 진동 노이즈인지 교차 검증할 때 사용
* heartRateEst_HarmonicEnergy:
  - 용도: 신호의 '순도' 측정.
  - 의미: 심박 신호는 단일 주파수보다 여러 고조파가 조화롭게 나타날 때 신뢰도가 높습니다. FFT 값은 높은데 이 에너지가 낮다면, 그 FFT 피크는 일시적인 노이즈일 확률이 크다.
* confidenceMetricHeartOut:
  - 용도: 최종 합격/불합격 판독기.
  - 의미: 알고리즘이 여러 지표를 종합해 계산한 '자신감' 점수입니다. 보통 이 값이 특정 임계값(Threshold) 미만이면 FFT 결과가 아무리 선명해도 화면에 표시하지 않도록 로직을 짭니다.

## 2. 만약 FFT 값이 튀거나 부정확하다면
  - 1. 호흡 신호 대비 심박 신호의 비율(SNR)  
    + 심박수는 Breathing 신호의 고조파에 묻히는 경우가 많다. 호흡 에너지(breathingEst_FFT)가 너무 강하면 심박 FFT 값은 왜곡된다(Breath Rate 관련 Clutter 여부)  
  - 2. Phase Unwrapping Quality (신호 파형 상태):
    + FFT 전 단계의 Phase 신호가 깨끗한지 확인 필요  
       (target이 미세한 움직임이 있다면 위상 신호 자체가 튀어 FFT는 무의미)
   
※ heartRateEst_HarmonicEnergy와 confidenceMetricHeartOut이 동시에 낮은데 heartRateEst_FFT만 높게 나온다면, 그 FFT 값은 100% 무시해야 하는 Artifact 값
※ FFT 값은 일정한데 Confidence만 요동치는 상황인가? 아니면 값이 아예 엉뚱한 대역으로 튀는 상황인가?


---


Q: 다른 추천할 만한 값은?
A: 
heartRateEst_FFT가 흔들릴 때, 신뢰도를 획기적으로 높여줄 수 있는 파라미터 

## 1. heartRateEst_xCorr (교차 상관 계수)

* 이유: FFT는 주파수 도메인만 보지만, xCorr(Cross-correlation)은 시간 도메인에서 신호의 주기성이 얼마나 일정한지를 봅니다.
* 활용: FFT 피크가 있더라도 신호의 모양이 불규칙하면 xCorr 값이 낮게 나옵니다. heartRateEst_FFT와 heartRateEst_xCorr가 가리키는 주파수가 일치할 때만 "진짜"로 판정하면 오검출을 확 줄일 수 있습니다.

## 2. sumAllSpectralEnergy 대비 비율 (SNR)

* 이유: 심박 대역의 에너지가 아무리 높아도, 전체 노이즈(Clutter) 에너지가 같이 높으면 신뢰할 수 없습니다.
* 활용: heartRateEst_HarmonicEnergy / sumAllSpectralEnergy (전체 에너지 대비 심박 고조파 에너지 비율)를 계산해 보세요. 이 비율이 낮다면 현재 측정 환경에 움직임 노이즈가 너무 많다는 뜻입니다.

## 3. numMovingAvgFilter (또는 지수 이동 평균의 편차)

* 이유: 심박수는 생체 특성상 1~2초 사이에 급격한 점프는 발생하기 어렵다(예: 70bpm → 110bpm으로 점프).
* 활용: 직전 프레임들의 평균값과 현재 FFT 값의 차이(Delta)를 보세요. 만약 이 차이가 설계한 임계값(예: 10bpm)보다 크다면, 현재의 FFT 값은 신뢰하지 않고 이전 값을 유지하거나 confidenceMetric을 강제로 낮추는 로직이 필요

------------------------------
## 추천하는 "최종 검증 조합"
정확도를 높이려면 아래 조건을 모두 만족하는지 체크하는 것이 가장 좋습니다.

   1. 에너지 조건: heartRateEst_HarmonicEnergy > Threshold
   2. 안정성 조건: heartRateEst_FFT와 heartRateEst_xCorr의 차이가 5bpm 이내
   3. 신뢰도 점수: confidenceMetricHeartOut > Threshold
   4. 연속성 조건: 현재 값과 직전 평균값의 차이가 합리적 범위 내

지금 겪고 계신 현상이 값이 튀는 것(Outlier)인가요, 아니면 측정값이 고정되어 안 움직이는 것인가요? 증상에 따라 위 지표 중 우선순위가 달라집니다.



Q: sumAllSpectralEnergy가 sumEnergyHeartWfm?
A: 
- sumEnergyHeartWfm은 말씀하신 sumAllSpectralEnergy(전체 스펙트럼 에너지)와 같은 맥락에서 사용되는 값  
  (※ 변수명은 SDK 버전이나 커스텀 코드에 따라 다를 수 있음)  
- 의미 : 심박수 관련 처리 범위 내에서의 전체 에너지

- 활용 시 체크 point  
## 1. sumEnergyHeartWfm의 의미  
* Heart Waveform의 총 에너지 : 필터(HPF or BPF)를 거친 후의 심박 Waveform 전체 에너지  
* 분모(Background) 역할: heartRateEst_HarmonicEnergy가 "유효한 성분"의 합, sumEnergyHeartWfm은 "노이즈를 포함한 전체 에너지"의 합  

## 2. SNR 지표 생성
heartRateEst_FFT가 불안정할 때, Ratio 확인 필요
$$SNR_{Heart} = \frac{heartRateEst\_HarmonicEnergy}{sumEnergyHeartWfm}$$ 

* 비율이 높을 때: 신호 중에서 심박 고조파 성분이 지배적(FFT 값 신뢰도가 높음)
* 비율이 낮을 때: 신호에 Clutter나 Motion이 섞여 있어 심박 성분이 묻힌 상태(FFT 값 제거가 바람직)

## 3. 함께 보면 좋은 관계
보통 TI의 Vital Signs 처리 로직에서는 다음과 같은 흐름을 가집니다.

   1. sumEnergyHeartWfm: 일단 심박 대역에 에너지가 충분히 들어오는지 확인 (사람이 있는지 확인).
   2. heartRateEst_HarmonicEnergy: 그 에너지 중 규칙적인 고조파 성분이 얼마나 되는지 확인.
   3. confidenceMetricHeartOut: 위 두 값의 비율과 Peak의 선명도를 종합해 최종 점수 도출.

결론적으로:
heartRateEst_FFT가 튈 때 sumEnergyHeartWfm 대비 heartRateEst_HarmonicEnergy의 비율을 계산해 보세요. 이 비율이 평소보다 떨어진다면, 현재 FFT 값은 가짜 피크(False Positive)일 확률이 매우 높습니다.
현재 보고 계신 데이터에서 두 값의 비율이 보통 어느 정도로 유지되고 있나요? (예: 보통 0.1~0.2 수준 등)




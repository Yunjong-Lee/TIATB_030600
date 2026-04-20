

# Index

[1. CFG 4 Vital-sign estimation](#1-CFG-4-Vital-sign-estimation)   
[2. Parameters](#2-Parameters)   
[ . 2.1. config Params](#2-1-config-parameter)   
[ . 2.2. Function & parameter](#2-2-Function-and-parameter)   
[3. code debugging](#3-code-debugging) 
[Papers](#Papers)
[References](#References)  
  
---  

# 1. CFG 4 Vital sign estimation
## 1-1. profileCfg 0 60 250 10 40 0 0 98 1 64 2200 0 0 40 
```
───────────┬────────────────────────────────────────────
           | 1  2  3   4  5 6 7  8 9 10  11  12 13 14
───────────┼────────────────────────────────────────────            
profileCfg | 0 60 250 10 40 0 0 98 1 64 2200  0  0 40
───────────┴────────────────────────────────────────────
```


## 1-2. frameCfg 0 3 128 0 200 1 0
```
───────────┬────────────────────────────────────────────
           | 1 2  3  4  5  6 7 
───────────┼────────────────────────────────────────────            
frameCfg   | 0 1 128 0 160 1 0
───────────┴────────────────────────────────────────────
```

+ frameCfg는 프레임 전송 간격을 제어하는 ​​데 사용된다.  
  ※ [frameCfg 명령은 밀리초 단위로 프레임 주기를 가져온다](https://e2e.ti.com/support/sensors-group/sensors/f/sensors-forum/656350/iwr1443-the-configure-profile-of-the-vital-sign-lab)  
  ※ mmwavelink[^mmWaveLink]로 전송되기 위해 rlSetFrameCfg[^rlSetFrameCfg]를 호출.   
  프레임 주기의 각 LSB는 5ns(mmwavelink doxygen 설명서,  "../mmwave_sdk_01_01_00_02/packages/ti/control/mmwavelink/docs/doxygen/html/group___sensor.html#gaf1e5cbc25891714db321da3bfa0d2014")
  기본적으로 CLI의 frameCfg 명령은 ms 단위의 값을 취하며, CLI 핸들러 함수에서 하드웨어 요구 값으로 변환된다.  

+ Number of loops
  - Tx당 전송된 total chirp의 수 / 1 frame (Tx_0->1->2 sequence를 가지는 chirp 반복회수)  
  ※ 도플러 chirp 수에 값 2를 원하는 경우, 도플러 DPU window 변경 (hanning window → rectangle window로)  
  (데모/객체 감지 DPC 소스 코드 업데이트)  

  ※ [frameCfg의 세 번째 매개변수 "Number of loops"는 1~255 범위(mmwave_sdk_user_guide에서)값을 가진다. 하지만 값은 2의 거듭제곱이어야 하므로 설정 가능한 최대값은 128](https://e2e.ti.com/support/sensors-group/sensors/f/sensors-forum/667681/iwr1642-regarding-the-framecfg-and-subframecfg-parameters?ReplyFilter=Answers&ReplySortBy=Answers&ReplySortOrder=Descending)  



## 1-3. vitalSignsCfg  
```
|  1    |  2    |  3        |  4        | 5       |  6    |  7    |   8   |   9   |  
├───────┼───────┼───────────┼───────────┼─────────┼───────┼───────┼───────┼───────┤ 
|  0.3  | 0.9   | 256       | 512       | 4       | 0.1   | 0.05  | 100000| 300000|
├───────┼───────┼───────────┼───────────┼─────────┼───────┼───────┼───────┼───────┤
| range | range | windowLen | windowLen | rx Ant. | alpha | alpha | scale | scale | 
| start | end   |        B  |        H  | process |  B wf |  H wf |  B wf |  H wf |

```

- output 데이터를 참조하면, 처음 10개 frame에서는 vital 신호가 나오지 않는다.
- moving average를 취하는데, size가 10이다.
  + code의 어디?
  + MA의 영향은?

## 1-4. motionDetection 0 20 2.0 0


---

# 2. Parameters  

```
|                                   |         |                                      |  
├───────────────────────────────────┼─────────┼──────────────────────────────────────┤
|  zone definition                  | O       | zone center에서 수정                  |
|                                   |         | - 운전석/조수석 탑승시 간섭 영향 최소화 |
|  range start & endr               | X       | code check 완                        |
|  rx Ant. Process                  | X       | code check 완                        |
|  clutter removal parameter        |         | code 미구현 상태 (구현 검토 중)        | 
|  - fixed target                   | -       |                                      |
|  window length                    | O       | median filter 수정 후 추가 확인 필요   |
|  waveform : alpha                 |         |                                      |
|  waveform : scale                 |         |                                      |
|  waveform : scale                 |         |                                      |
|  waveform : scale                 |         |                                      |
|  waveform : scale                 |         |                                      |
|  waveform : scale                 |         |                                      |

```

## 2-1. config parameter

- range start & end : 성능에 영향이 없음

- rx Ant. Process : 성능에 영향이 없음
  + vitalSignsParamCLI.rxAntennaProcess (mss)에서 obj_VS->rxAntennaProcess (dss) 이동
  + 초기화 이후 사용되지 않음

- window length Breathing & Heart : 성능개선은 추가 확인 필요
    + vitalSignsParamCLI.winLen_breathing (mss)로 저장되어 obj_VS->circularBufferSizeBreath (dss)에 저장
    + "circular Buffer"에 있는 Breathing Waveform을 circularBufferSizeBreath size(1~256, 255개)만큼 복사(obj_VS->pVitalSigns_Breath_CircularBuffer)한 다음 마지막 번지(256번째)에 IIR 필터링 결과(outputFilterBreathOut)를 추가
    + 복사한 obj_VS->pVitalSigns_Breath_CircularBuffer를 가지고 peak를 찾는 findPeaks 수행

- alpha Breathing Waveform & alpha Heart Waveform
    + vitalSignsParamCLI.alpha_breathingWfm (mss)에서 obj_VS->alpha_breathing (dss) 이동
    + VS_Processing_Chain 함수에서 exponential smoothing하는 과정에 사용

- scale Breathing Waveform & scale Heart Waveform


## 2-2. Function and parameter
- PHASE_FFT_SIZE 1024
  + frequency_step_size와 연관(freq_increment = sampling_freq / PHASE_FFT_SIZE)
- MAX_HEART_RATE_BPM 120
- MEDIAN_WINDOW_LENGTH  20

### 2-2-1. MmwDemo_interFrameProcessing

#### 2-2-1-1. ODDemo_Heatmap_aoaEstCaponBF_covInv

- vital signs extraction 과정에서 start_part(Calculate covariance matrix and invert)

    ```  
    // Index to the correct starting point for the Input Samples
    DVS_InputSamples = (cmplx16ReIm_t *)&gMmwL3[0];
    DVS_InputSamples += (chirpStep * rangeIndx);

    // covariance matrix and inversion. 
    ODDemo_Heatmap_aoaEstCaponBF_covInv(...);
    ```

- clutter removal 기능 
  + ODDemo_Heatmap_aoaEstCaponBF_covInv 함수에서 활성화 시킬 수 있음
  + 주요 기능 (동작 방식)
    * 레이더 신호에서 정지 물체(주변 벽면, 가구 등 움직이지 않는 Static Object)로부터 발생하는 반사파를 계산 과정에서 제외한다.
    * 안테나 채널별로 들어오는 신호의 평균값(DC 성분)을 구한 뒤, 현재 신호에서 이 평균을 빼면, 속도가 0인 신호 성분이 제거된다.  
      -. 실제 감지하고자 하는 대상(움직이는 사람이나 물체)의 신호를 배경 잡음으로부터 더 명확하게 분리하여 신호 대 잡음비(SNR)를 높이기 위함  
  ※ but, ref_code는 클러터 제거 기능을 사용하지 않음 (데드코드, 논리오류 중 논리오류인 듯 함), 기능(clutter removal 수행 및 32-bits sum)[^7]을 활성화시키면?

```
    [Hint_chat GPT]

-. clutterRmFlag가 활성화되어 있어도 실제 Capon 알고리즘의 핵심인 공분산 행렬(R_XX) 계산에는 클러터가 제거되지 않은 원본 신호를 그대로 사용하는 경우가 많음.
※ 이유는 Capon 빔포밍의 특성 때문
  +. 공분산 행렬의 무결성: Capon(Minimum Variance Distortionless Response) 알고리즘은 간섭 신호를 억제하기 위해 전체 신호의 통계적 특성(공분산 행렬)을 이용합니다. 인위적으로 특정 성분(Clutter)을 제거한 데이터로 역행렬(R^-1_XX)을 구하면, 수치적으로 불안정해지거나 빔 패턴이 왜곡될 수 있다.
  +. 후처리 단계의 적용: 많은 TI 데모 코드에서 clutterRmFlag는 공분산 행렬 계산 단계가 아니라, 최종 히트맵 결과물에서 정지 성분을 마스킹하거나 별도의 감지 목록(Object List)을 생성하는 단계에서 조건부로 사용되곤 합니다. 즉, 계산은 다 해놓고 출력 직전에 필터링하는 방식입니다.
    *. 플래그의 위치: 함수 내부 로직에 if (clutterRmFlag) 문이 있지만, 그 안에서 처리된 결과값이 다음 연산(bf_covInv)으로 이어지지 않고 로컬 변수에서 끝나버리는 '데드 코드' 이거나, 단순히 특정 디버그 값을 채우는 용도로 사용되는 버전이 존재합니다.

-. 소스 코드상에서 이 플래그가 세팅되어도 행렬 연산의 입력값으로 반영되지 않는다면 해당 버전의 SDK에서는 그 단계에서의 클러터 제거가 실제 알고리즘 성능에 영향을 주지 않도록 설계됨  

-. clutterRmFlag 결과가 어떤 출력 버퍼(Output Buffer) or 구조체에 저장되는지 추적할 필요가 있음
    1. 주요 확인 파일 경로 
    dss_main.c 또는 AoA 처리 관련 파일인 dpu_aoaproc.c (또는 oddemo_heatmap.c와 유사)

    2. 코드 내 확인 포인트
    히트맵 계산 루프: 
    if (clutterRmFlag) {
        // 정지 성분(DC)을 뺀 값을 별도의 변수에 저장하거나 
        // 공분산 행렬 계산 직전에 신호를 가공하는지 확인
        // 예: inputSignal = currentSignal - meanSignal;
    }

    3. 결과값 할당(Assignment):
    -. 출력이 사용하되 않는 것 같다면, 
        +. if (clutterRmFlag) 블록 안에서 계산된 값이 함수의 최종 리턴 구조체나 전역 히트맵 버퍼(pOutputHeatmap 등)에 대입되는 코드가 누락되었을 가능성
heatmap[index] = ... 형태의 코드가 if문 밖에 있다면, 플래그와 상관없이 원본 데이터를 쓰는 것입니다.
        +. if (clutterRmFlag) { val = A; } else { val = B; } 이후 heatmap[index] = val; 처럼 대입이 이루어지는지 확인.

※ Tip: 디버깅
-. CCS 활용: ODDEMO_Heatmap_aoaEstCaponBF_covInv 함수 시작 부분에 브레이크포인트를 걸고, clutterRmFlag 변수 위에 마우스를 올려 현재 값(0 또는 1)을 확인.
Step Over(F6) 실행: 플래그가 1일 때 실행되는 코드 라인이 실제로 최종 출력 배열의 인덱스에 접근하여 데이터를 쓰는지 line by line 확인
  +. 만약 해당 함수에서 클러터 제거 로직이 실행되지만 최종 히트맵 버퍼에 반영되지 않는다면
    *. SDK의 설계 의도(히트맵은 전체를 보여주고 감지 목록에서만 클러터를 빼는 방식)이거나
    *. 단순 구현 누락
  +. 직접 반영
    *. if문 내의 가공된 데이터를 최종 출력 포인터에 직접 써주도록 코드 수정

-. 현재 소스 코드에서 특정 변수명(예: res, out, heatmap)으로 데이터가 옮겨지는 부분
  +. mmwavelib_vecsubc((int16_t *) input, (int16_t *) input, (uint32_t *) meanVal, (int32_t*) nChirps); 에서 clutterRmFlag값을 저장하는 것 같은데...

  +. mmwavelib_vecsubc 함수가 실행되는 지점이 바로 클러터 제거(Static Clutter Removal)가 물리적으로 일어나는 핵심 라인
    *. input 벡터에서 meanVal(평균값, 즉 정지 성분)을 빼는 연산 수행
    *. 수행 결과 overwrite
  + but, covariance matrix 연산 시 동알힌 포인터(input)를 사용하지 않음 

  ㅁ 확인 필요 : heatmap에 정지 물체가 보이는지 확인
    - 보인다면, 복사본 문제, 평균값 갱신 주기 가능성이 있음

    ※ 복사본 문제 : input 데이터는 연산 전에 다른 버퍼로 복사되었으나 행렬 계산은 복사된 버퍼(클러터 제거 안 된)를 사용  
    ※ 평균값 갱신 주기 : eanVal이 실시간으로 정확히 계산되지 않거나, 아주 느리게 갱신되면 정지 물체가 완벽히 제거되지 않고 잔상이 남을 수 있음  
--- 

-. if (clutterRmFlag) 조건문 뒤에는 Rn estimation logic이 있는데, clutter가 제거된 결과인 input을 받아주는 곳이 없음
  +. 데드코드 or 알고리듬 설계 의도
    *. 1. if (clutterRmFlag) 내부에서 mmwavelib_vecsubc를 호출해 input 버퍼의 정지 성분을 제거함.
    *. 2. 공분산 행렬 추정 로직이 input 버퍼가 아닌, 별도의 샘플 버퍼나 가공되지 않은 ADC 데이터 원본을 참조

    *. 3. 알고리듬 설계의도 : 공분산 행렬을 계산할 때 강제로 특정 성분(Clutter)을 빼버린 데이터를 넣으면, 행렬의 Rank(계수)가 낮아지거나 수치적 불안정성이 생겨 역행렬 계산 시 오류가 발생되므로 계산은 원본으로 진행한 뒤 결과에서 필터링
    *. 4. 데드코드 : 여러 example이 존재하므로 반영코드가 누락된 결과가 있을 수 있음  
    
  +. [확인] Rn estimation 로직이 끝난 후, 최종적으로 히트맵 값(Power/Energy)을 결정하는 계산과정에서 input 변수가 다시 사용되는지, 아니면 역행렬과 조향벡터만 사용되는지(역행렬과 조향백터만 사용된다면, 클러터 제거 연산은 데드코드임)
  +. 클러터 제거 효과를 추가하려면, 클러터가 제거된 input 버퍼를 사용하도록 수정 필요함
     (추가적으로 diagonal loading 등 보정 추가 필요)

-. 공분산 계산에서 참조하는 입력 포인터는 클러터 제거 로직에서 사용하는 입력 포인터와 동일하다.
  +. clutter removal : int16_t *input = (int16_t *) &inputAntSamples[antIdx * nChirps];
  +. 공분산 estimation : input1 = (cplx16_t *) &inputAntSamples[antIdx * nChirps];
→ 이 경우, 연산 누락의 가능성이 높음
  (공분산 추정로직이 input_1을 참조할 때, vecsubc가 처리한 데이터의 형식(int16 vs cplx16)이나 처리 범위(nChirps)가 일치하지 않거나, 연산 순서가 꼬여 있으면 클러터 제거 효과 누락된다. 여기서, 처리 범위는 일치하지만, 데이터의 형식이 일치하지 않음)
  ※ 만약 Rn estimation 로직이 하드웨어 가속기(HWA)를 사용하거나, inputAntSamples의 원본 데이터(L3/L2 메모리)를 직접 긁어가는 구조라면, CPU가 vecsubc로 메모리 값을 바꾸기도 전에 이미 데이터를 가져갔거나, 바뀐 값을 무시하고 연산할 수 있다
  +. [확인] input_1이 가리키는 메모리 값이 실제로 바뀌었는지 확인
    *. vecsubc 호출 직후와 Rn 계산 직전에 input_1[0]의 값을 UART_printf나 CCS의 Expressions 이용 debugging
    *. 만약 값이 변하지 않는다면, 
      . vecsubc가 엉뚱한 메모리 영역을 건드리고 있거나 
      . input_1이 덮어쓰기 전의 캐시된 데이터를 참조  
    *. Rn estimation 로직이 루프로 구현되어 있다면, clutterRmFlag 로직의 결과가 반영되지 않는 원인을 소스 코드 수준에서 명확히 잡을 수 있다
    
---

-. for 루프 구현된 Rn estimation 로직에서, clutterRmFlag 로직의 결과가 반영되지 않는 원인 확인 방법
  +. 1. 연산 순서 (Sequential Logic)
  : for 루프가 시작되기 직전에 mmwavelib_vecsubc가 호출되는지 확인
    * vecsubc가 for 루프 안쪽에서 매번 호출된다면 연산 낭비,
    * for 루프가 끝난 뒤에 호출된다면 당연히 공분산($R_n$) 계산에는 클러터 제거 전 데이터임.

  2. 포인터 Offset 및 캐시 문제
  : input_1이 inputAntSamples의 특정 인덱스를 참조할 때, vecsubc가 건드리는 메모리 영역과 정확히 일치하는지 확인.
    * vecsubc가 inputAntSamples 전체를 처리하는지, 아니면 현재 antIdx에 해당하는 부분만 처리하는지...
    * L2/L3 메모리를 사용하는 경우, CPU가 vecsubc로 메모리 값을 바꿔도 for 루프가 참조하는 데이터가 캐시에 남아 있어 이전(원본) 값을 계속 쓸 수 있습니다. (이 경우 Cache_inv 등이 필요할 수 있습니다.)

## 3. 실제 수정 포인트 (Logic Bridge)
만약 vecsubc의 결과(input)를 for 루프가 무시하고 있다면, 다음과 같이 코드를 강제로 연결해야 합니다.

// 1. 클러터 제거 수행 (inputAntSamples의 내용을 직접 수정한다고 가정)
if (clutterRmFlag) 
{
    mmwavelib_vecsubc((int16_t *) inputAntSamples, (int16_t *) inputAntSamples, ...);
}
// 2. 공분산 추정 루프
for (i = 0; i < nChirps; i++) 
{
    // 여기서 참조하는 input_1이 위에서 수정된 inputAntSamples를 가리키는지 확인
    // 예: sample = input_1[i]; 
    // Rn += sample * conj(sample);
}

## 결론
for 루프 안에서 input_1[i] 또는 이와 유사한 방식으로 데이터를 읽어올 때, 그 값이 vecsubc에 의해 변했는지 디버거로 찍어보는 것이 가장 빠릅니다. 만약 값이 변하지 않는다면, vecsubc의 인자로 들어가는 주소값과 for 루프의 참조 주소가 어긋나 있는 것입니다.
혹시 for 루프 내부에서 $R_n$을 계산하는 핵심 수식(예: invRn[...].real += ...) 코드를 한 줄만 보여주실 수 있나요? 그 수식이 참조하는 변수명을 보면 범인을 바로 잡을 수 있습니다.
어떤 변수를 곱해서 Rn에 더하고 있나요?

```

- DVS_InputSamples는 레이더의 L3 메모리에 저장된 데이터의 시작 주소(&gMmwL3[0])를 가리킨다.
  + chirpStep x rangeIndx 에 해당하는 위치로 데이터 시작 주소 이동[^chirpStep]   

```
  Q. 분석을 위한 zone의 range profile을 얻어내려면, rangeIndx에 azimuthIndx를 더해야 맞는 것이 아닌가?  
  A. (Chat GPT)  
    - radar data(radar cube)의 memory 구조[^2604151123]때문에 rangeIndx와 azimuthIndx를 단순히 더하지 않음
    - azimuthIndx 사용은(언제, 어떻게)?
      * rangeIndx 만큼 데이터의 시작 주소를 이동 시킨 후 사용
      * 해당 range index에서 어떤 안테나(또는 어떤 각도)의 값을 읽을 것인가를 결정할 때 offset으로 더해진다.(찾은 rangeindx에서 특정 각도 성분을 추출할 때 별도로 사용됨)  
```

- Capon 빔포밍은 특정 Azimuth 각도(spatial angle)에 대해 가중치(weight)를 적용하여 공간적 공분산 행렬을 처리하므로, scratchpad에 저장된 data는 각 azimuth index별로 계산된 결과이다.  

  ```
  for (antIdx = 0; antIdx < nRxAnt; antIdx++) 
  {
    // 1. 대각 성분 계산 (안테나 자신과의 관계: 1-1, 2-2, 3-3, 4-4)
    
    for (i = antIdx + 1; i < nRxAnt; i++) 
    {
        // 2. 비대각 성분 계산 (서로 다른 안테나 간의 관계: 1-2, 1-3, 1-4, 2-3 등)
    }
  }
  ```  

- Capon Beamforming에서 공분산 행렬 $R$은 수신 안테나 수(nRxAnt) × nRxAnt 크기를 가진다  
  (각 안테나 사이의 신호관계를 나타내기 때문)

- 안테나별로 데이터를 분리하여 포인터를 지정하는 단계
  + antIdx별 루프에서 처리할 특정 안테나의 데이터 시작 위치(Offset) 계산  
    ※ nChirps = 1  

  ```
  input1      =   (cplx16_t *) &inputAntSamples[antIdx * nChirps];
  ```  

- 공유 메모리(scratchPad)의 0번째 번지부터 재사용  
  Doppler FFT를 수행하기 위한 입력 버퍼의 시작 주소를 scratchPad[0]으로 지정  
  FFT 입력 버퍼 주소를 Beamforming(bf)의 출력 버퍼로도 설정  
  ※ AoA(Capon BF) 연산 결과 → bfOutput에 저장 (= dopplerFFTInput)  

    ```
    scratchOffset = 0;

    dopplerFFTInput = (float *)&oddemo_dataPathObj.scratchPad[scratchOffset];
    oddemo_dataPathObj.bfOutput = dopplerFFTInput;
    ```  

- dopplerFFTInput은 scratchOffset을 통해 특정 Azimuth 위치의 데이터를 가져오므로, 특정 Beam 방향에 해당하는 데이터이다.
- dopplerFFTInput 데이터를 이용하여 Doppler FFT를 수행하여 특정 azimuth 방향의 Doppler 속도를 얻어냄


~~ODDEMO_Heatmap_aoaEstCaponBF_dopplerEstInput~~   
~~-  Capon Beamforming(AoA)과 Doppler 추정을 결합하여 특정 구역의 히트맵 생성~~   
~~+ 특정 Range Bin(거리)에서의 데이터를 입력으로 받아 Azimuth(방위각)와 Doppler(속도) 추정~~   
~~* 입력 데이터: 특정 거리(rangeIndx)에 쌓여 있는 모든 안테나 샘플과 처프(Chirp) 데이터들~~  
~~* 포인터 설정: 특정된 DVS_InputSamples (해당 거리의 데이터 뭉치의 시작 지점, rangeIndx * chirpStep)부터 안테나별 위상 차이를 계산~~  
~~-azimuthIndx는 Capon Beamforming을 수행할 타겟 각도를 지정하는 데 사용~~  
~~※ Capon BF 결과값이 예상보다 낮게 나오거나 Doppler 인덱스가 밀리는 현상이 나타난다면, chirpStep이 실제 numAntennas * numDopplerBins[^numDopplerBins] 크기 확인~~  



#### 2-2-1-2. ODDEMO_Heatmap_aoaEstCaponBF_dopplerEstInput  
- Capon Beamforming 결과로 검출된 물체의 정확한 Doppler를 추정하기 위한 입력 데이터 준비 과정
  (4개 nRxAnt 데이터를 Capon 가중치로 합쳐서, doppler FFT를 수행하기 위한 1차원 데이터로 변환)  
  + 가중치 (Beamsteering) 적용  
    : 특정 각도(Azimuth/Elevation)로 Beamforming하기 위해 안테나 샘플 데이터에 Capon 가중치를 곱한다.  
    : 효과, 주변 간섭 억제 및 타겟 신호가 강화된 1차원 시계열 data 재구성  
  + doppler estimation  
    : input, radarcube / output, nChirp 길이의 complex data  
      (안테나 차원을 Capon 가중치로 압축(Combine)하여, 이후 단계인 Doppler FFT가 안테나 간섭 없이 해당 지점의 정확한 속도 성분만 추출할 수 있게 만들기 위하여)  
  + aoaEstCaponBF : heatmap에서 angle 계산
  + dopplerEstInput : 검출된 지점의 안테나 데이터에 Capon 가중치를 더하여 doppler 결정을 위한 데이터로 변환.  
  + Doppler FFT : 변환된 data에서 doppler 결정
 

#### 2-2-1-3. VS_Processing_Chain
- VS 연산을 위한 입력은 oddemo_dataPathObj.bfOutput
  + bfOutput[^2604131748]은 dopplerFFTInput에서 전달
  + dopplerFFTInput은 inputAntSamples를 가지고 공분산 계산 및 역행렬을 계산한 결과 값(scratchpad의 0번째 번지의 값)

- heart와 breath의 최종 출력(uart)은 heartRateEst_FFT/breathRateEst_FFT
  + heartRateEst_FFT는 median filter 출력임.
  + median filter는 "heapsort_index[^2]" 함수에서 수행
  + median filter output으로 exponential smoothing을 진행
  + exponential smoothing output은 breathWfmOutUpdated/heartWfmOutUpdated 이고, 이 값에 10,000을 곱한 결과가 sumEnergyBreathWfm/sumEnergyHeartWfm[^3]  
    * sumEnergyBreathWfm에 peakValueBreathSpect을 곱한 값으로 sumEnergyBreathWfm update [^4]

  + sumEnergyBreathWfm/sumEnergyHeartWfm





### ...

## 3. code debugging 
- ...  

### check pripority
- Motion artifact
  + 특징 : 60 → 110 bpm 처럼 급격한 점프 (특정 구간에서만 발생)
  + 원인 : 
    * 몸의 미세한 움직임(근육 긴장 포함)
    * 호흡(깊은 호흡의 경우)
    * 외부 진동
  + 결과 : HR Peak가 가려지거나 다른 주파수가 선택됨
  
- respiration harmonic overlap
  + 특징 : 호흡이 강할때 발생
  + 원인 : 호흡 고조파(매우 강함)가 HR영역에 겹침(1~2 [Hz])  
  + 결과 : FFT에서 HR 대신 respiration harmonic을 선택

- phase unwrap 오류
  + 특징 : 급격한 변화
  + 원인 : unwrap이 깨지면, 2$\pi$ 점프되어 displacement가 비정상적으로 커짐
  + 결과 : HR의 급격한 변화

- range-bin 선택 오류
  + 특징 : target이 흔들리는 경우(사람이 살짝 움직이는 경우 등), 다른 거리의 bin이 선택됨
  + 원인 : multipath
  + 결과 : HR 급격한 변화(신호가 원래 다른 신호 임)

- 낮은(부족한?) snr (신호 품질의 문제)
  + 원인 : 거리 증가, 옷 두께, 안테나 방향 등
  + FFT peak가 불안정

- peak tracking/ Algorithm
  + 원인 : FFT 기반 HR 추출 시 noise peak를 선택
  + 결과 : HR이 frame 마다 튀는 현상
  + kalman, EMA 등 필터링 및 tracking 처리 필요

- 시간/주파수 resolution trade-off
  + 짧은 window는 freq. 해상도가 부족하게되는 원인이며, 결과적으로 HR이 흔들린다  
  + 너무 긴 window는 실제 변화를 반영하지 못하거나 느리게 반영됨


### debigging  
- HR 튀는 순간 raw phase 확인
  + HR 급변 순간 range profile(doppler_input_real, imag 값)이 순간적으로 큰 변화를 가짐
  + 변화의 원인으로는 아래 3가지 정도(chat-GPT의 답변) 예상  
    A. 흉부 표면의 물리적 변위(Displacement)가 비선형적   
    * 참고  

    ```  
    Q: 레이더 심박 측정, 심박변화가 큰 지점에서 range profile 값이 크게 변하는 이유는  
    A: '미세한 변위(Displacement)'를 반사 신호의 위상 및 진폭 변화로 감지하기 때문
    (심박에 의한 신체 표면의 물리적 떨림이 해당 거리의 반사 신호 데이터를 흔들기 때문)  
        주요 이유  
        1. 흉벽의 물리적 움직임: 심장의 수축/이완 시 가슴 표면의 움직임(앞뒤로 움직인다)은
        미세함(0.1~0.5mm 수준). 송신파가 이 움직이는 표면에 맞고 돌아올 때 거리 변화에 따른
        Phase 변화 발생(이것이 Range Profile 상의 에너지 값(복소수 값의 변화)에 영향을 준다).  
        2. Doppler Effect: 심박으로 인해 흉벽이 움직이는 속도가 발생하면, 
        반사된 신호에 주파수 변이가 생깁니다. 이 속도 성분이 클수록(심박 변화가 역동적일수록) 
        Range Bin(해당 거리)에서 신호 변동 폭이 두드러진다.  

        3. 신호 대 잡음비(SNR)의 순간적 변화: 심장이 강하게 뛸 때 흉벽의 움직임이 커지면,
        정지해 있는 다른 신체 부위나 주변 지형지물(Clutter)에 비해 반사 신호의 변동성이 커짐.
        Range Profile은 거리별 반사 강도를 나타내는데, 특정 거리에서 신호가 계속 흔들리므로
        데이터상에서 값이 크게 튀는 것처럼 보인다.
    ```  

    B. Phase Unwrapping 및 신호처리 오류 (신호의 위상 점프 발생)  
    * phase unwrapping은 tc260417A 참고  

    C. 호흡 신호와의 혼입 (interference, HR 급변 순간 호흡과 심박의 간섭이 심해진다)  
    ```
    ㅁ 
    ```

- 같은 구간에서 respiration amplitude 확인
- fixed range-bin vs dynamic range-bin 비교
- FFT spectrum에서 peak 개수는 얼마인가?

- ~~unwrap 전/후 비교~~ : Gen. tc260417A  




---

Q. 심장 운동과 흉부 변위의 비선형적 관계를 해결하는 방법은?  
A. 레이더 기반 심박 측정에서 심장 운동(박동)과 흉부 변위 사이의 비선형적 관계는 주로 위상 왜곡(Phase Distortion)과 호흡 조화파(Respiratory Harmonics) 간섭에 의해 발생합니다. 이를 해결하기 위해 사용되는 주요 신호처리 기법은 다음과 같습니다. [1] 
## 1. 위상 모호성 해결을 위한 위상 복구 (Phase Unwrapping) [2] 
흉부의 움직임(변위)이 레이더 파장의 1/4을 초과할 경우, 위상이 $2\pi$를 넘어가며 신호가 겹치는 'Phase Wrapping' 현상이 발생하여 관계가 비선형적으로 변합니다. [2] 

* Arctangent Demodulation: 복소수 신호($I/Q$ 채널)의 아크탄젠트를 계산하여 위상을 직접 추출합니다.
* DACM (Differentiate and Cross Multiply): 위상을 직접 미분하고 다시 적분하는 방식으로, 아크탄젠트 계산 시 발생하는 위상 불연속성과 불필요한 계산 복잡도를 줄여 비선형 왜곡을 억제합니다. [2] 

## 2. 호흡 조화파 제거 (Harmonic Interference Cancellation)
심박 신호는 호흡 신호에 비해 진폭이 매우 작기 때문에, 호흡의 고조파(2차, 3차 조화파 등)가 심박의 기본 주파수 대역과 겹치며 비선형적인 간섭을 일으킵니다. [3, 4] 

* ECA (Extensive Cancellation Algorithm): 적응형 필터를 통해 호흡 신호와 그 조화파 성분을 추정하여 전체 신호에서 제거함으로써 순수한 심박 성분만 남깁니다.
* Comb Filter (빗살 필터): 호흡 주파수와 그 배수 지점의 에너지를 급격히 감쇄시켜 조화파의 영향을 최소화합니다. [5, 6] 

## 3. 고정밀 스펙트럼 분석 기술
비선형 잡음이 섞인 상태에서도 특정 주파수를 정확히 찾아내기 위한 알고리즘을 적용합니다.

* MUSIC (Multiple Signal Classification): 신호의 고유값 분해를 통해 해상도를 높여, 아주 미세한 심박 신호를 호흡 잡음으로부터 분리해냅니다.
* Wavelet Transform (웨이브렛 변환): 시간에 따라 변하는 심박의 비정상성(Non-stationarity)을 포착하여 노이즈를 제거하고 변위 신호를 재구성합니다. [1, 7] 

## 4. 딥러닝 기반 비선형 보정
최근에는 수학적 모델링 대신 HRConvNet과 같은 신경망을 활용합니다. 레이더 원시 데이터와 실제 심박 사이의 복잡한 비선형 관계를 학습시켜, 호흡이나 신체 움직임에 의한 왜곡을 지능적으로 보정하고 심박수를 추정합니다. [8] 
이러한 기법들은 주로 [Nature](https://www.nature.com/articles/s41598-025-09112-w.pdf) 및 [IEEE Xplore](https://ieeexplore.ieee.org/document/11226453) 등 전문 학술 자료에 상세히 기술되어 있으며, 정확도를 95% 이상으로 높이는 핵심 기술로 평가받고 있습니다. [7, 9, 10] 
구현하시려는 시스템에서 가장 큰 오차 원인이 호흡에 의한 간섭인가요, 아니면 위상 튐 현상인가요? 이에 따라 우선순위 알고리즘이 달라집니다. [11] 

[1] [https://www.nature.com](https://www.nature.com/articles/s41598-024-77683-1)
[2] [https://eusipco2025.org](https://eusipco2025.org/wp-content/uploads/pdfs/0002567.pdf)
[3] [https://ieeexplore.ieee.org](https://ieeexplore.ieee.org/document/6697732/)
[4] [https://koreascience.kr](https://koreascience.kr/article/JAKO201919059190643.page?&lang=ko)
[5] [https://arxiv.org](https://arxiv.org/pdf/2503.07062)
[6] [https://www.mdpi.com](https://www.mdpi.com/1424-8220/22/1/83)
[7] [https://www.nature.com](https://www.nature.com/articles/s41598-025-09112-w.pdf)
[8] [https://www.sciencedirect.com](https://www.sciencedirect.com/science/article/abs/pii/S0263224125012825)
[9] [https://ieeexplore.ieee.org](https://ieeexplore.ieee.org/document/11226453)
[10] [https://test.narangdesign.com](http://test.narangdesign.com/mail/kscvi/201812/a1.html)
[11] [https://pmc.ncbi.nlm.nih.gov](https://pmc.ncbi.nlm.nih.gov/articles/PMC7085680/)

---  

레이더 심박 측정에서 Phase Unwrapping(위상 언래핑)은 불연속적인 위상 값($-\pi \sim \pi$)을 연속적인 물리적 변위로 복원하는 핵심 과정입니다. 이 과정에서 오류가 발생하면 심박 신호에 거대한 스파이크(Spike)나 계단형 왜곡이 생겨 데이터 신뢰도가 떨어집니다. [1] 
오류 여부를 확인하는 주요 방법은 다음과 같습니다.
## 1. 시각적 스파이크 및 불연속점 확인 [2] 
가장 직관적인 방법은 언래핑된 위상 신호의 시계열 그래프를 확인하는 것입니다. [2] 

* $\pm 2\pi$ 급격한 점프: 언래핑 오류는 대개 $2\pi$의 정수배만큼 신호가 튀는 형태로 나타납니다.
* 비정상적인 위상 편차: 이상적인 환경에서 위상 변화는 완만한 파형을 그리지만, 노이즈나 낮은 SNR 구역에서는 위상이 무작위로 튀거나(Fluctuation) 분산이 급격히 커지는 현상이 관찰됩니다. [1, 3, 4, 5] 

## 2. 위상 미분(Phase Derivative) 분석
위상 신호를 시간에 대해 미분하여 변화율을 확인합니다. [^ChatGPT_1] 

* 스파이크 검출: 언래핑 오류가 있는 지점에서는 미분 값이 정상적인 생체 신호(호흡/심박)의 범위를 훨씬 초과하는 거대한 스파이크로 나타납니다.
* 속도 제한 조건: 흉부의 움직임 속도는 물리적인 한계가 있으므로, 특정 임계값(Threshold)을 넘는 급격한 위상 변화는 언래핑 실패로 간주할 수 있습니다. [^ChatGPT_3] 

## 3. I/Q 플롯(Constellation Diagram) 모니터링
복소 평면(I/Q 채널)에서 신호의 궤적을 확인합니다.

* 원호(Arc) 이탈: 정상적인 위상 변화는 복소 평면에서 원호를 그리며 움직여야 합니다. 만약 데이터 포인트가 원점에서 멀어지거나 가까워지는 등 궤적을 크게 벗어나면 해당 시점의 위상 추출과 언래핑에 오류가 발생했을 가능성이 높습니다.
* 위상 폐합(Phase Closure) 확인: 여러 경로의 신호를 사용하는 경우, 위상 차이의 합이 0이 되는지 확인하는 '위상 폐합' 방식을 통해 오류를 탐지하고 수정하기도 합니다. [2, 6, 7] 

## 4. 참조 신호 및 알고리즘 비교 [^ChatGPT_8] 

* 레퍼런스 비교: [OMRON과 같은 접촉식 장비](https://pmc.ncbi.nlm.nih.gov/articles/PMC9104941/)나 ECG 등 참조 기기의 데이터와 편차(MAE, MSE)를 비교하여 이상치를 탐지합니다.
* 알고리즘 교차 검증: 전통적인 Unwrapping 방식과 오류에 더 강한 DACM(Discrete Arctangent Differentiation and Cross-Multiplication) 또는 EDACM 알고리즘의 결과를 비교해 봅니다. 이들은 아크탄젠트 연산 대신 미분을 사용하여 원천적으로 언래핑 오류를 줄이는 방식입니다. [1, 9, 10, 11] 

팁: 언래핑 오류가 잦다면 샘플링 주파수($f_s$)를 높여 인접한 위상 간의 차이가 $\pi$를 넘지 않도록 설정하는 것이 가장 근본적인 해결책입니다. [^ChatGPT_12] 
언래핑 오류를 자동으로 보정하거나 필터링하는 특정 알고리즘(예: Kalman Filter, Wavelet Transform)에 대해 더 궁금하신가요?

[^ChatGPT_1]: [https://arxiv.org](https://arxiv.org/pdf/2508.20864)  
[2] [https://pmc.ncbi.nlm.nih.gov](https://pmc.ncbi.nlm.nih.gov/articles/PMC12431206/)  
[^ChatGPT_3]: [https://www.researchgate.net](https://www.researchgate.net/publication/398395539_Phase_Unwrapping_for_Heart_and_Breathing_Rate_Estimation_Using_mmWave_FMCW_Radar)  
[4] [https://opg.optica.org](https://opg.optica.org/abstract.cfm?uri=oe-30-5-6934)  
[5] [https://opus4.kobv.de](https://opus4.kobv.de/opus4-bam/files/54391/oe-30-5-6934.pdf)  
[6] [https://www.researchgate.net](https://www.researchgate.net/publication/286296948_Detection_and_correction_of_phase_unwrapping_errors_in_InSAR_time_series_analysis_with_discrete_coherent_points)  
[7] [https://arxiv.org](https://arxiv.org/html/2405.12659v1)  
[^ChatGPT_8]: [https://re.public.polimi.it](https://re.public.polimi.it/retrieve/3a83f4ef-7a1d-4176-b896-a65da188a6b9/2603.09791v1.pdf)  
[9] [https://www.sciencedirect.com](https://www.sciencedirect.com/science/article/abs/pii/S143484112500247X)  
[10] [https://pmc.ncbi.nlm.nih.gov](https://pmc.ncbi.nlm.nih.gov/articles/PMC9104941/)  
[11] [https://pmc.ncbi.nlm.nih.gov](https://pmc.ncbi.nlm.nih.gov/articles/PMC9104941/)  
[^ChatGPT_12]: [https://eurasip.org](https://eurasip.org/Proceedings/Eusipco/Eusipco2025/pdfs/0002567.pdf)  


---

# Papers  
[A radar vital signs detection method in complex environments](https://www.nature.com/articles/s41598-025-32042-6https://www.nature.com/articles/s41598-025-32042-6) [^1]

---  

# References  

[^1]: Traditional methods 로는 I/Q curve length method, amplitude variance analysis, and Moving Target Indication (MTI) techniques 등이 있음.  
[^2]: MEDIAN_WINDOW_LENGTH은 20, sort result는 obj_VS->pPeakSortTempIndex에 save.  
[^3]: sumEnergyHeartWfm, sumEnergyBreathWfm (confidence metric): 파형의 에너지가 유효한 파형으로 분류될 만큼 충분한지 판단하는 데 사용된다
      ※ confidence metric은 검출한 것에 대해 알고리즘이 얼마나 확신이 있는지를 알려주는 값으로, 값이 높을 수록 검출한 결과에 큰 확신이 있다는 것(confidence 레벨이 높다고 검출 성능이 정확한 것은 아니고, confidence level에 대해 threshold 값을 부여해서 그 이상이 되어야 검출한 것으로 인정)  
[^4]: sumEnergyBreathWfm*peakValueBreathSpect 하는 이유 및 이 연산을 breath에서만 하는 이유?  
[^7]: 클러터 제거 : 각 채널의 DC 성분(평균값)을 빼는 산술연산으로, vecsubc 함수에서 기능 수행  
[^mmWaveLink]: mmWaveLink : 센서 프런트 엔드와 통신하는 데 사용되는 저수준 API  
[^rlSetFrameCfg]: rlSetFrameCfg : 하위 수준 mmwavelink 함수  
[^2604131748]: beamforming output for the current range-bin and azimuth-bin  
[^2604151123]: Radar Cube Layout
  - TI mmwave의 L3 메모리는 2차원 배열 형태로 저장된다
    + [Range Bin] x [Chirp / Antenna (Doppler & Azimuth)]

[^chirpStep]: 하나의 거리(range bin) 안에 들어있는 모든 안테나와 모든 chirp의 sample 수 (안테나 데이터 전체의 길이)
[^numDopplerBins]: nChirps는 한 프레임 동안 실제로 발사된 chirp의 수
  - 신호처리 관점에서 chirp 방향으로 FFT(Doppler FFT)를 수행한 후의 데이터 칸 수
  - zero padding을 사용한다면, 두 값은 달라진다.(nChirp의 값이 2의 거듭제곱으로 설정되어있다면 zero padding을 사용하지 않았을 것)
    






# INDEX

1. [Heart Rate estimation process in Radar Signal](#Heart-Rate-estimation-process-in-Radar-Signal)
2. [filter_IIR_BiquadCascade](#filter_IIR_BiquadCascade)
3. 
---

# Heart Rate estimation process in Radar Signal
- 위상 추출 → BPF → FFT → 피크 검출
  ```
  #include <stdio.h>
  #include <math.h>
  
  #define SAMPLING_RATE 20.0     // 샘플링 주파수 (Hz)
  #define N_FFT 512              // FFT 포인트 수
  #define HEART_MIN_HZ 0.8       // 심박 최저 (약 48 bpm)
  #define HEART_MAX_HZ 2.5       // 심박 최고 (약 150 bpm)
  
  // 복소수 구조체
  typedef struct {
      float real;
      float imag;
  } Complex;
  
  // 심박수 추정 함수
  float estimateHeartRate(Complex* inputSignal, int length) {
      float phase[N_FFT];
      float fft_magnitude[N_FFT];
      
      // 1. 위상(Phase) 추출 및 Unwrapping (단순화 버전)
      for (int i = 0; i < length; i++) {
          phase[i] = atan2f(inputSignal[i].imag, inputSignal[i].real);
      }
  
      // 2. 대역 통과 필터링 (0.8Hz ~ 2.5Hz) 
      // *실제로는 FIR/IIR 필터 계수를 적용해야 함*
      // 여기서는 FFT 후 해당 영역의 피크만 찾는 방식으로 대체
  
      // 3. FFT 수행 (외부 라이브러리 호출 가정)
      // fft_execute(phase, fft_magnitude);
  
      // 4. 심박 대역 내 Peak 검출
      float maxMag = 0;
      int maxIdx = 0;
      
      for (int i = 0; i < N_FFT / 2; i++) {
          float freq = (float)i * SAMPLING_RATE / N_FFT;
          
          // 심박 대역(0.8~2.5Hz) 내에서만 최대값 검색
          if (freq >= HEART_MIN_HZ && freq <= HEART_MAX_HZ) {
              if (fft_magnitude[i] > maxMag) {
                  maxMag = fft_magnitude[i];
                  maxIdx = i;
              }
          }
      }
  
      // 5. 결과 변환 (Hz -> BPM)
      float heartRateHz = (float)maxIdx * SAMPLING_RATE / N_FFT;
      return heartRateHz * 60.0f; 
  }
  
  int main() {
      // 예시 데이터 입력 및 함수 호출
      // Complex iinput_data[4] = {{-0.00034, -0.00021}, ...};
      // float bpm = estimateHeartRate(iinput_data, 4);
      return 0;
  }
  ```

  + 위상 Unwrapping: 데이터가 $2\pi$ 범위를 넘어갈 때 값이 튀지 않도록 unwrap 로직 필요
  + DC 제거: 호흡 신호는 심박보다 매우 크기 때문에, 심박 FFT 전에 High-pass Filter나 Mean Removal를 통해 호흡 성분 억제
  + Windowing: FFT 전 Hamming 또는 Hanning 윈도우를 적용하여 주파수 Leakage을 줄여야 심박 피크가 잘 보입니다.
  + Zero-padding: 데이터 포인트가 적을 경우 N_FFT를 512나 1024로 크게 잡고 0을 채워 넣으면 주파수 해상도가 좋아집니다.  


# filter_IIR_BiquadCascade
## 개요  
- 레이더에서 수집된 Raw 신호(Phase Data)에서 호흡과 심박수에 해당하는 미세한 움직임 신호만을 추출하기 위해, 불필요한 노이즈를 제거하는 신호 처리(필터링) 역할
- Biquad Cascade 구조 (2차 IIR 필터를 직렬로 연결)
- 호흡 주파수 대역(약 $0.1 - 0.5 Hz$, $6 - 30 회/min$)과 심박 주파수 대역(약 $0.8 - 2.0 Hz$, $48 - 120 회/min$)만 통과시키는 BPF
- 필터를 cascade로 쌓아 차수를 증가시킴으로써 sharp한 cutoff(날카로운 필터링)
  
## 함수에서 필터링 주파수 대역을 정의하는 변수
- 입력 신호와 이미 계산된 계수(Coefficients)를 받아 연산을 수행하는 범용 함수
- Cutoff Frequency band를 정의하는 변수는 함수 내부에 없다(이 함수에 전달되는 필터 계수 배열이 그 대역 정보를 담고 있다)

## 샘플링 주파수
- 필터 계수를 설계할 때 가장 중요한 변수 중 하나(samplingFreq : 레이더의 fps에 따라 결정되는 샘플링 주파수, 이 값이 변하면 동일한 coefficient에서도 필터링 대역이 달라짐)
  ※ 주파수 대역 변경은 이 coefficient를 변경(matlab등을 이용하여 신규로 생성)해야 한다.

  

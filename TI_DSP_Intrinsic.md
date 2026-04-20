# TI DSP Intrinsic for TMS320C6000, c66x   

[ 1. _amem8_f2](#-amem8-f2)  
[ 2. _ftof2](#_ftof2)  
[ 3. __lof2, __hif2](#__lof2-__hif2)  
[ 4. _daddsp](#_daddsp)  
[ 5. _complex_mpysp](#_complex_mpysp)  
[ 6. __dinthsp](#__dinthsp)  
[ 7. _complex_conjugate_mpysp](#_complex_conjugate_mpysp)  
[ 8. _dmpysp](#_dmpysp)  
[ 9. __amem4](#__amem4)  


---

# _amem8_f2
- 8바이트(64비트) 데이터를 한 번에 읽거나 쓰는 정렬된 메모리 액세스(Aligned Load/Store)를 수행
- __float2_t 타입을 처리하는 데 사용되며, 이는 두 개의 단정밀도 부동 소수점(float) 값을 한 쌍으로 묶은 것

```
#include <string.h>

// __float2_t 정의 (TI 환경이 아닐 경우 가정)
typedef struct {
    float a;
    float b;
} float2_standard;  

...  
  
// _amem8_f2 → C
float2_standard c_amem8_f2(void *ptr) 
{
    float2_standard result;
    // 메모리에서 8바이트를 안전하게 복사 (정렬 문제 방지)
    memcpy(&result, ptr, sizeof(float2_standard));

    return result;
}
```

# _ftof2
- 두 개의 float 값을 하나의 __float2_t(64비트 가상 레지스터 타입)로 결합  

```
typedef struct {
    float f_low;  // 하위 32비트
    float f_high; // 상위 32비트
} float2_t;

// _ftof2(high, low) → C
float2_t c_ftof2(float high, float low) 
{
    float2_t result;
    result.f_high = high;
    result.f_low = low;
    return result;
}
```

# __lof2, __hif2
- __lof2 : __float2_t(64비트) 데이터에서 하위(Low) 32비트에 해당하는 float 값을 추출하는 함수  

```
typedef struct {
    float low;  // 하위 32비트
    float high; // 상위 32비트
} float2_t;

// __lof2(val)
float c_lof2(float2_t val) {
    return val.low;
}
```

- hif2 : __float2_t(64비트) 데이터에서 상위(High) 32비트에 해당하는 float 값을 추출하는 함수  

```
typedef struct {
    float low;  // 하위 32비트 (index 0)
    float high; // 상위 32비트 (index 1)
} float2_t;

// _hif2(val)
float c_hif2(float2_t val) {
    return val.high;
}
```

# _daddsp
- 두 개의 float 쌍을 한 번에 더하는 SIMD(Single Instruction Multiple Data) 내장 함수  
  (64비트 데이터 안에 들어있는 두 개의 float을 각각 더해 다시 64비트 결과)  

```
typedef struct {
    float low;
    float high;
} float2_t;

// _daddsp(src1, src2)
float2_t c_daddsp(float2_t a, float2_t b) 
{
    float2_t result;
    result.low = a.low + b.low;   // 하위 32비트끼리 덧셈
    result.high = a.high + b.high; // 상위 32비트끼리 덧셈
    return result;
}
```

# _complex_mpysp
- 복소수 곱셈(Complex Multiply) 내장 함수  
  $a(r1+i1)$ ｘ $b(r2+i2)$ = $(r1ｘr2 - i1ｘi2) + (r1ｘi2 + i1ｘr2)i$   

```
typedef struct {
    float r; // 실수를 low(하위 32비트)에 배치한다고 가정
    float i; // 허수를 high(상위 32비트)에 배치한다고 가정
} float2_t;

float2_t c_complex_mpysp(float2_t a, float2_t b) 
{
    float2_t res;
    // real = (a.r * b.r) - (a.i * b.i)
    res.r = (a.r * b.r) - (a.i * b.i);
    // imag = (a.r * b.i) + (a.i * b.r)
    res.i = (a.r * b.i) + (a.i * b.r);
    return res;
}
```

# __dinthsp  
- 데이터 타입 변환(정수 → 실수)
- 2개의 16비트 정수(Integer)를 2개의 단정밀도 부동 소수점(Float)으로 변환하는 SIMD 명령어  
  + 패킹된 short 타입 데이터 2개를 float 2개로 확장 변환
  + ADC(아날로그-디지털 변환기)에서 들어오는 정수형 데이터를 부동 소수점 연산(FFT, Filter 등)을 하기 위해 전처리할 때 주로 사용

```
typedef struct {
    float low;
    float high;
} float2_t;

// __dinthsp(src)
float2_t c_dinthsp(int32_t src) 
{
    float2_t res;
    
    // 1. 하위 16비트 추출 후 float 변환 (Signed 16-bit)
    int16_t low_int = (int16_t)(src & 0xFFFF);
    res.low = (float)low_int;
    
    // 2. 상위 16비트 추출 후 float 변환 (Signed 16-bit)
    int16_t high_int = (int16_t)((src >> 16) & 0xFFFF);
    res.high = (float)high_int;
    
    return res;
}
```

# _complex_conjugate_mpysp  
- 한쪽 복소수에 켤레(Conjugate)를 취한 후 곱셈을 수행하는 내장 함수(두 신호의 상관관계(Correlation)나 위상차를 구할 때 주로 사용)  
  $a(r1+i1)$과 $b(r2+i2)$가 존재할 때, $b$의 허수부 부호를 바꾼 $b^*(r2-i2)$를 $a(r1+i1)$와 곱하는 연산
  $a(r1+i1) ｘ b^*(r2-i2)$ = $(r1ｘr2 + i1ｘi2) + (i1ｘr2 + r1ｘi2)i$  


```
typedef struct {
    float r; // 실수를 low (하위 32비트)
    float i; // 허수를 high (상위 32비트)
} float2_t;

float2_t c_complex_conjugate_mpysp(float2_t a, float2_t b) 
{
    float2_t res;
    // b의 허수부에 마이너스를 취한 채로 곱셈하는 것과 동일
    // 실수부 = (a.r * b.r) - (a.i * -b.i) = a.r*b.r + a.i*b.i
    res.r = (a.r * b.r) + (a.i * b.i);
    // 허수부 = (a.r * -b.i) + (a.i * b.r) = a.i*b.r - a.r*b.i
    res.i = (a.i * b.r) - (a.r * b.i);
    return res;
}
```

# _dmpysp
- 두 쌍의 float 값을 각각 곱하는 SIMD(Single Instruction Multiple Data) 곱셈 내장 함수  
  (_daddsp 함수는 덧셈을 한 번에 두 개 처리)

```
typedef struct {
    float low;
    float high;
} float2_t;

// _dmpysp(src1, src2)
float2_t c_dmpysp(float2_t a, float2_t b) 
{
    float2_t result;
    result.low = a.low * b.low;   // 하위 32비트끼리 곱셈
    result.high = a.high * b.high; // 상위 32비트끼리 곱셈
    return result;
}
```

# __amem4  
- 4바이트(32비트) 데이터를 정렬 제한 없이(Unaligned) 읽거나 쓸 때 사용하는 내장 함수  

```
#include <string.h>
#include <stdint.h>

// __amem4를 이용한 읽기(Load)
uint32_t c_amem4_load(void *ptr) 
{
    uint32_t val;
    memcpy(&val, ptr, sizeof(uint32_t));
    return val;
}

// __amem4를 이용한 쓰기(Store)
void c_amem4_store(void *ptr, uint32_t val) 
{
    memcpy(ptr, &val, sizeof(uint32_t));
}
```























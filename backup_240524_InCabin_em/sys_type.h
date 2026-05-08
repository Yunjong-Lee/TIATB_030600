
// #include <stdbool.h>
#include <stdio.h>
#include <stdint.h>



/*! @brief  Complex data type. This type of input, (first real than imaginary part), is required for DSP lib FFT functions */
/*  Complex type is often manipulated in the DSP using special compiler intrinsics
 *  (e.g _cmpyr) which requires the data of this type to be 32-bit (4 byte) aligned.
 *  Using intrinsics on unaligned memory can cause unexpected results.		*/   




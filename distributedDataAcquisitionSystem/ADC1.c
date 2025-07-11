/* ADC1.c
 * ANMOL DUBEY & NISCHAY HEGDE
 * Modified: put the date here
 * 12-bit ADC input on ADC1 channel 5, PB18
 */

#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"


void ADCinit(void){
// write code to initialize ADC1 channel 5, PB18
// Your measurement will be connected to PB18
// 12-bit mode, 0 to 3.3V, right justified
// software trigger, no averaging
  ADC1->ULLMEM.GPRCM.RSTCTL = 0xB1000003; // 1) reset
  ADC1->ULLMEM.GPRCM.PWREN = 0x26000001;  // 2) activate
  Clock_Delay(24);                        // 3) wait
  ADC1->ULLMEM.GPRCM.CLKCFG = 0xA9000000; // 4) ULPCLK
  ADC1->ULLMEM.CLKFREQ = 7;               // 5) 40-48 MHz
  ADC1->ULLMEM.CTL0 = 0x03010000;         // 6) divide by 8
  ADC1->ULLMEM.CTL1 = 0x00000000;         // 7) mode
  ADC1->ULLMEM.CTL2 = 0x00000000;         // 8) MEMRES
  ADC1->ULLMEM.MEMCTL[0] = 5;             // 9) channel 6 is PB20
  ADC1->ULLMEM.SCOMP0 = 0;                // 10) 8 sample clocks
  ADC1->ULLMEM.CPU_INT.IMASK = 0;         // 11) no interrupt
}
uint32_t ADCin(void){
  // write code to sample ADC1 channel 5, PB18 once
  ADC1->ULLMEM.CTL0 |= 0x00000001;             // 1) enable conversions
  ADC1->ULLMEM.CTL1 |= 0x00000100;             // 2) start ADC
  uint32_t volatile delay=ADC0->ULLMEM.STATUS; // 3) time to let ADC start
  while((ADC1->ULLMEM.STATUS&0x01)==0x01){}    // 4) wait for completion
  return ADC1->ULLMEM.MEMRES[0];               // 5) 12-bit result
  // return digital result (0 to 4095)
}

// your function to convert ADC sample to distance (0.001cm)
// use main2 to calibrate the system fill in 5 points in Calibration.xls
//    determine constants k1 k2 to fit Position=(k1*Data + k2)>>12
uint32_t Convert(uint32_t input){
  uint32_t Integer = (2000*input)>>12;
  return Integer; // replace this with a linear function
}

// do not use this function for the final lab
// it is added just to show you how SLOW floating point in on a Cortex M0+
float FloatConvert(uint32_t input){
  return 0.00048828125*input -0.0001812345;
}

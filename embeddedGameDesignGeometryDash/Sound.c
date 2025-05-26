// Sound.c
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// your name
// your data 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/ADC1.h"
#include "../inc/DAC5.h"
#include "../inc/Arabic.h"

uint16_t cnt = 0;

void SysTick_IntArm(uint32_t period, uint32_t priority){
  SysTick->CTRL = 0x00; // disable SysTick during setup
  SysTick->LOAD = period-1;  // reload value
  SCB->SHP[1] = (SCB->SHP[1]&(~0xC0000000))|(priority<<30);
  SysTick->VAL = 0; // any write to VAL clears COUNT and sets VAL equal to LOAD
  SysTick->CTRL = 0x07; // enable SysTick with 80 MHz bus clock and interrupts  
}
// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5-bit DAC
void Sound_Init(void){
  SysTick_IntArm(1, 0);
}
void SysTick_Handler(void){ // called at 11 kHz
  // write this
    // output one value to DAC
    const uint8_t SinWave[32] = {16,19,22,24,27,28,30,31,31,31,30,28,27,24,22,19,16,13,10,8,5,4,2,1,1,1,2,4,5,8,10,13};
    const uint8_t SquareWave[32] = {31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    DAC5_Out(SquareWave[cnt]);
    cnt++;
    if(cnt>31) {cnt=0;}
}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Stop(void){
  SysTick->CTRL = (SysTick->CTRL) & ~0x1; // disable SysTick during setup
  // either set LOAD to 0 or clear bit 1 in CTRL
}


void Sound_Start(uint32_t period){
    SysTick->LOAD = period-1;
    SysTick->VAL = 0; // any write to VAL clears COUNT and sets VAL equal to LOAD
    SysTick->CTRL = 0x07; // enable SysTick with 80 MHz bus clock and interrupts  
    // set reload value
    // write any value to VAL, cause reload
    // write this

}

/*
 * Switch.c
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"

// LaunchPad.h defines all the indices into the PINCM table
void Switch_Init(void){
    // write this
    IOMUX->SECCFG.PINCM[PB17INDEX] = 0x00040081;
    IOMUX->SECCFG.PINCM[PB12INDEX] = 0x00050081;
}
// return current state of switches
uint32_t Switch_In1(void){
    // write this
    uint32_t o = GPIOB->DIN31_0&0x20000;
    return o; // replace this line
}

// return current state of switches
uint32_t Switch_In2(void){
    // write this
    uint32_t o = GPIOB->DIN31_0&0x1000;
    return o; // replace this line
}

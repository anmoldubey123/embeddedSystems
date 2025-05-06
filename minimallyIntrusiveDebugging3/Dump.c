// Dump.c
// Your solution to ECE319K Lab 3 Spring 2025
// Author: Anmol Dubey
// Last Modified: 2/11/25

#include <ti/devices/msp/msp.h>
#include "../inc/Timer.h"
#define MAXBUF 50
uint32_t DataBuffer[MAXBUF];
uint32_t TimeBuffer[MAXBUF];  
uint32_t DebugCnt; // 0 to MAXBUF (0 is empty, MAXBUF is full)

//Global variables for DebugDump2
uint32_t callCnt = 0;
uint32_t prevData;


// *****Debug_Init******
// Initializes your index or pointer.
// Input: none
// Output:none
// students write this for Lab 3
// This function should also initialize Timer G12, call TimerG12_Init.
void Debug_Init(void)
{
DebugCnt = 0; //initialze buffer by clearing index 
TimerG12_Init(); //initialize TimerG12 by calling TimerG12_Init()
}

// *****Debug_Dump******
// Records one data and one time into the two arrays.
// Input: data is value to store in DataBuffer
// Output: 1 for success, 0 for failure (buffers full)
// students write this for Lab 3
// The software simply reads TIMG12->COUNTERREGS.CTR to get the current time in bus cycles.
uint32_t Debug_Dump(uint32_t data)
{
  if(DebugCnt >= MAXBUF)
  {
    return 0; //Failure: Return 0 if buffers are full
  }

  DataBuffer[DebugCnt] = data; 
  TimeBuffer[DebugCnt] = TIMG12->COUNTERREGS.CTR;
  DebugCnt++; //increment DebugCnt 

  return 1; // success 
}

// *****Debug_Dump2******
// Always record data and time on the first call to Debug_Dump2
// However, after the first call
//    Records one data and one time into the two arrays, only if the data is different from the previous call.
//    Do not record data or time if the data is the same as the data from the previous call
// Input: data is value to store in DataBuffer
// Output: 1 for success (saved or skipped), 0 for failure (buffers full)
// students write this for Lab 3
// The software simply reads TIMG12->COUNTERREGS.CTR to get the current time in bus cycles.
uint32_t Debug_Dump2(uint32_t data)
{
  if(DebugCnt >= MAXBUF)
  {
    return 0; //Failure: Return 0 if buffers are full
  }

  if(callCnt<1)
  {
    DataBuffer[DebugCnt] = data; 
    prevData=data;
    TimeBuffer[DebugCnt] = TIMG12->COUNTERREGS.CTR;
    DebugCnt++;
    callCnt++;
  }
  else if (prevData!=data)
  {
    DataBuffer[DebugCnt] = data; 
    prevData=data;
    TimeBuffer[DebugCnt] = TIMG12->COUNTERREGS.CTR;
    DebugCnt++;
  }
  else 
  {
    return 0; //Failure: skipped data
  }

  return 1; // success
}

// *****Debug_Period******
// Calculate period of the recorded data using mask
// Input: mask specifies which bit(s) to observe
// Output: period in bus cycles
// Period is defined as rising edge (low to high) to the next rising edge.
// Return 0 if there is not enough collected data to calculate period .
// students write this for Lab 3
// This function should not alter the recorded data.
// AND each recorded data with mask,
//    if nonzero the signal is considered high.
//    if zero, the signal is considered low.
uint32_t Debug_Period(uint32_t mask)
{
  if(DebugCnt < 2)
  {
    return 0; // Not enough data for collection
  }

  uint32_t lastEdgeTime = 0;
  uint32_t edgeCount = 0;
  uint32_t totalPeriod = 0;
  int32_t period = 0;
  bool firstEdgeFound = false;
  bool wasLow = true; // Track if the previous value was low

  for(uint32_t i = 0; i < DebugCnt; i++)
  {
    if((DataBuffer[i] & mask) != 0) // Check if the masked value is nonzero (rising edge)
    {
      if(wasLow) // Only consider this if the previous value was low
      {
        if(firstEdgeFound) // If we already found the first valid edge, calculate the period
        {
          period = lastEdgeTime - TimeBuffer[i]; // Timer is counting down
          totalPeriod += period;
          edgeCount++;
        }
        else
        {
          firstEdgeFound = true; // Mark the first rising edge detection
        }
        lastEdgeTime = TimeBuffer[i]; // Update lastEdgeTime
      }
      wasLow = false; // This value is high, so next must transition to low to reset
    }
    else
    {
      wasLow = true; // Reset when data is low
    }
  }

  if(edgeCount > 0)
  {
    return totalPeriod / edgeCount;
  }

  return 0; // No valid period found
}




// *****Debug_Duty******
// Calculate duty cycle of the recorded data using mask
// Input: mask specifies which bit(s) to observe
// Output: period in percent (0 to 100)
// Period is defined as rising edge (low to high) to the next rising edge.
// High is defined as rising edge (low to high) to the next falling edge.
// Duty cycle is (100*High)/Period
// Return 0 if there is not enough collected data to calculate duty cycle.
uint32_t Debug_Duty(uint32_t mask){
// students write this for Lab 3
// This function should not alter the recorded data.
// AND each recorded data with mask,
//    if nonzero the signal is considered high.
//    if zero, the signal is considered low.

  return 42; // average duty cycle in percent
}

// Lab2 specific debugging code
uint32_t Theperiod;
void Dump(void) {
    uint32_t southLEDs = (GPIOB->DOUT31_0 & 0x07);  // Extract PB0, PB1, PB2 (South LEDs)
    uint32_t westLEDs = ((GPIOB->DOUT31_0 >> 6) & 0x07); // Extract PB6, PB7, PB8 (West LEDs)
    uint32_t walkLEDs = ((GPIOB->DOUT31_0 >> 22) & 0x07); // Extract PB22, PB26, PB27 (Walk LEDs)

    uint32_t data = (southLEDs | (westLEDs << 6) | (walkLEDs << 22));  // Combine all LED states

    uint32_t result = Debug_Dump(data);  // Call Debug_Dump with the combined data
    if(result == 0) {  // If Debug_Dump failed (buffers full)
        Theperiod = Debug_Period(1<<18);  // Optionally check period
        __asm volatile("bkpt; \n"); // breakpoint here for debugging
    }
}






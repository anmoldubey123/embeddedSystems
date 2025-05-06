/* ECE319K_Lab4main.c
 * Traffic light FSM
 * ECE319H students must use pointers for next state
 * ECE319K students can use indices or pointers for next state
 * Put your names here or look silly
 Anmol Dubey
 Nischay Hegde
  */

#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/UART.h"
#include "../inc/Timer.h"
#include "../inc/Dump.h"  // student's Lab 3
#include <stdio.h>
#include <string.h>
// put both EIDs in the next two lines
const char EID1[] = "AD56328"; //  ;replace abc123 with your EID
const char EID2[] = "NSH787"; //  ;replace abc123 with your EID


uint32_t PREVIOUS_OUTPUT_WALK = 1; //(WALK_P26_R + WEST_P8_RED + SOUTH_P2_RED);
uint32_t PREVIOUS_OUTPUT_SOUTH = 1;
uint32_t PREVIOUS_OUTPUT_WEST = 1;

// Hint implement Traffic_Out before creating the struct, make struct match your Traffic_Out

// initialize all 6 LED outputs and 3 switch inputs
// assumes LaunchPad_Init resets and powers A and B
void Traffic_Init(void)
{
  IOMUX->SECCFG.PINCM[PB0INDEX] = 0x00000081; //SOUTH
  IOMUX->SECCFG.PINCM[PB1INDEX] = 0x00000081;
  IOMUX->SECCFG.PINCM[PB2INDEX] = 0x00000081;

  IOMUX->SECCFG.PINCM[PB6INDEX] = 0x00000081; //WEST
  IOMUX->SECCFG.PINCM[PB7INDEX] = 0x00000081;
  IOMUX->SECCFG.PINCM[PB8INDEX] = 0x00000081;

  IOMUX->SECCFG.PINCM[PB22INDEX] = 0x00000081; //WALK
  IOMUX->SECCFG.PINCM[PB26INDEX] = 0x00000081;
  IOMUX->SECCFG.PINCM[PB27INDEX] = 0x00000081;

  IOMUX->SECCFG.PINCM[PB15INDEX] = 0x00040081; //SENSOR
  IOMUX->SECCFG.PINCM[PB16INDEX] = 0x00040081;
  IOMUX->SECCFG.PINCM[PB17INDEX] = 0x00040081;

  uint32_t temp = GPIOB->DOE31_0;
  GPIOB->DOE31_0 = temp | (1<<0);
  temp = GPIOB->DOE31_0;
  GPIOB->DOE31_0 = temp | (1<<1);
  temp = GPIOB->DOE31_0;
  GPIOB->DOE31_0 = temp | (1<<2);
  temp = GPIOB->DOE31_0;
  GPIOB->DOE31_0 = temp | (1<<6);
  temp = GPIOB->DOE31_0;
  GPIOB->DOE31_0 = temp | (1<<7);
  temp = GPIOB->DOE31_0;
  GPIOB->DOE31_0 = temp | (1<<8);
  temp = GPIOB->DOE31_0;
  GPIOB->DOE31_0 = temp | (1<<22);
  temp = GPIOB->DOE31_0;
  GPIOB->DOE31_0 = temp | (1<<26);
  temp = GPIOB->DOE31_0;
  GPIOB->DOE31_0 = temp | (1<<27);
}

/* Activate LEDs
* Inputs: west is 3-bit value to three east/west LEDs
*         south is 3-bit value to three north/south LEDs
*         walk is 3-bit value to 3-color positive logic LED on PB22,PB26,PB27
* Output: none
* - west =1 sets west green
* - west =2 sets west yellow
* - west =4 sets west red
* - south =1 sets south green
* - south =2 sets south yellow
* - south =4 sets south red
* - walk=0 to turn off LED
* - walk bit 22 sets blue color
* - walk bit 26 sets red color
* - walk bit 27 sets green color
* Feel free to change this. But, if you change the way it works, change the test programs too
* Be friendly*/
void Traffic_Out(uint32_t west, uint32_t south, uint32_t walk){
  GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 & (0xF3BFFE38)); //clears out all the bits that we will use in the D OUT register
 
  GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 | south);
  
  GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 | (west<<6));

  GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 | walk); //Walk we do not care of because they are already in 32 bit format

}

void Delay1(uint32_t timex){
  SysTick_Wait10ms(timex*15);
}


/* Read sensors
 * Input: none
 * Output: sensor values
 * - bit 0 is west car sensor
 * - bit 1 is south car sensor
 * - bit 2 is walk people sensor
* Feel free to change this. But, if you change the way it works, change the test programs too
 */
uint32_t Traffic_In(void) {
    uint32_t sensors = GPIOB->DIN31_0; // Read the entire input register

    uint32_t west = (sensors >> 15) & 0x1;  // Extract PB15 (West car sensor)
    uint32_t south = (sensors >> 16) & 0x1; // Extract PB16 (South car sensor)
    uint32_t walk = (sensors >> 17) & 0x1;  // Extract PB17 (Walk sensor)

    return (west | (south << 1) | (walk << 2)); // Pack into bits 0,1,2
}


#define goS 0
#define waitS 1
#define redS 2
#define goWalk 3
#define Fon1 4
#define Foff1 5
#define Fon2 6
#define Foff2 7
#define redWalk 8
#define goW 9
#define waitW 10
#define redW 11


struct state {
  uint32_t Out[3];
  uint32_t Time;
  uint32_t Next[8]; 
};
typedef struct state State_t;

// {south walk west}, wait time, {possible inputs}
State_t FSM[12] = {
  {{4,1,0x04000000}, 300, {goS,     waitS,   goS,     waitS,   waitS,    waitS,   waitS,   waitS}}, //goS
  {{4,2,0x04000000}, 200, {redS,    redS,    redS,    redS,    redS,     redS,    redS,    redS}}, //wait South - waitS
  {{4,4,0x04000000}, 200, {goWalk,  goW,     goWalk,  goWalk,  goWalk,   goWalk,  goWalk,  goWalk}}, //All red south - redS
  {{4,4,0x0C400000}, 300, {goWalk,  Fon1,    Fon1,    Fon1,    goWalk,   Fon1,    Fon1,    Fon1}}, //go walk - goWalk
  {{4,4,0x04000000}, 100, {Foff1,   Foff1,   Foff1,   Foff1,   Foff1,    Foff1,   Foff1,   Foff1}}, //Flash on (red) - FlashOn1
  {{4,4,0},          100, {Fon2,    Fon2,    Fon2,    Fon2,    Fon2,     Fon2,    Fon2,    Fon2}}, //FlashOff1 
  {{4,4,0x04000000}, 100, {Foff2,   Foff2,   Foff2,   Foff2,   Foff2,    Foff2,   Foff2,   Foff2}}, //Flash on (red) - FlashOn2
  {{4,4,0},          100, {redWalk, redWalk, redWalk, redWalk, redWalk,  redWalk, redWalk, redWalk}}, //FlashOff2
  {{4,4,0x04000000}, 200, {goW,     goW,     goS,     goW,     goW,      goW,     goW,     goW}}, //All red - redWalk
  {{1,4,0x04000000}, 300, {goW,     goW,     waitW,   waitW,   waitW,    waitW,   waitW,   waitW}},//go West - goW
  {{2,4,0x04000000}, 200, {redW,    redW,    redW,    redW,    redW,     redW,    redW,    redW}}, //wait West - waitW
  {{4,4,0x04000000}, 200, {goS,     goS,     goS,     goS,     goWalk,   goS,     goS,     goS}}//All Red West - redW
 
  };


// use main1 to determine Lab4 assignment
void Lab4Grader(int mode);
void Grader_Init(void);
int main1(void){ // main1
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Lab4Grader(0); // print assignment, no grading
  while(1){
  }
}
// use main2 to debug LED outputs
// at this point in ECE319K you need to be writing your own test functions
// modify this program so it tests your Traffic_Out  function
int main2(void){ // main2
  uint32_t counter = 0;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Grader_Init(); // execute this line before your code
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
  if((GPIOB->DOE31_0 & 0x20)==0){
    UART_OutString("access to GPIOB->DOE31_0 should be friendly.\n\r");
  }
  UART_Init();
  UART_OutString("Lab 4, Spring 2025, Step 1. Debug LEDs\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");

  while(1){
    // Test different traffic light states
    UART_OutString("Setting South Green, West Red\n\r");
    Traffic_Out(0, 1, 4);  // Walk Off, South Green, West Red
    Debug_Dump(counter);
    counter++;
    Clock_Delay1ms(1000);

    UART_OutString("Setting South Yellow, West Red\n\r");
    Traffic_Out(0, 2, 4);  // Walk Off, South Yellow, West Red
    Debug_Dump(counter);
    counter++;
    Clock_Delay1ms(1000);

    UART_OutString("Setting South Red, West Green\n\r");
    Traffic_Out(0, 4, 1);  // Walk Off, South Red, West Green
    Debug_Dump(counter);
    counter++;
    Clock_Delay1ms(1000);

    UART_OutString("Setting South Red, West Yellow\n\r");
    Traffic_Out(0, 4, 2);  // Walk Off, South Red, West Yellow
    Debug_Dump(counter);
    counter++;
    Clock_Delay1ms(1000);

    UART_OutString("Setting Walk LED On (Red)\n\r");
    Traffic_Out(0x04000000, 4, 4);  // Walk Red On, South Red, West Red
    Debug_Dump(counter);
    counter++;
    Clock_Delay1ms(1000);

    UART_OutString("Setting Walk LED Off\n\r");
    Traffic_Out(0, 4, 4);  // Walk Off, South Red, West Red
    Debug_Dump(counter);
    counter++;
    Clock_Delay1ms(1000);

    if((GPIOB->DOUT31_0 & 0x20) == 0){
      UART_OutString("DOUT not friendly\n\r");
    }
  }
}



// use main3 to debug the three input switches
// at this point in ECE319K you need to be writing your own test functions
// modify this program so it tests your Traffic_In  function
int main3(void){ // main3
  uint32_t last=0, now;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Traffic_Init(); // your Lab 4 initialization
  Debug_Init();   // Lab 3 debugging
  UART_Init();
  __enable_irq(); // UART uses interrupts
  UART_OutString("Lab 4, Spring 2025, Step 2. Debug switches\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");

  while(1){
    now = Traffic_In(); // Your Lab4 input
    if(now != last){ // change detected
      UART_OutString("Switch= 0x"); 
      UART_OutUHex(now); 
      UART_OutString("\n\r");

      // Print corresponding message
      if(now & 0x1){ // West sensor is active
        UART_OutString("West switch pressed\n\r");
      }
      if(now & 0x2){ // South sensor is active
        UART_OutString("South switch pressed\n\r");
      }
      if(now & 0x4){ // Walk sensor is active
        UART_OutString("Walk switch pressed\n\r");
      }

      Debug_Dump(now);
    }
    last = now;
    Clock_Delay(800000); // 10ms, to debounce switch
  }
}

uint8_t currentState;

int main4(void){// main4
  Clock_Init80MHz(0);
  LaunchPad_Init();
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
  Grader_Init();
 // set initial state
  Debug_Init();   // Lab 3 debugging
  UART_Init();
  __enable_irq(); // UART uses interrupts
  UART_OutString("Lab 4, Spring 2025, Step 3. Debug FSM cycle\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
// initialize your FSM
  SysTick_Init();   // Initialize SysTick for software waits

  currentState = 0;
  Traffic_Out(FSM[currentState].Out[0], FSM[currentState].Out[1], FSM[currentState].Out[2]);

  while(1){
      // 1) output depending on state using Traffic_Out
      // call your Debug_Dump logging your state number and output
      // 2) wait depending on state
      // 3) hard code this so input always shows all switches pressed
      // 4) next depends on state and input
    uint32_t dumpOUT;

    dumpOUT = currentState<<24; //state number
//west
    dumpOUT += ((GPIOB->DOUT31_0 & 0x1C0) <<16); //how to get the mask for West, PB8 - PB6 0x1C0 // looking for the west output 
//south
    dumpOUT += ((GPIOB->DOUT31_0 & 0x7) <<8); //how to get the mask for South, PB2 - PB0 0x7 // looking for the west output 

//dumpOUT Walk Ports

    dumpOUT += (GPIOB->DOUT31_0 & 0x08000000) >> 27; //mask for walk, PB27 green, shift 27 bits to right

    dumpOUT += (GPIOB->DOUT31_0 & 0x04000000) >> 26; //mask for walk, PB27 red, shift 26 bits to right    

    dumpOUT += (GPIOB->DOUT31_0 & 0x00400000) >> 22; //mask for walk, PB27 blue, shift 22 bits to right


    currentState = FSM[currentState].Next[7];
    Traffic_Out(FSM[currentState].Out[0], FSM[currentState].Out[1], FSM[currentState].Out[2]);
    SysTick_Wait10ms(FSM[currentState].Time);    
    Debug_Dump(dumpOUT);
    if ((GPIOB->DOUT31_0 & 0x20) == 0){
      UART_OutString("DOUT not friendly\n\r");
  }
}
}



int main(void) { // main5
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Grader_Init(); // execute this line before your code
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization

  // Initialize FSM and SysTick for software waits
  SysTick_Init();

  // Initialize grader, UART, and interrupts
  Lab4Grader(1);

  currentState = 0;
  Traffic_Out(FSM[currentState].Out[0], FSM[currentState].Out[1], FSM[currentState].Out[2]);

  while (1) {
      uint32_t dumpOUT;
      
      dumpOUT = currentState << 24; // state number

      // West Traffic Lights (PB8 - PB6)
      dumpOUT += ((GPIOB->DOUT31_0 & 0x1C0) << 16); // Mask for West (PB8 - PB6 = 0x1C0)
      
      // South Traffic Lights (PB2 - PB0)
      dumpOUT += ((GPIOB->DOUT31_0 & 0x7) << 8); // Mask for South (PB2 - PB0 = 0x7)

      // Walk LED Ports (PB22 - PB27)
      dumpOUT += (GPIOB->DOUT31_0 & 0x08000000) >> 27; // Mask for walk green, PB27
      dumpOUT += (GPIOB->DOUT31_0 & 0x04000000) >> 26; // Mask for walk red, PB26
      dumpOUT += (GPIOB->DOUT31_0 & 0x00400000) >> 22; // Mask for walk blue, PB22

      // Input from sensors (Walk, South, West - PB17, PB16, PB15)
      uint32_t input = Traffic_In(); // Traffic_In should be a function that returns the input status

      // State transition based on input
      currentState = FSM[currentState].Next[input];

      // Output the state and traffic signals
      Traffic_Out(FSM[currentState].Out[0], FSM[currentState].Out[1], FSM[currentState].Out[2]);

      // Wait according to the FSM's timing
      SysTick_Wait10ms(FSM[currentState].Time);

      // Dump the state output for debugging
      Debug_Dump(dumpOUT);
  }
}
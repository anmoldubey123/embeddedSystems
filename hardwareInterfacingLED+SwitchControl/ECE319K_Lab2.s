//****************** ECE319K_Lab2.s ***************
// Your solution to Lab 2 with debugging instrumentation
// Author: Anmol Dubey
// Last Modified: Your date
// ECE319K Spring 2025 (ECE319H students do Lab2H)
// I/O port addresses
    .include "../inc/msp.s"

        .data
        .align 2

        .text
        .thumb
        .align 2
        .global EID
EID:    .string "AD56328" // replace ZZZ123 with your EID here
        .align 2

// Lab2Grader and Lab3 Debugging Functions
        .global Lab2Grader
        .global Lab2
        .global Debug_Init
        .global Dump

Lab2:
     MOVS R0,#1  // Debug mode: logic analyzer enabled
     BL   Lab2Grader
     BL   Debug_Init // Initialize debugging system
     BL   Lab2Init   // Initialize LED and Ports
     MOVS R4, #0     // Cycle counter

loop:
        CMP R4, #4
        BNE NEXT_CYCLE
        MOVS R4, #0   // Reset cycle counter

NEXT_CYCLE:
        CMP R4, #0
        BEQ CYCLEONE
        CMP R4, #1
        BEQ CYCLETWO
        CMP R4, #2
        BEQ CYCLETHREE
        CMP R4, #3
        BEQ CYCLEFOUR
        B loop

CYCLEONE:
        BL Dump      // Log before LED turns on
        BL turnOn
        LDR R0, =160000 // H=2ms
        BL Delay

        BL Dump      // Log before LED turns off
        BL turnOff
        LDR R0, =1440000 // L=18ms
        BL Delay

        ADDS R4, R4, #1
        B loop

CYCLETWO:
        BL Dump      // Log before LED turns on
        BL turnOn
        LDR R0, =480000 // H=6ms
        BL Delay

        BL Dump      // Log before LED turns off
        BL turnOff
        LDR R0, =1120000 // L=14ms
        BL Delay

        ADDS R4, R4, #1
        B loop

CYCLETHREE:
        BL Dump      // Log before LED turns on
        BL turnOn
        LDR R0, =640000 // H=8ms
        BL Delay

        BL Dump      // Log before LED turns off
        BL turnOff
        LDR R0, =960000 // L=12ms
        BL Delay

        ADDS R4, R4, #1
        B loop

CYCLEFOUR:
        BL Dump      // Log before LED turns on
        BL turnOn
        LDR R0, =1120000 // H=14ms
        BL Delay

        BL Dump      // Log before LED turns off
        BL turnOff
        LDR R0, =480000 // L=6ms
        BL Delay

        ADDS R4, R4, #1
        B loop

Delay:
        SUBS R0, R0, #2 
dloop:  SUBS R0, R0, #4
        NOP
        BHS dloop
        BX LR

turnOn: 
     LDR R1, = GPIOB_DOUT31_0
     LDR R2, [R1]
     LDR R3, =(1<<18)
     ORRS R2, R2, R3
     STR R2, [R1]
     BX LR

turnOff:
        LDR R1, = GPIOB_DOUT31_0
        LDR R2, [R1]
        LDR R3, =(1<<18)
        BICS R2, R2, R3
        STR R2, [R1]
        BX LR

// Initialize LED and Switch
Lab2Init:
   MOVS R1, #0X81
   LDR R0, =IOMUXPB18
   STR R1, [R0]

   LDR R0, =GPIOB_DOE31_0
   LDR R1, [R0]
   LDR R2, =0x40000
   ORRS R1, R1, R2
   STR R1, [R0]

   BX   LR

   .end

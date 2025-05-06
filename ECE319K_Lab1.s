//****************** ECE319K_Lab1.s ***************
// Your solution to Lab 1 in assembly code
// Author: Anmol Dubey
// Last Modified: 1/26/2025
// Spring 2025
        .data
        .align 2
// Declare global variables here if needed
// with the .space assembly directive

        .text
        .thumb
        .align 2
        .global EID
EID:    .string "AD56328" // replace ZZZ123 with your EID here

        .global Phase
        .align 2
Phase:  .long 10
// Phase= 0 will display your objective and some of the test cases, 
// Phase= 1 to 5 will run one test case (the ones you have been given)
// Phase= 6 to 7 will run one test case (the inputs you have not been given)
// Phase=10 will run the grader (all cases 1 to 7)
        .global Lab1
// Input: R0 points to the list
// Return: R0 as specified in Lab 1 assignment and terminal window
// According to AAPCS, you must save/restore R4-R7
// If your function calls another function, you must save/restore LR

Lab1: PUSH {R4-R7,LR}
       // your solution goes here
        LDR R1, =EID // R1 is base address of my EID
        MOVS R7, R1

        LDR R2, [R0] // load R2 w mem[R0] aka address of first character of string
        MOVS R3, R2 // move address of first char of string to R3

LOOP:
        LDRB R4, [R3] // R4 = mem[R3] gets char from data structure EID 
        LDRB R5, [R7] // R5 = mem[R1] gets char from my eid

        ADDS R6, R4, R5
        CMP R6, #0
        BEQ FIN

        CMP R4, R5
        BNE NEXT

        ADDS R7, R7, #1
        ADDS R3, R3, #1

        B LOOP

NEXT:
        MOVS R7, R1
        ADDS R0, R0, #8       
        LDR R2, [R0] 

        CMP R2, #0
        BEQ DONE 

        MOVS R3, R2
        B LOOP

FIN:   
        ADDS R0, R0, #4
        LDR R2, [R0]
        MOVS R0, R2
        POP  {R4-R7,PC} // return

DONE:
        LDR R0, =-1


      POP  {R4-R7,PC} // return


        .align 2
        .global myClass
myClass: .long pAB123  // pointer to EID
         .long 95      // Score
         .long pXYZ1   // pointer to EID
         .long 96      // Score
         .long pAB5549 // pointer to EID
         .long 94      // Score
         .long 0       // null pointer means end of list
         .long 0
pAB123:  .string "AB123"
pXYZ1:   .string "XYZ1"
pAB5549: .string "AB5549"
        .end

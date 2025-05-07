// StringConversion.s
// Student names: Anmol Dubey & Nischay Hegde
// Last modification date: 3/10/25
// Runs on any Cortex M0
// ECE319K lab 6 number to string conversion
//
// You write udivby10 and Dec2String
     .data
     .align 2
// no globals allowed for Lab 6
    .global OutChar    // virtual output device
    .global OutDec     // your Lab 6 function
    .global Test_udivby10

    .text
    .align 2

// **test of udivby10**
// since udivby10 is not AAPCS compliant, we must test it in assembly
Test_udivby10:
    PUSH {LR}

    MOVS R0,#43
    BL   udivby10
// put a breakpoint here
// R0 should equal 12 (0x0C)
// R1 should equal 3

    //LDR R0,=12345
    LDR R0, =65535
    BL   udivby10
// put a breakpoint here
// R0 should equal 1234 (0x4D2)
// R1 should equal 5

    MOVS R0,#0
    BL   udivby10
// put a breakpoint here
// R0 should equal 0
// R1 should equal 0
    POP {PC}

//****************************************************
// divisor=10
// Inputs: R0 is 16-bit dividend
// quotient*10 + remainder = dividend
// Output: R0 is 16-bit quotient=dividend/10
//         R1 is 16-bit remainder=dividend%10 (modulus)
// not AAPCS compliant because it returns two values
udivby10:
    PUSH {R4, LR}         // Save return address
      MOVS R2, R0
      LDR R3, =52429
      MULS R0, R0, R3
      LSRS R0, R0, #19
      MOVS R3, R0
      MOVS R4, #10
      MULS R3, R3, R4
      SUBS R1, R2, R3
    POP {R4, PC}         // Return


  
//-----------------------OutDec-----------------------
// Convert a 16-bit number into unsigned decimal format
// Call the function OutChar to output each character
// You will call OutChar 1 to 5 times
// OutChar does not do actual output, OutChar does virtual output used by the grader
// Input: R0 (call by value) 16-bit unsigned number
// Output: none
// Invariables: This function must not permanently modify registers R4 to R11
//-----------------------OutDec-----------------------
// Convert a 16-bit number into unsigned decimal format
// Calls OutChar to output each character
// Input: R0 (call by value) - 16-bit unsigned number
// Output: none
// Preserves registers R4 to R11

.equ i, 0 // BINDING
.equ digitFour, 4  
.equ digitThree, 8 
.equ digitTwo, 12
.equ digitOne, 16    
.equ digitZero, 20

OutDec:
    PUSH {R4-R7, LR}      // Preserve registers and return address

    MOVS R4, #0
    SUB SP, SP, #24

    // ALLOCATION
    MOV R7, SP
    STR R4, [R7, #i]
    STR R4, [R7, #digitFour]
    STR R4, [R7, #digitThree]
    STR R4, [R7, #digitTwo]
    STR R4, [R7, #digitOne]
    STR R4, [R7, #digitZero]

    // ACCESS
    loop:
    BL udivby10
    ADDS R4, R4, #4 
    STR R4, [R7, #i] // increment counter by four
    ADDS R1, R1, #48 // convert remainder to ascii
    STR R1, [R7, R4] // push ascii form remainder to stack
    CMP R0, #0
    BEQ printStack
    B loop


    printStack:
    LDR R0, [R7, #digitZero]
    CMP R0, #0
    BEQ one 
    BL OutChar

    one:
    LDR R0, [R7, #digitOne]
    CMP R0, #0
    BEQ two
    BL OutChar

    two:
    LDR R0, [R7, #digitTwo]
    CMP R0, #0
    BEQ three 
    BL OutChar

    three:
    LDR R0, [R7, #digitThree]
    CMP R0, #0
    BEQ four
    BL OutChar

    four:
    LDR R0, [R7, #digitFour]
    CMP R0, #0
    BEQ done 
    BL OutChar

    // DEALLOCATION
    done:
    ADD SP,SP, #24
    POP {R4-R7, PC}       // Restore registers and return



//* * * * * * * * End of OutDec * * * * * * * *

// ECE319H recursive version
// Call the function OutChar to output each character
// You will call OutChar 1 to 5 times
// Input: R0 (call by value) 16-bit unsigned number
// Output: none
// Invariables: This function must not permanently modify registers R4 to R11

OutDec2:
   PUSH {LR}
// write this

   POP  {PC}



     .end

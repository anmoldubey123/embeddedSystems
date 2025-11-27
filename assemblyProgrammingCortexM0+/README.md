# Assembly Programming on Cortex-M0+ 

## Overview

This lab introduces **ARM Cortex-M0+ assembly language programming** on the TI MSPM0G3507 microcontroller. The assignment involves searching through a linked data structure (student records) to find and return information based on the student's own EID.

## Author

- Anmol Dubey 

## Objective

Implement an assembly function `Lab1` that searches a list of student records. Depending on the randomly assigned option, the function returns either:

- **Option 1**: The score associated with your EID, or -1 if not found
- **Option 2**: The index position of your EID in the list, or -1 if not found

Additional options (3-6) involve finding shortest/longest EIDs or counting students with similar EID patterns.

## Data Structure

The student list is an array of records, each containing:

```
struct student {
    char *eid;      // Pointer to EID string (offset +0)
    int32_t grade;  // Score value (offset +4)
};
```

The list terminates with a null pointer (`eid = 0`).

## Algorithm

The assembly solution implements string matching:

1. Load the base address of the student's own EID
2. Iterate through each record in the list
3. Compare strings character-by-character
4. On match: return the associated score (at offset +4)
5. On mismatch: advance to next record (offset +8)
6. If list exhausted: return -1

## Key Assembly Concepts

| Concept | Implementation |
|---------|----------------|
| Register preservation | `PUSH {R4-R7,LR}` / `POP {R4-R7,PC}` |
| Memory access | `LDR` (word), `LDRB` (byte) |
| String traversal | Increment pointer, check for null terminator |
| Conditional branching | `CMP` + `BEQ`/`BNE` |
| AAPCS compliance | Callee-saved registers R4-R7 preserved |

## Register Usage

| Register | Purpose |
|----------|---------|
| R0 | Input: list pointer / Output: result |
| R1 | Base address of student's EID |
| R2 | Current record's EID pointer |
| R3 | Working pointer for string comparison |
| R4 | Character from list EID |
| R5 | Character from student's EID |
| R6 | Temporary for null check |
| R7 | Saved base of student's EID for reset |

## Testing

The `Phase` variable controls execution:

| Phase | Behavior |
|-------|----------|
| 0 | Display test cases and expected outputs |
| 1-5 | Run individual known test cases |
| 6-7 | Run hidden test cases |
| 10 | Run full grader (all 7 cases) |

## Building and Running

1. Set your EID in the `.string "AD56328"` directive
2. Set `Phase` to desired test mode
3. Build with TI-CLANG toolchain in Code Composer Studio
4. Connect via UART (115200 baud) to view results
5. Ensure jumpers J25 and J26 are installed for UART communication

## Hardware

- TI MSPM0G3507 LaunchPad
- UART connection via XDS110 debugger (PA10/PA11)

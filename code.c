#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#define MEMORY_MAX (1 << 16) 
uint16_t memory[MEMORY_MAX];  /* 65536 memory locations, each 16bits, total of 128KB */

/* 10 total regs, 8 GPR, PC, Cond flag */
enum {
    R_RO = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    R_COND,
    R_COUNT
};
uint16_t reg[R_COUNT];

/* 16 opcode*/
enum{
    OP_BR = 0,  /* branch */
    OP_ADD,     /* add */
    OP_LD,      /* load */
    OP_ST,      /* store */
    OP_JSR,     /* jump reg */
    OP_AND,     /* bitw and */
    OP_LDR,     /* load reg */
    OP_STR,     /* save reg */
    OP_RTI,     /* unused */
    OP_NOT,     /* bitw not */
    OP_LDI,     /* load indirect */
    OP_STI,     /* store indirect */
    OP_JMP,     /* jump */
    OP_RES,     /* reserved (unused)*/
    OP_LEA,     /* load effective address */
    OP_TRAP     /* execute trap */
};

enum {
    FL_POS = 1 << 0, 
    FL_ZRO = 1 << 1,
    FL_NEG = 1 << 2
};


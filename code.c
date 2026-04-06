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

/* 001 == pos, 010 == zero, 100 == neg */
enum {
    FL_POS = 1 << 0, 
    FL_ZRO = 1 << 1,
    FL_NEG = 1 << 2
};

int main (int argc, const char* argv[]) {
    if (argc < 2){
        printf("lc3 [image-file1]: \n");
        exit(2);
    }

    for (int j = 1; j < argc; ++j){
        if (!read_image(argv[j])){
            printf("failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }

    reg[R_COND] = FL_ZRO;  /* initial cond flag is 0 */
    enum {
        PC_START = 0x3000 
    };
    reg[R_PC] = PC_START;

    int running = 1;
     while (running) {
        /* Fetch */
        uint16_t instr = mem_read(reg[R_PC]++);
        uint16_t op = instr >> 12;

        /* Decode then Execute */
        switch(op){
            case OP_ADD:
                break;
            case OP_AND:
                break;
            case OP_NOT:
                break;
            case OP_BR:
                break;
            case OP_JMP:
                break;
            case OP_JSR:
                break;
            case OP_LD:
                break;
            case OP_LDI:
                break;
            case OP_LDR:
                break;
            case OP_LEA:
                break;
            case OP_ST:
                break;
            case OP_STI:
                break;
            case OP_STR:
                break;
            case OP_TRAP:
                break;
            case OP_RES:
            case OP_RTI:
            default:
                break;
        }
     }
}
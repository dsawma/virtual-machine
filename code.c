#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#define MEMORY_MAX (1 << 16) 
uint16_t memory[MEMORY_MAX];  /* 65536 memory locations, each 16bits, total of 128KB */
uint16_t sign_extend(uint16_t x, int bit_count);
void update_flags(uint16_t r);
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
        uint16_t op = instr >> 12;  /* opcode is bit [12-15]*/

        /* Decode then Execute */
        switch(op){
            case OP_ADD: 
                uint16_t r0 = (instr >> 9) & 0x7;  /* bit [9-11]. & 0111*/
                uint16_t r1 = (instr >> 6) & 0x7;   /* bit [6-8] */
                uint16_t imm_flag = (instr >> 5) & 0x1; /* bit [5] = 0 then reg-reg else reg-imm */

                if (imm_flag) {
                    uint16_t imm5 = sign_extend(instr & 0x1F, 5); /* instr * 0001 1111*/
                    reg[r0] = reg[r1] + imm5;
                } else{
                    uint16_t r2 = instr & 0x7; 
                    reg[r0] = reg[r1] + reg[r2];
                }
                update_flags(r0);
                break;
            case OP_AND:
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7; 
                uint16_t imm_flag = (instr >> 5) & 0x1; 

                if (imm_flag) {
                    uint16_t imm5 = sign_extend(instr & 0x1F, 5); 
                    reg[r0] = reg[r1] & imm5; 
                } else{
                    uint16_t r2 = instr & 0x7; 
                    reg[r0] = reg[r1] & reg[r2];
                }
                update_flags(r0);
                break;
            case OP_NOT:
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7; 
                
                reg[r0] = !reg[r1];
                update_flags(r0);
                break;
            case OP_BR:
                uint16_t offset = sign_extend(instr & 0x1FF, 9);
                uint16_t cond_flag = (instr >> 0) & 0x7; 
                if (cond_flag & reg[R_COND]){
                    reg[R_PC] +=  offset;
                } 
                break;
            case OP_JMP:
                uint16_t r1 = (instr >> 6) & 0x7;
                reg[R_PC] = reg[r1]; 
                break;
            case OP_JSR:
                /* opcode: flag: SR: imm9 */
                /* bit [11] = flag, bit [6-8] = SR if JSSR, bit [0-11]*/
                uint16_t flag = (instr >> 11) & 1; 
                reg[R_R7] = reg[R_PC];
                if (flag) {
                    uint16_t pc_offset = sign_extend(instr & 0x7FF, 11);
                    reg[R_PC] += (1 + pc_offset);
                } else{
                    uint16_t r1 = (instr >> 6) & 0x7; /* JSSR case */
                    reg[R_PC] = reg[r1];
                }
                break;
            case OP_LD:
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                reg[r0] = mem_read(reg[R_PC] + pc_offset);
                update_flags(r0);
                break;
            case OP_LDI:
                /* DR bit [9-11], PCoffset9 bit [0-8] */
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9); /* instr & 0001 1111 1111 */
                reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));
                update_flags(r0);
                break;
            case OP_LDR:
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7; 
                uint16_t pc_offset = sign_extend(instr & 0x3F, 6);
                reg[r0] = mem_read(reg[r1] + pc_offset);
                update_flags(r0);
                break;
            case OP_LEA:
                uint16_t r0 = (instr >> 9) & 0x7; 
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                reg[r0] = reg[R_PC] +  pc_offset;
                update_flags(r0);
                break;
            case OP_ST:
                uint16_t r1 = (instr >> 9) & 0x7; 
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9); 
                mem_write(reg[R_PC] + pc_offset, reg[r1]);
                break;
            case OP_STI:
                uint16_t r1 = (instr >> 9) & 0x7; 
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9); 
                mem_write(mem_read(reg[R_PC] + pc_offset),reg[r1]);
                break;
            case OP_STR:
                uint16_t r1 = (instr >> 9) & 0x7;
                uint16_t base = (instr >> 6) & 0x7;
                uint16_t pc_offset = sign_extend(instr & 0x3F, 6); 
                mem_write(reg[base] + pc_offset,reg[r1]);
                break;
            case OP_TRAP:
                break;
            case OP_RES:
            case OP_RTI:
            default:
                abort(); /* unimplemented */
                break;
        }
     }
}

uint16_t sign_extend(uint16_t x, int bit_count){
    if ((x >> (bit_count - 1)) & 1){
        x |= (0xFFFF << bit_count);  /* x | 1111 1111 1110 0000*/
    }
    return x; 
}

void update_flags(uint16_t r){
    if (reg[r] == 0 ) {
        reg[R_COND] = FL_ZRO;
    } else if (reg[r] >> 15) {
        reg[R_COND] = FL_NEG;
    } else{
        reg[R_COND] = FL_POS;
    }
}
#include <stdint.h>
#define MEMORY_MAX (1 << 16)

void disable_input_buffering();
void restore_input_buffering();
uint16_t check_key();
void mem_write(uint16_t address, uint16_t val);
uint16_t mem_read(uint16_t address);
uint16_t sign_extend(uint16_t x, int bit_count);
void update_flags(uint16_t r);
void handle_interrupt(int signal);
uint16_t swap16(uint16_t x);
int read_image(const char *image_path);
void read_image_file(FILE *file);
uint16_t memory[MEMORY_MAX]; /* 65536 memory locations, each 16bits, total of 128KB */

enum
{
    MR_KBSR = 0xFE00, /* keyboard status*/
    MR_KBDR = 0xFE02  /* keyboard data */
};

/* 10 total regs, 8 GPR, PC, Cond flag */
enum
{
    R_R0 = 0,
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
enum
{
    OP_BR = 0, /* branch */
    OP_ADD,    /* add */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump reg */
    OP_AND,    /* bitw and */
    OP_LDR,    /* load reg */
    OP_STR,    /* save reg */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitw not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused)*/
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};

/* 001 == pos, 010 == zero, 100 == neg */
enum
{
    FL_POS = 1 << 0,
    FL_ZRO = 1 << 1,
    FL_NEG = 1 << 2
};



enum
{
    TRAP_GETC = 0x20,  /* get ch from keyboard, not echoed onto terminal */
    TRAP_OUT = 0x21,   /* output a ch */
    TRAP_PUTS = 0x22,  /* output a word string */
    TRAP_IN = 0x23,    /* get ch from keyboard, echoed to terminal */
    TRAP_PUTSP = 0x24, /* output a byte string */
    TRAP_HALT = 0x25   /* halt */
};
struct termios original_tio;
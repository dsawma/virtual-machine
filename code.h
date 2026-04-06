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
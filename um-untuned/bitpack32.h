#include <bitpack.h>

//type casts for bitpack functions
uint32_t get32u(uint32_t word, unsigned width, unsigned lsb);
uint32_t new32u(uint32_t word, unsigned width, unsigned lsb, uint32_t value);

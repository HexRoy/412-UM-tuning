#include "bitpack32.h"

//type casts for bitpack functions
uint32_t get32u(uint32_t word, unsigned width, unsigned lsb)
{
	return (uint32_t)Bitpack_getu((uint64_t)word, width, lsb);
}
uint32_t new32u(uint32_t word, unsigned width, unsigned lsb, uint32_t value)
{
	return (uint32_t)Bitpack_newu((uint64_t)word, width, lsb, (uint64_t)value);
}

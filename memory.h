#include <table.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//initialize memory
void initMem();

//load a program file by path
void loadProgramFile(char *programPath);

//get register value
uint32_t getReg(uint32_t index);

//set register value
void setReg(uint32_t index, uint32_t value);

//map segment
void mapSeg(uint32_t segNum, uint32_t wordNum);

//unmap segment
void unmapSeg(uint32_t segNum);

//map copy of segment to segNum 0, unmapping old 0
void dupeSeg(uint32_t segNum);

//get segment value
uint32_t getSeg(uint32_t segNum, uint32_t addr);

//set segment value
void setSeg(uint32_t segNum, uint32_t addr, uint32_t val);

void dumpMem();

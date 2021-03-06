#include "instruction.h"

uint32_t progCounter = 0;
const uint32_t MAX32 = 4294967295;

//if r[C] != 0 then r[A] = r[B]
void CMove(uint32_t regA, uint32_t regB, uint32_t regC)
{
	if(getReg(regC) != 0)
		setReg(regA, getReg(regB));
}

//r[A] = m[r[B]][r[C]]
void SLoad(uint32_t regA, uint32_t regB, uint32_t regC)
{
	setReg(regA, getSeg(getReg(regB), getReg(regC)));
}

//m[r[A]][r[B]] = r[C]
void SStore(uint32_t regA, uint32_t regB, uint32_t regC)
{
	setSeg(getReg(regA), getReg(regB), getReg(regC));
}

//r[A] = (r[B] + r[C]) % 2^32
void Add(uint32_t regA, uint32_t regB, uint32_t regC)
{
	setReg(regA, getReg(regB) + getReg(regC));
}

//r[A] = (r[B] * r[C]) % 2^32
void Mult(uint32_t regA, uint32_t regB, uint32_t regC)
{
	setReg(regA, getReg(regB) * getReg(regC));
}

//r[A] = r[B] / r[C]
void Div(uint32_t regA, uint32_t regB, uint32_t regC)
{
	uint32_t regCval = getReg(regC);
	if(regCval == 0)
	{
		fprintf(stderr, "Attempting to divide by zero with register %u.\n", (unsigned)regC);
		exit(1);
	}
	setReg(regA, getReg(regB) / regCval);
}

//r[A] = ~(r[B] & r[C])
void Nand(uint32_t regA, uint32_t regB, uint32_t regC)
{
	setReg(regA, ~(getReg(regB) & getReg(regC)));
}

//map m[r[B]] with r[C] words, changing r[B] to the identifier
void Map(uint32_t regB, uint32_t regC)
{
	mapSeg(regB, getReg(regC));
}

//unmap m[r[C]]
void Unmap(uint32_t regC)
{
	unmapSeg(getReg(regC));
}

//output r[C]
void Out(uint32_t regC)
{
	putchar((char)getReg(regC));
}

//r[C] = input
void In(uint32_t regC)
{
	int temp = getchar();
	if(temp == EOF)
		temp = MAX32;
	setReg(regC, (uint32_t)temp);
}

//m[0] = m[r[b]] and set program counter to m[0][r[c]]
void LoadP(uint32_t regB, uint32_t regC)
{
	uint32_t regBval = getReg(regB);
	if(regBval != 0)
		dupeSeg(regBval);
	progCounter = getReg(regC);
}

//r[A] = val
void LoadV(uint32_t regA, uint32_t val)
{
	setReg(regA, val);
}

//executes next instruction, returns -1 if program terminated
int execute()
{
	uint32_t instruction = getSeg(0, progCounter);
	//fprintf(stderr, "%u\n", (unsigned)instruction);
	
        
        uint32_t opcode = (uint32_t)Bitpack_getu((uint64_t)instruction, 4, 28);
        
	//fprintf(stderr, "%u\n", (unsigned)opcode);
	if(opcode != 13)
	{
		uint32_t regA = (uint32_t)Bitpack_getu((uint64_t)instruction, 3, 6);
		uint32_t regB = (uint32_t)Bitpack_getu((uint64_t)instruction, 3, 3);
		uint32_t regC = (uint32_t)Bitpack_getu((uint64_t)instruction, 3, 0);
		//fprintf(stderr, "%u %u %u\n", (unsigned)regA, (unsigned)regB, (unsigned)regC);
		switch(opcode)
		{
			case 0:
				CMove(regA, regB, regC);
				break;
			case 1:
				SLoad(regA, regB, regC);
				break;
			case 2:
				SStore(regA, regB, regC);
				break;
			case 3:
				Add(regA, regB, regC);
				break;
			case 4:
				Mult(regA, regB, regC);
				break;
			case 5:
				Div(regA, regB, regC);
				break;
			case 6:
				Nand(regA, regB, regC);
				break;
			case 7:
				return -1;
				break;
			case 8:
				Map(regB, regC);
				break;
			case 9:
				Unmap(regC);
				break;
			case 10:
				Out(regC);
				break;
			case 11:
				In(regC);
				break;
			case 12:
				LoadP(regB, regC);
				break;
			default:
				fprintf(stderr, "Invalid opcode %u, exiting.\n", (unsigned)opcode);
				return -1;
		}
	}
	else
	{
		uint32_t regA = (uint32_t)Bitpack_getu((uint64_t)instruction, 3, 25);
		uint32_t val = (uint32_t)Bitpack_getu((uint64_t)instruction, 25, 0);
		LoadV(regA, val);
		//fprintf(stderr, "%u %u\n", (unsigned)regA, (unsigned)val);
	}
	if(opcode != 12)
		progCounter++;
	return 0;
}

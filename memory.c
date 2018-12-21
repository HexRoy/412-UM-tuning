#include "memory.h"

uint32_t *reg = NULL;
Table_T segs = NULL;
uint32_t segIndex = 0;

typedef struct Value{
	uint32_t size;
	uint32_t *array;
	uint32_t *key;
} Value;

//table interface functions
int cmp(const void *x, const void *y)
{
	return *((uint32_t *)x) - *((uint32_t *)y);
}

unsigned hash(const void *key)
{
	return (unsigned)*((uint32_t *)key);
}

//initialize memory
void initMem()
{
	reg = calloc(8, sizeof(uint32_t));
	segs = Table_new(10, cmp, hash);
}

//load a program file by path
void loadProgramFile(char *programPath)
{
	//open file
	FILE *prog = fopen(programPath, "r");
	if(prog == NULL)
	{
		fprintf(stderr, "Invalid file path for program\n");
		exit(1);
	}
	//check the file is valid
	fseek(prog, 0L, SEEK_END);
	long int size = ftell(prog);
	if (size % 4 != 0)
	{
		fprintf(stderr, "Invalid file path, program not only 32 bit words\n");
		exit(1);
	}
	rewind(prog);
	//map file to segment 0
	segIndex = 0;
	mapSeg(0, (uint32_t)size/4);
	if(getReg(0) != 0)
	{
		fprintf(stderr, "Error loading program into segment 0.\n");
		exit(1);
	}
	uint32_t word = 0;
	char *c = (char *)&word;
	uint32_t tempword = 0;
	char *tempc = (char *)&tempword;
	uint32_t counter = 0;
	while(fread(&word, sizeof(uint32_t), 1, prog) == 1)
	{
		//reverse order of word
		for(unsigned i = 0; i < 4; i++)
		{
			tempc[i] = c[3 - i];
		}
		setSeg(0, counter++, tempword);
	}
	fclose(prog);
}

//get register value
uint32_t getReg(uint32_t index)
{
	if(index > 7)
	{
		fprintf(stderr, "Invalid register get: %u.\n", (unsigned)index);
		exit(1);
	}
	return reg[index];
}

//set register value
void setReg(uint32_t index, uint32_t value)
{
	if(index > 7)
	{
		fprintf(stderr, "Invalid register set: %u.\n", (unsigned)index);
		exit(1);
	}
	reg[index] = value;
}

//map segment
void mapSeg(uint32_t regIndex, uint32_t wordNum)
{
	//ensure next segment identifier available
	while(Table_get(segs, &segIndex) != NULL)
		segIndex++;
	//create segment
	Value *v = malloc(sizeof(Value));
	v->size = wordNum;
	v->array = calloc(wordNum, sizeof(uint32_t));
	v->key = malloc(sizeof(uint32_t));
	*(v->key) = segIndex;
	Table_put(segs, v->key, v);
	//update register with identifier
	setReg(regIndex, segIndex);
	segIndex++;
}

//unmap segment
void unmapSeg(uint32_t segNum)
{
	if(segNum == 0)
	{
		fprintf(stderr, "Attempting to unmap segment 0.\n");
		exit(1);
	}
	Value *v = Table_get(segs, &segNum);
	if(v == NULL)
	{
		fprintf(stderr, "Attempting to unmap unmapped segment %u.\n", (unsigned)segNum);
		exit(1);
	}
	free(v->array);
	uint32_t *key = v->key;
	free(v);
	Table_remove(segs, &segNum);
	free(key);
}

//map copy of segment to segNum 0, unmapping old 0
void dupeSeg(uint32_t segNum)
{
	Value *vnew = Table_get(segs, &segNum);
	if(vnew == NULL)
	{
		fprintf(stderr, "Attempting to dupe unmapped segment %u.\n", (unsigned)segNum);
		exit(1);
	}
	uint32_t zero = 0;
	Value *vold = Table_get(segs, &zero);
	free(vold->array);
	vold->size = vnew->size;
	size_t arraySize = vold->size * sizeof(uint32_t);
	vold->array = malloc(arraySize);
	memcpy(vold->array, vnew->array, arraySize);
}

//get segment value
uint32_t getSeg(uint32_t segNum, uint32_t addr)
{
	Value *v = Table_get(segs, &segNum);
	return v->array[addr];
}

//set segment value
void setSeg(uint32_t segNum, uint32_t addr, uint32_t val)
{
	Value *v = Table_get(segs, &segNum);
	if(v == NULL)
	{
		fprintf(stderr, "Attempting to set unmapped segment [%u].\n", (unsigned)segNum);
		exit(1);
	}
	if(addr >= v->size)
	{
		fprintf(stderr, "Invalid segment set: [%u][%u] when size only %u.\n", (unsigned)segNum, (unsigned)addr, (unsigned)v->size);
		exit(1);
	}
	v->array[addr] = val;
}

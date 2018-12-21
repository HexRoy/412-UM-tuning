#include <stdio.h>

#include "instruction.h"
#include "memory.h"

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stderr, "UM requires only program as an argument\n");
		return 1;
	}
	initMem();
	loadProgramFile(argv[1]);
	while(execute() != -1)
		continue;
	return 0;
}

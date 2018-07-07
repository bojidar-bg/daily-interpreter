#include <stdio.h>
#include "interpreter.h"
#include "bytecode.h"

int main(int argc, char **argv) {
	printf("Running!\n");

	DailyOp opcodes[] = {
		{DailyOpAlloc, 8, 0, 0},
		{DailyOpSet, 0, 0, 0},
		{DailyOpSet, 1, 1, 0},
		{DailyOpInt16Add, 0, 0, 0},
		{DailyOpInt16Add, 0, 0, 0},
		{DailyOpInt16Add, 0, 0, 0},
		{DailyOpSet, 4, 0, 0},
		{DailyOpSet, 5, 1, 0},
		{DailyOpInt16Add, 4, 4, 4},
		{DailyOpInt16Add, 4, 0, 4},
		{DailyOpCopy, 6, 4, 2},
		{DailyOpDebug, 0, 0, 0},
		// {DailyOpAssign, 0, 4},
		// {DailyOpAssign, 1, 5},
		{DailyOpNil},
	};
	daily_run_bytecode(opcodes);
}

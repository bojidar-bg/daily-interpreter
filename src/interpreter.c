#define EXPORT_OPS
#include "interpreter.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>


int daily_run_bytecode(DailyOp* operations) {

	#if defined(_MSC_VER) || defined(DAILY_USE_SWITCH)

	#define OP_SWITCH while (true) switch (operation->operation)
	#define OP_CASE(op) case op:
	#define OP_NEXT operation++; break

	#else

	static void *opcodes[] = {
		#define X_OP(op) &&op,
		DAILY_OPS
		#undef X_OP
	};

	#define OP_SWITCH operation--; OP_NEXT; if (false)
	#define OP_CASE(op) op:
	#define OP_NEXT goto *opcodes[(++operation)->operation]

	#endif

	#define CHECK_POINTER(pointer, size) if (pointer < stack || pointer - size > stack_pointer) return 2
	#define CHECK_PARAMETER(parameter, size) CHECK_POINTER(stack_pointer - parameter, size)
	#define GET_PARAMETER(parameter, size) stack_pointer[1 - (size) - (parameter)]
	#define GET_PARAMETER_AS(parameter, type) *(type*)&GET_PARAMETER(parameter, sizeof(type))

	DailyOp* operation = &operations[0];
	unsigned char* stack = 0;
	unsigned char* stack_pointer = 0;
	long stack_size_available = 0;


	OP_SWITCH {
		OP_CASE(DailyOpNil) {
			free(stack);
			stack_size_available = 0;
			stack = NULL;
			stack_pointer = NULL;
			return 0;
		}
		OP_CASE(DailyOpError) {
			printf("ERROR %d\n", operation->param1);
			free(stack);
			stack_size_available = 0;
			stack = NULL;
			stack_pointer = NULL;
			return 1;
		}

		OP_CASE(DailyOpAlloc) {
			if (stack_size_available < operation->param1) {
				int pointer_offset = stack_pointer - stack;
				stack_size_available = (operation->param1 + stack_size_available) * 2;
				stack = realloc(stack, pointer_offset + stack_size_available + 1);
				stack_pointer = stack + pointer_offset;
			}
			memset(stack_pointer + 1, operation->param2, operation->param1);
			stack_size_available -= operation->param1;
			stack_pointer += operation->param1;
			OP_NEXT;
		}
		OP_CASE(DailyOpDealloc) {
			stack_pointer -= operation->param1;
			stack_size_available += operation->param1;
			CHECK_POINTER(stack_pointer, 0);
			OP_NEXT;
		}

		OP_CASE(DailyOpSet) {
			CHECK_PARAMETER(operation->param1, 1);
			GET_PARAMETER(operation->param1, 1) = operation->param2;
			OP_NEXT;
		}
		OP_CASE(DailyOpCopy) {
			CHECK_PARAMETER(operation->param1, operation->param3);
			CHECK_PARAMETER(operation->param2, operation->param3);
			memcpy(&GET_PARAMETER(operation->param1, operation->param3), &GET_PARAMETER(operation->param2, operation->param3), operation->param3);
			OP_NEXT;
		}

		OP_CASE(DailyOpDebug) {
			printf("---- DEBUG -----\n");
			printf("Stack: %ld used, %ld allocated\n", stack_pointer - stack, (stack_pointer - stack) + stack_size_available);
			printf("Stack values:\n");
			for (long i = 0; i < (stack_pointer - stack); i++) {
				printf("\t%5ld: %02x %3d\n", i, stack_pointer[-i], stack_pointer[-i]);
			}
			OP_NEXT;
		}

		#define MATH_OP(op, operator, type) \
			OP_CASE(op) { \
				CHECK_PARAMETER(operation->param1, sizeof(type)); \
				CHECK_PARAMETER(operation->param2, sizeof(type)); \
				CHECK_PARAMETER(operation->param3, sizeof(type)); \
				GET_PARAMETER_AS(operation->param1, type) = \
					GET_PARAMETER_AS(operation->param2, type) \
					operator \
					GET_PARAMETER_AS(operation->param3, type); \
				OP_NEXT; \
			}
		#define MATH_OPS(name, type) \
			MATH_OP(DailyOp##name##Add, +, type) \
			MATH_OP(DailyOp##name##Subtract, -, type) \
			MATH_OP(DailyOp##name##Multiply, *, type) \
			MATH_OP(DailyOp##name##Divide, /, type) \
			MATH_OP(DailyOp##name##Modulo, %, type)

		MATH_OPS(Int16, int16_t)
		MATH_OPS(Int32, int32_t)

		#undef MATH_OPS
		#undef MATH_OP
	}

	#undef CHECK_POINTER
	#undef CHECK_PARAMETER
}
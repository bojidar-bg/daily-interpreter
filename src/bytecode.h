#ifndef _DAILY_BYTECODE
#define _DAILY_BYTECODE


#define _DAILY_BYTECODE_MATH_OPS(type) \
	X_OP(DailyOp##type##Add) \
	X_OP(DailyOp##type##Subtract) \
	X_OP(DailyOp##type##Multiply) \
	X_OP(DailyOp##type##Divide) \
	X_OP(DailyOp##type##Modulo)

#define DAILY_OPS \
	X_OP(DailyOpNil) \
	X_OP(DailyOpAlloc) \
	X_OP(DailyOpDealloc) \
	X_OP(DailyOpSet) \
	X_OP(DailyOpCopy) \
	_DAILY_BYTECODE_MATH_OPS(Int16) \
	_DAILY_BYTECODE_MATH_OPS(Int32) \
	/*_DAILY_BYTECODE_MATH_OPS(Float16) \
	_DAILY_BYTECODE_MATH_OPS(Float32) */ \
	X_OP(DailyOpDebug) \
	X_OP(DailyOpError) \

typedef struct {
	enum {
		#define X_OP(op) op,
		DAILY_OPS
		#undef X_OP
	} operation : 8;

	unsigned int param1 : 8;
	unsigned int param2 : 8;
	unsigned int param3 : 8;
} DailyOp;

#ifndef EXPORT_OPS
#undef DAILY_OPS
#undef _DAILY_BYTECODE_MATH_OPS
#endif

#endif

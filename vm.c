#include "vm.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "object.h"
#include "table.h"

VM vm;

static void resetStack() { vm.stackTop = vm.stack; }

static Value peek(int distance) { return vm.stackTop[-1 - distance]; }

static bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
  ObjString* stringB = AS_STRING(pop());
  ObjString* stringA = AS_STRING(pop());

  // copy A then B into a newly allocated piece of memory
  int totalLength = stringA->length + stringB->length;
  char* chars = ALLOCATE(char, totalLength + 1);
  memcpy(chars, stringA->chars, stringA->length);
  memcpy(chars + stringA->length, stringB->chars, stringB->length);
  chars[totalLength] = '\0';

  ObjString* newStringObj = takeString(chars, totalLength);
  push(OBJ_VAL(newStringObj));
}

static void runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  // note: VM consumes the token before it throws a
  // runtime error, hence the "-1" to get the prev inst
  size_t instruction = vm.ip - vm.chunk->code - 1;
  int line = vm.chunk->lines[instruction];
  fprintf(stderr, "[line %d] in script\n", line);

  resetStack();
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(valueType, op)                      \
  do {                                                \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
      runtimeError("Operands must be numbers.");      \
      return INTERPRET_RUNTIME_ERROR;                 \
    }                                                 \
    double right = AS_NUMBER(pop());                  \
    double left = AS_NUMBER(pop());                   \
    push(valueType(left op right));                   \
  } while (false)

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
      printf("[");
      printValue(*slot);
      printf("]");
    }
    printf("\n");

    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

    uint8_t instruction = READ_BYTE();
    switch (instruction) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(constant);
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_NIL:
        push(NIL_VAL());
        break;
      case OP_TRUE:
        push(BOOL_VAL(true));
        break;
      case OP_FALSE:
        push(BOOL_VAL(false));
        break;
      case OP_NOT:
        push(BOOL_VAL(isFalsey(pop())));
        break;
      case OP_EQUAL: {
        Value right = pop();
        Value left = pop();
        push(BOOL_VAL(areValuesEqual(left, right)));
        break;
      }
      case OP_GREATER:
        BINARY_OP(BOOL_VAL, >);
        break;
      case OP_LESS:
        BINARY_OP(BOOL_VAL, <);
        break;
      case OP_NEGATE:
        if (!IS_NUMBER(peek(0))) {
          runtimeError("Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
        }

        double numberValue = AS_NUMBER(pop());
        push(NUMBER_VAL(-numberValue));
        break;

      case OP_ADD:
        if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
          concatenate();
        } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
          double right = AS_NUMBER(pop());
          double left = AS_NUMBER(pop());
          push(NUMBER_VAL(left + right));
        } else {
          runtimeError("Operands must be 2 numbers or 2 strings.");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;

      case OP_SUBTRACT:
        BINARY_OP(NUMBER_VAL, -);
        break;

      case OP_MULTIPLY:
        BINARY_OP(NUMBER_VAL, *);
        break;

      case OP_DIVIDE:
        BINARY_OP(NUMBER_VAL, /);
        break;

      case OP_PRINT:
        printValue(pop());
        printf("\n");
        break;

      case OP_RETURN: {
        return INTERPRET_OK;
      }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

void initVM() {
  resetStack();
  vm.objects = NULL;
  initTable(&vm.strings);
}

void freeVM() {
  freeObjects();
  freeTable(&vm.strings);
}

InterpretResult interpret(const char* source) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  InterpretResult result = run();

  freeChunk(&chunk);
  return result;
}

void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
}

Value pop() {
  vm.stackTop--;
  return *vm.stackTop;
}

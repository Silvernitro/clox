#include "vm.h"

#include <stdio.h>

#include "chunk.h"

VM vm;

void initVM() {}

void freeVM() {}

InterpretResult interpret(Chunk *chunk) {
  vm.chunk = chunk;
  vm.ip = chunk->code;
  return run();
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

  for (;;) {
    uint8_t instruction = READ_BYTE();

    switch (instruction) {
      case OP_CONSTANT:
        Value constant = READ_CONSTANT();
        printValue(constant);
        printf("\n");
        break;

      case OP_RETURN:
        return INTERPRET_OK;
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
}

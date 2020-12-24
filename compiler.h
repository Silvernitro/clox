#ifndef clox_compiler_h
#define clox_compiler_h

#include "chunk.h"
#include "stdlib.h"

bool compile(const char* source, Chunk* chunk);

#endif

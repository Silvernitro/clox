#include "object.h"

#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
  ((type*)allocateObject(sizeof(type), objectType))

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;
  return object;
}

ObjString* allocateString(char* string, int length) {
  ObjString* stringObj = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  stringObj->chars = string;
  stringObj->length = length;

  return stringObj;
}

ObjString* copyString(const char* string, int length) {
  char* heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, string, length);
  heapChars[length] = '\0';

  return allocateString(heapChars, length);
}

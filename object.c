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
  object->next = vm.objects;
  vm.objects = object;
  return object;
}

static ObjString* allocateString(char* string, int length, uint32_t hash) {
  ObjString* stringObj = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  stringObj->chars = string;
  stringObj->length = length;
  stringObj->hash = hash;

  return stringObj;
}

static uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;

  for (int i = 0; i < length; i++) {
    hash ^= key[i];
    hash *= 16777619;
  }

  return hash;
}

// public interface for allocateString
ObjString* takeString(char* string, int length) {
  uint32_t hash = hashString(string, length);
  return allocateString(string, length, hash);
}

ObjString* copyString(const char* string, int length) {
  char* heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, string, length);
  heapChars[length] = '\0';
  uint32_t hash = hashString(string, length);

  return allocateString(heapChars, length, hash);
}

void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
  }
}

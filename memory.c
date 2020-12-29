#include "memory.h"

#include <stdlib.h>

#include "object.h"
#include "vm.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  void* result = realloc(pointer, newSize);
  if (result == NULL) {
    exit(1);
  }

  return result;
}

static void freeObject(Obj* object) {
  switch (object->type) {
    case OBJ_STRING: {
      ObjString* stringObj = (ObjString*)object;
      // account for '\0'
      FREE_ARRAY(char, stringObj->chars, stringObj->length + 1);
      FREE(ObjString, stringObj);
      break;
    }
  }
}

void freeObjects() {
  Obj* head = vm.objects;
  // traverse singly-linked list
  while (head != NULL) {
    Obj* next = head->next;
    freeObject(head);
    head = next;
  }
}

#include "table.h"

#include "memory.h"
#include "object.h"
#include "value.h"

void initTable(Table* table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

void freeTable(Table* table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}

static Entry* findEntry(Entry* entries, int capacity, ObjString* key) {
  uint32_t index = key->hash % capacity;

  for (;;) {
    Entry* entry = &entries[index];

    if (entry->key == NULL || entry->key == key) {
      return entry;
    }

    index = (index + 1) % capacity;
  }
}

static void adjustCapacity(Table* table, int capacity) {
  Entry* entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NIL_VAL();
  }

  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (entry->key == NULL) continue;

    Entry* newEntry = findEntry(entries, capacity, entry->key);
    newEntry->key = entry->key;
    newEntry->value = entry->value;
  }

  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->capacity = capacity;
  table->entries = entries;
}

bool tableSet(Table* table, ObjString* key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int newCapacity = GROW_CAPACITY(table->capacity);
  }

  Entry* entry = findEntry(table->entries, table->capacity, key);

  bool isNewKey = entry->key == NULL;
  if (isNewKey) {
    table->count++;
  }

  entry->key = key;
  entry->value = value;
  return isNewKey;
}

// stores the found value in a given value pointer
bool tableGet(Table* table, ObjString* key, Value* value) {
  if (table->count == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  bool isEntryFound = entry->key != NULL;

  if (isEntryFound) {
    *value = entry->value;
  }

  return isEntryFound;
}

void tableAddAll(Table* fromTable, Table* toTable) {
  for (int i = 0; i < fromTable->capacity; i++) {
    Entry* entry = &fromTable->entries[i];
    if (entry->key != NULL) {
      tableSet(toTable, entry->key, entry->value);
    }
  }
}

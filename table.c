#include "table.h"

#include <string.h>

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
  Entry* tombstone = NULL;

  for (;;) {
    Entry* entry = &entries[index];
    // Current tombstone definition: NULL key and non-nil value
    bool isTombstone = entry->key == NULL && !IS_NIL(entry->value);

    if (tombstone == NULL && isTombstone) {
      tombstone = entry;
    } else if (entry->key == NULL) {
      // reached an empty bucket
      return tombstone == NULL ? entry : tombstone;
    }

    if (entry->key == key) {
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

  // reset table count to exclude tombstones
  table->count = 0;

  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (entry->key == NULL) continue;

    Entry* newEntry = findEntry(entries, capacity, entry->key);
    newEntry->key = entry->key;
    newEntry->value = entry->value;
    table->count++;
  }

  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->capacity = capacity;
  table->entries = entries;
}

bool tableSet(Table* table, ObjString* key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int newCapacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, newCapacity);
  }

  Entry* entry = findEntry(table->entries, table->capacity, key);

  bool isNewKey = entry->key == NULL;
  // only increment count if not tombstone
  if (isNewKey && IS_NIL(entry->value)) {
    table->count++;
  }

  entry->key = key;
  entry->value = value;
  return isNewKey;
}

bool tableDelete(Table* table, ObjString* key) {
  if (table->count == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  entry->key = NULL;
  // set tombstone value
  entry->value = BOOL_VAL(true);
  return true;
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

ObjString* tableFindString(Table* table, const char* chars, int length,
                           uint32_t hash) {
  if (table->count == 0) return NULL;

  uint32_t index = hash % table->capacity;

  for (;;) {
    Entry* entry = &table->entries[index];

    if (entry->key == NULL && IS_NIL(entry->value)) {
      return NULL;
    } else if (entry->key->length == length && entry->key->hash == hash &&
               memcmp(entry->key->chars, chars, length) == 0) {
      return entry->key;
    }

    index = (index + 1) % table->capacity;
  }
}

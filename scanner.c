#include "scanner.h"

typedef struct {
  const char* current;
  const char* start;
  int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
  scanner.current = source;
  scanner.start = source;
  scanner.line = 1;
}

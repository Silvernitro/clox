#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "common.h"
#include "scanner.h"
#include "vm.h"

typedef struct {
  Token previous;
  Token current;
  bool hadError;
  bool panicMode;
} Parser;

Parser parser;

//---------- START ERROR UTILS ------------//
static void errorAt(Token* token, const char* message) {
  if (parser.panicMode) return;
  parser.panicMode = true;

  fprintf(stderr, "[Line %d] Error ", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, "at end");
  } else if (token->type == TOKEN_ERROR) {
    // pass
  } else {
    fprintf(stderr, "at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, " : %s\n", message);
  parser.hadError = true;
}

static void errorAtCurrent(const char* message) {
  errorAt(&parser.current, message);
}

static void error(const char* message) { errorAt(&parser.previous, message); }
//---------- END ERROR UTILS ------------//

//---------- START PARSING UTILS -----------//
static void advance() {
  parser.previous = parser.current;

  for (;;) {
    parser.current = scanToken();
    if (parser.current.type != TOKEN_ERROR) break;

    errorAtCurrent(parser.current.start);
  }
}

static void consume(TokenType type, const char* message) {
  if (parser.current.type == type) {
    advance();
    return;
  }

  errorAtCurrent(message);
}
//---------- END PARSING UTILS -----------//

bool compile(const char* source, Chunk* chunk) {
  initScanner(source);

  // reset all error flags
  parser.hadError = false;
  parser.panicMode = false;

  advance();
  expression();
  consume(TOKEN_EOF, "Expect end of expression.");

  // false when parse error occurs
  return !parser.hadError;
}

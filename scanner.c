#include "scanner.h"

#include "common.h"

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

//-------- START SCANNER NAVIGATOR UTILS -----------//
static bool isAtEnd() { return *scanner.current == "\0"; }

static char advance() { return *scanner.current++; }

static char peek() { return *scanner.current; }

static char peekNext() {
  if (isAtEnd()) return '\0';
  return scanner.current[1];
}

static bool match(char expected) {
  if (isAtEnd()) return false;
  if (*scanner.current != expected) return false;

  scanner.current++;
  return true;
}
//-------- END SCANNER NAVIGATOR UTILS -----------//

//-------- START TOKEN UTILS ----------//
static Token makeToken(TokenType type) {
  Token token;

  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.line;

  return token;
}

static Token errorToken(const char* msg) {
  Token token;

  token.type = TOKEN_ERROR;
  token.start = msg;
  token.length = (int)strlen(msg);
  token.line = scanner.line;

  return token;
}
//-------- END TOKEN UTILS ----------//

//-------- START SCANNING UTILS --------//
static void skipWhitespace() {
  for (;;) {
    char c = peek();

    switch (c) {
      case ' ':
      case '\r':
      case '\t':
        advance();
        break;
      case '\n':
        advance();
        scanner.line++;
        break;

      case '/':
        if (peekNext() == '/') {
          while (peek() != '\n' && !isAtEnd()) {
            advance();
          }
        } else {
          return;
        }
        break;

      default:
        return;
    }
  }
}

static Token string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == "\n") scanner.line++;
    advance();
  }

  if (isAtEnd()) return errorToken("Unterminated string.");

  // consume the closing quote
  advance();
  return makeToken(TOKEN_STRING);
}

static bool isDigit(char c) { return c >= '0' && c <= '9'; }

static Token number() {
  while (isDigit(peek())) {
    advance();
  }

  if (peek() == '.' && isDigit(peekNext())) {
    // consume decimal point '.'
    advance();

    while (isDigit(peek())) {
      advance();
    }
  }

  return makeToken(TOKEN_NUMBER);
}
//-------- END SCANNING UTILS --------//

Token scanToken() {
  // ensure that we are always at a meaningful token
  skipWhitespace();
  scanner.start = scanner.current;

  if (isAtEnd()) {
    return makeToken(TOKEN_EOF);
  }

  char c = advance();

  if (isDigit(c)) return number();

  switch (c) {
    case '(':
      return makeToken(TOKEN_LEFT_PAREN);
    case ')':
      return makeToken(TOKEN_RIGHT_PAREN);
    case '{':
      return makeToken(TOKEN_LEFT_BRACE);
    case '}':
      return makeToken(TOKEN_RIGHT_BRACE);
    case ',':
      return makeToken(TOKEN_COMMA);
    case '.':
      return makeToken(TOKEN_DOT);
    case '-':
      return makeToken(TOKEN_MINUS);
    case '+':
      return makeToken(TOKEN_PLUS);
    case ';':
      return makeToken(TOKEN_SEMICOLON);
    case '/':
      return makeToken(TOKEN_SLASH);
    case '*':
      return makeToken(TOKEN_STAR);
    case '!':
      return match('=') ? makeToken(TOKEN_BANG_EQUAL) : makeToken(TOKEN_BANG);
    case '=':
      return match('=') ? makeToken(TOKEN_EQUAL_EQUAL) : makeToken(TOKEN_EQUAL);
    case '>':
      return match('=') ? makeToken(TOKEN_GREATER_EQUAL)
                        : makeToken(TOKEN_GREATER);
    case '<':
      return match('=') ? makeToken(TOKEN_LESS_EQUAL) : makeToken(TOKEN_LESS);
    case '"':
      return string();
  }

  return errorToken("Unexpected character.");
}

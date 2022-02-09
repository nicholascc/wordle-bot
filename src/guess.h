#ifndef GUESS_H
#define GUESS_H value

#include "word_list.h"
#include <stdbool.h>

typedef enum {
  NOWHERE = 0,
  SOMEWHERE = 1,
  EXACT = 2
} Response;

typedef struct Char_Response {
  Response r;
  char c;
} Char_Response;

typedef struct Guess_Response {
  Char_Response chars[5];
} Guess_Response;

int guess_to_int(Guess_Response r);
Guess_Response response_from_string(Word guess, char *str);
Guess_Response make_guess(Word guess, Word answer);
bool matches(Word w, Guess_Response response);
void print_guess_response(Guess_Response r);
bool is_perfect_guess(Guess_Response guess);

#endif

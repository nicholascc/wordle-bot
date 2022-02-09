#include "guess.h"

#include <stdio.h>
#include <assert.h>

int guess_to_int(Guess_Response r) {
  int x = 0;
  for(int i = 0; i < 5; i++) {
    x *= 3;
    x += r.chars[i].r;
  }
  return x;
}

Guess_Response make_guess(Word guess, Word answer) {
  Guess_Response r;
  for(int i = 0; i < 5; i++) {
    char gc = guess.chars[i];
    r.chars[i].c = gc;
    if(gc == answer.chars[i]) {
      r.chars[i].r = EXACT;
    } else {
      bool found = false;
      for(int j = 0; j < 5; j++) {
        if(gc == answer.chars[j]) {
          found = true;
          break;
        }
      }
      if(found) r.chars[i].r = SOMEWHERE;
      else r.chars[i].r = NOWHERE;
    }
  }
  return r;
}

Guess_Response response_from_string(Word guess, char *str) {
  Guess_Response r;
  for(int i = 0; i < 5; i++) {
    r.chars[i].c = guess.chars[i];
    char c = str[i];
    if(c == '.') r.chars[i].r = NOWHERE;
    else if(c == '?') r.chars[i].r = SOMEWHERE;
    else if(c == '!') r.chars[i].r = EXACT;
    else assert(false);
  }
  return r;
}

bool is_perfect_guess(Guess_Response guess) {
  for(int i = 0; i < 5; i++) {
    if(guess.chars[i].r != EXACT) return false;
  }
  return true;
}

bool matches(Word w, Guess_Response response) {
  for(int i = 0; i < 5; i++) {
    Char_Response r = response.chars[i];
    if(r.r == NOWHERE) {
      for(int j = 0; j < 5; j++) {
        if(r.c == w.chars[j]) return false;
      }
    } else if(r.r == SOMEWHERE) {
      if(r.c == w.chars[i]) return false;
      bool found = false;
      for(int j = 0; j < 5; j++) {
        if(r.c == w.chars[j]) {
          found = true;
          break;
        }
      }
      if(!found) return false;
    } else {
      if(r.c != w.chars[i]) return false;
    }
  }
  return true;
}


void print_guess_response(Guess_Response r) {
  for(int i = 0; i < 5; i++) {
    printf("%s", "\x1b[3");
    if(r.chars[i].r == NOWHERE) printf("7");
    else if(r.chars[i].r == SOMEWHERE) printf("3");
    else printf("2");
    printf(";4m");
    printf("%c", r.chars[i].c);
    printf("%s", "\033[0m");
  }
}

#ifndef WORD_LIST_H
#define WORD_LIST_H value

#include <stdbool.h>

char *os_load_file(char *filename);

typedef struct Word {
  char chars[5];
} Word;

Word word_from_string(char *str);
void print_word(Word w);
bool words_equal(Word a, Word b);

typedef struct Word_List {
  int count;
  Word *words;
} Word_List;

Word_List copy_list(Word_List list);
Word_List list_from_string(char *str);
bool is_in_list(Word word, Word_List list);
void remove_from_list(Word_List *list, int index);


#endif

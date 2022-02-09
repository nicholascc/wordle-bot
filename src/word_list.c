#include "word_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

char *os_load_file(char *filename) {
  int size = 0;
	FILE *f = fopen(filename, "rb");
  if(f == NULL) {
    printf("Could not open file '%s'. Make sure you are running this in the right directory.\n", filename);
    exit(1);
  }
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *result = (char *)malloc(size+2);
	assert(result != NULL && "malloc failed");
	assert(size == fread(result, sizeof (unsigned char), size, f) && "failed to read file");
	fclose(f);
	result[size] = ' ';
	result[size+1] = 0;
	return result;
}

Word word_from_string(char *str) {
  Word w;
  for(int i = 0; i < 5; i++) {
    w.chars[i] = str[i];
  }
  return w;
}

void print_word(Word w) {
  for(int i = 0; i < 5; i++) {
    printf("%c", w.chars[i]);
  }
}

bool words_equal(Word a, Word b) {
  for(int i = 0; i < 5; i++)
    if(a.chars[i] != b.chars[i]) return false;
  return true;
}

Word_List copy_list(Word_List list) {
  Word_List new;
  new.count = list.count;
  new.words = (Word *)malloc(sizeof(Word) * list.count);
  memcpy(new.words, list.words, sizeof(Word) * list.count);
  return new;
}

Word_List list_from_string(char *str) {
  Word_List list;
  list.count = 0;
  {
    int i = 0;
    char c;
    while(c=str[i++])
      if(c == '\n') list.count++;
  }
  list.words = malloc(list.count*sizeof(Word));
  {
    int i = 0;
    char c;
    int j = 0;
    list.words[j] = word_from_string(str);
    j++;
    while((c=str[i++]) && j < list.count) {
      if(c == '\n') {
        list.words[j] = word_from_string(&str[i]);
        j++;
      }
    }
  }
  return list;
}

bool is_in_list(Word word, Word_List list) {
  for(int i = 0; i < list.count; i++) {
    if(words_equal(word, list.words[i])) return true;
  }
  return false;
}

void remove_from_list(Word_List *list, int index) {
  list->count--;
  for(int i = index; i < list->count; i++) {
    list->words[i] = list->words[i+1];
  }
}

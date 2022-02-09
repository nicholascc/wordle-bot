#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <time.h>


#include "word_list.h"
#include "guess.h"


void print_help() {
  printf("Available commands:\n");
  printf(" benchmark         Run the bot on all possible answers and find the average score.\n");
  printf(" simulate [word]   Simulate running the bot on a word. If a word is not provided it is picked randomly.\n");
  printf(" play              Run the bot by providing feedback on the command line.\n");
  printf(" help              Print this help sheet.\n");
}

char *get_line() {
  char *result = calloc(1, 1);
  char *buf = malloc(256);
  bool done = false;
  while(!done) {
    char *buf = malloc(256*sizeof(char));
    fgets(buf, 256, stdin);
    result = realloc(result, strlen(result) + strlen(buf) + 2);
    for(int i = 0;; i++) {
      if(buf[i] == '\n') {
        done = true;
        break;
      }
      if(buf[i] == 0) break;
    }
    strcat(result, buf);
    free(buf);
  }
  return result;
}

Word decide_guess(Word_List guesses, Word_List answers) {

  double max_score = 0;
  Word max_guess;
  for(int i = 0; i < guesses.count; i++) {
    Word guess = guesses.words[i];
    int counts[243];
    for(int j = 0; j < 243; j++) counts[j] = 0;
    for(int j = 0; j < answers.count; j++) {
      Guess_Response res = make_guess(guess, answers.words[j]);
      counts[guess_to_int(res)]++;
    }

    double total = answers.count;
    double sum = 0;
    for(int j = 0; j < 243; j++) {
      if(counts[j] == 0) continue;
      double bucket = counts[j];
      sum += bucket * log2(total/bucket);
    }
    double entropy = sum/total;
    double score = entropy;
    if(answers.count < 50) { // Add a bias towards selecting answers which could be correct.
      for(int j = 0; j < answers.count; j++) {
        if(words_equal(guess, answers.words[j])) {
          score += 0.0001;
          break;
        }
      }
    }
    if(score > max_score) {
      max_score = score;
      max_guess = guess;
    }
  }

  return max_guess;
}

int play(Word answer, Word_List guesses, Word_List answers, Word first, bool verbose) {
  Word_List remain = copy_list(answers);

  int score = 0;
  while(true) {
    Word guess;
    if(score == 0) {
      guess = first;
    } else if(remain.count > 1) {
      guess = decide_guess(guesses, remain);
    } else if(remain.count == 1) {
      guess = remain.words[0];
    } else assert(false);

    Guess_Response r = make_guess(guess, answer);
    int prev_count = remain.count;
    for(int i = remain.count-1; i >= 0; i--) {
      if(!matches(remain.words[i], r)) {
        remove_from_list(&remain, i);
      }
    }

    if(verbose) {
      print_guess_response(r);
      printf(" (gained %.2f bits of information)\n", log2((double)prev_count/(double)remain.count));
    }

    score++;
    if(is_perfect_guess(r)) break;
  }
  free(remain.words);
  if(verbose) printf("Score: %i\n", score);
  return score;
}


void benchmark(Word_List guesses, Word_List known_answers, Word_List answers) {
  Word first_word = decide_guess(guesses, known_answers);
  int score_sum = 0;
  int loss_count = 0;
  for(int i = 0; i < answers.count; i++) {
    Word w = answers.words[i];
    int score = play(w, guesses, known_answers, first_word, false);
    if(score > 6) loss_count++;
    score_sum += score;
    printf("Score on '");
    print_word(w);
    printf("': %i\n", score);
  }
  printf("I had an average score of %.2f with %i total losses.\n", score_sum/(float)answers.count, loss_count);
}

int main(int argc, char *argv[]) {
  printf("\n");
  Word_List guesses;
  {
    char *file = os_load_file("guess_list.txt");
    guesses = list_from_string(file);
    free(file);
  }
  Word_List answers;
  {
    char *file = os_load_file("answer_list.txt");
    answers = list_from_string(file);
    free(file);
  }

  if(argc < 2) {
    printf("You must run this with at least one argument.\n");
    print_help();
    exit(1);
  }

  char *operation = argv[1];

  if(strcmp(operation, "benchmark") == 0) {
    printf("Running benchmark...\n");
    benchmark(guesses, answers, answers);
  } else if(strcmp(operation, "simulate") == 0) {
    Word word;
    Word_List this_answers;
    Word first_word;

    if(argc > 3) {
      printf("The 'simulate' command takes exactly at most 1 argument: the five letter word to simulate a game with.\n");
      exit(1);
    } else if(argc == 3) {
      char *str = argv[2];
      {
        int length = -1;
        while(str[++length]);
        if(length != 5) {
          printf("The bot can only simulate five-letter words. You supplied a %i-letter word\n", length);
          exit(1);
        }
      }
      {
        for(int i = 0; i < 5; i++) {
          if(!isalpha(str[i])) {
            printf("The word must only contain letters. You supplied a word containing '%c'.\n", str[i]);
            exit(1);
          }
          str[i] = tolower(str[i]);
        }
      }
      word = word_from_string(str);
      if(is_in_list(word, answers)) {
        this_answers = answers;
        first_word = word_from_string("soare"); // this takes a few seconds to compute so is hardcoded
      } else if(is_in_list(word, guesses)) {
        this_answers = guesses;
        first_word = word_from_string("tares"); // this takes a few seconds to compute so is hardcoded
      } else {
        printf("Word '%s' is not in either the possible guesses list or the possible answers list.\n", str);
        exit(1);
      }
    } else if(argc == 2) {
      srand(time(NULL));
      rand(); // Without this line the random number generator just follows time.
      word = answers.words[rand()%answers.count];
      printf("Simulating bot with randomly picked word '");
      print_word(word);
      printf("'.\n");
      this_answers = answers;
      first_word = word_from_string("soare"); // this takes a few seconds to compute so is hardcoded
    } else assert(false);

    play(word, guesses, this_answers, first_word, true);
  } else if(strcmp(operation, "play") == 0) {

    Word_List remain;
    Word first_word;
    while(true) {
      printf("Is this word in the wordle possible answers list? (y/n) ");
      char *str = get_line();
      char c = tolower(str[0]);
      if(c == 'y') {
        remain = copy_list(answers);
        first_word = word_from_string("soare"); // this takes a few seconds to compute so is hardcoded
        break;
      } else if(c == 'n') {
        remain = copy_list(guesses);
        first_word = word_from_string("tares"); // this takes a few seconds to compute so is hardcoded
        break;
      }
    }

    printf("\n");

    int score = 0;
    while(true) {
      Word guess;

      if(score == 0) {
        guess = first_word;
      } else if(remain.count > 1) {
        guess = decide_guess(guesses, remain);
      } else if(remain.count == 1) {
        guess = remain.words[0];
      } else if(remain.count == 0) {
        printf("This word isn't in my word list!\n");
        exit(1);
      } else assert(false);

      printf("I guess: '");
      print_word(guess);
      printf("'\n");

      printf("Enter your response using '.' for white, '?' for yellow, and '!' for green.\n  ");
      char *str = get_line();
      for(int i = 0; i < 5; i++) {
        if(str[i] != '.' && str[i] != '?' && str[i] != '!') {
          printf("Your response isn't formatted correctly.\n");
          exit(1);
        }
      }

      Guess_Response r = response_from_string(guess, str);
      int prev_count = remain.count;
      for(int i = remain.count-1; i >= 0; i--) {
        if(!matches(remain.words[i], r)) {
          remove_from_list(&remain, i);
        }
      }

      print_guess_response(r);
      printf(" (gained %.2f bits of information)\n\n", log2((double)prev_count/(double)remain.count));

      score++;
      if(is_perfect_guess(r)) break;
    }
    printf("I win! Score: %i\n", score);
  } else if(strcmp(operation, "help") == 0) {
    print_help();
  } else {
    printf("Unrecognized first argument: '%s'.\n", operation);
    print_help();
    exit(1);
  }
}

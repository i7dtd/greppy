#define _GNU_SOURCE

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e_flag;
  int i_flag;
  int v_flag;
  int c_flag;
  int l_flag;
  int n_flag;
  int err;
} Flags;

void AddPattern(char** pattern, const char* srs, int* err);
void Parser(int argc, char** argv, Flags* flags, char** pattern, int* err);
void PrintMatch(const char* filename, Flags* flags, char* line, int line_num,
                int file_count, ssize_t read);
void ProcessFile(const char* filename, Flags* flags, regex_t* preg,
                 int file_count, int* err);

int main(int argc, char** argv) {
  Flags flag = {0};
  int err = 0;
  char* pattern = NULL;
  Parser(argc, argv, &flag, &pattern, &err);

  if (flag.err == 1) {
    fprintf(stderr, "usage: greppy [-eivcln] [pattern] [file...]\n");
    err = 2;
  }
  if (!err && flag.e_flag == 0) {  // !err don't have mistakes remind it err0
    if (optind < argc) {
      AddPattern(&pattern, argv[optind], &err);
      optind++;
    } else {
      err = 2;  // dont have pattern
    }
  }
  if (!err) {
    regex_t preg;
    int cflags = REG_EXTENDED;  // for ->| with many e

    if (flag.i_flag == 1) {
      cflags |= REG_ICASE;  // i
    }
    if (regcomp(&preg, pattern, cflags) != 0) {
      fprintf(stderr, "greppy: regex compilation error\n");
      err = 2;
    } else {
      int file_count = argc - optind;
      for (int i = optind; i < argc; i++) {
        ProcessFile(argv[i], &flag, &preg, file_count, &err);
      }
      regfree(&preg);
    }
  }
  free(pattern);
  return err;
}
void PrintMatch(const char* filename, Flags* flags, char* line, int line_num,
                int file_count, ssize_t read) {
  if (flags->c_flag == 0 && flags->l_flag == 0) {
    if (file_count > 1) {
      printf("%s:", filename);
    }
    if (flags->n_flag == 1) {
      printf("%d:", line_num);
    }
    printf("%s", line);
    if (read > 0 && line[read - 1] != '\n') {
      printf("\n");
    }
  }
}

void ProcessFile(const char* filename, Flags* flags, regex_t* preg,
                 int file_count, int* err) {
  FILE* file = fopen(filename, "r");

  if (file == NULL) {
    perror(filename);
    *err = 2;
    return;
  }
  char* line = NULL;
  size_t len = 0;
  ssize_t read = 0;

  int line_num = 0;
  int match_count = 0;
  while ((read = getline(&line, &len, file)) != -1) {
    line_num++;
    int match = regexec(preg, line, 0, NULL, 0);
    int is_matched = (flags->v_flag == 1) ? (match != 0) : (match == 0);
    if (is_matched) {
      match_count++;
      if (flags->l_flag == 0 && flags->c_flag == 0) {
        PrintMatch(filename, flags, line, line_num, file_count, read);
      }
      if (flags->l_flag == 1 && flags->c_flag == 0) {
        break;
      }
    }
  }
  if (flags->l_flag == 1) {
    if (match_count > 0) {
      printf("%s\n", filename);
    }
  } else if (flags->c_flag == 1) {
    if (file_count > 1) {
      printf("%s:", filename);
    }
    printf("%d\n", match_count);
  }
  free(line);
  fclose(file);
}

void AddPattern(char** pattern, const char* add_pattern, int* err) {
  size_t len = *pattern ? strlen(*pattern) : 0;
  size_t add_len = strlen(add_pattern);

  char* tmp = realloc(*pattern, len + add_len + 2);  // +2 is | and \0
  if (tmp == NULL) {
    perror("greppy: realloc error");
    free(*pattern);
    *pattern = NULL;
    *err = 2;
  } else {
    *pattern = tmp;
    if (len == 0) {
      (*pattern)[0] = '\0';
    } else {
      strcat(*pattern, "|");
    }
    strcat(*pattern, add_pattern);
  }
}

void Parser(int argc, char** argv, Flags* flags, char** pattern, int* err) {
  int option = 0;

  while ((option = getopt(argc, argv, "e:ivcln")) != -1) {
    switch (option) {
      case 'e':
        flags->e_flag = 1;
        AddPattern(pattern, optarg, err);
        break;
      case 'i':
        flags->i_flag = 1;
        break;
      case 'v':
        flags->v_flag = 1;
        break;
      case 'c':
        flags->c_flag = 1;
        break;
      case 'l':
        flags->l_flag = 1;
        break;
      case 'n':
        flags->n_flag = 1;
        break;
      default:
        flags->err = 1;
        break;
    }
  }
}
#ifndef __REG
#define __REG
#include <regex.h>

/* #include "regex.c" */
#define REG_ERROR -1
#define MAX_FNAME_LENGTH 100

typedef struct TUPLE_NODE {
  int start;
  int end;

  struct TUPLE_NODE *next;
} TUPLE_NODE;

typedef struct TUPLE_LIST {
  TUPLE_NODE *head;
  TUPLE_NODE *tail;

} TUPLE_LIST;

typedef struct STRING_NODE {
  char *val;
  struct STRING_NODE *next;
} STRING_NODE;

typedef struct STRING_LIST {
  STRING_NODE *head;
  STRING_NODE *tail;
  int numCounts;
} STRING_LIST;

int compile_regex(regex_t *r, const char *regex_text);
int match_regex(regex_t *r, const char *to_match, TUPLE_LIST *l);

int findFileNames(const char *srcStr, TUPLE_LIST *tList, STRING_LIST *sList, char *directoryPath);
#endif

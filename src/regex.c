
//Note that parts of this little library come from lemoda.net
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include "regex.h"
#include "memory.h"

/* The following is the size of a buffer to contain any error messages
   encountered when the regular expression is compiled. */

#define MAX_ERROR_MSG 0x1000
/* Compile the regular expression described by "regex_text" into
   "r". */



int compile_regex (regex_t * r, const char * regex_text)
{
  int status = regcomp (r, regex_text, REG_EXTENDED|REG_NEWLINE);
  if (status != 0) {
    char error_message[MAX_ERROR_MSG];
    regerror (status, r, error_message, MAX_ERROR_MSG);
    printf ("Regex error compiling '%s': %s\n",
	    regex_text, error_message);
    return 1;
  }
  return 0;
}


/*
Match the string in "to_match" against the compiled regular
expression in "r".
*/
int match_regex (regex_t * r, const char * to_match, TUPLE_LIST *l)
{
  /* "P" is a pointer into the string which points to the end of the
     previous match. */
  const char * p = to_match;
  /* "N_matches" is the maximum number of matches allowed. */
  const int n_matches = 10;
  /* "M" contains the matches found. */


  regmatch_t m[n_matches];
  TUPLE_NODE *newNode;
  int init = 1;

  while (1) {
    int i = 0;
    int nomatch = regexec (r, p, n_matches, m, 0);


    if (nomatch) {
      return init;
    }

    for (i = 0; i < n_matches; i++) {
      int start;
      int finish;

      if (m[i].rm_so == -1) {
	break;
      }

      start = m[i].rm_so + (p - to_match);
      finish = m[i].rm_eo + (p - to_match);
      newNode = NEW(TUPLE_NODE);
      newNode->start = start;
      newNode->end = finish;

      // Create a linked list of the beginnings and ends of the things we've found
      if (init) {
	l->head = newNode;
	l->tail = l->head;

	init = 0; // We have successfully initialized our list
      }

      else {
	//Append this node to the list
	l->tail->next = newNode;
	l->tail = l->tail->next;
      }

    }

    p += m[0].rm_eo;

  }
  return 0;
}

/*
Find the filenames associated with the use statements in tList
*/
int findFileNames(const char *srcStr, TUPLE_LIST *tList, STRING_LIST *sList, char *directoryPath) {
  TUPLE_NODE *curNode = tList->head;
  STRING_NODE *newStrNode;
  char *tmp;
  char *fullPathWithDirectory;
  int first = 1;
  int curChar;
  int countTotalChars;
  int finished = 0;

  // We're going to create a linked list of strings containing the filenames we want
  while (!finished) {
    newStrNode = NEW(STRING_NODE);
    tmp = malloc(MAX_FNAME_LENGTH * sizeof(int));


    // Look for the filename being requested. Should be surrounded by quotes
    for (curChar = curNode->end; curChar<strlen(srcStr); curChar++) {

      if (srcStr[curChar] != ' ') {
	break;
      }
    }
    curChar--;

    // If we didn't find any quotes, error
    if (curChar == (strlen(srcStr)-1) )
      return REG_ERROR;

    curChar++;
    countTotalChars = 0;

    // Otherwise, save everything in between
    for ( ; curChar < strlen(srcStr); curChar++) {

      if (srcStr[curChar] == ' ' || srcStr[curChar] == '\n') {
	break;
      }

      //As long as we don't go over our max number of characters for a filename
      if (countTotalChars<MAX_FNAME_LENGTH) {
	// Copy this character
	tmp[countTotalChars] = srcStr[curChar];
	countTotalChars++;
      }
      else {
	return REG_ERROR;
      }

      tmp[countTotalChars] = '\0';

    }

    // Add a node to our linked list of strings


    fullPathWithDirectory = malloc((strlen(tmp) + strlen(directoryPath)) * sizeof(char));
    strcpy(fullPathWithDirectory, directoryPath);
    strcat(fullPathWithDirectory, tmp);
    newStrNode->val = fullPathWithDirectory;

    if (first) {
      sList->head = newStrNode;
      sList->tail = newStrNode;
      sList->numCounts = 1;
      first = 0;
    }

    else {
      sList->tail->next = newStrNode;
      sList->tail = sList->tail->next;
      sList->numCounts++;
    }

    //If we've reached the end of our list of indecies, then stop the loop
    if (curNode==tList->tail)
      finished = 1;
    
    curNode = curNode->next;

  }
  return 0;
}

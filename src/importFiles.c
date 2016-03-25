#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regex.h"
#include "memory.h"
#include "importFiles.h"

/* char *performImportation(char *srcStr) { */
/* const char *regex_text; */
/* const char *find_text; */
/* char *testString; */
/* TUPLE_LIST *tList; */
/* tList = NEW(TUPLE_LIST); */
/* STRING_LIST *sList; */
/* sList = NEW(STRING_LIST); */
/* regex_t r; */
/* regex_text = "use"; */
/* find_text = "use \"test1\" use \"test2\" use \"test3\""; */
/* compile_regex(&r, regex_text); */
/* match_regex(&r, find_text, tList); */
/* findFileNames(find_text, tList, sList); */
/* printf("%s\n", addFilesToSrcStr(find_text, sList)); */
/* /\* while ((testString = importFiles(sList)) != NULL ) { *\/ */
/* /\* printf("%s\n", testString); *\/ */
/* /\* } *\/ */
/* regfree(&r); */
/* return 0; */
/* } */
/*
Each time this function is run, it imports a file from sList and returns their content. When there are no more
files, it returns NULL.
*/





char *convertFileToStr(char *filename) {

  char *directoryPath = malloc(strlen(filename) * sizeof(char));
  int pathMarker = 0;
  int i=0;

  for (i=0; i<strlen(filename); i++) {
    if (filename[i] == '/')
      pathMarker = i;
  }
  if (pathMarker>0) {
    pathMarker++;
    memcpy(directoryPath, filename, pathMarker);
    directoryPath[pathMarker] = '\0';
  }



  char *file_contents;
  long input_file_size;
  FILE *input_file = fopen(filename, "rb");
  fseek(input_file, 0, SEEK_END);
  input_file_size = ftell(input_file);
  rewind(input_file);
  file_contents = malloc(input_file_size * (sizeof(char)));
  fread(file_contents, sizeof(char), input_file_size, input_file);
  fclose(input_file);



  TUPLE_LIST *tList = NEW(TUPLE_LIST);
  STRING_LIST *sList = NEW(STRING_LIST);
  regex_t r;
  char *regex_text="use";
  int nomatch;
  compile_regex(&r, regex_text);

  nomatch = match_regex(&r, file_contents, tList);

  if (!nomatch) {


    findFileNames(file_contents, tList, sList, directoryPath);

    return addFilesToSrcStr(file_contents, sList, tList);
  }



  return file_contents;
  
}



char *importFiles(STRING_LIST *sList) {
  int finished = 0;
  STRING_NODE *curNode = sList->head;
  int totalLength = 0;
  int firstTime = 1;

  FILE *fp;
  char *mode = "r";
  char buf[1000];
  char *allFilesStr = malloc(MAX_FILE_LENGTH * sizeof(char));
  char *tmpStr = NULL;
  int count =0;

  if (sList->head == NULL)
    return NULL;


  
  // Open file described in curNode
  fp = fopen(curNode->val, mode);
  if (fp==NULL) {
    printf("Fatal error! External group file \"%s\" does not exist\n", curNode->val);
    exit(-1);
  }

  // Copy whole file
  while (fgets(buf,1000, fp)!=NULL) {

    if (count * 1000 > MAX_FILE_LENGTH) {
      printf("Fatal eror: Imported file is too long\n");
      exit(-1);
    }

    if (!firstTime)
      strcpy(allFilesStr, buf);

    else
      strcat(allFilesStr, buf);


  }

  fclose(fp);


  if (curNode == sList->tail) {
    curNode->next = NULL;
  }

  sList->head = curNode->next;

  /* // Make sure that if we're at the end of the list, we just return NULL */
  /* if (curNode->next!=NULL) { */

  /*   sList->head = curNode->next; */
  /*   if (sList->head == sList->tail) { */
  /*     sList->head->next = NULL; */
  /*   } */
  /* } */

  /* else { */

  /*   return NULL; */
  /* } */

  return allFilesStr;
}


char *addFilesToSrcStr(const char *srcStr, STRING_LIST *sList, TUPLE_LIST *tList) {
  char *importedSrc;
  char *srcWithIncludes;
  char *justIncludesStr;
  char *tmp = malloc(strlen(srcStr) * sizeof(char));
  int count = 0;


  justIncludesStr = malloc((sList->numCounts * MAX_FILE_LENGTH) * sizeof(char));
  strcpy(justIncludesStr, "");

  while ((importedSrc = importFiles(sList)) != NULL) {

    strcat(justIncludesStr, importedSrc);

  }

  srcWithIncludes = malloc((strlen(justIncludesStr) + strlen(srcStr)) * sizeof(char));


  memcpy(srcWithIncludes, srcStr, tList->head->start);
  srcWithIncludes[tList->head->start] = '\0';



  int i;
  for (i=tList->tail->end; i<strlen(srcStr); i++) {
    if (srcStr[i]=='\n')
      break;
  }



  memcpy(tmp, &srcStr[i], strlen(srcStr)-i);
  tmp[strlen(srcStr)-i] = '\0';



  strcat(srcWithIncludes, justIncludesStr);
  strcat(srcWithIncludes, "\n");
  strcat(srcWithIncludes, tmp);
  strcat(srcWithIncludes, "\n");
  return srcWithIncludes;
}

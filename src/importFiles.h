#ifndef __IMPORT
#define __IMPORT
#define MAX_FILE_LENGTH 10000

#include "regex.h"
char *importFiles(STRING_LIST *sList);
char *addFilesToSrcStr(const char *srcStr, STRING_LIST *sList, TUPLE_LIST *tList);
char *convertFileToStr(char *filename);

#endif 

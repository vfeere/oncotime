#ifndef __PREPROCESS
#define __PREPROCESS

#include "memory.h"
#include "tree.h"

void preprocessSECTION_NODE(SECTION_NODE *s, ID_LIST *l);
ID_LIST *preprocessPROGRAM(PROGRAM_NODE *program);
void addGroupsPROGRAM(PROGRAM_NODE *program, GROUP_LIST *glist);

#endif

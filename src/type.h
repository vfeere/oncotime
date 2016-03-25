#ifndef __TYPE
#define __TYPE

#include "tree.h"
#include "symtab.h"

#define TYPE_ERROR -2

void typeImplementationCOMPUTATION_STMT_LIST(COMPUTATION_STMT_LIST *l, SymbolTable *st);
void typeImplementationID_NODE(ID_NODE *n, SymbolTable *st);
void typeImplementationID_LIST(ID_LIST *l, SymbolTable *st);
void typeImplementationPROGRAM_NODE(PROGRAM_NODE *p, SymbolTable *st);


#endif

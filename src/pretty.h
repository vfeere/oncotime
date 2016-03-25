#ifndef PRETTY_H
#define PRETTY_H
#include <stdio.h>
#include "tree.h"
#include "filterRefinements.h"
#include "symtab.h"

extern FILE *output;
extern FILE *source;

void prettyPROGRAM(PROGRAM_NODE *program, FILTERS *filters, SymbolTable *symbolTable);
void prettySECTION_NODE(SECTION_NODE *s); 
void prettyGROUP_LIST(GROUP_LIST *l);
void prettyFILTER_LIST(FILTER_LIST *f);
void prettyPOPULATION_ATTR_LIST(POPULATION_ATTR_LIST *list);
void prettyPERIOD_ATTR_LIST(PERIOD_ATTR_LIST *list);
void prettyINT_LIST(INT_LIST *l);
void prettyHOUR_LIST(HOUR_LIST *h);
void prettyDECLARATION(DECLARATION *dcl);
void prettyID_LIST(ID_LIST *l); 
void prettyCOMPUTATION_LIST(COMPUTATION_STMT_LIST *stmts);
void prettyFOR_ITEM(FOR_ITEM *forItem, int isComplex);
void prettyFOR_STMT(COMPUTATION_STMT *stmt);
void prettyCOMPLEX_FOR_STMT(COMPUTATION_STMT *stmt);
void prettyIF_STMT(COMPUTATION_STMT *stmt);
void prettyEXPRESSION(EXPR *expr);
void prettyPRINT_STMT(PRINT_STMT *stmt);
void prettyBARCHART_STMT(COMPUTATION_STMT *stmt);
void prettyDECLARATION_STMT(COMPUTATION_STMT *stmt);
void prettySECTION_NODE(SECTION_NODE *s);
void prettySEQUENCE_LIST(SEQUENCE_LIST *s);
void prettyDECLARATION(DECLARATION  *dcl);
void prettyINT_ATTRIBUTE_VALUES(INT_VALUE *value);
void prettyID_ATTRIBUTE_VALUES(ID_VALUE *value);
void prettyHOUR_ATTRIBUTE_VALUES(HOUR_VALUE *value);
char *symbolTypeToString(SYMBOL *symbol);

#endif

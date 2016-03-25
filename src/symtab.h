/*
 * Symbol Table Interface
 */
#ifndef SYMTAB_H_
#define SYMTAB_H_

#include "tree.h"
#include <stdio.h>

/*~~~~~~~~~~~~~~~~~~~~~~~~~
 * Symbol Table Global Vars
 * ~~~~~~~~~~~~~~~~~~~~~~~~*/
extern int DUMPSYMBOLTABLE;

/*~~~~~~~~~~~~~~~~~~~~~~~
 Symbol Table typedefs
 ~~~~~~~~~~~~~~~~~~~~~~~*/

typedef enum
{IfS, ElseS, ComplexForS, SimpleForS, TopLevelS, ComputationSectionS} ScopeType;

typedef struct SYMBOL
{
  ScopeType scope;
	char *name;
	SymType type;
	struct SYMBOL *next;
} SYMBOL;

#define HashSize 317
/* Symbol table is a cactus stack of Symbol Tables */
typedef struct SymbolTable
{
  ScopeType scope;
	SYMBOL *table[HashSize];
	struct SymbolTable *next;
} SymbolTable;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Symbol Table Stack Stacks 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct SymbolTableStack
{
  SymbolTable *table;
  SymbolTable **contents;
  int top;
}SymbolTableStack;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Symobl Table Operations
 * ~~~~~~~~~~~~~~~~~~~~~~~~*/
SymbolTable *initSymbolTable();
SymbolTable *scopeSymbolTable(SymbolTable *t);
SYMBOL *putSymbol(SymbolTable *t, char *name, SymType type);
SYMBOL *getSymbol(SymbolTable *t, char *name);
int defSymbol( SymbolTable *t, char *name);
SymbolTable *symReverse(SymbolTable *t);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  BUILD SYMBOL TABLE FUNCTIONS 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

SymbolTable *symProgram(PROGRAM_NODE *p);
void symHeaderSection(SymbolTable *t, SECTION_NODE *hdr_section);
void symHeaderSection(SymbolTable *t, SECTION_NODE *hdr_section);
SymbolTable *symOptSection(SymbolTable *t, SECTION_NODE *opt_section);
void symUseSection(SymbolTable *t,SECTION_NODE *opt_section);
void symGroupSection(SymbolTable *t, SECTION_NODE *opt_section);
SymbolTable *symComputationSection(SymbolTable *t, COMPUTATION_STMT_LIST *smtList, SymType idType);
SymbolTable *symSimpleForStmt(SymbolTable *t, COMPUTATION_STMT *stmt);
SymbolTable *symComplexForStmt(SymbolTable *t, COMPUTATION_STMT *stmt);
SymbolTable *symIfStmt(SymbolTable *t, COMPUTATION_STMT *stmt);
void symPrintStmt(SymbolTable *t, PRINT_STMT *stmt, SymType type);
void symBarchartStmt(SymbolTable *t, COMPUTATION_STMT *stmt, SymType idType);
void symDeclarationStmt(SymbolTable *t, DECLARATION *stmt);
void symSequenceList(SymbolTable *t, SEQUENCE_LIST *seq);
void symSequenceArgsList(SymbolTable *t, SEQUENCE_ARGS_LIST *args);

/* prints the symbol table to the output file */
void printSymTab(FILE *output, SymbolTable *t);
#endif

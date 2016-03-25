/*
 * TODO: Add type inference for scrip input arguments 
 */

/*
 * Implements the symbol table interface 
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "symtab.h"
#include "tree.h"
#include "pretty.h"
#include <stdarg.h>
#include "events.h"
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *          GLOBAL VARIABLES 
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*global varibales keep track of the current scope*/
static ScopeType currScope;
static SymbolTable *currTable;
/* a scope stack to keep track of the current scope */
static SymbolTableStack *tableStack;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *        HELPER FUNCTIONS
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static const char *scopeAsString(ScopeType s)
{
  const char* scopes[] = {"If", "ELse", "Complex For", "Simple For", "Top Level", "Computation Section"};
  return scopes[s];
}

/*returns the string value of a type */
static const char *typeAsString(SymType t)
{

  const char *types[] = {"patient", "doctor","diagnosis", "sequence","sequence parameter",
    "list", "list element","table", "table element", "group", "group file", "script",
   "string", "attribute", "identifier"};
  return types[t];
}

/* checks if a given string is an attribute */
static int lookupAtrr(char *name)
{
  const char*  attributes[] = {"Gender", "Name", "Age", "Diagnosis"};
  int i;
  for(i = 0; i < 4; i++)
    if(strcmp(attributes[i],name) == 0) return 1;
  return 0;
}

/* checks if a string is part of the predefined events */
static int lookupEvents(char *name)
{
  int i;
  for(i = 0; i < NUM_EVENTS; i++)
    if(strcmp(name,events[i]) == 0) return 1;
  return 0;
}


/* helper error function prints an error to the stderr */
static void symError(char* err_mssg, ...)
{ 
  /*format the error string using the variable arguments*/
  char* frmt_err_str = malloc(strlen(err_mssg)+1);

  va_list va;
  va_start(va,err_mssg);
  vsnprintf(frmt_err_str, strlen(err_mssg)+25, err_mssg,va);
  frmt_err_str = realloc(frmt_err_str, strlen(frmt_err_str)+1);
  va_end(va);

  fprintf(stderr,"Symbol Table Error: %s\n",frmt_err_str);
  exit(-1);
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *    SYMBOL TABLE FUNCTIONS
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* shift size used in the hashing function */
#define SHIFT 1

/* Hash function returns the hash of a symbol */
static int Hash(char *key)
{
  unsigned int hash = 0;
  while(*key) hash  = (hash << SHIFT) + *key++;
  return hash % HashSize;
}


SymbolTable *initSymbolTable()
{
  SymbolTable *t = malloc(sizeof(SymbolTable));
  int i;
  t->next = NULL;
  t->scope = currScope;
  return t;
}

/* adds a symbol table to the cactus stack */
SymbolTable *scopeSymbolTable(SymbolTable *s)
{
  SymbolTable *t;
  t = initSymbolTable();
  t->next = s;
  return t;
}

/* puts a symbol in a given symbol table */
SYMBOL *putSymbol(SymbolTable *t, char *name, SymType type)
{
  int i = Hash(name);
  SYMBOL *s;
  /* if symbol is already in table return symbol */
  for(s = t->table[i]; s != NULL; s = s->next)
    if(strcmp(s->name, name) == 0) return s;
  /* symbol is not found insert it in table */
  s = malloc(sizeof(SYMBOL));
  s->name = name;
  s->type = type;
  s->scope = currScope;
  s->next = t->table[i];
  t->table[i] = s;
  return s;
}

/* searches for a symbol in the symbol table
 * returns the symbol if found else returns null */
SYMBOL *getSymbol(SymbolTable *t, char *name)
{
  int i = Hash(name);
  SYMBOL *s;
  for( s = t->table[i]; s != NULL; s = s->next)
    if(strcmp(s->name,name) == 0) return s;
  if(t->next == NULL) return NULL;
  return getSymbol(t->next,name);
}

/* checks if a symbol is already defined within a scope of the symbol table 
 * returns 1 if found 0 otherwise */
int defSymbol( SymbolTable *t, char *name)
{
  int i = Hash(name);
  SYMBOL *s;
  for(s = t->table[i]; s != NULL; s = s->next)
    if(strcmp(s->name,name) == 0) return 1;
  return 0;
}

/* check if an indentfier is an input argument and if it is it sets its
 * type to t and returns the symbol oherwise returns NULL */
SYMBOL *checkInputArgs(SymType t, char *name)
{
  SYMBOL *inputArg = getSymbol(currTable,name);
  if(inputArg && inputArg->type == IDT)
  {
    inputArg->type = t;
    return inputArg;
  }
  else
   return  NULL;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  STACK OF SYMBOL TABLE  OPERATIONS
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 
static int stacksize = 317;

static SymbolTableStack *initTableStack()
{
  SymbolTableStack *s = malloc(sizeof(SymbolTableStack));
  SymbolTable **contents = malloc(sizeof(**contents)*stacksize);
  s->contents = contents;
  s->top = -1;
  return s;
}

static void push(SymbolTable *table, ScopeType scope)
{
  if(tableStack->top >= stacksize)
  {
    stacksize += stacksize;
    tableStack = realloc(tableStack,stacksize);
  }
  tableStack->contents[(tableStack->top +1)] = table;
  tableStack->top++;
}

static SymbolTable *pop()
{
  SymbolTable *table = tableStack->contents[tableStack->top];
  tableStack->top--;
  if(DUMPSYMBOLTABLE)
    printSymTab(output,table);
  return table;
}

static SymbolTable *peek()
{
  return tableStack->contents[tableStack->top];
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Symbol Table Printing
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void printSymTab(FILE *output, SymbolTable *t)
{
  
  while(t)
  { 
    fprintf(output,"VARIABLE NAME        VAR TYPE             SCOPE\n");
    fprintf(output,"-------------        --------             _____\n");
    int i;
    for(i = 0; i < HashSize; i++)
    {
      if(t->table[i])
      {

        SYMBOL *s = t->table[i];
        while(s)
        {

          fprintf(output,"%-21s ", s->name);
          fprintf(output,"%-17s  ", typeAsString(s->type));
          fprintf(output,"%-13s  ",scopeAsString(s->scope));
          fprintf(output,"\n");
          s = s->next;
        }
      }
    }
    fprintf(output,"\n\n");

    t = t->next;
  }
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *     REVERSE SYMBOL TABLE
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

SymbolTable *symReverse(SymbolTable *t) {


  SymbolTable *head = t;

  if (head->next==NULL)
    return head;
  
  SymbolTable *snd = head->next;
  SymbolTable *thrd;


  int first = 1;


  while(snd!=NULL) {

    if (first) {
      head->next = NULL;
      first = 0;
    }
      
    
    thrd = snd->next;
    snd->next = head;
    head = snd;
    snd = thrd;
  }  
  return head;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *    BUILDING SYMBOL TABLE FUNCTIONS 
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



SymbolTable *symProgram(PROGRAM_NODE *p)
{
  tableStack = initTableStack();
  /* currScope has to be set before initSymbolTable is called, since 
   * initsymboltaable uses currScope */
  currScope = TopLevelS;
  SymbolTable *t = initSymbolTable();
  currTable = t;
  push(t,TopLevelS);
  symHeaderSection(t,p->hdr_section);
  t = symOptSection(t, p->opt_sections);
  return t;
}

void symHeaderSection(SymbolTable *t, SECTION_NODE *hdr_section)
{
  putSymbol(currTable,hdr_section->val.headerE.id,ScriptT);
  /* if there's a parameter list add the parameters to the symbol table */
  ID_LIST *l = hdr_section->val.headerE.paramList;
  if( l != NULL)
  {
    ID_NODE *n = l->head;
    while(n != NULL)
    {
      putSymbol(currTable,n->val,IDT);
      n = n->next;
    }
  }   
}

SymbolTable *symOptSection(SymbolTable *t, SECTION_NODE *opt_section)
{

  while(opt_section != NULL)
  {
    switch(opt_section->kind)
    {
      case useK:
        symUseSection(t, opt_section); break;
      case groupK:
        symGroupSection(t,opt_section); break;
      case filterK: break;
      case computationK:
        /* new scope for computation section */
        /* curr scope has to be set before scopeSymbolTable since scopeSymbolTable calls
         * initSymbolTable which uses currScope */
        currScope = ComputationSectionS;
        t = scopeSymbolTable(t);
        push(t,ComputationSectionS);
        currTable = t;
        t = symComputationSection(t, opt_section->val.computationList, IDT); 
        break;
      default:
        fprintf(stderr, "ERROR: uknown section kind: %d",opt_section->kind);
        exit(EXIT_FAILURE);
        break;
    }
    opt_section = opt_section->next;
  }
  pop();
  return t;
}

void symUseSection(SymbolTable *t, SECTION_NODE  *use_section)
{
  ID_NODE *filename = use_section->val.groupFiles->head;
  while(filename != NULL)
  {
    putSymbol(currTable,filename->val,GrpFileT);
    filename = filename->next;
  }
}

void symGroupSection(SymbolTable *t, SECTION_NODE  *group_section)
{
 /* add group name and param list to sym table 
  * and iterate throught the group list */
 GROUP_NODE *grp = group_section->val.groupList->head;

 while(grp != NULL)
 {
   /* check that variable being declared as group isn't a script argument */
   SYMBOL *s =getSymbol(currTable,grp->id);
   if(s && s->type == IDT)
     symError("can't redeclare script argument %s as a group on line %d", grp->id, grp->lineno);
   else if( defSymbol(currTable,grp->id)) /* no redeclarations */
      symError("can't redeclare symbol %s as group on line %d",grp->id, grp->lineno);  
   else {
      putSymbol(currTable,grp->id,GrpT);
   }
   ID_NODE *groupElt = grp->paramList->head;
   while(groupElt != NULL)
   {
     if(groupElt->kind == idK)
     {
       s = getSymbol(currTable,groupElt->val);
       if(s != NULL && (s->type == IDT || s->type == StringT))
         s->type = StringT;
       else
         symError("invalid identifier in group declaration on line: %d",grp->lineno);
     }
     groupElt = groupElt->next;
   }
   grp = grp->next;
 }
}

SymbolTable *symComputationSection(SymbolTable *t, COMPUTATION_STMT_LIST *stmtList, SymType idType)
{
  COMPUTATION_STMT *stmt = stmtList->head;
  while(stmt != NULL)
  {
    switch(stmt->kind)
    {
      case forK:
        t = symSimpleForStmt(t,stmt); 
        currTable = peek(); 
        currScope = currTable->scope;
        break;
      case complexForK:
        t = symComplexForStmt(t,stmt); 
        currTable = peek();
        currScope = currTable->scope;
        break;
      case ifK:
        t = symIfStmt(t,stmt);
        currTable = peek();
        currScope = currTable->scope;
        break;
      case printK:
        symPrintStmt(t,stmt->value.printSTATEMENT.printStmt,idType); 
        break;
      case barChartK:
        symBarchartStmt(t,stmt,idType); 
        break;
      case declarationK:
        symDeclarationStmt(t,stmt->value.declarationSTATEMENT.declaration); 
        break;
      default:
        fprintf(stderr,"ERROR: uknown computation statementkind:%d",stmt->kind);
        exit(EXIT_FAILURE);
      }
    stmt = stmt->next;
  }

  return t;
}

SymbolTable *symSimpleForStmt(SymbolTable *t, COMPUTATION_STMT *stmt)
{
  /* new for statement scope */
  currScope = SimpleForS;
  t = scopeSymbolTable(t);
  currTable = t;
  push(t,currScope);
  /* add for item */
  FOR_ITEM *forItem = stmt->value.forSTATEMENT.forItem;
  switch(forItem->type)
  {
    case patientK:
      putSymbol(currTable,forItem->value.idSTATEMENT.id,PatientT);
      break;
    case elementK:
      putSymbol(currTable,forItem->value.elementSTATEMENT.id,TableEltT);
      putSymbol(currTable,forItem->value.elementSTATEMENT.table, TableT);
      break;
    case diagnosisIdK:
      putSymbol(currTable,forItem->value.idSTATEMENT.id,DiagnosisT);
      break;
    case doctorK:
      putSymbol(currTable,forItem->value.idSTATEMENT.id,DoctorT);
      break;
    case sequenceK:
      putSymbol(currTable,forItem->value.sequenceSTATEMENT.id, SequenceT);
      symSequenceList(currTable,forItem->value.sequenceSTATEMENT.sequenceList);
      break;
    case memberK:
      putSymbol(currTable,forItem->value.memberSTATEMENT.id, SequenceT);
      break;
    case timelineK:
    default:
      break;
  }

  /* add for loop body */
  if(stmt->value.forSTATEMENT.type == printT)
    symPrintStmt(t, stmt->value.forSTATEMENT.printStmt,forItem->type);
  else
  {
    COMPUTATION_STMT *barchartStmt = stmt->value.forSTATEMENT.barChartStmt;
    putSymbol(currTable,barchartStmt->value.barChartSTATEMENT.id,forItem->type);
  }
  pop();
  return t;
}

SymbolTable *symComplexForStmt(SymbolTable *t, COMPUTATION_STMT *stmt)
{
  /* new for statement scope */
  currScope = ComplexForS;
  t = scopeSymbolTable(t);
  currTable = t;
  push(t,currScope);
  /* add for item */
  FOR_ITEM *forItem = stmt->value.complexForSTATEMENT.forItem;
  switch(forItem->type)
  {
    case patientK:
      putSymbol(currTable,forItem->value.idSTATEMENT.id,PatientT);
      break;
    case elementK:
      putSymbol(currTable,forItem->value.elementSTATEMENT.id,TableT);
      break;
    case diagnosisIdK:
      putSymbol(currTable,forItem->value.idSTATEMENT.id,DiagnosisT);
      break;
    case doctorK:
      putSymbol(currTable,forItem->value.idSTATEMENT.id,DoctorT);
      break;
    case sequenceK:
      putSymbol(currTable,forItem->value.sequenceSTATEMENT.id, SequenceT);
      break;
    case memberK:
      putSymbol(currTable,forItem->value.memberSTATEMENT.id, ListT);
      break;
    case timelineK:
    default:
      break;
  }

  /* add computation section */
  t = symComputationSection(t,stmt->value.complexForSTATEMENT.stmtList, forItem->type);
  pop();
  return t;
}

SymbolTable *symIfStmt(SymbolTable *t, COMPUTATION_STMT *stmt)
{
  /* new scope for the if section */
  currScope = IfS;
  t = scopeSymbolTable(t);
  currTable = t;
  push(t,currScope);
  /* add if section */
  t = symComputationSection(t,stmt->value.ifSTATEMENT.ifComputationStmtList, IDT);
  pop();
  /* add optional else section */
  if(stmt->value.ifSTATEMENT.type == ifElseT)
  {
    currScope = ElseS;
    t = scopeSymbolTable(t);
    currTable = t;
    push(t,currScope);
    t = symComputationSection(t,stmt->value.ifSTATEMENT.elseComputationStmtList,IDT);
    pop();
  }
  return t; 
}

void symPrintStmt(SymbolTable *t, PRINT_STMT *stmt, SymType stmtType)
{
  while(stmt)
  {
    switch(stmt->type)
    {
      case xOfYK:
        if(getSymbol(t,stmt->value.xOfYSTATEMENT.element) == NULL)
          symError("Can't print undeclared identifier");
        else
          {
            stmtType = getSymbol(t,stmt->value.xOfYSTATEMENT.element)->type; 
            putSymbol(currTable,stmt->value.xOfYSTATEMENT.element,stmtType);
          }
        break;
      case tableElementK: 
        /* if table element not defined then error */
        if(!getSymbol(t,stmt->value.tableElementSTATEMENT.tableName))
          symError("Use of undeclared table variable %s", 
              stmt->value.tableElementSTATEMENT.tableName);
        else
          putSymbol(currTable,stmt->value.tableElementSTATEMENT.index,TableEltT);
        break;
      case tableLengthK:
        if(!getSymbol(t,stmt->value.tableElementSTATEMENT.tableName))
          symError("Use of undeclared table variable %s", 
              stmt->value.tableElementSTATEMENT.tableName);
        break;
      case timelinePrintK:
      case identifierK:
      {
       
        if(!lookupAtrr(stmt->value.idSTATEMENT.id))
        {
          SYMBOL *s = getSymbol(t,stmt->value.idSTATEMENT.id); 
          if(s == NULL)
            symError("Can't print undeclared symbol %s", stmt->value.idSTATEMENT.id);
          else 
          putSymbol(currTable,stmt->value.idSTATEMENT.id,s->type);
        }
        break;
      }
      default:
        break;
     }
    stmt = stmt->next;
  }
}

void symBarchartStmt(SymbolTable *t, COMPUTATION_STMT *stmt, SymType idType)
{
  SYMBOL *s = getSymbol(t, stmt->value.barChartSTATEMENT.id);
  if(!s)
    symError("Undeclared symbol %s in barchart on line %d", stmt->value.barChartSTATEMENT.id, stmt->lineno);
  else
    putSymbol(currTable,stmt->value.barChartSTATEMENT.id,s->type);
}

static void symID_LIST(SymbolTable *t, ID_LIST *l, SymType idType)
{
  ID_NODE *curr = l->head;
  while(curr != NULL)
  {
    if(curr->kind == stringK)
      putSymbol(currTable,curr->val,StringT);
    else
      putSymbol(currTable,curr->val,idType);
    curr = curr->next;
  }
}

/* checks if id is an input argument if true sets the input argument type to t
 * and returns the symbol otherwise returns null */
static SYMBOL *setTypeScriptArgs(char *id, SymType t)
{
  SYMBOL *s = getSymbol(currTable,id);
  if(s && s->type == IDT)
    s->type = t;
  return s;
}

void symSequenceList(SymbolTable *t, SEQUENCE_LIST *seq)
{
  SYMBOL *s;
  switch(seq->kind)
  {
    case IdK:
      if(lookupEvents(seq->val.idS.id) == 0)
        symError("sequence %s on line %d not part of predefined events",seq->val.idS.id, seq->lineno); 

      else {
        /* putSymbol(currTable,seq->val.idS.input,SequenceParamT); */
	symID_LIST(t, seq->val.idS.input, SequenceParamT);
      }
      break;
    case OrK:
      if(lookupEvents(seq->val.orS.lid)==0 || lookupEvents(seq->val.orS.rid)==0)
        symError("near sequence %s on line %d not part of predefined events", seq->val.idS.id, seq->lineno);
      else
      { 

        /* putSymbol(currTable,seq->val.orS.lInput,SequenceParamT); */
        /* putSymbol(currTable,seq->val.orS.rInput,SequenceParamT); */

	symID_LIST(t, seq->val.orS.lInput, SequenceParamT);
	symID_LIST(t, seq->val.orS.rInput, SequenceParamT);



      }
      break;
    case ArrowK:
      symSequenceList(t,seq->val.arrowS.lSeq);
      if(lookupEvents(seq->val.arrowS.id) == 0)
        symError("near sequence %s on line %d not part of predefined events", seq->val.idS.id, seq->lineno);

      else {
        /* putSymbol(currTable,seq->val.arrowS.input,SequenceParamT); */
	symID_LIST(t, seq->val.arrowS.input, SequenceParamT);
      }


      break;
    case ArrowOrK:
      symSequenceList(t,seq->val.arrowOrS.lSeq);
      if(lookupEvents(seq->val.arrowOrS.lID)==0||lookupEvents(seq->val.arrowOrS.rID)==0)
        symError("near sequence %s on line %d not part of predefined events",seq->val.idS.id, seq->lineno);
      else
      {

        /* putSymbol(currTable,seq->val.arrowOrS.lInput,SequenceParamT); */
        /* putSymbol(currTable,seq->val.arrowOrS.rInput,SequenceParamT); */
	symID_LIST(t, seq->val.arrowOrS.lInput, SequenceParamT);
	symID_LIST(t, seq->val.arrowOrS.rInput, SequenceParamT);

      }
      break;
    case NonTermListK:
    case NonTermMultListK:
      symSequenceArgsList(t,seq->val.nonTermListS.argsList);
      symSequenceList(t,seq->val.nonTermListS.lSeq);
      break;
    case ListK:
    case MultListK:
      symSequenceArgsList(t,seq->val.listS.argsList);
      break;
   }
} 

void symSequenceArgsList(SymbolTable *t, SEQUENCE_ARGS_LIST *SeqList)
{
  SEQUENCE_ARGS_NODE *node = SeqList->head;
  while(node)
  {
    if(!lookupEvents(node->id))
      symError("sequence %s on line %d not part of predefined list of events",
          node->id, SeqList->lineno);

    else {
      /* putSymbol(currTable, node->arg, SequenceT); */
      symID_LIST(t, node->arg, SequenceParamT);
    }

    node = node->next;
  }
}


void symDeclarationStmt(SymbolTable *t, DECLARATION *stmt)
{
  SYMBOL *s;
  switch(stmt->kind)
  {
    case DclListK:
      if(defSymbol(t,stmt->val.ListD.id))
        symError("Redefinition of symbol %s on line %d", stmt->val.ListD.id, stmt->lineno);
      else if( getSymbol(currTable,stmt->val.ListD.id) &&  getSymbol(currTable,stmt->val.ListD.id)->type == IDT)
        symError("Can't redeclare a script argument %s as list on line: %d",stmt->val.ListD.id, stmt->lineno);
      else
      {
        putSymbol(currTable, stmt->val.ListD.id,ListT);
        symSequenceList(t,stmt->val.ListD.s);
      }
      break;
    case NativeListK:
      if(defSymbol(currTable,stmt->val.NatListD.id))
        symError("Redefinition of symbol %s on line %d", stmt->val.NatListD.id, stmt->lineno);
       else if(getSymbol(currTable,stmt->val.NatListD.id) && getSymbol(currTable,stmt->val.NatListD.id)->type == IDT)
        symError("Can't redeclare a script argument %s as native list on line: %d",stmt->val.TableD.lID, stmt->lineno);
      else
      {
        putSymbol(currTable, stmt->val.NatListD.id,ListT);
        symID_LIST(t,stmt->val.NatListD.l, IDT);
      }
      break;
    case NativeTableK:
      if(defSymbol(currTable,stmt->val.NatTableD.id))
        symError("Redefinition of symbol %s on line %d", stmt->val.NatTableD.id, stmt->lineno);
      else if(getSymbol(currTable,stmt->val.NatTableD.id) && getSymbol(currTable,stmt->val.NatTableD.id)->type == IDT)
        symError("Can't redeclare a script argument %s as native table on line: %d",stmt->val.TableD.lID, stmt->lineno);
      else
      {
        putSymbol(currTable, stmt->val.NatTableD.id,TableT);
        symID_LIST(t,stmt->val.NatTableD.l, IDT);
      }
      break;
    case TableK:

      if(defSymbol(currTable,stmt->val.TableD.lID)) {
	
        symError("Redefinition of symbol %s on line %d", stmt->val.TableD.lID, stmt->lineno);
      }
      else if( getSymbol(currTable,stmt->val.TableD.lID) &&  getSymbol(currTable,stmt->val.TableD.lID)->type == IDT)
        symError("Can't redeclare a script argument %s as table on line: %d",stmt->val.TableD.lID, stmt->lineno);
      else
      {
        putSymbol(currTable, stmt->val.TableD.lID,TableT);
      }
      break;
  }
}

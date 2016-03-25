#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"
#include "type.h"
#include "symtab.h"
#include "events.h"


// Implement the type of an ID_NODE
void typeImplementationID_NODE(ID_NODE *n, SymbolTable *st) {
  SYMBOL *s;
  if (n!=NULL) {
    switch(n->kind) {
    case stringK:
      n->type = StringT;
      break;
    case idK:
      /* printSymTab(stdout,st); */
      if (!defSymbol(st, n->val)) {
      	printf("Fatal error: %s not declared\n", n->val);
      	exit(TYPE_ERROR);
      }

      s = getSymbol(st, n->val);

      n->type = s->type;
      break;
    }
  }
}



// Implement the type of an ID_LIST
// Records if all elements in the list have the same type, and if so records that type
void typeImplementationID_LIST(ID_LIST *l, SymbolTable *st) {
  ID_NODE *curNode = l->head;

  SymType allSameType;
  int areSameType = 1;

  while (curNode) {

    typeImplementationID_NODE(curNode, st);


    //If we're looking at the first node, record its type
    if (curNode==l->head) {
      allSameType = curNode->type;
    }

    // If we discover a new type, we know this list doesn't have uniform type
    if (areSameType) {
      if (curNode->type != allSameType) {
	areSameType = 0;
      }
    }
    

    // Move forward
    if (curNode==l->tail)
      curNode = NULL;
    else
      curNode = curNode->next;
  }

  l->isUniformType = areSameType;
  if (areSameType) {
    l->type = allSameType;
  }
}



//Verify that this group only consists of strings and symbols of type string
void typeImplementationGROUP_NODE(GROUP_NODE *g, SymbolTable *st) {


  typeImplementationID_LIST(g->paramList, st);


  if (!g->paramList->isUniformType) {
    printf("Type Error at line %d: Groups can only be of type string\n", g->lineno);
    exit(TYPE_ERROR);
  }

  if (g->paramList->type!=StringT) {
    printf("Type Error at line %d: Groups can only be of type string\n", g->lineno);
    exit(TYPE_ERROR);
  }

}


// Type for group lists. Basically verifies that all groups only consist of strings
void typeImplementationGROUP_LIST(GROUP_LIST *l, SymbolTable *st) {
  GROUP_NODE *curNode = l->head;


  while (curNode) {

    typeImplementationGROUP_NODE(curNode, st);

    if (curNode==l->tail) {
      curNode = NULL;
    }
    else {
      curNode = curNode->next;
    }
  }

  
}

void typeImplementationPOPULATION_ATTR_LIST(POPULATION_ATTR_LIST *p, SymbolTable *st) {
  
  while (p) {
  switch(p->kind) {

  case DiagK: {
    ID_NODE *diagnosis = p->val.idList->head;
    while (diagnosis) {
      switch(diagnosis->kind) {
      
      // if the diagnosis is a string skip through
      case stringK:
        break;

      // if the diagnosis is an identifier ensure it was declared at top level
      case idK: {
        SYMBOL *idSymbol = getSymbol(st, diagnosis->val);
        if (idSymbol) {

          switch(idSymbol->scope) {
          case TopLevelS:
            break;
          default:
            printf("Type Error at line %d: Identifier %s not declared in scope for filter attribute \"diagnosis\" \n", p->lineno, diagnosis->val);
            exit(TYPE_ERROR);
          }
        } else {
            printf("Type Error at line %d: Identifier %s not declared in scope for filter attribute \"diagnosis\" \n", p->lineno, diagnosis->val);
            exit(TYPE_ERROR);
        }
        break;
      }

      }

      diagnosis = diagnosis->next;
    }
    break;
  }
  
  case GenderK: {
    
    ID_NODE *gender = p->val.idList->head;
    // verify all genders are either M or F, Male or Female (all case insensitive)
    while (gender) {
      

      // convert identifier to lowercase
      for(int i = 0; i < strlen(gender->val); i++){
         gender->val[i] = tolower(gender->val[i]);
      }

      if ((strcmp(gender->val, "female") != 0 && strcmp(gender->val, "f") != 0) && (strcmp(gender->val, "male") != 0 && strcmp(gender->val, "m") != 0)) {
          printf("Type Error at line %d: Identifier %s invalid \"gender\" type.  Must be either M, F, male or female (case insensitive)\n", p->lineno, gender->val);
          exit(TYPE_ERROR);          
      }

      gender = gender->next;
    }
    break;
  }
  
  case PstlCodeK: {

    ID_NODE *postalCode = p->val.idList->head;
    
    // ensure that each postal code adheres to a postal code pattern
    while (postalCode) {

      // ensure postal code is 6 characters long
      if (strlen(postalCode->val) != 6) {
        printf("Type Error at line %d: Identifier %s does not contain only 6 characters for the \"postalcode\" attribute\n", p->lineno, postalCode->val);
        exit(TYPE_ERROR);  
      }

      for (int i = 0; i < 6; i++) {
         if (i % 2 == 0) {
          if (!isalpha(postalCode->val[i])) {
            printf("Type Error at line %d: Identifier %s is not a valid postal code\n", p->lineno, postalCode->val);
            exit(TYPE_ERROR);  
          }
         } else if (!isdigit(postalCode->val[i])) {
            printf("Type Error at line %d: Identifier %s is not a valid postal code\n", p->lineno, postalCode->val);
            exit(TYPE_ERROR);  
         }
      }

      postalCode = postalCode->next;
    }
    
    break;
  }
  
  case AgeK:
  case BirthYK:
  case PopIdK:
    // grammar restricts everything to an integer or an integer range - no type check needed
    break;
  }

  p = p->next;
  
  }
}

void typeImplementationPERIOD_ATTR_LIST(PERIOD_ATTR_LIST *p, SymbolTable *st) {

  int start = 0, end = 0;
  while (p) {

    switch(p->kind) {
    case DaysK: {

    // check that these are days of the week, weekday or weekend
     ID_NODE *day = p->val.idList->head;
    
      while (day) {

        int isValid = 0;
        for (int i = 0; i < strlen(day->val); i++) {
            day->val[i] = tolower(day->val[i]);
        }
      
        if(strcmp(day->val, "monday") == 0 || (
           strcmp(day->val, "tuesday") == 0 || (
           strcmp(day->val, "wednesday") == 0 ||(
           strcmp(day->val, "thursday") == 0 || (
           strcmp(day->val, "friday") == 0 || (
           strcmp(day->val, "saturday") == 0 || (
           strcmp(day->val, "sunday") == 0 || (
           strcmp(day->val, "weekday") == 0 ||
           strcmp(day->val, "weekend") == 0)))))))) {
           day = day->next;
           continue;
        }

        printf("Type Error at line %d: %s not a valid day of the week \n", p->lineno, day->val);
        exit(TYPE_ERROR);  
    
      } 

      break;
    }

    case MonthK: {

      // ensure integers in months list are 1 - 12
      INT_NODE *month = p->val.intList->head;
      while (month) {

        switch(month->kind) {
          case rangeK:
            if ((0 < month->val.rangeE.beginning && month->val.rangeE.beginning < 13) && (0 < month->val.rangeE.end && month->val.rangeE.end < 13)) {
              month = month->next;
              continue;
            }
            break;

          case intK:
            if (0 < month->val.intE && month->val.intE < 13) {
              month = month->next;
              continue;
            }
            break;

          
        }


        printf("Type Error at line %d: value for months attribute not within range 1 - 12 \n", p->lineno);
        exit(TYPE_ERROR);  

      } 

      break;
    }

    case YearK:
    case HoursK:
      break;           
    case StartK:
      start = p->val.num;
      break;
    case EndK: 
      end = p->val.num;
      if(end < start)
      {
        printf("Type Error at line %d: end value can't be lower than start value \n",p->lineno);
        exit(TYPE_ERROR);
      }
    }

    p = p->next;
  }
}


void typeImplementationFILTER_NODE(FILTER_NODE *f, SymbolTable *st) {
  switch(f->kind) {
  case popK:
    typeImplementationPOPULATION_ATTR_LIST(f->val.popAttrList, st);
    break;

  case periodK:
    typeImplementationPERIOD_ATTR_LIST(f->val.periodAttrList, st);
    break;

  case eventsK:
    break;
  }
}



void typeImplementationFILTER_LIST(FILTER_LIST *l, SymbolTable *st) {
  FILTER_NODE *curNode = l->head;

  while (curNode) {
    typeImplementationFILTER_NODE(curNode, st);

      curNode = curNode->next;
  }
}


void typeImplementationPRINT_STMT(PRINT_STMT *p, SymbolTable *st) {
  SYMBOL *s, *indexSym;
  switch (p->type) {
  case printStringK:
    break;
    
  case xOfYK:

    s = getSymbol(st, p->value.xOfYSTATEMENT.element);
    
    int invalidAttribute = 0;

    if (s->type!=PatientT && s->type!=DoctorT) {
      printf("Type error: Only doctors and patients have attributes. %s is not a doctor or a patient.\n", s->name);
      exit(TYPE_ERROR);
    }

    if (s->type==PatientT) {
      if (strcmp(p->value.xOfYSTATEMENT.id, "Gender")!=0
	   && strcmp(p->value.xOfYSTATEMENT.id, "Birthyear")!=0
	   && strcmp(p->value.xOfYSTATEMENT.id, "Id")!=0
	   && strcmp(p->value.xOfYSTATEMENT.id, "Diagnosis")!=0
	   && strcmp(p->value.xOfYSTATEMENT.id, "Postalcode")!=0
	   )
	   {
      invalidAttribute = 1;
      }
    } else if (s->type==DoctorT) {
      if (strcmp(p->value.xOfYSTATEMENT.id, "Id")!=0) invalidAttribute = 1;
    }

    if (invalidAttribute) {
      printf("Type error: Tried to access attribute %s, but %s is not an attribute\n", p->value.xOfYSTATEMENT.id, p->value.xOfYSTATEMENT.id);
      exit(TYPE_ERROR);
    }

   
      
    break;

  case tableElementK:
    /* if (!defSymbol(st, p->value.tableElementSTATEMENT.tableName)) { */
    /*   printf("Fatal error: %s not declared\n", p->value.tableElementSTATEMENT.tableName); */
    /*   exit(TYPE_ERROR); */
    /* } */

    /* if (!defSymbol(st, p->value.tableElementSTATEMENT.index)) { */
    /*   printf("Fatal error: %s not declared\n", p->value.tableElementSTATEMENT.index); */
    /*   exit(TYPE_ERROR); */
    /* } */

    s = getSymbol(st, p->value.tableElementSTATEMENT.tableName);
    indexSym = getSymbol(st, p->value.tableElementSTATEMENT.index);    

    if (s->type==TableT) {

      if (indexSym->type!=TableEltT) {
	printf("Type error: Tried to access an index of table %s with invalid object %s\n", s->name, indexSym->name);
	exit(TYPE_ERROR);
      }
    }

    else if (s->type==ListT) {
      if (indexSym->type!=ListEltT) {
	printf("Type error: Tried to access an index of list %s with invalid object %s\n", s->name, indexSym->name);
	exit(TYPE_ERROR);
      }
    }
      
    else {
      printf("Type error: We can only access indecies of tables. %s is not a table\n", p->value.tableElementSTATEMENT.tableName);
      exit(TYPE_ERROR);
    }

    break;

  case tableLengthK:
    s = getSymbol(st, p->value.idSTATEMENT.id);
    if (s->type!=TableT) {
      printf("Type error: Only tables have length. %s is not a table\n", s->name);
      exit(TYPE_ERROR);
    }
    break;

  case identifierK:
    //We can print anything :)
    break;

  case timelinePrintK:
    s = getSymbol(st, p->value.idSTATEMENT.id);
    if (s->type!=PatientT && s->type!=DoctorT) {
      printf("Type error: Only patients and doctors have timelines. %s is not a patient or a doctor.\n", s->name);
      exit(TYPE_ERROR);
    }
    break;
  }
}


void typeImplementationPRINT_STMT_LIST(PRINT_STMT *p, SymbolTable *st) {
  PRINT_STMT *curNode = p;
  while (curNode) {

    
    typeImplementationPRINT_STMT(curNode, st);
    curNode = curNode->next;
  }
}


void typeImplementationSEQUENCE_ARGS_LIST(SEQUENCE_ARGS_LIST *l, SymbolTable *st) {
  SEQUENCE_ARGS_NODE *curNode;
  curNode = l->head;
  int count;
  int numEntries;

  while (curNode!=NULL) {
    count = countArgs(curNode->id);
    numEntries = curNode->arg->numEntries;

    if (count!=numEntries) {
      printf("Type error: Event %s requires %d arguments. %d arguments given\n", curNode->id, count, numEntries);
      exit(TYPE_ERROR);
    }


    if (curNode == l->tail)
      curNode->next = NULL;

    curNode = curNode->next;
  }
  
}

void typeCheckSeqIdList(char *eventName, ID_LIST *l, SymbolTable *st) {
  ID_NODE *curNode;
  curNode = l->head;

  int numEntries = countArgs(eventName);
  int i;

  SymType correctType;
  SYMBOL *s;

  i=0;
  while (curNode!=NULL) {
    correctType = typeAt(eventName, i);

    
    s = getSymbol(st, curNode->val);

    if (s->type==SequenceT) {
      s->type = correctType;

    }
    else if (s->type!=correctType) {
      printf("Type error: Type conflict for event parameter %s at line %d\n", s->name, l->lineno);
      exit(TYPE_ERROR);
    }
      

    
    
    i++;
    if (curNode==l->tail)
      curNode->next = NULL;
    curNode = curNode->next;
  }
}

void typeImplementationSEQUENCE_LIST(SEQUENCE_LIST *l, SymbolTable *st) {
  int count;
  switch(l->kind) {
  case IdK:

    count = countArgs(l->val.idS.id);
    if (l->val.idS.input->numEntries != count) {
      printf("Type error: Event %s requires %d arguments. %d arguments given\n", l->val.idS.id, count, l->val.idS.input->numEntries);
      exit(TYPE_ERROR);
    }

    typeCheckSeqIdList(l->val.idS.id, l->val.idS.input, st);
    
    break;
    
  case OrK:

    count = countArgs(l->val.orS.lid);
    if (l->val.orS.lInput->numEntries != count) {
      printf("Type error: Event %s requires %d arguments. %d arguments given\n", l->val.orS.lid, count, l->val.orS.lInput->numEntries);
      exit(TYPE_ERROR);
    }

    typeCheckSeqIdList(l->val.orS.lid, l->val.orS.lInput, st);

    count = countArgs(l->val.orS.rid);
    if (l->val.orS.rInput->numEntries != count) {
      printf("Type error: Event %s requires %d arguments. %d arguments given\n", l->val.orS.rid, count, l->val.orS.rInput->numEntries);
      exit(TYPE_ERROR);
    }

    typeCheckSeqIdList(l->val.orS.rid, l->val.orS.rInput, st);


    break;

  case ListK:
    typeImplementationSEQUENCE_ARGS_LIST(l->val.listS.argsList, st);
    break;

  case MultListK:
    typeImplementationSEQUENCE_ARGS_LIST(l->val.listS.argsList, st);
    break;

  case ArrowK:
    typeImplementationSEQUENCE_LIST(l->val.arrowS.lSeq, st);

    count = countArgs(l->val.arrowS.id);
    if (l->val.arrowS.input->numEntries != count) {
      printf("Type error: Event %s requires %d arguments. %d arguments given\n", l->val.arrowS.id, count, l->val.arrowS.input->numEntries);
      exit(TYPE_ERROR);
    }

    typeCheckSeqIdList(l->val.arrowS.id, l->val.arrowS.input, st);

    break;

  case ArrowOrK:
    typeImplementationSEQUENCE_LIST(l->val.arrowOrS.lSeq, st);

    count = countArgs(l->val.arrowOrS.lID);
    if (l->val.arrowOrS.lInput->numEntries != count) {
      printf("Type error: Event %s requires %d arguments. %d arguments given\n", l->val.arrowOrS.lID, count, l->val.arrowOrS.lInput->numEntries);
      exit(TYPE_ERROR);
    }

    typeCheckSeqIdList(l->val.arrowOrS.lID, l->val.arrowOrS.lInput, st);


    count = countArgs(l->val.arrowOrS.rID);
    if (l->val.arrowOrS.rInput->numEntries != count) {
      printf("Type error: Event %s requires %d arguments. %d arguments given\n", l->val.arrowOrS.rID, count, l->val.arrowOrS.rInput->numEntries);
      exit(TYPE_ERROR);
    }

    typeCheckSeqIdList(l->val.arrowOrS.rID, l->val.arrowOrS.rInput, st);

    break;

  case NonTermListK:
    typeImplementationSEQUENCE_LIST(l->val.nonTermListS.lSeq, st);
    typeImplementationSEQUENCE_ARGS_LIST(l->val.nonTermListS.argsList, st);

    break;

  case NonTermMultListK:
    typeImplementationSEQUENCE_LIST(l->val.nonTermMultListS.lSeq, st);
    typeImplementationSEQUENCE_ARGS_LIST(l->val.nonTermMultListS.argsList, st);
    
    break;
  }
}


void typeImplemenetationDECLARATION(DECLARATION *d, SymbolTable *st) {
  switch (d->kind) {
  case NativeListK:
    break;

  case DclListK:
    typeImplementationSEQUENCE_LIST(d->val.ListD.s, st);
    break;

  case NativeTableK:
    break;

  case TableK: {
    int invalidAttribute = 1;
    if(strcmp(d->val.TableD.rID,"Age") == 0) { invalidAttribute = 0; }
    if(strcmp(d->val.TableD.rID,"Diagnosis") == 0) { invalidAttribute = 0; }
    if(strcmp(d->val.TableD.rID,"Gender") == 0) { invalidAttribute = 0; }
    if(strcmp(d->val.TableD.rID,"PostalCode") == 0) { invalidAttribute = 0; }
    if(strcmp(d->val.TableD.rID,"Id") == 0) { invalidAttribute = 0; }

    if (invalidAttribute) {
      printf("Type Error: invalid table attribute %s\n", d->val.TableD.rID);
      exit(TYPE_ERROR);
    }
  }
  }
  
}

void typeImplementationCOMPUTATION_STMT(COMPUTATION_STMT *c, SymbolTable *st) {
  SYMBOL *s;
  switch(c->kind) {
  case forK:
    if (c->value.forSTATEMENT.type==printT) {
      /* st = st->next; */
      /* typeImplementationPRINT_STMT(c->value.forSTATEMENT.printStmt, st); */
      /* st = st->next; */

      st = st->next;
      typeImplementationPRINT_STMT_LIST(c->value.forSTATEMENT.printStmt, st);
      st = st->next;
    }
    else if (c->value.forSTATEMENT.type==barChartT) {
      st = st->next;
      typeImplementationCOMPUTATION_STMT(c->value.forSTATEMENT.barChartStmt, st);
      st = st->next;
    }
    break;

  case complexForK:
    st = st->next;
    typeImplementationCOMPUTATION_STMT_LIST(c->value.complexForSTATEMENT.stmtList, st);
    st = st->next;
    break;

  case ifK:
    st = st->next;
    typeImplementationCOMPUTATION_STMT_LIST(c->value.ifSTATEMENT.ifComputationStmtList, st);
    st = st->next;
    if (c->value.ifSTATEMENT.type==ifElseT) {
      st = st->next;
      typeImplementationCOMPUTATION_STMT_LIST(c->value.ifSTATEMENT.elseComputationStmtList, st);
      st = st->next;
    }
    break;

  case printK:
    typeImplementationPRINT_STMT_LIST(c->value.printSTATEMENT.printStmt, st);
    break;

  case barChartK:
    s = getSymbol( st, c->value.barChartSTATEMENT.id);
    if (s->type!=TableT) {
      printf("Type error: We may only produce barcharts for tables. %s is not a table.\n", s->name);
      exit(TYPE_ERROR);
    }
    break;

  case declarationK:
    typeImplemenetationDECLARATION(c->value.declarationSTATEMENT.declaration, st);
    break;
  }
}

void typeImplementationCOMPUTATION_STMT_LIST(COMPUTATION_STMT_LIST *l, SymbolTable *st) {
  COMPUTATION_STMT *curNode = l->head;

  while (curNode) {
    /* st = st->next; */
    typeImplementationCOMPUTATION_STMT(curNode, st);
    /* st = st->next; */
      
    if (curNode==l->tail) {
      curNode = NULL;
    }
    else {
      curNode = curNode->next;
    }
  }
}


void typeImplementationSECTION_NODE(SECTION_NODE *s, SymbolTable *st) {
  switch(s->kind) {
  case headerK:
    // Type for headers are already saved into symbol table
    break;

  case useK:
    // Do nothing
    break;

  case groupK:
    typeImplementationGROUP_LIST(s->val.groupList, st);
    break;

  case filterK:
    typeImplementationFILTER_LIST(s->val.filterList, st);
    break;

  case computationK:
    typeImplementationCOMPUTATION_STMT_LIST(s->val.computationList, st);
    break;
  }
}



// Top level type implementation. Should type check the whole program and exit with an error if there is a type error
void typeImplementationPROGRAM_NODE(PROGRAM_NODE *p, SymbolTable *st) {
  SECTION_NODE *curNode;
  curNode = p->opt_sections;
  while (curNode) {

    typeImplementationSECTION_NODE(curNode, st);
    curNode = curNode->next;

  }
}

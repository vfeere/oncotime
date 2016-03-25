#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pretty.h"

FILTERS *refinedFilters;
SymbolTable *st;
int ifSectionTab = 0;

void prettyID_LIST(ID_LIST *l) {
  ID_NODE *cur;

  cur = l->head;

  while (cur!=NULL) {
    fprintf(output, "%s", cur->val);
    cur = cur->next;
    if (cur) fprintf(output, ", ");
  }
}

void prettyINT_LIST(INT_LIST *l) {
  INT_NODE *node = l->head;
  while (node) {
    switch(node->kind) {
      case(rangeK):
        fprintf(output, "%d", node->val.intE);
        break;
      case(intK):
        fprintf(output, "%d to %d", node->val.rangeE.beginning, node->val.rangeE.end);
        break;
    }
    node = node->next;
    if(node) fprintf(output, ", ");
  }
}

void prettyHOUR_LIST(HOUR_LIST *h) {
  HOUR_NODE *node = h->head;
  while(node) {
    switch(node->kind) {
      case(hourK):
        fprintf(output, "%s", node->val.hour);
        break;
      case(hourRangeK):
        fprintf(output, "%s to %s", node->val.rangeH.begin, node->val.rangeH.end);
        break;
    }
    node = node->next;
    if (node) fprintf(output, ", ");
  }
}

void prettyGROUP_LIST(GROUP_LIST *l) {
  GROUP_NODE *cur;

  cur = l->head;

  while (cur != NULL) {
    fprintf(output,"group %s = {", cur->id);
    prettyID_LIST(cur->paramList);
    fprintf(output,"} // type of %s : %s \n", cur->id, symbolTypeToString(getSymbol(st, cur->id)));
    cur = cur->next;
  }
}

/* PRINT FILTER SECTION */

void prettyFILTER_LIST(FILTER_LIST *f) {
  
  FILTER_NODE *node = f->head;
  while (node) {
    switch(node->kind) {
      case(popK):
        fprintf(output, "\n\tpopulation is\n");
        prettyPOPULATION_ATTR_LIST(node->val.popAttrList);
        break;
      case(periodK):
        fprintf(output, "\n\tperiod is\n");
        prettyPERIOD_ATTR_LIST(node->val.periodAttrList);
        break;
      case(eventsK):
        fprintf(output, "\n\tevents are\n\t\t");
        prettyID_LIST(node->val.events);
        break;
    }
    node = node->next;
  }

}

/* MILESTONE 2 -- function used for printing the refined filter list
*/
void prettyRefinedFILTER_LIST() {

  POPULATION_FILTER *population_filter = refinedFilters->populationFilter;
  PERIOD_FILTER *period_filter = refinedFilters->periodFilter;
  EVENT_FILTER *event_filter = refinedFilters->eventFilter;

  // PRINT POPULATION FILTER
  // If empty then print as a comment
  if (!(population_filter->age) && 
    (!(population_filter->gender) && 
    (!(population_filter->birthyear) && 
    (!(population_filter->diagnosis) &&
      (!(population_filter->postalcode) &&
        (!(population_filter->id))))))) {
    fprintf(output, "\n// ");
  } else {
    fprintf(output, "\n");
  }
  fprintf(output, "\tpopulation is\n");
  
  // age attribute
  if (population_filter->age) {
    fprintf(output, "\t\t age : ");
    prettyINT_ATTRIBUTE_VALUES(population_filter->age);
  } else {
    fprintf(output, "\t\t //age : *\n");
  }

  // gender attribute
   if (population_filter->gender) {
    fprintf(output, "\t\t gender : ");
    prettyID_ATTRIBUTE_VALUES(population_filter->gender);
  } else {
    fprintf(output, "\t\t //gender : *\n");
  }

  // birthyear attribute
   if (population_filter->birthyear) {
    fprintf(output, "\t\t birthyear : ");
    prettyINT_ATTRIBUTE_VALUES(population_filter->birthyear);
  } else {
    fprintf(output, "\t\t //birthyear : *\n");
  }

  // diagnosis attribute
   if (population_filter->diagnosis) {
    fprintf(output, "\t\t diagnosis : ");
    prettyID_ATTRIBUTE_VALUES(population_filter->diagnosis);
  } else {
    fprintf(output, "\t\t //diagnosis : *\n");
  }

  // postalcode attribute
   if (population_filter->postalcode) {
    fprintf(output, "\t\t postalcode : ");
    prettyID_ATTRIBUTE_VALUES(population_filter->postalcode);
  } else {
    fprintf(output, "\t\t //postalcode : *\n");
  }

  // id attribute
   if (population_filter->id) {
    fprintf(output, "\t\t id : ");
    prettyINT_ATTRIBUTE_VALUES(population_filter->id);
  } else {
    fprintf(output, "\t\t //id : *\n");
  }

  
  // PRINT PERIOD FILTER
  if (!(period_filter->years) && 
    (!(period_filter->months) && 
    (!(period_filter->start) && 
    (!(period_filter->end) &&
      (!(period_filter->hours) &&
        (!(period_filter->days))))))) {
    fprintf(output, "\n// ");
  }else {
    fprintf(output, "\n");
  }
  fprintf(output, "\t period is\n");

  // years attribute
  if (period_filter->years) {
    fprintf(output, "\t\t years : ");
    prettyINT_ATTRIBUTE_VALUES(period_filter->years);
  } else {
    fprintf(output, "\t\t //years : *\n");
  }

  // months attribute
  if (period_filter->months) {
    fprintf(output, "\t\t months : ");
    prettyINT_ATTRIBUTE_VALUES(period_filter->months);
  } else {
    fprintf(output, "\t\t // months : *\n");
  }

  // start attribute
  if (period_filter->start) {
    fprintf(output, "\t\t start : %d\n", period_filter->start->value.i);
  } else {
    fprintf(output, "\t\t // start : *\n");
  }

  // end attribute
  if (period_filter->end) {
    fprintf(output, "\t\t end : %d\n", period_filter->end->value.i);
  } else {
    fprintf(output, "\t\t // end : *\n");
  }

  // hours attribute
  if (period_filter->hours) {
    fprintf(output, "\t\t hours : ");
    prettyHOUR_ATTRIBUTE_VALUES(period_filter->hours);
  } else {
    fprintf(output, "\t\t // hours : *\n");
  }

  // days attribute
  if (period_filter->days) {
    fprintf(output, "\t\t days : ");
    prettyID_ATTRIBUTE_VALUES(period_filter->days);
  } else {
    fprintf(output, "\t\t // days : *\n");
  }


  // PRINT EVENT FILTER
  if (event_filter->event) {
    fprintf(output, "\n\t events are\n\t\t");
    prettyID_ATTRIBUTE_VALUES(event_filter->event);
  } else {
    fprintf(output, "\n\t //events are *\n");
  }

}

/* Milestone 2 -- function used for printing integer values of an attribute of integer list type
*/
void prettyINT_ATTRIBUTE_VALUES(INT_VALUE *value) {
  while(value) {
    switch(value->kind) {
      case intK:
        fprintf(output, "%d", value->value.i);
        break;
      case rangeK:
        fprintf(output, "%d to %d", value->value.range.first, value->value.range.last);
        break;
    }

    value = value->next;
    if (value) {
      fprintf(output, ", ");
    } else 
      fprintf(output, "\n");
  }

}

/* Milestone 2 - function used for printing identifier values of an attribute of identifier list type
*/
void prettyID_ATTRIBUTE_VALUES(ID_VALUE *value) {
  while(value) {
    fprintf(output, "%s", value->value);
    
    value = value->next;
    if (value) {
      fprintf(output, ", ");
    } else {
      fprintf(output, "\n");
    }
  }
}

/* Milestone 2 - function used for printing identifier values of an attribute of hour list type
*/
void prettyHOUR_ATTRIBUTE_VALUES(HOUR_VALUE *value) {
  while (value) {
    switch(value->type) {
      case hourT:
        fprintf(output, "%s", value->value.hour);
        break;
      case rangeT:
        fprintf(output, "%s to %s", value->value.range.first, value->value.range.last);
        break;
    }

    value = value->next;
    if (value) {
      fprintf(output, ", ");
    } else {
      fprintf(output, "\n");
    }
  }
}



void prettyPOPULATION_ATTR_LIST(POPULATION_ATTR_LIST *list) {
    POPULATION_ATTR_LIST *head = list;
    while (head) {
      switch(head->kind) {
        case(GenderK):
          fprintf(output, "\t\tgender : ");
          prettyID_LIST(head->val.idList);
          break;
        case(PstlCodeK):
          fprintf(output, "\t\tpostalcode : ");
          prettyID_LIST(head->val.idList);
          break;
        case(DiagK):
          fprintf(output, "\t\tdiagnosis : ");
          prettyID_LIST(head->val.idList);
          break;
        case(AgeK):
          fprintf(output, "\t\tage : ");
          prettyINT_LIST(head->val.intList);
          break;
        case(BirthYK):
          fprintf(output, "\t\tbirthyear : ");
          prettyINT_LIST(head->val.intList);
          break;
        case(PopIdK):
          fprintf(output, "\t\tid : ");
          prettyINT_LIST(head->val.intList);
          break;
      }

      fprintf(output, "\n");
      head = head->next;
    }

    fprintf(output, "\n");
}

void prettyPERIOD_ATTR_LIST(PERIOD_ATTR_LIST *list) {
  PERIOD_ATTR_LIST *head = list;
  while(head) {
    switch(head->kind) {
      case(YearK):
        fprintf(output, "\t\tyears : ");
        prettyINT_LIST(head->val.intList);
        break;
      case(MonthK):
        fprintf(output, "\t\tmonths : ");
        prettyINT_LIST(head->val.intList);
        break;
      case(DaysK):
        fprintf(output, "\t\tdays : ");
        prettyID_LIST(head->val.idList);
        break;
      case(HoursK):
        fprintf(output, "\t\thours : ");
        prettyHOUR_LIST(head->val.hList);
        break;
      case(StartK):
        fprintf(output, "\t\tstart : %d", head->val.num);
        break;
      case(EndK):
        fprintf(output, "\t\tend : %d", head->val.num);
    }

    fprintf(output, "\n");
    head = head->next;
  }

  fprintf(output, "\n");

}


/* PRINT COMPUTATION SECTION */

void prettyCOMPUTATION_LIST(COMPUTATION_STMT_LIST *stmts) {
  if (ifSectionTab) fprintf(output, "\t");
  
  COMPUTATION_STMT *stmt = stmts->head;

  while (stmt) {
    switch(stmt->kind) {
    case(forK): {
      SymbolTable *temp = st;
      st = st->next;
      prettyFOR_STMT(stmt);
      st = temp;
      break;
    }
    case(complexForK): {
      SymbolTable *temp = st;
      st = st->next;
      prettyCOMPLEX_FOR_STMT(stmt);
      st = temp;
      break;
    }
    case(ifK): {
      SymbolTable *temp = st;
      st = st->next;
      prettyIF_STMT(stmt);
      st = temp;
      break;
    }
    case(printK):
      prettyPRINT_STMT(stmt->value.printSTATEMENT.printStmt);
      break;
    case(barChartK):
      prettyBARCHART_STMT(stmt);
      break;
    case(declarationK):
      prettyDECLARATION_STMT(stmt);
      break;
    }
    stmt = stmt->next;
  }
}


/* PRINT FOR STATEMENTS */
void prettyFOR_ITEM(FOR_ITEM *forItem, int isComplex) {
  switch(forItem->type) {
    case(patientK):
      fprintf(output,"patient %s ", forItem->value.idSTATEMENT.id);
      if (isComplex) fprintf(output, "{");
      fprintf(output, " // type of %s : %s \n\t", forItem->value.idSTATEMENT.id, symbolTypeToString(getSymbol(st, forItem->value.idSTATEMENT.id)));
      break;
    case(timelineK):
      fprintf(output,"timeline of %s \n\t", forItem->value.idSTATEMENT.id);
      break;
    case(elementK):
      fprintf(output,"element %s of %s ", forItem->value.elementSTATEMENT.id, forItem->value.elementSTATEMENT.table);
      if (isComplex) fprintf(output, "{");
      fprintf(output, " // type of %s : %s  \n\t", forItem->value.elementSTATEMENT.id, symbolTypeToString(getSymbol(st, forItem->value.elementSTATEMENT.id)));
      break;
    case(diagnosisIdK):
      fprintf(output,"diagnosis %s ", forItem->value.idSTATEMENT.id);
      if (isComplex) fprintf(output, "{");
      fprintf(output, " //type of %s : %s \n\t", forItem->value.idSTATEMENT.id, symbolTypeToString(getSymbol(st, forItem->value.idSTATEMENT.id)));
      break;
    case(doctorK):
      fprintf(output,"doctor %s ", forItem->value.idSTATEMENT.id);
      if (isComplex) fprintf(output, "{");
      fprintf(output, " // type of %s : %s\n\t", forItem->value.idSTATEMENT.id, symbolTypeToString(getSymbol(st, forItem->value.idSTATEMENT.id)));
      break;
    case(sequenceK):
      fprintf(output,"sequence %s like [ ",forItem->value.sequenceSTATEMENT.id);
      prettySEQUENCE_LIST(forItem->value.sequenceSTATEMENT.sequenceList);
      fprintf(output,"] ");
      if (isComplex) fprintf(output, "{");
      fprintf(output, " // type of %s : %s \n\t", forItem->value.sequenceSTATEMENT.id, symbolTypeToString(getSymbol(st, forItem->value.sequenceSTATEMENT.id)));
      break;
    case(memberK):
      fprintf(output,"member %s in %s ", forItem->value.memberSTATEMENT.id, forItem->value.memberSTATEMENT.list);
      if (isComplex) fprintf(output, "{");
      fprintf(output, " // type of %s : %s \n\t", forItem->value.memberSTATEMENT.id, symbolTypeToString(getSymbol(st, forItem->value.memberSTATEMENT.id)));
      break;
  }
}


void prettyFOR_STMT(COMPUTATION_STMT *stmt) {
  fprintf(output,"\tforeach ");
  prettyFOR_ITEM(stmt->value.forSTATEMENT.forItem, 0);
  switch(stmt->value.forSTATEMENT.type) {
    case(printT):
      prettyPRINT_STMT(stmt->value.forSTATEMENT.printStmt);
      break;
    case(barChartT):
      prettyBARCHART_STMT(stmt->value.forSTATEMENT.barChartStmt);
      break;
  }
}

void prettyCOMPLEX_FOR_STMT(COMPUTATION_STMT *stmt) {
  fprintf(output,"\tforeach ");
  prettyFOR_ITEM(stmt->value.complexForSTATEMENT.forItem, 1);
  prettyCOMPUTATION_LIST(stmt->value.complexForSTATEMENT.stmtList);
  fprintf(output,"\t}\n");
}

/* PRINT IF STATEMENTS */

void prettyIF_STMT(COMPUTATION_STMT *stmt) {
  fprintf(output,"\tif ");
  prettyEXPRESSION(stmt->value.ifSTATEMENT.ifCondition);
  fprintf(output,"then\n");
  ifSectionTab = 1;
  prettyCOMPUTATION_LIST(stmt->value.ifSTATEMENT.ifComputationStmtList);
  switch(stmt->value.ifSTATEMENT.type) {
    case(ifT):
      break;
    case(ifElseT):
      fprintf(output,"\telse\n");
      prettyCOMPUTATION_LIST(stmt->value.ifSTATEMENT.elseComputationStmtList);
  }
  fprintf(output,"\tendif\n");
  ifSectionTab = 0;
}

void prettyEXPRESSION(EXPR *expr) {
  switch(expr->type) {
    case(ageK):
      fprintf(output,"age %s %d ", expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
      break;
    case(genderK):
      fprintf(output,"gender = %s ", expr->value.compareSTATEMENT.id);
      break;
    case(diagnosisK):
      fprintf(output,"diagnosis = %s ", expr->value.compareSTATEMENT.id);
      break;
    case(birthYearK):
      fprintf(output,"birthyear %s %d ", expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
      break;
    case(postalCodeK):
      fprintf(output,"postalcode = %s ", expr->value.compareSTATEMENT.id);
      break;
    case(idExprK):
      fprintf(output, "id %s %d ", expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
      break;
    case(yearsK):
      fprintf(output, "years %s %d ", expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
      break;
    case(monthsK):
      fprintf(output, "months %s %d ", expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
      break;
    case(daysK):
      fprintf(output, "days %s %s ", expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.id);
      break;
    case(hoursK):
     fprintf(output, "hours %s %s ", expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.id);
      break;
    case(startK):
      fprintf(output, "start %s %s ", expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.id);
      break;
    case(endK):
      fprintf(output, "end %s %s ", expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.id);
      break;
    case(notK):
      fprintf(output,"not ");
      prettyEXPRESSION(expr->value.notSTATEMENT.expr);
  }
}

/* PRINT PRINT STATEMENTS */

void prettyPRINT_STMT(PRINT_STMT *stmt) {

  fprintf(output,"\tprint ");

  while(stmt) {
   switch(stmt->type) {
   case(xOfYK):
     fprintf(output,"%s of %s ", stmt->value.xOfYSTATEMENT.id, stmt->value.xOfYSTATEMENT.element);
     break;
   case(tableElementK):
     fprintf(output,"%s[%s] ", stmt->value.tableElementSTATEMENT.tableName, stmt->value.tableElementSTATEMENT.index);
     break;
   case(tableLengthK):
     fprintf(output,"%s.length ", stmt->value.idSTATEMENT.id);
     break;
   case(identifierK):
     fprintf(output,"%s", stmt->value.idSTATEMENT.id);
     break;
   case(timelinePrintK):
     fprintf(output,"timeline of %s", stmt->value.idSTATEMENT.id);
     break;
   case printStringK:
     fprintf(output, "\"%s\"", stmt->value.stringVal);
     break;

    }

    stmt = stmt->next;
    if (stmt) fprintf(output,", ");
  }

  fprintf(output,"\n");
  
}


/* PRINT BARCHAR STATEMENTS */

void prettyBARCHART_STMT(COMPUTATION_STMT *stmt) {
  fprintf(output,"\tbarchart %s\n", stmt->value.barChartSTATEMENT.id); 
}

/* PRINT DECLARATION STATEMETNS */

void prettyDECLARATION_STMT(COMPUTATION_STMT *stmt) {
  prettyDECLARATION(stmt->value.declarationSTATEMENT.declaration);
}


/* PRINT SECTION NODE */

void prettySECTION_NODE(SECTION_NODE *s) {
  ID_NODE *cur;
  ID_LIST *l;

  switch(s->kind) {
    
  case(headerK):
    fprintf(output,"script %s", s->val.headerE.id);
    if (s->val.headerE.paramList != NULL) {
      fprintf(output,"(");
      prettyID_LIST(s->val.headerE.paramList);
      fprintf(output,")");
    }
    fprintf(output, " // type of %s : %s \n", s->val.headerE.id, symbolTypeToString(getSymbol(st, s->val.headerE.id)));
    fprintf(output, "/* COMMENT */ \n");
    break;

  case(useK):
    break;

  case(groupK):
    fprintf(output, "\n");
    prettyGROUP_LIST(s->val.groupList);
    break;
    
  case(filterK):
    prettyRefinedFILTER_LIST();
    //prettyFILTER_LIST(s->val.filterList);
    break;

  case(computationK):
    fprintf(output, "\n{\n");
    prettyCOMPUTATION_LIST(s->val.computationList);
    fprintf(output, "\n}");
    break; 
  }
}
  /* PRINT PROGRAM */

void prettyPROGRAM(PROGRAM_NODE *program, FILTERS *filters, SymbolTable *symbolTable){
    refinedFilters = filters;
    st = symbolTable;
    prettySECTION_NODE(program->hdr_section);

    // Loop through printing the sections until there are none
    SECTION_NODE *sections = program->opt_sections;

    while (sections) {
      prettySECTION_NODE(sections);
      sections = sections->next;
    }

    return;
}

void prettySEQ_ARGS_LIST(SEQUENCE_ARGS_LIST *l) {
  SEQUENCE_ARGS_NODE *curNode;
  curNode = l->head;

  while (curNode!=NULL) {

    
    /* fprintf(output, "%s ( %s )", curNode->id, curNode->arg);*/
    fprintf(output, "%s (", curNode->id);
    prettyID_LIST(curNode->arg);
    fprintf(output, ")");

    if (curNode == l->tail)
      curNode->next = NULL;
    else
      fprintf(output, ", ");
    curNode = curNode->next;
    
  }
}


void prettySEQUENCE_LIST(SEQUENCE_LIST *s)
{
	switch(s->kind)
	{
	case IdK:
	  /* fprintf(output, " %s(%s) ", s->val.idS.id, s->val.idS.input);*/
	  fprintf(output, "%s (", s->val.idS.id);
	  prettyID_LIST(s->val.idS.input);
	  fprintf(output, ")");
	  break;
	case OrK:
	  /* fprintf(output, " %s(%s) |  %s(%s) ", s->val.orS.lid, s->val.orS.lInput, s->val.orS.rid, s->val.orS.rInput);	 */

	  fprintf(output, " %s(", s->val.orS.lid);
	  prettyID_LIST(s->val.orS.lInput);
	  fprintf(output, ")");

	  fprintf(output, " | %s(", s->val.orS.rid);
	  prettyID_LIST(s->val.orS.rInput);
	  fprintf(output, ")");
	  break;
	case ListK:
	  fprintf(output," { ");
	  prettySEQ_ARGS_LIST(s->val.listS.argsList);
	  /* prettyID_LIST(s->val.listS.idList); */
	  fprintf(output," } ");
	  break;
	case MultListK:
	  fprintf(output," { ");
	  prettySEQ_ARGS_LIST(s->val.listS.argsList);
	  /* prettyID_LIST(s->val.listS.idList); */
	  fprintf(output," } * ");
	  break;
	case ArrowK:
	  prettySEQUENCE_LIST(s->val.arrowS.lSeq);
	  /* fprintf(output," -> %s(%s) ", s->val.arrowS.id, s->val.arrowS.input);*/
	  fprintf(output, " -> %s(", s->val.arrowS.id);
	  prettyID_LIST(s->val.arrowS.input);
	  fprintf(output, ")");

	  break;
	case ArrowOrK:
	  prettySEQUENCE_LIST(s->val.arrowOrS.lSeq);
	  /* fprintf(output," -> %s(%s) | %s(%s) ",s->val.arrowOrS.lID, s->val.arrowOrS.lInput, s->val.arrowOrS.rID, s->val.arrowOrS.rInput); */

	  fprintf(output, " -> %s(", s->val.arrowOrS.lID);
	  prettyID_LIST(s->val.arrowOrS.lInput);
	  fprintf(output, ")");

	  fprintf(output, " | %s(", s->val.arrowOrS.rID);
	  prettyID_LIST(s->val.arrowOrS.rInput);
	  fprintf(output, ")");


	  break;
	case NonTermListK:
	  prettySEQUENCE_LIST(s->val.nonTermListS.lSeq);
	  prettySEQ_ARGS_LIST(s->val.nonTermListS.argsList);
	  break;
	case NonTermMultListK:
	  prettySEQUENCE_LIST(s->val.nonTermMultListS.lSeq);
	  fprintf(output, " { ");
	  prettySEQ_ARGS_LIST(s->val.nonTermListS.argsList);
	  fprintf(output, " } * ");
	  break;
	}
}

void prettyDECLARATION(DECLARATION  *dcl)
{
	switch(dcl->kind)
	{
		case NativeListK:
			fprintf(output,"\tlist %s = native ( ",dcl->val.NatListD.id);
			prettyID_LIST(dcl->val.NatListD.l);
			fprintf(output," ) // type of %s : %s \n", dcl->val.NatListD.id, symbolTypeToString(getSymbol(st, dcl->val.NatListD.id)));
			break;
		case DclListK:
			fprintf(output,"\tlist %s = sequences like [ " ,dcl->val.ListD.id);
			prettySEQUENCE_LIST(dcl->val.ListD.s);
			fprintf(output," ] // type of %s : %s \n", dcl->val.ListD.id, symbolTypeToString(getSymbol(st, dcl->val.ListD.id)));
			break;
		case TableK:
			fprintf(output,"\ttable %s = count patients by %s // type of %s : %s \n",dcl->val.TableD.lID, dcl->val.TableD.rID, dcl->val.TableD.lID, symbolTypeToString(getSymbol(st, dcl->val.TableD.lID)));
			break;
		case NativeTableK:
			fprintf(output,"\ttable %s = native ( ",dcl->val.NatTableD.id);
			prettyID_LIST(dcl->val.NatTableD.l);
			fprintf(output," ) // type of %s : %s \n", dcl->val.NatTableD.id, symbolTypeToString(getSymbol(st, dcl->val.NatTableD.id)));
			break;
	}
}

  char *symbolTypeToString(SYMBOL *symbol) {
    
    if (symbol == NULL) {
      return "";
    }
    
    switch(symbol->type) {
      case PatientT:
        return "patient";
      case DoctorT:
        return "doctor";
      case DiagnosisT:
        return "diagnosis";
      case SequenceT:
        return "sequence";
      case SequenceParamT:
        return "sequence parameter";
      case ListT:
        return "list";
      case ListEltT:
        return "list element";
      case TableT:
        return "table";
      case TableEltT:
        return "table element";
      case GrpT:
        return "group";
      case GrpFileT:
        return "group file";
      case ScriptT:
        return "script";
      case StringT:
        return "string";
      case AttrT:
        return "attribute";
      case IDT:
        return "identifier";
    }
  }

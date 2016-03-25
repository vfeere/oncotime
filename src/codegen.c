#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "symtab.h"
#include <ctype.h>

int curIndentLevel;
int numTmpSeqs;
char *curVar;
char *curIter;

/* void printWithIndent(FILE *output, int level, const char *str) { */
/*   int i; */
  
/*   for (i = 0; i < level; i++) { */
/*     fprintf(output, "\t"); */
/*   } */
/*   fprintf(output, "%s", str); */
  
/*   return;  */
/* } */

char *generateIndentPrefix(int level) {

  if (level==0)
    return "";
  
  char *prefix;
  prefix = (char *)malloc (level * sizeof(char));
  

  strcpy(prefix, "");

  int i;
  for (i = 0; i<level; i++) {
    strcat(prefix, "\t");
  }

  return prefix;
}

void codegenPROGRAM(PROGRAM_NODE *program, FILTERS *filters) {
	curIndentLevel = 0; // Because this is Python, indentation is important - We must keep track of it
	numTmpSeqs = 0;

  // Imports & SQL connection/cursor
  fprintf(output, "from datetime import date\n");
	fprintf(output, "from matplotlib import pyplot as plt\n");
  fprintf(output, "from sqlalchemy import create_engine\n\nengine = create_engine('%s')\n", DB_CREDS);
	// Global Variables - SELECT Queries
	codegenGLOBALS(filters);

	// Method Definition
	codegenHEADER_SECTION(program->hdr_section);
	curIndentLevel++;

	char *pref = generateIndentPrefix(curIndentLevel);

	// Computation Section (if any)
	int hasCompSection = 0;
	SECTION_NODE *optSection = program->opt_sections;
	while (optSection) {
		if (optSection->kind == computationK) {
			codegenCOMPUTATION_STMT_LIST(optSection->val.computationList);
			hasCompSection = 1;
		}
		optSection = optSection->next;
	}
  
  // Close the DB connection at end of function
  fprintf(output, "\tconnection.close()\n");

}

void codegenGLOBALS(FILTERS *filters) {

	// Print all static queries to be executed later in the program
	fprintf(output, "patientQuery = \"%s", patientQuery);
	codegenPatientFilterFieldsToSQL(filters);
	fprintf(output, "\"\ndoctorQuery = \"%s", doctorQuery);
	fprintf(output, "\"\ndiagnosisQuery = \"%s", diagnosisQuery);
	codegenDiagnosisFilterFieldsToSQL(filters);
	fprintf(output, "\"\nCT_SIM_BOOKEDQuery = \"%s", CT_SIM_BOOKEDQuery);
	codegenPeriodFilterFieldsToEventsSQL(filters, CT_SIM_BOOKED);
	fprintf(output, "\"\nCT_SIM_COMPLETEDQuery = \"%s", CT_SIM_COMPLETEDQuery);
	codegenPeriodFilterFieldsToEventsSQL(filters, CT_SIM_COMPLETED);
	fprintf(output, "\"\nREADY_FOR_PHYSICS_QAQuery = \"%s", READY_FOR_PHYSICS_QAQuery);
	codegenPeriodFilterFieldsToEventsSQL(filters, READY_FOR_PHYSICS_QA);
	fprintf(output, "\"\nCONSULT_REFERRAL_RECEIVEDQuery = \"%s", CONSULT_REFERRAL_RECEIVEDQuery);
	codegenPeriodFilterFieldsToEventsSQL(filters, CONSULT_REFERRAL_RECEIVED);
	fprintf(output, "\"\nAPPOINTMENT_SCHEDULEDQuery = \"%s", APPOINTMENT_SCHEDULEDQuery);
	codegenPeriodFilterFieldsToEventsSQL(filters, APPOINTMENT_SCHEDULED);
	fprintf(output, "\"\n\n");
}

void codegenHEADER_SECTION(SECTION_NODE *header) {

 	char *pref = generateIndentPrefix(curIndentLevel);
	fprintf(output, "def %s(", header->val.headerE.id);
	
	// Loop through script params
	ID_LIST* paramList = header->val.headerE.paramList;
	if (paramList) {
		ID_NODE* param = paramList->head;
		while(param) {
			fprintf(output, "%s", param->val);
			param = param->next;
			if (param) fprintf(output, ", ");
		}
	}

	fprintf(output, "): \n");
	fprintf(output, "\tconnection = engine.connect()\n");
}

void codegenCOMPUTATION_STMT_LIST(COMPUTATION_STMT_LIST *comps) {
  COMPUTATION_STMT *s = comps->head;
  PRINT_STMT *p;

  while (s!=NULL) {

    switch (s->kind) {
    case forK:
      codegenFOR_STMT(s);
      break;

    case complexForK:
      codegenCOMPLEX_FOR_STMT(s);
      break;

    case ifK:
      codegenIF_STMT(s);
      break;

    case printK:
      p = s->value.printSTATEMENT.printStmt;
      while (p) {
	     codegenPRINT_STMT(p);
	     p = p->next;
      }
      break;

    case barChartK:
      codegenBARCHART_STMT(s);
      break;

    case declarationK:
      codegenDECLARATION_STMT(s->value.declarationSTATEMENT.declaration);
      break;


    }

    s = s->next;
  }

}

void codegenFOR_STMT(COMPUTATION_STMT *forStmt) {
  char *pref = generateIndentPrefix(curIndentLevel);
  int numEvents, dontPrint = 0;
  FOR_ITEM *forItem = forStmt->value.forSTATEMENT.forItem;


  // All of our information will be stored in some kind of set of dictionaries whose name we'll stick in pyIterable
  char *pyIterable;
  char *pyInstance;

  pyIterable = "";
  pyInstance = "";

  //NOTE: We have to make sure in some way that we never have naming conflicts between python space and Oncotime space

  switch (forItem->type) {
  case patientK:
  	if (!hasAllPatients) {
  		fprintf(output, "%sallPatients = connection.execute(patientQuery).fetchall()\n", pref);
  		hasAllPatients = 1;
  	}
    pyIterable = "allPatients";
    pyInstance = forItem->value.idSTATEMENT.id;
    break;

  case timelineK:
    // REMOVED
    // TODO: I don't understand what this should do
    break;

  case elementK:
    pyIterable = forItem->value.elementSTATEMENT.table;
    pyInstance = forItem->value.elementSTATEMENT.id;
    break;

  case diagnosisIdK:
  	if (!hasAllDiagnosis) {
  		fprintf(output, "%sallDiagnoses = connection.execute(diagnosisQuery).fetchall()\n", pref);
  		hasAllDiagnosis = 1;
  	}
    pyIterable = "allDiagnoses";
    pyInstance = forItem->value.idSTATEMENT.id;
    break;

  case doctorK:
    if (!hasAllDoctors) {
  		fprintf(output, "%sallDoctors = connection.execute(doctorQuery).fetchall()\n", pref);
  		hasAllDoctors = 1;
  	}
    pyIterable = "allDoctors";
    pyInstance = forItem->value.idSTATEMENT.id;
    break;

  case sequenceK:
    numEvents = codegenCreateSequence(NULL, forItem->value.sequenceSTATEMENT.sequenceList, 0)+1;
    
    fprintf(output, "%ss_tmp_%d = []\n", pref, numTmpSeqs);


    codegenSearchForSequence(numTmpSeqs ,forItem->value.sequenceSTATEMENT.sequenceList, 0, numEvents);

    fprintf(output, "%sfor %s in s_tmp_%d:\n",pref, forItem->value.sequenceSTATEMENT.id, numTmpSeqs);

    dontPrint = 1;
    numTmpSeqs++;


    break;

  case memberK:
    pyIterable = forItem->value.memberSTATEMENT.list;
    pyInstance = forItem->value.memberSTATEMENT.id;
    break;
  }

  curVar = pyIterable;
  curIter = pyInstance;

  if (!dontPrint)
    fprintf(output, "%sfor %s in %s:\n", pref, pyInstance, pyIterable);
  curIndentLevel++;

  free(pref);
  pref = generateIndentPrefix(curIndentLevel);


  switch(forStmt->value.forSTATEMENT.type) {
  case printT:
    codegenPRINT_STMT(forStmt->value.forSTATEMENT.printStmt);
    break;

  case barChartT:
    codegenBARCHART_STMT(forStmt->value.forSTATEMENT.barChartStmt);
    break;
  }


  curIndentLevel--;
  
  
}

void codegenCOMPLEX_FOR_STMT(COMPUTATION_STMT *complexForStmt) {
  char *pref = generateIndentPrefix(curIndentLevel);

  FOR_ITEM *forItem = complexForStmt->value.complexForSTATEMENT.forItem;
  int numEvents;

  // All of our information will be stored in some kind of set of dictionaries whose name we'll stick in pyIterable
  char *pyIterable;
  char *pyInstance;


  int dontPrint = 0;

  pyIterable = "";
  pyInstance = "";

  
  switch (forItem->type) {
  case patientK:
  	if (!hasAllPatients) {
  		fprintf(output, "%sallPatients = connection.execute(patientQuery).fetchall()\n", pref);
  		hasAllPatients = 1;
  	}
    pyIterable = "allPatients";
    pyInstance = forItem->value.idSTATEMENT.id;
    break;

  case timelineK:
    // TODO: I don't understand what this should do
    break;

  case elementK:
    pyIterable = forItem->value.elementSTATEMENT.table;
    pyInstance = forItem->value.elementSTATEMENT.id;
    break;

  case diagnosisIdK:
  	if (!hasAllDiagnosis) {
  		fprintf(output, "%sallDiagnoses = connection.execute(diagnosisQuery).fetchall()\n", pref);
  		hasAllDiagnosis = 1;
  	}
    pyIterable = "allDiagnoses";
    pyInstance = forItem->value.idSTATEMENT.id;
    break;

  case doctorK:
  	if (!hasAllDoctors) {
  		fprintf(output, "%sallDoctors = connection.execute(doctorQuery).fetchall()\n", pref);
  		hasAllDoctors = 1;
  	}
    pyIterable = "allDoctors";
    pyInstance = forItem->value.idSTATEMENT.id;
    break;

  case sequenceK:
    numEvents = codegenCreateSequence(NULL, forItem->value.sequenceSTATEMENT.sequenceList, 0)+1;
    
    fprintf(output, "%ss_tmp_%d = []\n", pref, numTmpSeqs);
    
    
    codegenSearchForSequence(numTmpSeqs ,forItem->value.sequenceSTATEMENT.sequenceList, 0, numEvents);
    
    fprintf(output, "%sfor %s in s_tmp_%d:\n",pref, forItem->value.sequenceSTATEMENT.id, numTmpSeqs);
    
    dontPrint = 1;
    numTmpSeqs++;


    break;

  case memberK:

    pyIterable = forItem->value.memberSTATEMENT.list;
    pyInstance = forItem->value.memberSTATEMENT.id;

    
    break;
  }

  curVar = pyIterable;
  curIter = pyInstance;

  if (!dontPrint)
    fprintf(output, "%sfor %s in %s:\n", pref, pyInstance, pyIterable);
  curIndentLevel++;  

  free(pref);
  pref = generateIndentPrefix(curIndentLevel);

  codegenCOMPUTATION_STMT_LIST(complexForStmt->value.complexForSTATEMENT.stmtList);

  curIndentLevel--;
}

void codegenIF_STMT(COMPUTATION_STMT *ifStmt) {
  char *pref = generateIndentPrefix(curIndentLevel);
  EXPR *expr = ifStmt->value.ifSTATEMENT.ifCondition;
  
  switch(ifStmt->value.ifSTATEMENT.type) {
  case ifT:
    fprintf(output, "%sif ", pref);
    codegenIF_EXPR(expr);
    fprintf(output, ":\n");
    curIndentLevel++;
    codegenCOMPUTATION_STMT_LIST(ifStmt->value.ifSTATEMENT.ifComputationStmtList);
    curIndentLevel--;
    break;
  case ifElseT:
    fprintf(output, "%sif ", pref);
    codegenIF_EXPR(expr);
    fprintf(output, ":\n");
    curIndentLevel++;
    codegenCOMPUTATION_STMT_LIST(ifStmt->value.ifSTATEMENT.ifComputationStmtList);
    curIndentLevel--;
    fprintf(output, "%selse:\n", pref);
    curIndentLevel++;
    codegenCOMPUTATION_STMT_LIST(ifStmt->value.ifSTATEMENT.elseComputationStmtList);
    curIndentLevel--;    
    break;
  }
}


void codegenIF_EXPR(EXPR *expr) {

  switch(expr->type) {
  case ageK:
    fprintf(output, "%s['BirthYear'] %s (date.today().year - %d) ", curIter, expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
    break;

  case genderK:
    fprintf(output, "%s['Sex'].lower() == '%s'.lower() ", curIter, expr->value.compareSTATEMENT.id);
    break;

  case diagnosisK:
    fprintf(output, "%s in %s['Description'] ", expr->value.compareSTATEMENT.id, curIter);
    break;

  case birthYearK:
    fprintf(output, "%s['BirthYear'] %s %d ", curIter, expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
    break;

  case postalCodeK:
    fprintf(output, "%s['PostalCode'].lower() == %s.lower()", curIter, expr->value.compareSTATEMENT.id);
    break;

  case idExprK:
  	fprintf(output, "%s['SerNum'] %s %d", curIter, expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
    break;

  case yearsK:
    fprintf(output, "%s['TimeStamp'].date().year %s %d ", curIter, expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
    break;

  case monthsK:
    fprintf(output, "%s['TimeStamp'].date().month %s %d ", curIter, expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
    break;

  case daysK: {

    char *day = expr->value.intCompareSTATEMENT.id;
    if (strcmp(day, "monday")) {
    	fprintf(output, "%s['TimeStamp'].date().isoweekday() %s 1 ", curIter, expr->value.intCompareSTATEMENT.comparator);
    } else if(strcmp(day, "tuesday")) {
    	fprintf(output, "%s['TimeStamp'].date().isoweekday() %s 2 ", curIter, expr->value.intCompareSTATEMENT.comparator);
    } else if (strcmp(day, "wednesday")) {
    	fprintf(output, "%s['TimeStamp'].date().isoweekday() %s 3 ", curIter, expr->value.intCompareSTATEMENT.comparator);
    } else if (strcmp(day, "thursday")) {
    	fprintf(output, "%s['TimeStamp'].date().isoweekday() %s 4 ", curIter, expr->value.intCompareSTATEMENT.comparator);
    } else if (strcmp(day, "friday")) {
    	fprintf(output, "%s['TimeStamp'].date().isoweekday() %s 5 ", curIter, expr->value.intCompareSTATEMENT.comparator);
    } else if (strcmp(day, "saturday")) {
    	fprintf(output, "%s['TimeStamp'].date().isoweekday() %s 6 ", curIter, expr->value.intCompareSTATEMENT.comparator);
    } else if (strcmp(day, "sunday")) {
    	fprintf(output, "%s['TimeStamp'].date().isoweekday() %s 7 ", curIter, expr->value.intCompareSTATEMENT.comparator);
    } else if (strcmp(day, "weekday")) {
    	if (*(expr->value.intCompareSTATEMENT.comparator) == '=') {
    		fprintf(output, "%s['TimeStamp'].date().isoweekday() < 6 ", curIter);
    	} else {
    		fprintf(output, "%s['TimeStamp'].date().isoweekday() > 5 ", curIter);
    	}
    } else {
    	if (*(expr->value.intCompareSTATEMENT.comparator) == '=') {
    		fprintf(output, "%s['TimeStamp'].date().isoweekday() > 5 ", curIter);
    	} else {
    		fprintf(output, "%s['TimeStamp'].date().isoweekday() < 6 ", curIter);
    	}
    }
    break;
	}

  case hoursK: {
  	int hourToCompare = 0;
  	char *c = expr->value.intCompareSTATEMENT.id; 
  	for (int i = 0; i < strlen(c); i++) {
  		if (c[i] == ':') break;
  		hourToCompare = (10 * hourToCompare) + (c[i] - '0');
  	}

    fprintf(output, "%s['TimeStamp'].time().hour %s %d ", curIter, expr->value.intCompareSTATEMENT.comparator, hourToCompare);
    break;

	}
  case startK:
    fprintf(output, "%s['TimeStamp'].date().day %s %d ", curIter, expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
    break;

  case endK:
    fprintf(output, "%s['TimeStamp'].date().day %s %d ", curIter, expr->value.intCompareSTATEMENT.comparator, expr->value.intCompareSTATEMENT.i);
    break;

  case notK:
    fprintf(output, "not (");
    codegenIF_EXPR(expr);
    fprintf(output, ") ");
    break;
  }
}

void codegenPRINT_STMT(PRINT_STMT *printStmt) {
  char *pref = generateIndentPrefix(curIndentLevel);
  ID_LIST *l;

  switch(printStmt->type) {
  case xOfYK:
    switch(printStmt->value.xOfYSTATEMENT.type) {
    case PatientT: {
      char *element = printStmt->value.xOfYSTATEMENT.id;
      char *attribute;

      if (strcmp(element, "Gender") == 0) attribute = "Sex"; 
      if (strcmp(element, "Birthyear") == 0) attribute = "BirthYear"; 
      if (strcmp(element, "Id") == 0) attribute = "SerNum"; 
      if (strcmp(element, "Diagnosis") == 0) attribute = "Description"; 
      if (strcmp(element, "Postalcode") == 0) attribute = "PostalCode"; 
      
       fprintf(output, "%sprint(str(%s['%s']))\n", pref, printStmt->value.xOfYSTATEMENT.element, attribute);
      break;
    }

    case DoctorT:
      fprintf(output, "%sprint(str(%s['%s']))\n", pref, printStmt->value.xOfYSTATEMENT.element, "SerNum");
      break;
      
    default:
      fprintf(stderr, "Abnormal code generation error: Tried to access attribute of non patient/doctor '%s'. This should be caught by the type checker\n", printStmt->value.xOfYSTATEMENT.id);
      exit(-1);
      break;
    }
      
    break;

  case tableElementK: {
    fprintf(output, "%sprint(str(%s))\n", pref, printStmt->value.tableElementSTATEMENT.index);
    break;
  }

  case tableLengthK:
    fprintf(output, "%sprint(str(len(%s)))\n", pref, printStmt->value.idSTATEMENT.id);
    break;

  case identifierK:
    fprintf(output, "%sprint(str(%s))\n", pref, printStmt->value.idSTATEMENT.id);
    break;

  case timelinePrintK:
    l = malloc(sizeof(ID_LIST));

    l->head = malloc(sizeof(ID_NODE));
    l->head->val = printStmt->value.idSTATEMENT.id;
    l->tail = l->head;

    
    fprintf(output, "\n\n%s#Beginning PRINT TIMELINE SECTION\n", pref);
    codegenGetEvents("CT_sim_booked", l, 0);
    codegenGetEvents("CT_sim_completed", l, 1);
    codegenGetEvents("ready_for_physics_QA", l, 2);
    codegenGetEvents("appointment_scheduled", l, 3);
    codegenGetEvents("consult_refferal_received", l, 4);

    fprintf(output, "%ss_tmp_%d = []\n", pref, numTmpSeqs);

    fprintf(output, "%sfor s_0 in S_0:\n", pref);
    fprintf(output, "%s\ts_tmp_%d.append( ('CT_sim_booked', s_0['TimeStamp']) )\n", pref, numTmpSeqs);

    fprintf(output, "%sfor s_1 in S_1:\n", pref);
    fprintf(output, "%s\ts_tmp_%d.append( ('CT_sim_completed', s_1['TimeStamp']) )\n", pref, numTmpSeqs);

    fprintf(output, "%sfor s_2 in S_2:\n", pref);
    fprintf(output, "%s\ts_tmp_%d.append( ('ready_for_physics', s_2['TimeStamp']) )\n", pref, numTmpSeqs);

    fprintf(output, "%sfor s_3 in S_3:\n", pref);
    fprintf(output, "%s\ts_tmp_%d.append( ('appointment_scheduled', s_3['TimeStamp']) )\n", pref, numTmpSeqs);

    fprintf(output, "%sfor s_4 in S_4:\n", pref);
    fprintf(output, "%s\ts_tmp_%d.append( ('consult_referral_received', s_4['TimeStamp']) )\n", pref, numTmpSeqs);


    
    fprintf(output, "%ss_tmp_%d = sorted(s_tmp_%d, key = lambda x : x[1])\n", pref, numTmpSeqs, numTmpSeqs);
    fprintf(output, "%sprint(str(s_tmp_%d))\n", pref, numTmpSeqs);
    numTmpSeqs++;

    fprintf(output, "%s#ending PRINT TIMELINE SECTION\n\n\n", pref);

    break;

  case printStringK:
    fprintf(output, "%sprint(str(%s))\n", pref, printStmt->value.stringVal);
    break;
  }

  free(pref);
}

void codegenBARCHART_STMT(COMPUTATION_STMT *barchartStmt) {
  char *pref = generateIndentPrefix(curIndentLevel);
  fprintf(output,"%sy,x = zip(*%s)\n",pref,barchartStmt->value.barChartSTATEMENT.id);
  fprintf(output, "%stry:\n", pref);
  fprintf(output,"%s\tplt.bar(x,y)\n",pref);
  fprintf(output,"%s\tplt.show()\n",pref);
  fprintf(output, "%sexcept TypeError:\n", pref);
  fprintf(output, "%s\tpass\n", pref);
  free(pref);
}

void codegenGetEvents(char *eventTp, ID_LIST *args, int depthLevel) {
  char *pref = generateIndentPrefix(curIndentLevel);
  char *dId;
  char *pId;
  
  if (strcmp(eventTp, "CT_sim_booked") == 0) {
    pId = args->head->val;


    fprintf(output, "%sS_%d = connection.execute(CT_SIM_BOOKEDQuery + \" AND P.PatientSerNum='\"+str(%s['SerNum'])+\"'\").fetchall()\n", pref, depthLevel, pId);
    
    
    
  }
  
  else if (strcmp(eventTp, "CT_sim_completed") == 0) {
    pId = args->head->val;

    fprintf(output, "%sS_%d = connection.execute(CT_SIM_COMPLETEDQuery+ \" AND P.PatientSerNum='\"+str(%s['SerNum'])+\"'\").fetchall()\n", pref, depthLevel, pId);
    
  }
  
  else if (strcmp(eventTp, "ready_for_physics_QA") == 0) {
    pId = args->head->val;

    fprintf(output, "%sS_%d = connection.execute(READY_FOR_PHYSICS_QAQuery+ \" AND P.PatientSerNum='\"+str(%s['SerNum'])+\"'\").fetchall()\n", pref, depthLevel, pId);

  }
  
  else if (strcmp(eventTp, "appointment_scheduled") == 0) {
    pId = args->head->val;

    fprintf(output, "%sS_%d = connection.execute(APPOINTMENT_SCHEDULEDQuery+ \" AND P.PatientSerNum='\"+str(%s['SerNum'])+\"'\").fetchall()\n", pref, depthLevel, pId);

  }


  else if (strcmp(eventTp, "consult_refferal_received") == 0) {
    pId = args->head->val;

    if (args->head != args->tail) {
      dId = args->tail->val;

  

      fprintf(output, "%sS_%d = connection.execute(CONSULT_REFERRAL_RECEIVEDQuery+ \" AND P.PatientSerNum='\"+str(%s['SerNum'])+\"' AND D.DoctorSerNum = '\"+%s['SerNum']+\"'\").fetchall()\n", pref, depthLevel, pId, dId);
    }
    else {
      fprintf(output, "%sS_%d = connection.execute(CONSULT_REFERRAL_RECEIVEDQuery+ \" AND P.PatientSerNum='\"+str(%s['SerNum'])+\"'\").fetchall()\n", pref, depthLevel, pId);
    }


  }

  

  else {
    fprintf(stderr, "Code generation error: Tried to access invalid event %s.\n", eventTp);
    exit(-1);
  }

  free(pref);

  
}



void codegenSearchForSequence(int numTmpSeqs, SEQUENCE_LIST *sequenceList, int depthCount, int numArrows) {
  char *pref = generateIndentPrefix(curIndentLevel);
  char *eventTp;
  
  int i;
  
    
  switch (sequenceList->kind) {
    
  case IdK:
    fprintf(output, "%sfor s_%d in S_%d:\n", pref, depthCount, depthCount);

    curIndentLevel++;
    /* free(pref); */
    /* pref = generateIndentPrefix(curIndentLevel); */

    /* fprintf(output, "%ss_tmp_%d.append(S_%d[s_%d])\n ", pref, numTmpSeqs, depthCount, depthCount); */

    /* curIndentLevel--; */


    break;

  case OrK:
    fprintf(output, "%sfor s_%d in S_%d:\n", pref, depthCount, depthCount);

    curIndentLevel++;

    
    break;

  case ListK:
    // TODO semantics??
    break;

  case MultListK:
    // TODO semantics??
    break;

  case ArrowK:
    codegenSearchForSequence(numTmpSeqs, sequenceList->val.arrowS.lSeq, depthCount+1, numArrows);
    pref = generateIndentPrefix(curIndentLevel);
    fprintf(output, "%sfor s_%d in S_%d:\n", pref, depthCount, depthCount);
    curIndentLevel++;

    free(pref);
    pref = generateIndentPrefix(curIndentLevel);
    if (depthCount == 0) {

      fprintf(output, "%sif ", pref);
      for (i=numArrows-1; (i-1)>=0; i--) {
	fprintf(output, "(s_%d['TimeStamp'].date() < s_%d['TimeStamp'].date())", i, i-1);
	if (i-1 > 0) {
	  fprintf(output, " and ");
	}
      }
      fprintf(output, ":\n");

      curIndentLevel++;
      free(pref);
      pref = generateIndentPrefix(curIndentLevel);

      fprintf(output, "%ss_tmp_%d.append([", pref, numTmpSeqs);

      for (i=numArrows-1; i>=0; i--) {
	fprintf(output, "s_%d",  i);
	if (i>0) {
	  fprintf(output, ", ");
	}
      }

      fprintf(output, "])\n");

      curIndentLevel = curIndentLevel - (numArrows+1);

      
    }
    break;

  case ArrowOrK:
    codegenSearchForSequence(numTmpSeqs, sequenceList->val.arrowOrS.lSeq, depthCount+1, numArrows);
    pref = generateIndentPrefix(curIndentLevel);
    fprintf(output, "%sfor s_%d in S_%d:\n", pref, depthCount, depthCount);
    curIndentLevel++;

    free(pref);
    pref = generateIndentPrefix(curIndentLevel);
    if (depthCount == 0) {

      fprintf(output, "%sif ", pref);
      for (i=numArrows-1; (i-1)>=0; i--) {
	fprintf(output, "(s_%d['TimeStamp'].date() < s_%d['TimeStamp'].date())", i, i-1);
	if (i-1 > 0) {
	  fprintf(output, " and ");
	}
      }
      fprintf(output, ":\n");

      curIndentLevel++;
      free(pref);
      pref = generateIndentPrefix(curIndentLevel);

      fprintf(output, "%ss_tmp_%d.append([", pref, numTmpSeqs);

      for (i=numArrows-1; i>=0; i--) {
	fprintf(output, "s_%d",  i);
	if (i>0) {
	  fprintf(output, ", ");
	}
      }

      fprintf(output, "])\n");

      curIndentLevel = curIndentLevel - (numArrows+1);

      
    }

    break;

  case NonTermListK:
    // TODO semantics??
    break;

  case NonTermMultListK:
    // TODO semantics??
    break;
      
  }

}

int codegenCreateSequence(char *varName, SEQUENCE_LIST *sequenceList, int depthLevel) {
  char *pref = generateIndentPrefix(curIndentLevel);
  char *eventTp;
  ID_LIST *args;

  int retVal;
  
  switch (sequenceList->kind) {
    
  case IdK:
    eventTp = sequenceList->val.idS.id;
    args = sequenceList->val.idS.input;
    codegenGetEvents(eventTp, args, depthLevel);
    return depthLevel;

    break;

  case OrK:
    /* codegenGetEvents(sequenceList->val.orS.lid); */
    /* codegenGetEvents(sequenceList->val.orS.rid); */
    codegenGetEvents(sequenceList->val.orS.lid, sequenceList->val.orS.lInput, depthLevel);
    codegenGetEvents(sequenceList->val.orS.rid, sequenceList->val.orS.rInput, depthLevel+1000);

    fprintf(output, "%sS_%d = S_%d + S_%d\n", pref, depthLevel, depthLevel, depthLevel+1000);
    return depthLevel;
    
    break;

  case ListK:
    printf("ERROR: Lists of sequences have not been implemented in this version of Oncotime.\n");
    exit(-1);
    // TODO semantics??
    break;

  case MultListK:
    printf("ERROR: Lists of sequences have not been implemented in this version of Oncotime.\n");
    exit(-1);
    
    // TODO semantics??
    break;

  case ArrowK:
    retVal = codegenCreateSequence(NULL, sequenceList->val.arrowS.lSeq, depthLevel+1);
    codegenGetEvents(sequenceList->val.arrowS.id, sequenceList->val.arrowS.input, depthLevel);
    
    return retVal;
    
    break;

  case ArrowOrK:
    retVal = codegenCreateSequence(NULL, sequenceList->val.arrowOrS.lSeq, depthLevel+1);
    codegenGetEvents(sequenceList->val.arrowOrS.lID, sequenceList->val.arrowOrS.lInput, depthLevel);
    codegenGetEvents(sequenceList->val.arrowOrS.rID, sequenceList->val.arrowOrS.rInput, depthLevel+1000);
    
    fprintf(output, "%sS_%d = S_%d + S_%d\n", pref, depthLevel, depthLevel, depthLevel+1000);
    return retVal;
    break;

  case NonTermListK:
    printf("ERROR: Lists of sequences have not been implemented in this version of Oncotime.\n");
    exit(-1);

    // TODO semantics??
    break;

  case NonTermMultListK:
    printf("ERROR: Lists of sequences have not been implemented in this version of Oncotime.\n");
    exit(-1);

    // TODO semantics??
    break;
      
  }
  return -1;
  
}

static char *attributeToSql(const char *str)
{
  if(!strcmp(str,"Age"))
    return "P.DateOfBirth";
  if(!strcmp(str,"Diagnosis"))
    return "D.Description";
  if(!strcmp(str,"Gender"))
    return "P.Sex" ;
  if(!strcmp(str,"PostalCode"))
    return "P.PostalCode";
  if(!strcmp(str,"Id"))
    return "P.PatientSerNum";
  else
    return "";
}

void codegenDECLARATION_STMT(DECLARATION *declStmt){
  
  char *pref = generateIndentPrefix(curIndentLevel);
  ID_NODE *cur;

  int numEvents;
  switch(declStmt->kind) {
  case NativeListK:



    if (declStmt->val.NatListD.l->numEntries<2) {
      printf("Code generation error: Native calls must have at least one parameter\n");
      exit(-1);
    }


    // Native calls are very unsafe. We just assume the user knows what their doing
    // Blindly call a Python function (the first parameter of Native() ) and use any
    // other parameters as parameters for the Python function
    cur = declStmt->val.NatListD.l->head;
    fprintf(output, "%s%s = %s(", pref, declStmt->val.NatListD.id, cur->val );
    cur = cur->next;
    fprintf(output, "%s ", cur->val);

    if (cur == declStmt->val.NatListD.l->tail)
      cur->next = NULL;

    cur = cur->next;
    

    while (cur) {
      fprintf(output, ", %s", cur->val);
      if (cur == declStmt->val.NatListD.l->tail)
	cur->next = NULL;
      cur = cur->next;
    }

    fprintf(output, ")\n");
    

    break;

  case DclListK:
    fprintf(output, "\n\n%s#Beginning list declaration\n", pref); 
    numEvents = codegenCreateSequence(NULL, declStmt->val.ListD.s, 0)+1;
    
    fprintf(output, "%ss_tmp_%d = []\n", pref, numTmpSeqs);


    codegenSearchForSequence(numTmpSeqs ,declStmt->val.ListD.s, 0, numEvents);

    
    fprintf(output, "%s%s = s_tmp_%d\n", pref, declStmt->val.ListD.id, numTmpSeqs);

    numTmpSeqs++;

    fprintf(output, "%s#End of list declaration\n\n\n", pref);

    
    break;

  case NativeTableK:
    if (declStmt->val.NatTableD.l->numEntries<2) {
      printf("Code generation error: Native calls must have at least one parameter\n");
      exit(-1);
    }
    
    
    
    // Exactly the same as Native Table
    cur = declStmt->val.NatTableD.l->head;
    fprintf(output, "%s%s = %s(", pref, declStmt->val.NatTableD.id, cur->val );
    cur = cur->next;
    fprintf(output, "%s ", cur->val);

    if (cur == declStmt->val.NatTableD.l->tail)
      cur->next = NULL;

    cur = cur->next;
    

    while (cur) {
      fprintf(output, ", %s", cur->val);
      if (cur == declStmt->val.NatTableD.l->tail)
	cur->next = NULL;
      cur = cur->next;
    }

    fprintf(output, ")\n");


    break;

  case TableK:
    /* insert COUNT( ... ) after SELECT */
    fprintf(output,"%stableQuery = patientQuery[:6] + ' COUNT(%s) as NumPatients, %s ' + patientQuery[88:] + ' GROUP BY %s ORDER BY NumPatients'\n",
          pref,attributeToSql(declStmt->val.TableD.rID), attributeToSql(declStmt->val.TableD.rID), attributeToSql(declStmt->val.TableD.rID));
    fprintf(output, "%s%s = connection.execute(tableQuery).fetchall()\n",pref,declStmt->val.TableD.lID);
    break;
  }

  free(pref);
}
  
void codegenPatientFilterFieldsToSQL(FILTERS *filters) {

	POPULATION_FILTER *populationFilter = filters->populationFilter;

	// create GENDER constraints
	ID_VALUE* gender = populationFilter->gender;

	int disallowMale = 1;
	int disallowFemale = 1;

	while (gender) {

		if (strcmp(gender->value, "m") == 0 || strcmp(gender->value, "male") == 0) {
			disallowMale = 0;
		} else if(strcmp(gender->value, "f") == 0 || strcmp(gender->value, "female") == 0) {
			disallowFemale = 0;
		}

		gender = gender->next;
	}

	if (disallowFemale && !disallowMale) {
		fprintf(output, " AND Sex=\'Male\'");
	} else if (disallowMale && !disallowFemale) {
		fprintf(output, " AND Sex=\'Female\'");
	} else {
		fprintf(output, " AND (Sex=\'Female\' OR Sex=\'Male\')");
	}

	// create BIRTHYEAR constraints --- attribute in table Patient: DateOfBirth
	INT_VALUE* birthyear = populationFilter->birthyear;

	if (birthyear) {
		fprintf(output, " AND ( ");

		if (birthyear->kind == rangeK) {
			fprintf(output, "(DateOfBirth >= \'%d\' AND DateOfBirth <= \'%d\')", birthyear->value.range.first, birthyear->value.range.last);
		} else {
			fprintf(output, "DateOfBirth = \'%d\'", birthyear->value.i);
		}
	
		birthyear = birthyear->next;

		while (birthyear) {

			if (birthyear->kind == rangeK) {
				fprintf(output, " OR (DateOfBirth >= \'%d\' AND DateOfBirth <= \'%d\')", birthyear->value.range.first, birthyear->value.range.last);
			} else {
				fprintf(output, " OR DateOfBirth = \'%d\'", birthyear->value.i);
			}
	
			birthyear = birthyear->next;
		}

		fprintf(output, " ) ");
	}

	// create DIAGNOSIS constraints -- attribute in Table Diagnosis: DiagnosisId 
	ID_VALUE* diagnosis = populationFilter->diagnosis;

	if (diagnosis) {
		fprintf(output, " AND ( Description LIKE \'%%%s%%\'", replaceQuotationMarks(diagnosis->value));

		diagnosis = diagnosis->next;
		while(diagnosis) {
			fprintf(output, " OR Description LIKE \'%%%s%%\'", replaceQuotationMarks(diagnosis->value));
			diagnosis = diagnosis->next;
		}

		fprintf(output, " ) ");
	}

	// create POSTALCODE constraints --- attribute in Table Patient: PostalCode
	ID_VALUE* postalcode = populationFilter->postalcode;

	if (postalcode) {
		fprintf(output, " AND ( PostalCode=\'%s\'", postalcode->value);

		postalcode = postalcode->next;
		while(diagnosis) {
			fprintf(output, " OR PostalCode=\'%s\'", postalcode->value);
			postalcode = postalcode->next;
		}

		fprintf(output, " ) ");
	}

	// create ID constraints --- attribute in Table Patient: PatientSerNum
	INT_VALUE* id = populationFilter->id;

	if (id) {
		fprintf(output, " AND ( ");

		if (id->kind == rangeK) {
			fprintf(output, "(P.PatientSerNum >= \'%d\' AND P.PatientSerNum <= \'%d\')", id->value.range.first, id->value.range.last);
		} else {
			fprintf(output, "P.PatientSerNum = \'%d\'", id->value.i);
		}
	
		id = id->next;

		while (id) {

			if (id->kind == rangeK) {
				fprintf(output, " OR (P.PatientSerNum >= \'%d\' AND P.PatientSerNum <= \'%d\')", id->value.range.first, id->value.range.last);
			} else {
				fprintf(output, " OR P.PatientSerNum = \'%d\'", id->value.i);
			}
	
			id = id->next;
		}

		fprintf(output, " ) ");
	}
}

char* replaceQuotationMarks(char *diagnosis) {
	char *newDiagnosis = (char*)malloc(sizeof(char) * (strlen(diagnosis) - 2));
	strcpy(newDiagnosis, &diagnosis[1]);
	newDiagnosis[strlen(diagnosis)-2] = '\0';
	return newDiagnosis;
}

void codegenDiagnosisFilterFieldsToSQL(FILTERS *filters) {

	POPULATION_FILTER *populationFilter = filters->populationFilter;
	ID_VALUE* diagnosis = populationFilter->diagnosis;

	if (diagnosis) {
		fprintf(output, " WHERE Description LIKE \'%%%s%%\'", replaceQuotationMarks(diagnosis->value));

		diagnosis = diagnosis->next;
		while(diagnosis) {
			fprintf(output, " OR Description LIKE \'%%%s%%\'", replaceQuotationMarks(diagnosis->value));
			diagnosis = diagnosis->next;
		}
	}
}

/* converts a given string to lower case character */
static void stringToLowerCase(char *str)
{ 
  int i;
  for(i = 0; str[i]; i++)
    str[i] = tolower(str[i]);
}

/* returns the numerical value of a day given it's name starting at sunday = 1 
 * with the key words weekend & weekday returning 9 and 8 */
static int dayOfWeekToNum(char *day)
{
  if(!strcmp(day,"sunday"))
    return 1;
  if(!strcmp(day,"monday"))
    return 2;
  else if(!strcmp(day,"tuesday"))
    return 3;
  else if(!strcmp(day,"wednesday"))
    return 4;
  else if(!strcmp(day,"thursday"))
    return 5;
  else if(!strcmp(day,"friday"))
    return 6;
  else if(!strcmp(day,"saturday"))
    return 7;
  else if(!strcmp(day,"weekday"))
    return 8;
  else if(!strcmp(day,"weekend"))
    return 9;
  else
    return 0;
}

/* returns the numerical value of an hour i.e 13:00->13, 24:32 -> 24 
 * ignoring the minutes/secs */
static char *tokenizeHour(char *hour)
{
  char *tok = strtok(hour,":");
  return tok;
}

/* given and event returns the corresponding timeStamp column */
static const char *getTimeStampFromEvent(EventType event)
{
  const char *events[] = {"ap.LastUpdated", "ap.LastUpdated", "T.CreationDate", "ap.LastUpdated", "T.CreationDate"};
 return events[event]; 
}

void codegenPeriodFilterFieldsToEventsSQL(FILTERS *filters, EventType event)
{
  
  const char *timeStamp = getTimeStampFromEvent(event);

  codegenPatientFilterFieldsToSQL(filters);
  
  PERIOD_FILTER *periodFilter = filters->periodFilter;
  
  INT_VALUE *year = periodFilter->years;
  if(year)
  {
    fprintf(output," AND (");
    
    if(year->kind == rangeK)
      fprintf(output, " YEAR(%s) >= %d AND YEAR(%s) <= %d",timeStamp, year->value.range.first, timeStamp, year->value.range.last);
    else
      fprintf(output, " YEAR(%s) = %d",timeStamp, year->value.i);
    
    year = year->next;
    
    while(year){
      if(year->kind == rangeK)
        fprintf(output," OR (YEAR(%s) >= %d AND YEAR(%s) <= %d )",timeStamp, year->value.range.first, timeStamp, year->value.range.last);
      else
        fprintf(output," OR YEAR(%s) = %d",timeStamp,year->value.i);
      
      year = year->next;
    }

    fprintf(output," )");
  }

  INT_VALUE *month = periodFilter->months;
  if(month)
  {
    fprintf(output, " AND(");
    
    if(month->kind == rangeK)
      fprintf(output," MONTH(%s) >= %d AND MONTH(%s) <= %d",timeStamp, month->value.range.first, timeStamp, month->value.range.last);
    else
      fprintf(output, " MONTH(%s) = %d",timeStamp, month->value.i);
    
    month = month->next;
    
    while(month)
    {
      
      if(month->kind == rangeK)
        fprintf(output," OR (MONTH(%s) >= %d AND MONTH(%s) <= %d )",timeStamp, month->value.range.first,timeStamp, month->value.range.last);
      else 
        fprintf(output, " OR MONTH(%s) = %d ",timeStamp, month->value.i);
      
      month = month->next;
    }
    
    fprintf(output," )");
  }

  ID_VALUE *days = periodFilter->days;
  if(days)
  {
    /* convert day to lowercase */
    stringToLowerCase(days->value);
    fprintf(output, " AND (");
    if(dayOfWeekToNum(days->value) == 9)
      fprintf(output," DAYOFWEEK(%s) = 7 OR DAYOFWEEK(%s) = 1",timeStamp, timeStamp);
    else if(dayOfWeekToNum(days->value) == 8)
      fprintf(output," DAYOFWEEK(%s) != 7 AND DAYOFWEEK(%s) != 1",timeStamp, timeStamp);
    else
      fprintf(output," DAYOFWEEK(%s) = %d",timeStamp, dayOfWeekToNum(days->value));
    
    days = days->next;
    while(days)
    {
      stringToLowerCase(days->value);
      if(dayOfWeekToNum(days->value) == 9)
        fprintf(output," OR (DAYOFWEEK(%s) = 7 OR DAYOFWEEK(%s) = 1)",timeStamp, timeStamp);
      else if(dayOfWeekToNum(days->value) == 8)
        fprintf(output," OR (DAYOFWEEK(%s) != 7 AND DAYOFWEEK(%s) != 1)",timeStamp, timeStamp);
      else
        fprintf(output," OR DAYOFWEEK(%s) = %d",timeStamp, dayOfWeekToNum(days->value));
      days = days->next;
    }
    fprintf(output, " )");
  }
  
  HOUR_VALUE *hour = periodFilter->hours;
  if(hour)
  {
    fprintf(output," AND (");
    
    if(hour->type == rangeT)
      fprintf(output," HOUR(%s) >= %s AND HOUR(%s) <= %s",timeStamp, tokenizeHour(hour->value.range.first), timeStamp, tokenizeHour(hour->value.range.last));
    else
      fprintf(output, " HOUR(%s) = %s",timeStamp, tokenizeHour(hour->value.hour));
    
    hour = hour->next;
    
    while(hour)
    {
      if(hour->type == rangeT)
        fprintf(output," OR (HOUR(%s) >= %s AND HOUR(%s) <= %s )",timeStamp,tokenizeHour(hour->value.range.first),timeStamp,tokenizeHour(hour->value.range.last));
      else
        fprintf(output, " OR HOUR(%s) = %s",timeStamp,tokenizeHour(hour->value.hour));
      
      hour = hour->next;
    }

    fprintf(output," )");
  }

  INT_VALUE *start = periodFilter->start;
  if(start)
    fprintf(output," AND DAYOFMONTH(%s) >= %d",timeStamp,start->value.i);

  INT_VALUE *end = periodFilter->end;
  if(end)
    fprintf(output," AND DAYOFMONTH(%s) <= %d",timeStamp, end->value.i);
  
}



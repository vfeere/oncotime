#ifndef CODEGEN_H
#define CODEGEN_H
#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "filterRefinements.h"
#include "events.h"

extern FILE *output;

/********************/
/* Global Variables */
/********************/

static const char* DB_CREDS = "mysql+mysqlconnector://xxxxx:xxxxx@127.0.0.1/oncodb";

// SQL Queries with empty spaces to fill in with selection constraints as defined by filter fields
static const char *patientQuery = "SELECT P.PatientSerNum as SerNum, DateOfBirth as BirthYear, Sex, PostalCode, Description FROM Patient P, Diagnosis D WHERE P.PatientSerNum=D.PatientSerNum"; 
static const char *doctorQuery = "SELECT DoctorSerNum as SerNum FROM Doctor";
static const char *diagnosisQuery = "SELECT PatientSerNum as SerNum, Description FROM Diagnosis";
static const char *CT_SIM_BOOKEDQuery = "SELECT P.PatientSerNum as SerNum, ap.LastUpdated as TimeStamp from Patient P, Diagnosis D, Appointment ap, Alias a WHERE a.AliasName = \'Ct-sim\' and a.AliasSerNum = ap.AliasSerNum  and P.PatientSerNum = ap.PatientSerNum AND D.PatientSerNum = P.PatientSerNum";
static const char *CT_SIM_COMPLETEDQuery = "SELECT ap.ScheduledStartTime as Start, ap.ScheduledEndTime as End, ap.LastUpdated as TimeStamp,  P.PatientSerNum as SerNum from Patient P,  Diagnosis D, Appointment ap, Alias a WHERE a.AliasName=\'Ct-sim\' and a.AliasSerNum = ap.AliasSerNum and P.PatientSerNum = ap.PatientSerNum and ap.Status = \'Manually Completed\' and D.PatientSerNum = P.PatientSerNum"; 
static const char *READY_FOR_PHYSICS_QAQuery = "SELECT P.PatientSerNum as SerNum, T.CreationDate as TimeStamp, T.DueDateTime, T.CompletionDate, T.LastUpdated, pr.PrioritySerNum from Patient P, Diagnosis D, Task T, Priority pr, Alias a WHERE a.AliasName = \'Ready for Physics QA\' and a.AliasSerNum = T.AliasSerNum and T.PatientSerNum = P.PatientSerNum and pr.PrioritySerNum = T.PrioritySerNum AND D.PatientSerNum = P.PatientSerNum";
static const char *APPOINTMENT_SCHEDULEDQuery = "SELECT P.PatientSerNum as SerNum, pl.PatientLocationID, ap.ScheduledStartTime as \'Scheduled For\', ap.LastUpdated as TimeStamp from Patient P, Diagnosis D, PatientLocation pl, Appointment ap WHERE P.PatientSerNum = ap.PatientSerNum and ap.AppointmentSerNum = pl.AppointmentSerNum and D.PatientSerNum = P.PatientSerNum";
static const char *CONSULT_REFERRAL_RECEIVEDQuery = "SELECT P.PatientSerNum as SerNum, T.CreationDate as TimeStamp from Patient P, Diagnosis D, Task T, Priority PR, Alias a WHERE a.AliasName = \'CRR with Origin\' and a.AliasSerNum = T.AliasSerNum and T.PatientSerNum = P.PatientSerNum and Status= \'Open\' and PR.PrioritySerNum = T.PrioritySerNum and D.PatientSerNum = P.PatientSerNum";

// SQL Query flags
static int hasAllPatients = 0;
static int hasAllDoctors = 0;
static int hasAllDiagnosis = 0;
static int hasAllCT_SIM_BOOKED = 0;
static int hasAllCT_SIM_COMPLETED = 0;
static int hasAllREADY_FOR_PHYSICS_QA = 0;
static int hasAllCONSULT_REFERRAL_RECEIVED = 0;
static int hasAllAPPOINTMENT_SCHEDULED = 0;

/**********************/
/* Code Gen Functions */
/**********************/

/* Functions which take part of the AST of a program, 
store as globals the filters for sql queries 
and outputs python code to a file */

void codegenPROGRAM(PROGRAM_NODE *program, FILTERS *filters);
void codegenGLOBALS(FILTERS *filters);
void codegenHEADER_SECTION(SECTION_NODE *header);
void codegenCOMPUTATION_STMT_LIST(COMPUTATION_STMT_LIST *comps);
void codegenFOR_STMT(COMPUTATION_STMT *forStmt);
void codegenCOMPLEX_FOR_STMT(COMPUTATION_STMT *complexForStmt);
void codegenIF_STMT(COMPUTATION_STMT *ifStmt);
void codegenPRINT_STMT(PRINT_STMT *printStmt);
void codegenBARCHART_STMT(COMPUTATION_STMT *barchartStmt);
void codegenDECLARATION_STMT(DECLARATION *dcl);
void codegenIF_EXPR(EXPR *expr);

/********************/
/* Helper Functions */
/********************/

void codegenPatientFilterFieldsToSQL(FILTERS *filters);
void codegenDiagnosisFilterFieldsToSQL(FILTERS *filters);

void codegenCT_SIM_BOOKEDFilterFieldsToSQL(FILTERS *filters);
void codegenCT_SIM_COMPLETEDFilterFieldsToSQL(FILTERS *filters);
void codegenREADY_FOR_PHYSICS_QAFilterFieldsToSQL(FILTERS *filters);
void codegenAPPOINTMENT_SCHEDULEDFilterFieldsToSQL(FILTERS *filters);
void codegenCONSULT_REFERRAL_RECEIVEDFilterFieldsToSQL(FILTERS *filters);
int codegenCreateSequence(char *varName, SEQUENCE_LIST *sequenceList, int depthLevel);
void codegenSearchForSequence(int numTmpSeqs, SEQUENCE_LIST *sequenceList, int depthCount, int numArrows);
void codegenGetEvents(char *eventTp, ID_LIST *args, int depthLevel);

void codegenPeriodFilterFieldsToEventsSQL(FILTERS *filters, EventType event);
char* replaceQuotationMarks(char* diagnosis);


#endif

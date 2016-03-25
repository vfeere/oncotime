#ifndef TREE_H
#define TREE_H

typedef enum 
{PatientT, DoctorT, DiagnosisT, SequenceT, SequenceParamT,  ListT, ListEltT, TableT, TableEltT, 
  GrpT, GrpFileT, ScriptT, StringT, IDT, AttrT} SymType; 


typedef struct PROGRAM_NODE
{
  enum {normalK, justGroupK} kind;
	struct SECTION_NODE *hdr_section;
	struct DOC_NODE		 	*doc_section;
  struct SECTION_NODE *opt_sections;

  struct GROUP_LIST *group_list;
}PROGRAM_NODE;

typedef struct DOC_NODE
{
	int lineno;
} DOC_NODE;

typedef struct HOUR_NODE {

  enum {hourK, hourRangeK} kind;

  union {
    char *hour;
    struct {char *begin; char *end; } rangeH; 
  } val;

  struct HOUR_NODE *next;
} HOUR_NODE;

typedef struct HOUR_LIST {
  int lineno;
  HOUR_NODE *head;
  HOUR_NODE *tail;
} HOUR_LIST;
  
typedef struct INT_NODE {
  enum {intK, rangeK} kind;

  union {
    int intE;
    struct {int beginning; int end; } rangeE;
  } val;
  struct INT_NODE *next;
} INT_NODE;


typedef struct INT_LIST {
  int lineno;

  INT_NODE *head;
  INT_NODE *tail;
  
} INT_LIST;


typedef struct ID_NODE {
  
  enum {stringK, idK} kind;
  SymType type;
  char *val;
  struct ID_NODE *next;
  
} ID_NODE;


typedef struct ID_LIST {
  int lineno;
  int isUniformType;
  int numEntries;
  SymType type;

  ID_NODE *head;
  ID_NODE *tail;
  
} ID_LIST;

typedef struct GROUP_NODE {
  char *id;
  int lineno;

  ID_LIST *paramList;
  struct GROUP_NODE *next;
  
} GROUP_NODE;


typedef struct GROUP_LIST {
  int lineno;

  GROUP_NODE *head;
  GROUP_NODE *tail;
  
} GROUP_LIST;

typedef struct POPULATION_ATTR_LIST{
	enum {DiagK, GenderK, AgeK,BirthYK, PstlCodeK, PopIdK} kind;
	int lineno;
	union
	{
		INT_LIST *intList;
		ID_LIST  *idList;
	}val;
	struct POPULATION_ATTR_LIST *next;
	
}POPULATION_ATTR_LIST;


typedef struct PERIOD_ATTR_LIST{
	enum {YearK, MonthK, DaysK, HoursK, StartK, EndK} kind;
	int lineno;
	union{
		INT_LIST *intList;
		ID_LIST  *idList;
		HOUR_LIST *hList;
    int  num;
	}val;
	struct PERIOD_ATTR_LIST *next;
}PERIOD_ATTR_LIST;	


typedef struct FILTER_NODE {

  enum {popK, periodK, eventsK} kind;
	int lineno;
  union {
    PERIOD_ATTR_LIST *periodAttrList;
    POPULATION_ATTR_LIST *popAttrList;
    ID_LIST *events;

  } val;

  struct FILTER_NODE *next;
  
}FILTER_NODE;

typedef struct FILTER_LIST {
  int lineno;

  FILTER_NODE *head;
  FILTER_NODE *tail;
  
} FILTER_LIST;


typedef struct COMPUTATION_STMT_LIST {

	struct COMPUTATION_STMT *head;
	struct COMPUTATION_STMT *tail;

} COMPUTATION_STMT_LIST;


typedef struct SECTION_NODE {
  int lineno;
  enum  {headerK, useK, groupK, filterK, computationK} kind;

  union {
    struct {char *id; ID_LIST *paramList; } headerE;
    ID_LIST *groupFiles;
    GROUP_LIST *groupList;
    FILTER_LIST *filterList;
    COMPUTATION_STMT_LIST *computationList;
  } val;

  struct SECTION_NODE *next;
  
} SECTION_NODE;

typedef struct COMPUTATION_STMT {

  int lineno;
	enum {forK, complexForK, ifK, printK, barChartK, declarationK} kind;

	struct COMPUTATION_STMT *next;

	union {
		struct { enum {printT, barChartT} type; struct FOR_ITEM *forItem; struct PRINT_STMT *printStmt; struct COMPUTATION_STMT *barChartStmt; } forSTATEMENT; 
		struct { struct COMPUTATION_STMT_LIST *stmtList; struct FOR_ITEM *forItem; } complexForSTATEMENT;
		struct { struct PRINT_STMT *printStmt; } printSTATEMENT;
		struct { enum {ifElseT, ifT} type; struct COMPUTATION_STMT_LIST *ifComputationStmtList; struct COMPUTATION_STMT_LIST *elseComputationStmtList; struct EXPR *ifCondition; } ifSTATEMENT;
		struct { char *id; } barChartSTATEMENT;
    struct { struct DECLARATION *declaration; } declarationSTATEMENT;
	} value;

} COMPUTATION_STMT;

typedef struct PRINT_STMT {

  enum {xOfYK, tableElementK, tableLengthK, identifierK, timelinePrintK, printStringK} type;

	union {
	  struct { char *tableName; char *index; SymType type; } tableElementSTATEMENT;
	  struct { char *id; SymType type; } idSTATEMENT;
	  struct { char *id; char *element; SymType type;} xOfYSTATEMENT;
	  char *stringVal;
	} value;

	struct PRINT_STMT *next;

} PRINT_STMT;

typedef struct FOR_ITEM {

	enum {patientK, timelineK, elementK, diagnosisIdK, doctorK, sequenceK, memberK} type;

	union {
		struct { char *id; } idSTATEMENT;
		struct { char *table; char* id; } elementSTATEMENT;
		struct { struct SEQUENCE_LIST *sequenceList; char *id; } sequenceSTATEMENT;
		struct { char *list; char *id; } memberSTATEMENT;
	} value;

} FOR_ITEM;

typedef struct EXPR {

	enum { ageK, genderK, diagnosisK, birthYearK, postalCodeK, idExprK, yearsK, monthsK, daysK, hoursK, startK, endK, notK } type;

	union {
	  struct { char *id; } compareSTATEMENT;
		struct { struct EXPR *expr; } notSTATEMENT;
		struct { char *comparator; char *id; int i; } intCompareSTATEMENT;
	} value;

} EXPR;


typedef struct SEQUENCE_ARGS_NODE {

  enum{strK, idSeqK} kind;

  char *id;
  ID_LIST *arg;
    
  struct SEQUENCE_ARGS_NODE *next;
  
} SEQUENCE_ARGS_NODE;

typedef struct SEQUENCE_ARGS_LIST{
  int lineno;
  SEQUENCE_ARGS_NODE *head;
  SEQUENCE_ARGS_NODE *tail;
} SEQUENCE_ARGS_LIST;



typedef struct SEQUENCE_LIST{
	int lineno;
	enum{IdK, OrK, ListK, MultListK, ArrowK, ArrowOrK, NonTermListK, NonTermMultListK} kind;
	union
	{ 	struct { char *id; ID_LIST *input;} idS;
		struct { char *lid; ID_LIST *lInput; char *rid; ID_LIST *rInput;} orS;
		struct { SEQUENCE_ARGS_LIST *argsList;} listS;
		struct { struct SEQUENCE_LIST *lSeq; char *id; ID_LIST *input; } arrowS;
		struct { struct SEQUENCE_LIST *lSeq; char *lID; ID_LIST *lInput; char *rID; ID_LIST *rInput;} arrowOrS;
		struct { struct SEQUENCE_LIST *lSeq; SEQUENCE_ARGS_LIST *argsList;}nonTermListS;
		struct { struct SEQUENCE_LIST *lSeq; SEQUENCE_ARGS_LIST *argsList;}nonTermMultListS;
	}val;
}SEQUENCE_LIST;



typedef struct DECLARATION {
	int lineno;
	enum{ NativeListK, DclListK, NativeTableK, TableK} kind;
	union
	{ struct{ char *id; SEQUENCE_LIST *s; } ListD;
		struct{ char *id; ID_LIST *l; } NatListD;
	  struct{ char *id; ID_LIST *l; } NatTableD;
		struct{ char *lID; char *rID; } TableD;	
	}val;
}DECLARATION;

PROGRAM_NODE *makeProgramNode(SECTION_NODE *hdr, DOC_NODE *doc, SECTION_NODE *opt);
PROGRAM_NODE *makeProgramGroupNode(GROUP_LIST *l);
DOC_NODE		 *makeDocNode(void);
SECTION_NODE *makeSECTION_NODEheader(char *id, ID_LIST *paramList);
SECTION_NODE *makeSECTION_NODEuse(ID_LIST *groupFiles);
SECTION_NODE *makeSECTION_NODEgroup(GROUP_LIST *groupList);
SECTION_NODE *makeSECTION_NODEfilter(FILTER_LIST *filterList);
SECTION_NODE *makeSECTION_NODEcomputation(COMPUTATION_STMT_LIST *stmts);

POPULATION_ATTR_LIST *makeDiagPopulationAttrList(ID_LIST *slist, POPULATION_ATTR_LIST *pnext);
POPULATION_ATTR_LIST *makeGenderPopulationAttrList(ID_LIST *slist, POPULATION_ATTR_LIST *pnext);
POPULATION_ATTR_LIST *makeAgePopulationAttrList(INT_LIST *ilist, POPULATION_ATTR_LIST *pnext);
POPULATION_ATTR_LIST *makeBirthPopulationAttrList(INT_LIST *ilist, POPULATION_ATTR_LIST *pnext);
POPULATION_ATTR_LIST *makePstlPopulationAttrList(ID_LIST *slist, POPULATION_ATTR_LIST *pnext);
POPULATION_ATTR_LIST *makeIdPopulationAttrList(INT_LIST *islist, POPULATION_ATTR_LIST *pnext);

PERIOD_ATTR_LIST *makeYearPeriodAttrList(INT_LIST *ylist, PERIOD_ATTR_LIST *pnext);
PERIOD_ATTR_LIST *makeMonthPeriodAttrList(INT_LIST *mlist, PERIOD_ATTR_LIST *pnext);
PERIOD_ATTR_LIST *makeDaysPeriodAttrList(ID_LIST *dlist, PERIOD_ATTR_LIST *pnext);
PERIOD_ATTR_LIST *makeHoursPeriodAttrList(HOUR_LIST *hlist, PERIOD_ATTR_LIST *pnext);
PERIOD_ATTR_LIST *makeStartPeriodAttrList(int start, PERIOD_ATTR_LIST *pnext);
PERIOD_ATTR_LIST *makeEndPeriodAttrList(int end, PERIOD_ATTR_LIST *pnext);


FILTER_NODE *makePopFilterNode(FILTER_NODE *fnext, POPULATION_ATTR_LIST *popattrl);
FILTER_NODE *makePeriodFilterNode(FILTER_NODE *fnext, PERIOD_ATTR_LIST *periodattrl);
FILTER_NODE *makeEventFilterNode(FILTER_NODE *fnext, ID_LIST *elist);

FILTER_LIST *makeFilterList(FILTER_NODE *head, FILTER_NODE *tail);

FILTER_LIST *initFILTER_LIST();

ID_LIST *initID_LIST();
ID_NODE *createID_NODEstr(char *val);
ID_NODE *createID_NODEid(char *val);

INT_LIST *initINT_LIST();
INT_NODE *createINT_NODEint(int i);
INT_NODE *createINT_NODErange(int beginning, int end);

HOUR_LIST *initHOUR_LIST();
HOUR_NODE *createHOUR_NODE(char *val);
HOUR_NODE *createHOUR_NODErange(char* begin, char* end);

GROUP_LIST *initGROUP_LIST();
GROUP_NODE *createGROUP_NODE(char *val, ID_LIST *l);

COMPUTATION_STMT_LIST *createEmptyCOMPUTATION_STMT_LIST();
COMPUTATION_STMT *createDeclarationCOMPUTATION_STMT(DECLARATION *decl);
COMPUTATION_STMT *createComplexForCOMPUTATION_STMT(COMPUTATION_STMT_LIST *stmtList, FOR_ITEM *forItem);
COMPUTATION_STMT *createForPrintCOMPUTATION_STMT(FOR_ITEM *forItem, PRINT_STMT *printStmt);
COMPUTATION_STMT *createForBarChartCOMPUTATION_STMT(FOR_ITEM *forItem, COMPUTATION_STMT *barChartStmt);
COMPUTATION_STMT *createIfCOMPUTATION_STMT(COMPUTATION_STMT_LIST *ifList, EXPR *ifCondition);
COMPUTATION_STMT *createIfElseCOMPUTATION_STMT(COMPUTATION_STMT_LIST *ifList, COMPUTATION_STMT_LIST *elseList, EXPR *ifCondition);
COMPUTATION_STMT *createPrintCOMPUTATION_STMT(PRINT_STMT *printStmt);
COMPUTATION_STMT *createBarChartCOMPUTATION_STMT(char *id);

FOR_ITEM *createTimelineFOR_ITEM(char *id);
FOR_ITEM *createPatientFOR_ITEM(char *id);
FOR_ITEM *createDoctorFOR_ITEM(char *id);
FOR_ITEM *createDiagnosisFOR_ITEM(char *id);
FOR_ITEM *createElementFOR_ITEM(char *id, char *table);
FOR_ITEM *createMemberFOR_ITEM(char *id, char *list);
FOR_ITEM *createSequenceFOR_ITEM(char *id, SEQUENCE_LIST *sequenceList);

PRINT_STMT *createXOfYPRINT_STMT(char *id, char *element, PRINT_STMT *next);
PRINT_STMT *createTimelinePRINT_STMT(char *id, PRINT_STMT *next);
PRINT_STMT *createIdPRINT_STMT(char *id, PRINT_STMT *next);
PRINT_STMT *createTableLengthPRINT_STMT(char *id, PRINT_STMT *next);
PRINT_STMT *createTableElementPRINT_STMT(char *index, char *id, PRINT_STMT *next);
PRINT_STMT *createStringPRINT_STMT(char *val, PRINT_STMT *next);

EXPR *createAgeIfCondition(int age, char *comparator);
EXPR *createGenderIfCondition(char *gender);
EXPR *createDiagnosisIfCondition(char *diagnosis);
EXPR *createBirthYearIfCondition(int birthYear, char *comparator);
EXPR *createPostalCodeIfCondition(char *postalCode);
EXPR *createIDIfCondition(int id, char *comparator);
EXPR *createYearsIfCondition(int year, char *comparator); 
EXPR *createMonthsIfCondition(int month, char *comparator);
EXPR *createDaysIfCondition(char *day, char *comparator);
EXPR *createHoursIfCondition(char *hour, char *comparator);
EXPR *createStartIfCondition(int start, char *comparator);
EXPR *createEndIfCondition(int end, char *comparator);
EXPR *createNotIfCondition(EXPR *expr);

SEQUENCE_LIST *makeIdSequence(char *id, ID_LIST *input);
SEQUENCE_LIST *makeOrSequence(char *lid, char *rid, ID_LIST *lInput, ID_LIST *rInput);
SEQUENCE_LIST *makeListSequence(SEQUENCE_ARGS_LIST *l);
SEQUENCE_LIST *makeMultListSequence(SEQUENCE_ARGS_LIST *l);
SEQUENCE_LIST *makeArrowSequence(SEQUENCE_LIST *s, char *id, ID_LIST *input);
SEQUENCE_LIST *makeOrArrowSequence(SEQUENCE_LIST *s, char *lID, char *rID, ID_LIST *lInput, ID_LIST *rInput);  
SEQUENCE_LIST *makeNonTermListSequence(SEQUENCE_LIST *s, SEQUENCE_ARGS_LIST *l);
SEQUENCE_LIST *makeNonTermMultListSequence(SEQUENCE_LIST *s, SEQUENCE_ARGS_LIST *l);
SEQUENCE_ARGS_LIST *makeSeqArgList(char *val, ID_LIST *param);

DECLARATION *makeListDeclaration(char *id, SEQUENCE_LIST *s);
DECLARATION *makeNatListDeclaration(char *id, ID_LIST *l);
DECLARATION *makeNatTableDeclaration(char *id, ID_LIST *l);
DECLARATION *makeTableDeclaration(char *lID, char *rID); 

PROGRAM_NODE *parse(void);
PROGRAM_NODE *groupParse(void);
#endif

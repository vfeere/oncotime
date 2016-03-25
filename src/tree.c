extern int line;

#include "tree.h"
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "string.h"


PROGRAM_NODE *makeProgramNode(SECTION_NODE *hdr, DOC_NODE *doc, SECTION_NODE *opt)
{
	PROGRAM_NODE *p = NEW(PROGRAM_NODE);
	p->kind = normalK;
	p->hdr_section = hdr;
	p->doc_section = doc;
	p->opt_sections = opt;
	return p;
}

PROGRAM_NODE *makeProgramGroupNode(GROUP_LIST *l) {
  PROGRAM_NODE *p = NEW(PROGRAM_NODE);
  p->kind = justGroupK;
  p->group_list = l;
  return p;
}

DOC_NODE *makeDocNode(void)
{
	DOC_NODE *d = NEW(DOC_NODE);
	d->lineno = line;
	return d;
}
SECTION_NODE *makeSECTION_NODEheader(char *id, ID_LIST *paramList) {
  SECTION_NODE *s;
  s = NEW(SECTION_NODE);
  s->kind = headerK;
  s->val.headerE.id = id;
  s->val.headerE.paramList = paramList;
  s->lineno = line;
  return s;
}

SECTION_NODE *makeSECTION_NODEuse(ID_LIST *groupFiles) {
  SECTION_NODE *s;
  s = NEW(SECTION_NODE);
  s->kind = useK;
  s->val.groupFiles = groupFiles;
  return s;
}

SECTION_NODE *makeSECTION_NODEgroup(GROUP_LIST *groupList) {
  SECTION_NODE *s;
  s = NEW(SECTION_NODE);
  s->kind = groupK;
  s->val.groupList = groupList;
  s->lineno = line;
  return s;
}

SECTION_NODE *makeSECTION_NODEcomputation(COMPUTATION_STMT_LIST *stmts) {
	SECTION_NODE *s = NEW(SECTION_NODE);
	s->kind = computationK;
	s->val.computationList = stmts;
	return s;
}

SECTION_NODE *makeSECTION_NODEfilter(FILTER_LIST *filterList) {
  SECTION_NODE *s;
  s = NEW(SECTION_NODE);
  s->kind = filterK;
  s->val.filterList = filterList;
  return s;
}


POPULATION_ATTR_LIST *makeDiagPopulationAttrList(ID_LIST *slist, POPULATION_ATTR_LIST *pnext)
{
	POPULATION_ATTR_LIST *p = NEW(POPULATION_ATTR_LIST);
	p->lineno = line;
	p->kind = DiagK;
	p->val.idList = slist;
	p->next = pnext;
	return p;
}

POPULATION_ATTR_LIST *makeGenderPopulationAttrList(ID_LIST *slist, POPULATION_ATTR_LIST *pnext)
{
	POPULATION_ATTR_LIST *p = NEW(POPULATION_ATTR_LIST);
	p->lineno = line;
	p->kind = GenderK;
	p->val.idList = slist;
	p->next = pnext;
	return p;
}

POPULATION_ATTR_LIST *makeBirthPopulationAttrList(INT_LIST *ilist, POPULATION_ATTR_LIST *pnext)
{
	POPULATION_ATTR_LIST *p = NEW(POPULATION_ATTR_LIST);
	p->lineno = line;
	p->kind = BirthYK;
	p->val.intList = ilist;
	p->next = pnext;
	return p;
}

POPULATION_ATTR_LIST *makeAgePopulationAttrList(INT_LIST *ilist, POPULATION_ATTR_LIST *pnext)
{
	POPULATION_ATTR_LIST *p = NEW(POPULATION_ATTR_LIST);
	p->lineno = line;
	p->kind = AgeK;
	p->val.intList = ilist;
	p->next = pnext;
	return p;
}

POPULATION_ATTR_LIST *makeIdPopulationAttrList(INT_LIST *ilist, POPULATION_ATTR_LIST *pnext)
{
	POPULATION_ATTR_LIST *p = NEW(POPULATION_ATTR_LIST);
	p->lineno = line;
	p->kind = PopIdK;
	p->val.intList = ilist;
	p->next = pnext;
	return p;
}

POPULATION_ATTR_LIST *makePstlPopulationAttrList(ID_LIST *slist, POPULATION_ATTR_LIST *pnext)
{
	POPULATION_ATTR_LIST *p = NEW(POPULATION_ATTR_LIST);
	p->lineno = line;
	p->kind = PstlCodeK;
	p->val.idList = slist;
	p->next = pnext;
	return p;
}

PERIOD_ATTR_LIST *makeYearPeriodAttrList(INT_LIST *ylist, PERIOD_ATTR_LIST *pnext)
{
	PERIOD_ATTR_LIST *p = NEW(PERIOD_ATTR_LIST);
	p->kind = YearK;
	p->val.intList = ylist;
	p->lineno = line;
	p->next = pnext;
	return p;
}

PERIOD_ATTR_LIST *makeMonthPeriodAttrList(INT_LIST *mlist, PERIOD_ATTR_LIST *pnext)
{
	PERIOD_ATTR_LIST *p = NEW(PERIOD_ATTR_LIST);
	p->kind = MonthK;
	p->val.intList = mlist;
	p->lineno = line;
	p->next = pnext;
	return p;
}

PERIOD_ATTR_LIST *makeDaysPeriodAttrList(ID_LIST *dlist, PERIOD_ATTR_LIST *pnext)
{
	PERIOD_ATTR_LIST *p = NEW(PERIOD_ATTR_LIST);
	p->kind = DaysK;
	p->val.idList = dlist;
	p->lineno = line;
	p->next = pnext;
	return p;
}


PERIOD_ATTR_LIST *makeHoursPeriodAttrList(HOUR_LIST *hlist, PERIOD_ATTR_LIST *pnext)
{
	PERIOD_ATTR_LIST *p = NEW(PERIOD_ATTR_LIST);
	p->kind = HoursK;
	p->val.hList = hlist;
	p->lineno = line;
	p->next = pnext;
	return p;
}


PERIOD_ATTR_LIST *makeStartPeriodAttrList(int start, PERIOD_ATTR_LIST *pnext)
{
	PERIOD_ATTR_LIST *p = NEW(PERIOD_ATTR_LIST);
	p->kind = StartK;
	p->val.num = start;
	p->lineno = line;
	p->next = pnext;
	return p;
}


PERIOD_ATTR_LIST *makeEndPeriodAttrList(int end, PERIOD_ATTR_LIST *pnext)
{
	PERIOD_ATTR_LIST *p = NEW(PERIOD_ATTR_LIST);
	p->kind = EndK;
	p->val.num = end;
	p->lineno = line;
	p->next = pnext;
	return p;
}


FILTER_NODE *makePopFilterNode(FILTER_NODE *fnext, POPULATION_ATTR_LIST *popattrl)
{
	FILTER_NODE *f = NEW(FILTER_NODE);
	f->lineno = line;
	f->kind = popK;
	f->val.popAttrList = popattrl;
	f->next = fnext;
	return f;
}

FILTER_NODE *makePeriodFilterNode(FILTER_NODE *fnext, PERIOD_ATTR_LIST *periodattrl)
{
	FILTER_NODE *f = NEW(FILTER_NODE);
	f->lineno = line;
	f->kind = periodK;
	f->val.periodAttrList = periodattrl;
	f->next = fnext;
	return f;
}

FILTER_NODE *makeEventFilterNode(FILTER_NODE *fnext, ID_LIST *elist)
{
	FILTER_NODE *f = NEW(FILTER_NODE);
	f->lineno = line;
	f->kind = eventsK;
	f->val.events = elist;
	f->next = fnext;
	return f;
}

FILTER_LIST *makeFilterList(FILTER_NODE *head, FILTER_NODE *tail)
{
	FILTER_LIST *fl = NEW(FILTER_LIST);
	fl->lineno = line;
	fl->head = head;
	fl->tail = tail;
	return fl;
}

FILTER_LIST *initFILTER_LIST() {
  FILTER_LIST *f;
  f = NEW(FILTER_LIST);
  f->head = NULL;
  f->tail = NULL;
  return f;
}



GROUP_LIST *initGROUP_LIST() {
  GROUP_LIST *g;
  g = NEW(GROUP_LIST);
  return g;
}

GROUP_NODE *createGROUP_NODE(char *val, ID_LIST*l) {
  GROUP_NODE *n;
  n = NEW(GROUP_NODE);
  n->paramList = l;
  n->lineno = line;
  n->id = val;
  return n;
}

ID_LIST *initID_LIST() {
  ID_LIST *l;
  l = NEW(ID_LIST);
  l->head = NULL;
  l->tail = NULL;
  l->lineno = line;

  return l;
  
}

INT_LIST *initINT_LIST() {
  INT_LIST *l;
  l = NEW(INT_LIST);
  l->head = NULL;
  l->tail = NULL;

  return l;

}

HOUR_LIST *initHOUR_LIST() {
  HOUR_LIST *l;
  l = NEW(HOUR_LIST);
  l->head = NULL;
  l->tail = NULL;
  return l;
}


ID_NODE *createID_NODEstr(char *val) {
  ID_NODE *n;
  n = NEW(ID_NODE);
  n->kind = stringK;
  n->val = val;
  n->next = NULL;
  return n;
}
ID_NODE *createID_NODEid(char *val) {
  ID_NODE *n;
  n = NEW(ID_NODE);
  n->kind = idK;
  n->val = val;
  n->next = NULL;
  return n;

}


INT_NODE *createINT_NODEint(int i) {
  INT_NODE *n;
  n = NEW(INT_NODE);
  n->kind = intK;
  n->val.intE = i;
  n->next = NULL;
  return n;
}

INT_NODE *createINT_NODErange(int beginning, int end) {
  INT_NODE *n;
  n = NEW(INT_NODE);
  n->kind = rangeK;
  n->val.rangeE.beginning = beginning;
  n->val.rangeE.end = end;
  n->next = NULL;
  return n;
}

HOUR_NODE *createHOUR_NODE(char *val) {
  HOUR_NODE *n;
  n = NEW(HOUR_NODE);
  n->kind = hourK;
  n->val.hour = val;
  n->next = NULL;
  return n;
}

HOUR_NODE *createHOUR_NODErange(char *begin, char *end) {
  HOUR_NODE *n;
  n = NEW(HOUR_NODE);
  n->kind = hourRangeK;
  n->val.rangeH.begin = begin;
  n->val.rangeH.end = end;
  n->next = NULL;
  return n;
}




/* CREATE COMPUTATION STATEMENT LIST */

COMPUTATION_STMT_LIST *createEmptyCOMPUTATION_STMT_LIST() {
	return NEW(COMPUTATION_STMT_LIST);
}

/* CREATE COMPUTATION STATEMENTS */

COMPUTATION_STMT *createDeclarationCOMPUTATION_STMT(DECLARATION *decl) {

	COMPUTATION_STMT *stmt = NEW(COMPUTATION_STMT);
	stmt->lineno = line;
	stmt->kind = declarationK;
	stmt->value.declarationSTATEMENT.declaration = decl;
	return stmt;
}


COMPUTATION_STMT *createComplexForCOMPUTATION_STMT(COMPUTATION_STMT_LIST *stmtList, FOR_ITEM *forItem) {

	COMPUTATION_STMT *stmt = NEW(COMPUTATION_STMT);
	stmt->lineno = line;
	stmt->kind = complexForK;
	stmt->value.complexForSTATEMENT.stmtList = stmtList;
	stmt->value.complexForSTATEMENT.forItem = forItem;	
	return stmt;
}

COMPUTATION_STMT *createForPrintCOMPUTATION_STMT(FOR_ITEM *forItem, PRINT_STMT *printStmt) {

	COMPUTATION_STMT *stmt = NEW(COMPUTATION_STMT);
	stmt->lineno = line;
	stmt->kind = forK;
	stmt->value.forSTATEMENT.type = printT;
	stmt->value.forSTATEMENT.printStmt = printStmt;
	stmt->value.forSTATEMENT.forItem = forItem;	
	return stmt;
}

COMPUTATION_STMT *createForBarChartCOMPUTATION_STMT(FOR_ITEM *forItem, COMPUTATION_STMT *barChartStmt) {
	
	COMPUTATION_STMT *stmt = NEW(COMPUTATION_STMT);
	stmt->lineno = line;
	stmt->kind = forK;
	stmt->value.forSTATEMENT.type = barChartT;
	stmt->value.forSTATEMENT.barChartStmt = barChartStmt;
	stmt->value.forSTATEMENT.forItem = forItem;	
	return stmt;
}

COMPUTATION_STMT *createIfCOMPUTATION_STMT(COMPUTATION_STMT_LIST *ifList, EXPR *ifCondition) {
	
	COMPUTATION_STMT *stmt = NEW(COMPUTATION_STMT);
	stmt->lineno = line;
	stmt->kind = ifK;
	stmt->value.ifSTATEMENT.type = ifT;
	stmt->value.ifSTATEMENT.ifComputationStmtList = ifList;
	stmt->value.ifSTATEMENT.ifCondition = ifCondition;	
	return stmt;
}

COMPUTATION_STMT *createIfElseCOMPUTATION_STMT(COMPUTATION_STMT_LIST *ifList, COMPUTATION_STMT_LIST *elseList, EXPR *ifCondition) {
	
	COMPUTATION_STMT *stmt = NEW(COMPUTATION_STMT);
	stmt->lineno = line;
	stmt->kind = ifK;
	stmt->value.ifSTATEMENT.type = ifElseT;
	stmt->value.ifSTATEMENT.ifComputationStmtList = ifList;
	stmt->value.ifSTATEMENT.elseComputationStmtList = elseList;
	stmt->value.ifSTATEMENT.ifCondition = ifCondition;	
	return stmt;
}

COMPUTATION_STMT *createPrintCOMPUTATION_STMT(PRINT_STMT *printStmt) {
	
	COMPUTATION_STMT *stmt = NEW(COMPUTATION_STMT);
	stmt->lineno = line;
	stmt->kind = printK;
	stmt->value.printSTATEMENT.printStmt = printStmt;
	return stmt;
}

COMPUTATION_STMT *createBarChartCOMPUTATION_STMT(char *id) {

	COMPUTATION_STMT *stmt = NEW(COMPUTATION_STMT);
	stmt->lineno = line;
	stmt->kind = barChartK;
	stmt->value.barChartSTATEMENT.id = id;
	return stmt;
}

/* CREATE FOR ITEM */

FOR_ITEM *createTimelineFOR_ITEM(char *id) {

	FOR_ITEM *item = NEW(FOR_ITEM);
	item->type = timelineK;
	item->value.idSTATEMENT.id = id;
	return item; 
}

FOR_ITEM *createPatientFOR_ITEM(char *id) {
	
	FOR_ITEM *item = NEW(FOR_ITEM);
	item->type = patientK;
	item->value.idSTATEMENT.id = id;
	return item; 
}

FOR_ITEM *createDoctorFOR_ITEM(char *id) {
	
	FOR_ITEM *item = NEW(FOR_ITEM);
	item->type = doctorK;
	item->value.idSTATEMENT.id = id;
	return item; 
}

FOR_ITEM *createDiagnosisFOR_ITEM(char *id) {

	FOR_ITEM *item = NEW(FOR_ITEM);
	item->type = diagnosisIdK;
	item->value.idSTATEMENT.id = id;
	return item; 
}

FOR_ITEM *createElementFOR_ITEM(char *id, char *table) {

	FOR_ITEM *item = NEW(FOR_ITEM);
	item->type = elementK;
	item->value.elementSTATEMENT.id = id;
	item->value.elementSTATEMENT.table = table;
	return item; 
}

FOR_ITEM *createMemberFOR_ITEM(char *id, char *list) {

	FOR_ITEM *item = NEW(FOR_ITEM);
	item->type = memberK;
	item->value.memberSTATEMENT.id = id;
	item->value.memberSTATEMENT.list = list;
	return item;
}

FOR_ITEM *createSequenceFOR_ITEM(char *id, SEQUENCE_LIST *sequenceList) {

	FOR_ITEM *item = NEW(FOR_ITEM);
	item->type = sequenceK;
	item->value.sequenceSTATEMENT.id = id;
	item->value.sequenceSTATEMENT.sequenceList = sequenceList;
	return item;
}

PRINT_STMT *createXOfYPRINT_STMT(char *id, char *element, PRINT_STMT *next) {

	PRINT_STMT *stmt = NEW(PRINT_STMT);
	stmt->type = xOfYK;
	stmt->value.xOfYSTATEMENT.id = id;
	stmt->value.xOfYSTATEMENT.element = element;
	stmt->next = next;
	return stmt;
}

PRINT_STMT *createTimelinePRINT_STMT(char *id, PRINT_STMT *next) {

	PRINT_STMT *stmt = NEW(PRINT_STMT);
	stmt->type = timelinePrintK;
	stmt->value.idSTATEMENT.id = id;
	stmt->next = next;
	return stmt;
}

PRINT_STMT *createIdPRINT_STMT(char *id, PRINT_STMT *next) {
	PRINT_STMT *stmt = NEW(PRINT_STMT);
	stmt->type = identifierK;
	stmt->value.idSTATEMENT.id = id;
	stmt->next = next;
	return stmt;
}

PRINT_STMT *createStringPRINT_STMT(char *val, PRINT_STMT *next) {
  PRINT_STMT *stmt = NEW(PRINT_STMT);
  stmt->type = printStringK;
  stmt->value.stringVal = val;
  stmt->next = next;
  return stmt;
}

PRINT_STMT *createTableLengthPRINT_STMT(char *id, PRINT_STMT *next) {

	PRINT_STMT *stmt = NEW(PRINT_STMT);
	stmt->type = tableLengthK;
	stmt->value.idSTATEMENT.id = id;
	stmt->next = next;
	return stmt;
}

PRINT_STMT *createTableElementPRINT_STMT(char *index, char *id, PRINT_STMT *next) {

	PRINT_STMT *stmt = NEW(PRINT_STMT);
	stmt->type = tableElementK;
	stmt->value.tableElementSTATEMENT.index = index;
	stmt->value.tableElementSTATEMENT.tableName = id;
	stmt->next = next;
	return stmt;
}

/* CREATE IF CONDITIONS */

EXPR *createGenderIfCondition(char *gender) {

	EXPR *expr = NEW(EXPR);
	expr->type = genderK;
	expr->value.compareSTATEMENT.id = gender;
	return expr;
}

EXPR *createAgeIfCondition(int age, char *comparator) {
	
	EXPR *expr = NEW(EXPR);
	expr->type = ageK;
	expr->value.intCompareSTATEMENT.i = age;
	expr->value.intCompareSTATEMENT.comparator = comparator; 
	return expr;
}

EXPR *createDiagnosisIfCondition(char *diagnosis) {

	EXPR *expr = NEW(EXPR);
	expr->type = diagnosisK;
	expr->value.compareSTATEMENT.id = diagnosis;
	return expr;
}

EXPR *createBirthYearIfCondition(int birthYear, char *comparator) {

	EXPR *expr = NEW(EXPR);
	expr->type = birthYearK;
	expr->value.intCompareSTATEMENT.i = birthYear;
	expr->value.intCompareSTATEMENT.comparator = comparator; 
	return expr;
}

EXPR *createPostalCodeIfCondition(char *postalCode) { 

	EXPR *expr = NEW(EXPR);
	expr->type = postalCodeK;
	expr->value.compareSTATEMENT.id = postalCode;
	return expr;
}

EXPR *createIDIfCondition(int id, char *comparator) {

	EXPR *expr = NEW(EXPR);
	expr->type = idExprK;
	expr->value.intCompareSTATEMENT.i = id;
	expr->value.intCompareSTATEMENT.comparator = comparator; 
	return expr;
}

EXPR *createYearsIfCondition(int year, char *comparator) {

	EXPR *expr = NEW(EXPR);
	expr->type = yearsK;
	expr->value.intCompareSTATEMENT.i = year;
	expr->value.intCompareSTATEMENT.comparator = comparator; 
	return expr;
}

EXPR *createMonthsIfCondition(int month, char *comparator) {

	EXPR *expr = NEW(EXPR);
	expr->type = monthsK;
	expr->value.intCompareSTATEMENT.i = month;
	expr->value.intCompareSTATEMENT.comparator = comparator; 
	return expr;
}

EXPR *createDaysIfCondition(char *day, char *comparator) {

	EXPR *expr = NEW(EXPR);
	expr->type = daysK;
	expr->value.intCompareSTATEMENT.id = day;
	expr->value.intCompareSTATEMENT.comparator = comparator; 
	return expr;
}

EXPR *createHoursIfCondition(char *hour, char *comparator) {

	EXPR *expr = NEW(EXPR);
	expr->type = hoursK;
	expr->value.intCompareSTATEMENT.id = hour;
	expr->value.intCompareSTATEMENT.comparator = comparator; 
	return expr;
}

EXPR *createStartIfCondition(int start, char *comparator) {

	EXPR *expr = NEW(EXPR);
	expr->type = startK;
	expr->value.intCompareSTATEMENT.i = start;
	expr->value.intCompareSTATEMENT.comparator = comparator; 
	return expr;
}

EXPR *createEndIfCondition(int end, char *comparator) {

	EXPR *expr = NEW(EXPR);
	expr->type = endK;
	expr->value.intCompareSTATEMENT.i = end;
	expr->value.intCompareSTATEMENT.comparator = comparator; 
	return expr;
}

EXPR *createNotIfCondition(EXPR *expr) {

	EXPR *expression = NEW(EXPR);
	expression->type = notK;
	expression->value.notSTATEMENT.expr = expr;
	return expression;
}

/* CREATE SEQUENCE LIST */

SEQUENCE_LIST *makeIdSequence(char *id, ID_LIST *input)
{
	SEQUENCE_LIST *seq = malloc(sizeof(SEQUENCE_LIST));
	seq->lineno = line;
	seq->kind = IdK;
	seq->val.idS.id = id;
	seq->val.idS.input = input;
	return seq;
}

SEQUENCE_LIST *makeOrSequence(char *lid, char *rid, ID_LIST *lInput, ID_LIST *rInput)
{
	SEQUENCE_LIST *seq = malloc(sizeof(SEQUENCE_LIST));
	seq->lineno = line;
	seq->kind = OrK;
	seq->val.orS.lid = lid;
	seq->val.orS.lInput = lInput;
	seq->val.orS.rid = rid;
	seq->val.orS.rInput = rInput;
	return seq;
}

SEQUENCE_ARGS_LIST *makeSeqArgList(char *val, ID_LIST *param) {
  SEQUENCE_ARGS_LIST *seqArg = malloc(sizeof(SEQUENCE_ARGS_LIST));
  SEQUENCE_ARGS_NODE *seqNode = malloc(sizeof(SEQUENCE_ARGS_NODE));

  seqNode->id = val;
  seqNode->arg = param;

  seqArg->lineno = line;
  seqArg->head = seqNode;
  seqArg->tail = seqNode;
  return seqArg;
}

SEQUENCE_LIST *makeListSequence(SEQUENCE_ARGS_LIST *l)
{
  SEQUENCE_LIST *seq = malloc(sizeof(SEQUENCE_LIST));
	seq->lineno = line;
	seq->kind = ListK;
	seq->val.listS.argsList = l;
	return seq;
}

SEQUENCE_LIST *makeMultListSequence(SEQUENCE_ARGS_LIST *l)
{
	SEQUENCE_LIST *seq = malloc(sizeof(SEQUENCE_LIST));
	seq->lineno = line;
	seq->kind = MultListK;
	seq->val.listS.argsList = l;
	return seq;
}

SEQUENCE_LIST *makeArrowSequence(SEQUENCE_LIST *s, char *id, ID_LIST *input)
{
	SEQUENCE_LIST *seq = malloc(sizeof(SEQUENCE_LIST));
	seq->kind = ArrowK;
	seq->lineno = line;
	seq->val.arrowS.lSeq = s;
	seq->val.arrowS.id = id;
	seq->val.arrowS.input = input;
	return seq;
}

SEQUENCE_LIST *makeOrArrowSequence(SEQUENCE_LIST *s, char *lID, char *rID, ID_LIST *lInput, ID_LIST *rInput)
{
	SEQUENCE_LIST *seq = malloc(sizeof(SEQUENCE_LIST));
	seq->kind = ArrowOrK;
	seq->lineno = line;
	seq->val.arrowOrS.lSeq = s;
	seq->val.arrowOrS.lID = lID;
	seq->val.arrowOrS.lInput = lInput;
	seq->val.arrowOrS.rID = rID;
	seq->val.arrowOrS.rInput = rInput;
	return seq;
}
	

SEQUENCE_LIST *makeNonTermListSequence(SEQUENCE_LIST *s, SEQUENCE_ARGS_LIST *l)
{
	SEQUENCE_LIST *seq = malloc(sizeof(SEQUENCE_LIST));
	seq->lineno = line;
	seq->kind = NonTermListK;
	seq->val.nonTermListS.lSeq = s;
	seq->val.nonTermListS.argsList = l;
	return seq;
}

SEQUENCE_LIST *makeNonTermMultListSequence(SEQUENCE_LIST *s, SEQUENCE_ARGS_LIST *l)
{
	SEQUENCE_LIST *seq = malloc(sizeof(SEQUENCE_LIST));
	seq->lineno = line;
	seq->kind = NonTermMultListK;
	seq->val.nonTermMultListS.lSeq = s;
	seq->val.nonTermMultListS.argsList = l;
	return seq;
}

DECLARATION *makeListDeclaration(char *id, SEQUENCE_LIST *s)
{
	DECLARATION *dcl = malloc(sizeof(DECLARATION));
	dcl->lineno = line;
	dcl->kind = DclListK;
	dcl->val.ListD.id = id;
	dcl->val.ListD.s = s;
	return dcl;
}

DECLARATION *makeNatListDeclaration(char *id, ID_LIST *l)
{
	DECLARATION *dcl = malloc(sizeof(DECLARATION));
	dcl->lineno = line;
	dcl->kind = NativeListK;
 	dcl->val.NatListD.id = id;
	dcl->val.NatListD.l = l;
	return dcl;
}

DECLARATION *makeNatTableDeclaration(char *id, ID_LIST *l)
{
	DECLARATION *dcl = malloc(sizeof(DECLARATION));
	dcl->lineno = line;
	dcl->kind = NativeTableK;
	dcl->val.NatTableD.id = id;
	dcl->val.NatTableD.l = l;
	return dcl;
}

DECLARATION *makeTableDeclaration(char *lID, char *rID)
{
	DECLARATION *dcl = malloc(sizeof(DECLARATION));
	dcl->lineno = line;
	dcl->kind = TableK;
	dcl->val.TableD.lID = lID;
	dcl->val.TableD.rID = rID;
	return dcl;
}

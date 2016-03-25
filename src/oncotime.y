%{
  #include <stdio.h>
  #include <stdlib.h>
  #include "tree.h"
  static PROGRAM_NODE *theprogram; /* the entire program structure */
  static PROGRAM_NODE *thegroup;
  extern int yylex();
  extern char *yytext;
  extern int line;
  int yydebug=1;
  void yyerror(char *error) { printf("Invalid. syntax on line %d  before token %s\n", line, yytext); exit(1);}
%}
			
%union {
    int ival;
    int icomp;
    char *hval;
    char *stringconst;
    struct PROGRAM_NODE *program_node;
    struct ID_LIST *id_list;
    struct INT_LIST *int_list;
    struct HOUR_LIST *hour_list;
    struct GROUP_LIST *group_list;
    struct SECTION_NODE *section_node;
    struct DOC_NODE *doc_node;
    struct COMPUTATION_STMT_LIST *computation_stmt_list;
    struct COMPUTATION_STMT *computation_stmt;
    struct PRINT_STMT *print_stmt;
    struct EXPR *expr;
    struct FOR_ITEM *for_item;
    struct DECLARATION *declaration;
    struct SEQUENCE_LIST *sequence_list;
    struct FILTER_NODE *filter_node;
    struct FILTER_LIST *filter_list;
    struct POPULATION_ATTR_LIST *population_attr_list;
    struct PERIOD_ATTR_LIST *period_attr_list;
    struct SEQUENCE_ARGS_LIST *seq_arg_list;
}			
			
%token <ival> tInt
%token <hval> tHour
%token <stringconst> tIdentifier tExternalFile tString tComparator tEquals			
			
%token IF ELSE ENDIF THEN FOREACH PRINT PATIENTS
%token TIMELINE OF IS EVENTS ARE USE GROUP ELEMENT MEMBER
%token SEQUENCES LIKE LIST NATIVE SCRIPT NOT ARROW POPULATION TO IN
%token PERIOD COMMENT PATIENT TYPE BARCHART DIAGNOSIS DOCTOR TABLE COUNT BY SEQUENCE LENGTH
%token GENDER ID POSTALCODE BIRTHYEAR YEARS MONTHS DAYS HOURS START END AGE

%type <program_node> program
%type <id_list> id_list string_id_list use_section
%type <int_list> int_list
%type <hour_list> hour_list
%type <group_list> group_section
%type <section_node> header_section optional_sections
%type <doc_node> doc_comment
%type <computation_stmt_list> computation_section
%type <computation_stmt> foreach_statement if_statement
%type <print_stmt> printable_statement
%type <expr> expr
%type <for_item> for_item
%type <declaration> declaration_statement
%type <sequence_list> sequence_section
%type <filter_list> filter_section
%type <filter_node> population_list events_list period_list
%type <population_attr_list> population_attribute_filter population_filter_list
%type <period_attr_list> period_attribute_filter period_filter_list
/* %type <stringconst> sequence_input */
%type	<seq_arg_list>	 sequence_list

%start program		
					
%%
program:
		header_section doc_comment optional_sections 							{theprogram = makeProgramNode($1, $2, $3); }
	|	group_section
	{ thegroup = makeProgramGroupNode($1); }
	;


/* ------ Types of Lists (id, string or id, int & hour) ------- */

id_list:
		tIdentifier																{
		    ID_LIST *l = initID_LIST();
		    l->numEntries = 1;
		    l->head = createID_NODEid($1);
		    l->tail = l->head;
		    $$ = l;
		    
		}
																							
																							  
|	id_list ',' tIdentifier 		 										{
    $1->tail->next = createID_NODEid($3);
    $1->tail = $1->tail->next;
    $1->numEntries = $1->numEntries+1;
    $$ = $1;
	}
	;

string_id_list:
		tString																	{ID_LIST *l = initID_LIST();
		    l->head = createID_NODEstr($1);
		    l->tail = l->head;
		    $$ = l;
		}
	|	tIdentifier																{ID_LIST *l = initID_LIST();
		    l->head = createID_NODEid($1);
		    l->tail = l->head;
		    $$ = l;
		}
|	string_id_list ','	tString
		{$1->tail->next = createID_NODEstr($3);
		    $1->tail = $1->tail->next;
		    $$ = $1;
	    }	
	|	string_id_list ',' tIdentifier
		{
		    $1->tail->next = createID_NODEid($3);
		    $1->tail = $1->tail->next;
		    $$ = $1;
		    }
	;

int_list:
		tInt 																	{
		    INT_LIST *l = initINT_LIST();
		    l->head = createINT_NODEint($1);
		    l->tail = l->head;
		    $$ = l;
		}
	|	tInt TO tInt 															{
	    INT_LIST *l = initINT_LIST();
	    l->head = createINT_NODErange($1, $3);
	    l->tail = l->head;
	    $$ = l;
	    }
	|	int_list ',' tInt														{
	    $1 ->tail->next = createINT_NODEint($3);
	    $1->tail = $1->tail->next;
	    $$ = $1;
	    }
	|	int_list ',' tInt TO tInt 	
											{
	    $1 ->tail->next = createINT_NODErange($3, $5);
	    $1->tail = $1->tail->next;
	    $$ = $1;
	    
	    }
	;

hour_list:
		tHour																	{
		HOUR_LIST *l = initHOUR_LIST();
	    l->head = createHOUR_NODE($1);
	    l->tail = l->head;
	    $$ = l;
}		
|	tHour TO tHour															{ HOUR_LIST *l = initHOUR_LIST();
	    l->head = createHOUR_NODErange($1, $3);
	    l->tail = l->head;
	    $$ = l;
	    }

	|	tHour ',' hour_list							      				{
	    HOUR_NODE *node = createHOUR_NODE($1);
	    node->next = $3->head;
	    $3->head = node;
	    $$ = $3;

	    }
	|	tHour TO tHour ',' hour_list											{
	    HOUR_NODE *node = createHOUR_NODErange($1, $3);
	    node->next = $5->head;
	    $5->head = node;
	    $$ = $5;

	    }
	;	


/* ------ 1. Header Section ---- */

header_section:
		SCRIPT tIdentifier 														{$$ = makeSECTION_NODEheader($2,NULL);}
	|	SCRIPT tIdentifier '(' id_list ')' 		 								{$$ = makeSECTION_NODEheader($2,$4); }
	;


/* ------ 2. Documentation Section ----- */

doc_comment:
	COMMENT 																	{$$ = makeDocNode(); }
	;


/* ------- 3. Optional Sections ------- */

optional_sections:
	/* empty */ 																{ $$ = NULL;}
	|	use_section 															{ $$ = makeSECTION_NODEuse($1);}
	|	filter_section 															{ $$ = makeSECTION_NODEfilter($1); }
	|	use_section filter_section 												{ 	SECTION_NODE *use = makeSECTION_NODEuse($1); 
																					SECTION_NODE *filter = makeSECTION_NODEfilter($2); 
																					use->next = filter;
																					$$ = use; }

	|	group_section 															{ $$ = makeSECTION_NODEgroup($1); }
	|	use_section group_section 												{ 	SECTION_NODE *use = makeSECTION_NODEuse($1); 
																					SECTION_NODE *group = makeSECTION_NODEgroup($2); 
																					use->next = group;
																					$$ = use;}

	|	group_section filter_section 											{ 	SECTION_NODE *group = makeSECTION_NODEgroup($1); 
																					SECTION_NODE *filter = makeSECTION_NODEfilter($2); 
																					group->next = filter;
																					$$ = group;}

	|	use_section group_section filter_section 								{ 	SECTION_NODE *use = makeSECTION_NODEuse($1); 
																					SECTION_NODE *group = makeSECTION_NODEgroup($2); 
																					SECTION_NODE *filter = makeSECTION_NODEfilter($3);
																					use->next = group;
																					group-> next = filter;
																					$$ = use;}

	|	'{' computation_section '}' 											{ $$ = makeSECTION_NODEcomputation($2);}
	|	group_section '{' computation_section '}'								{ 	SECTION_NODE *group = makeSECTION_NODEgroup($1); 
																					SECTION_NODE *computation = makeSECTION_NODEcomputation($3); 
																					group->next = computation;
																					$$ = group;}

	|	filter_section '{' computation_section '}' 								{ 	SECTION_NODE *filter = makeSECTION_NODEfilter($1); 
																					SECTION_NODE *computation = makeSECTION_NODEcomputation($3); 
																					filter->next = computation;
																					$$ = filter;}

	|	use_section '{' computation_section '}' 								{ 	SECTION_NODE *use = makeSECTION_NODEuse($1); 
																					SECTION_NODE *computation = makeSECTION_NODEcomputation($3); 
																					use->next = computation;
																					$$ = use;}

	|	use_section filter_section '{' computation_section '}' 					{ 	SECTION_NODE *use = makeSECTION_NODEuse($1); 
																					SECTION_NODE *filter = makeSECTION_NODEfilter($2);
																					SECTION_NODE *computation = makeSECTION_NODEcomputation($4); 
																					use->next = filter;
																					filter->next = computation;
																					$$ = use;}

	|	use_section group_section '{' computation_section '}' 					{ 	SECTION_NODE *use = makeSECTION_NODEuse($1); 
																					SECTION_NODE *group = makeSECTION_NODEgroup($2);
																					SECTION_NODE *computation = makeSECTION_NODEcomputation($4); 
																					use->next = group;
																					group->next = computation;
																					$$ = use;}

	|	group_section filter_section '{' computation_section '}'				{ SECTION_NODE *group = makeSECTION_NODEgroup($1);
	SECTION_NODE *filter = makeSECTION_NODEfilter($2);
	SECTION_NODE *computation = makeSECTION_NODEcomputation($4);
	group->next = filter;
	filter->next = computation;
	$$ = group;	}

	|	use_section group_section filter_section '{' computation_section '}' 	{ 	SECTION_NODE *use = makeSECTION_NODEuse($1); 
																					SECTION_NODE *group = makeSECTION_NODEgroup($2);
																					SECTION_NODE *filter = makeSECTION_NODEfilter($3);
																					SECTION_NODE *computation = makeSECTION_NODEcomputation($5); 
																					use->next = group;
																					group->next = filter;
																					filter->next = computation;
																					$$ = use;}
	;


/* ------ 3.1 Use Section ------ */

use_section:
		USE tExternalFile														{
		    ID_LIST *l = initID_LIST();
		    l->head = createID_NODEid($2);
		    l->tail = l->head;
		    $$ = l;
}
	|	use_section USE tExternalFile
		{
			ID_NODE *id = createID_NODEid($3);
		    $1 ->tail->next = id;
		    $1->tail = id;
		    $$ = $1;
		    }
	;


/* ------ 3.2 Group Section ------ */

group_section:
		GROUP tIdentifier tEquals '{' string_id_list '}'
		{
		    GROUP_LIST *l;
		    l = initGROUP_LIST();
		    l->head = createGROUP_NODE($2, $5);
		    l->tail = l->head;
		    $$ = l;
		    }
	|	group_section GROUP tIdentifier tEquals '{' string_id_list '}' {
	    GROUP_NODE *node = createGROUP_NODE($3, $6);
	    $1->tail->next = node;
	    $1->tail = node;
	    $$ = $1;
	    }
	;


/* ------ 3.3 Filter Section ------ */

filter_section:
		population_list 														{ $$ = makeFilterList($1, $1); }
	|	period_list 															{ $$ = makeFilterList($1, $1);}
	|	events_list 															{ $$ = makeFilterList($1, $1);}
	|	population_list filter_section 											{ $1->next = $2->head; $2->head = $1; $$ = $2; }
	|	period_list filter_section 												{ $1->next = $2->head; $2->head = $1; $$ = $2; }
	|	events_list filter_section												{ $1->next = $2->head; $2->head = $1; $$ = $2; }
	;


/* ------ 3.3.1 Population Filter Section ------ */

population_list:
		    POPULATION IS population_filter_list								{ $$ = makePopFilterNode(NULL, $3);}
	;

population_filter_list:
		population_attribute_filter												{ $$ = $1;}
	|	population_attribute_filter population_filter_list						{ $1->next = $2; $$ = $1;}
	;

population_attribute_filter:
		DIAGNOSIS ':' string_id_list											{ $$ = makeDiagPopulationAttrList($3, NULL);}
	|	GENDER ':' id_list	 													{ $$ = makeGenderPopulationAttrList($3, NULL);}
	|	AGE ':' int_list														{ $$ = makeAgePopulationAttrList($3, NULL);}
	|	BIRTHYEAR ':' int_list 													{ $$ = makeBirthPopulationAttrList($3, NULL); }
	|	POSTALCODE ':' id_list		 											{ $$ = makePstlPopulationAttrList($3, NULL);}
	|	ID ':' int_list 														{ $$ = makeIdPopulationAttrList($3, NULL);}
	;


/* ------ 3.3.2 Period Filter Section ------ */

period_list:
		PERIOD IS period_filter_list											{ $$ = makePeriodFilterNode(NULL, $3); }
	;

period_filter_list:
		period_attribute_filter													{ $$ = $1; }
	|	period_attribute_filter period_filter_list 								{ $1->next = $2; $$ = $1;}
	;

period_attribute_filter:
		YEARS ':' int_list														{ $$ = makeYearPeriodAttrList($3, NULL); }
	|	MONTHS ':' int_list														{ $$ = makeMonthPeriodAttrList($3, NULL); }
	|	DAYS ':' id_list 		 												{ $$ = makeDaysPeriodAttrList($3, NULL); }
	|	HOURS ':' hour_list														{ $$ = makeHoursPeriodAttrList($3, NULL);}
	|	START ':' tInt													{ $$ = makeStartPeriodAttrList($3, NULL);}
	|	END ':'	tInt														{ $$ = makeEndPeriodAttrList($3, NULL);} 
	;

/* ------ 3.3.3 Events Filter Section ------ */

events_list:
		EVENTS ARE string_id_list														{ $$ = makeEventFilterNode(NULL, $3); }
	;


/* ------ 3.4 Computation Section ------ */

computation_section:
		foreach_statement														{ COMPUTATION_STMT_LIST *stmt = createEmptyCOMPUTATION_STMT_LIST(); stmt->head = $1; $$ = stmt;}
	|	foreach_statement computation_section									{ $1->next = $2->head; $2->head = $1; $$ = $2;}
	|	PRINT printable_statement												{ COMPUTATION_STMT_LIST *stmt = createEmptyCOMPUTATION_STMT_LIST(); stmt->head = createPrintCOMPUTATION_STMT($2); $$ = stmt;}
	|	PRINT printable_statement computation_section							{ COMPUTATION_STMT *stmt = createPrintCOMPUTATION_STMT($2); stmt->next = $3->head; $3->head = stmt; $$ = $3; }
	|	BARCHART tIdentifier													{ COMPUTATION_STMT_LIST *stmt = createEmptyCOMPUTATION_STMT_LIST(); stmt->head = createBarChartCOMPUTATION_STMT($2); $$= stmt;}
	|	BARCHART tIdentifier computation_section								{ COMPUTATION_STMT *stmt= createBarChartCOMPUTATION_STMT($2); stmt->next = $3->head; $3->head = stmt; $$ = $3; }
	|	declaration_statement													{ COMPUTATION_STMT_LIST *stmt = createEmptyCOMPUTATION_STMT_LIST(); stmt->head = createDeclarationCOMPUTATION_STMT($1); $$ = stmt;}
	|	declaration_statement computation_section								{ COMPUTATION_STMT *stmt= createDeclarationCOMPUTATION_STMT($1); stmt->next = $2->head; $2->head = stmt; $$ = $2; }
	|	if_statement															{ COMPUTATION_STMT_LIST *stmt = createEmptyCOMPUTATION_STMT_LIST(); stmt->head = $1; $$ = stmt;}
	|	if_statement computation_section										{ $1->next = $2->head; $2->head = $1; $$ = $2;}
    ;

foreach_statement:
		FOREACH for_item '{' computation_section '}'							{ $$ = createComplexForCOMPUTATION_STMT($4, $2); }
	|	FOREACH for_item PRINT printable_statement								{ $$ = createForPrintCOMPUTATION_STMT($2, $4); }
	|	FOREACH for_item BARCHART tIdentifier									{ $$ = createForBarChartCOMPUTATION_STMT($2, createBarChartCOMPUTATION_STMT($4)); }
	;

for_item:
		PATIENT tIdentifier														{ $$ = createPatientFOR_ITEM($2); }
	|	DIAGNOSIS tIdentifier													{ $$ = createDiagnosisFOR_ITEM($2); }
	|	DOCTOR tIdentifier														{ $$ = createDoctorFOR_ITEM($2); }
	|	ELEMENT tIdentifier OF tIdentifier										{ $$ = createElementFOR_ITEM($2, $4); }
	|	SEQUENCE tIdentifier LIKE '[' sequence_section ']'						{ $$ = createSequenceFOR_ITEM($2, $5); }
	|	MEMBER tIdentifier IN tIdentifier										{ $$ = createMemberFOR_ITEM($2, $4); }
	;

printable_statement:
		tIdentifier																{ $$ = createIdPRINT_STMT($1, NULL); }
	|	tIdentifier '[' tIdentifier ']'											{ $$ = createTableElementPRINT_STMT($3, $1, NULL); }
	|	TIMELINE OF tIdentifier													{ $$ = createTimelinePRINT_STMT($3, NULL); }
	|	tIdentifier '.' LENGTH													{ $$ = createTableLengthPRINT_STMT($1, NULL); }
	|	tIdentifier OF tIdentifier												{ $$ = createXOfYPRINT_STMT($1, $3, NULL); }
	|	tIdentifier '[' tIdentifier ']'	',' printable_statement 				{ $$ = createTableElementPRINT_STMT($3, $1, $6); }
	|	tIdentifier '.' LENGTH ',' printable_statement							{ $$ = createTableLengthPRINT_STMT($1, $5); }
	|	tString
	{$$ = createStringPRINT_STMT($1, NULL);}
	|	tString printable_statement
	{$$ = createStringPRINT_STMT($1, $2);}
	;

if_statement:
 		IF expr THEN computation_section ENDIF									{ $$ = createIfCOMPUTATION_STMT($4, $2); }
 	|	IF expr THEN computation_section ELSE computation_section ENDIF			{ $$ = createIfElseCOMPUTATION_STMT($4, $6, $2); }
 	;

expr:
 		GENDER tEquals tIdentifier													{ $$ = createGenderIfCondition($3); }
 	|	AGE tComparator tInt													{ $$ = createAgeIfCondition($3, $2);}	
 	|	AGE tEquals tInt													{ $$ = createAgeIfCondition($3, $2); }	
 	|	DIAGNOSIS tEquals tIdentifier												{ $$ = createDiagnosisIfCondition($3); }
 	|	DIAGNOSIS tEquals tString													{ $$ = createDiagnosisIfCondition($3); }
 	|	BIRTHYEAR tComparator tInt											{ $$ = createBirthYearIfCondition($3, $2); }
 	|	BIRTHYEAR tEquals tInt													{ $$ = createBirthYearIfCondition($3, $2); }	 	
  	|	POSTALCODE tEquals tIdentifier												{ $$ = createPostalCodeIfCondition($3); }
  	|	ID tComparator tInt 													{ $$ = createIDIfCondition($3, $2); }
  	|	ID tEquals tInt 													{ $$ = createIDIfCondition($3, $2); }
  	|	YEARS tComparator tInt 												{ $$ = createYearsIfCondition($3, $2); }
  	|	YEARS tEquals tInt 												{ $$ = createYearsIfCondition($3, $2); }
  	|	MONTHS tComparator tInt 											{ $$ = createMonthsIfCondition($3, $2); }
  	|	MONTHS tEquals tInt 											{ $$ = createMonthsIfCondition($3, $2); }
  	|	DAYS tComparator tIdentifier 										{ $$ = createDaysIfCondition($3, $2); }
  	|	DAYS tEquals tIdentifier 										{ $$ = createDaysIfCondition($3, $2); }
  	|	HOURS tComparator tHour												{ $$ = createHoursIfCondition($3, $2); }
  	|	HOURS tEquals tHour												{ $$ = createHoursIfCondition($3, $2); }
  	|	START tComparator tInt										{ $$ = createStartIfCondition($3, $2); }
    |	START tEquals tInt										{ $$ = createStartIfCondition($3, $2); }
  	|	END tComparator tInt											{ $$ = createEndIfCondition($3, $2); }
  	|	END tEquals tInt											{ $$ = createEndIfCondition($3, $2); }
 	| 	NOT expr																{ $$ = createNotIfCondition($2); }
	;

declaration_statement:
		LIST tIdentifier tEquals SEQUENCES LIKE '[' sequence_section ']' 			{$$ = makeListDeclaration($2, $7); }
	|	LIST tIdentifier tEquals NATIVE '(' id_list ')'								{$$ = makeNatListDeclaration($2,$6); }
	|	TABLE tIdentifier tEquals NATIVE '(' id_list ')'							{$$ = makeNatTableDeclaration($2, $6); }
	|	TABLE tIdentifier tEquals COUNT PATIENTS BY tIdentifier						{$$ = makeTableDeclaration($2, $7); }
    ;



sequence_list:
		tIdentifier '(' id_list ')'
		{ $$ = makeSeqArgList($1, $3); }
	|	sequence_list ',' tIdentifier '(' id_list ')'
	{ SEQUENCE_ARGS_LIST *l; l = makeSeqArgList($3, $5); $1->tail->next = l->head; $1->tail = l->tail; $$ = $1; }
	;


sequence_section:	
		tIdentifier '(' id_list ')'											{ $$ = makeIdSequence($1, $3);}	
	|	tIdentifier '(' id_list ')' '|' tIdentifier '(' id_list ')'	{ $$ = makeOrSequence($1, $6, $3, $8);}
	|	'{' sequence_list '}'																{ $$ = makeListSequence($2);}
	|	'{' sequence_list '}' '*'															{ $$ = makeMultListSequence($2);}
	|	sequence_section ARROW tIdentifier '(' id_list ')'					{ $$ = makeArrowSequence($1, $3, $5);}
	|	sequence_section ARROW tIdentifier '(' id_list ')' '|' tIdentifier '(' id_list ')' { $$ = makeOrArrowSequence($1, $3, $8, $5, $10);} 
	|	sequence_section '{' sequence_list '}' '*'										{ $$ = makeNonTermMultListSequence($1,$3);}
	|	sequence_section '{' sequence_list '}' 											{ $$ = makeNonTermListSequence($1, $3);}
	;

/* sequence_input: */
/* 		tIdentifier																{$$ = $1;} */
/* 	|	tString																	{$$ = $1;} */
/* 	; */
		
%%
PROGRAM_NODE *parse(void)
{	yyparse();
		return theprogram;
}

PROGRAM_NODE *groupParse(void)
{
    yyparse();
    return thegroup;
}


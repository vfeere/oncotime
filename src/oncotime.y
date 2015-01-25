%{
  #include <stdio.h>
  extern char *yytext;
  extern int line;
  int yydebug=1;
  void yyerror() { printf("Invalid. syntax on line %d  before token %s\n", line, yytext); }
%}
			
%union {
    int ival;
    float fval;
    char *stringconst;
}			
			
%token <ival> tInt	
%token <fval> tFloat	
%token <stringconst> tIdentifier
%token <stringconst>	tExternalFile
%token	<stringconst>	tString
			
			
%token IF ELSE ENDIF THEN FOREACH PRINT PATIENTS
%token TIMELINE OF IS EVENTS ARE USE GROUP BARCHART ELEMENT
%token SEQUENCES LIKE LIST NATIVE SCRIPT NOT ARROW POPULATION TO
%token PERIOD COMMENT PATIENT TYPE FUNC

			
%start program		
			
%left '+' '-'		
%left '*' '/'		
%left UMINUS
			
			
			
%%


 // Program marks the beginning of an OncoTime program

    

program:
	    %empty { printf("Valid\n"); }
	|	header_section {printf("Valid\n");}
	|	header_section use_section {printf("Valid\n");}
	|	header_section filter_section {printf("Valid\n");}
	|	header_section use_section filter_section {printf("Valid\n");}
	|	header_section group_section {printf("Valid\n");}
	|	header_section use_section group_section {printf("Valid\n");}
	|	header_section group_section filter_section {printf("Valid\n");}
	|	header_section use_section group_section filter_section {printf("Valid\n");}
	|	header_section computation_section {printf("Valid\n");}
	|	header_section group_section computation_section {printf("Valid\n");}
	|	header_section filter_section computation_section {printf("Valid\n");}
	|	header_section use_section computation_section {printf("Valid\n");}
	|	header_section use_section filter_section computation_section {printf("Valid\n");}
	|	header_section use_section group_section computation_section {printf("Valid\n");}
	|	header_section use_section group_section filter_section computation_section {printf("Valid\n");}
	;

//List of arguments separated by commas. Used universally
id_arg_list:
		tIdentifier
	|	id_arg_list ',' tIdentifier
	;

string_arg_list:
		tString
	|	tIdentifier
	|	string_arg_list ',' tString
	|	string_arg_list ',' tIdentifier
	;

//List of arguments for a filter
filter_arg_list:
		filter_arg
	|	filter_arg_list ',' filter_arg
	|	filter_arg TO filter_arg
	;


filter_arg:
		tIdentifier
	|	tInt
	;

//Associative list for filters
filter_list:
		filter_statement
	|	filter_list filter_statement
	;

filter_statement:
		tIdentifier ':' filter_arg_list
	|	tIdentifier ':' '*'




//Header 
header_section:
		header 
	;

header:
		SCRIPT tIdentifier doc_comment
	|	SCRIPT tIdentifier '(' id_arg_list ')' doc_comment
	;



//TODO - Every program must have a multiline documentation comment
doc_comment:
	COMMENT
	;

//Section of the program that includes external files
use_section:
		USE tExternalFile
	|	use_section USE tExternalFile 
		
	;



//Group definitions
group_section:
		GROUP tIdentifier '=' '{' string_arg_list '}'
	|	group_section GROUP tIdentifier '=' '{' string_arg_list '}'
	;

//Filter out data that we are interested in
filter_section:
		population_list period_list events_list
	|	population_list
	|	period_list
	|	events_list
	|	population_list period_list
	|	period_list events_list
	;

population_list:
		POPULATION IS filter_list
	;

period_list:
		PERIOD IS filter_list
	;
events_list:
		EVENTS ARE id_arg_list
	;



//Computations that we wish to do
computation_section:
		PRINT tIdentifier
	|	foreach_statement
	|	declaration_statement
	|	computation_section PRINT tIdentifier
	|	computation_section foreach_statement
	|	computation_section declaration_statement
       	;
		

/* computation_section: */
/* 		computation_statement */
/* 	|	computation_section computation_section */
/* 	; */

/* computation_statement: */
/* 		PRINT tIdentifier */
	/* 	foreach_statement */
	/* |	PRINT tIdentifier */
	/* ; */

foreach_statement:
		FOREACH for_item FUNC tIdentifier
	|	FOREACH for_item '{' computation_section '}'
	;

for_item:
		PATIENT tIdentifier
	|	tIdentifier
	;

declaration_statement:
		TYPE tIdentifier '=' declarable_item
        ;

declarable_item:
		tIdentifier



/* numeric:	 */
/*     tInt */

/* floatNum: */
/*     tFloat */






%%

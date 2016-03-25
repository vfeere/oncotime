%{
#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include "pretty.h"

extern void yyerror(char *error);
extern int line;
extern FILE * yyin;
extern FILE * yyout;
%}

%x comment
%%
\n { line++;}
"//".*   
[ \t\r]+

[a-zA-Z_][a-zA-Z0-9_]*".grp" { yylval.stringconst =
      (char *) malloc (strlen(yytext) + 1 );
    sprintf(yylval.stringconst, "%s", yytext);
  	return tExternalFile; }

 "/*"         BEGIN(comment);
<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             ++line;
<comment>"*"+"/"        {
    BEGIN(INITIAL);
    return COMMENT;
}

\"(\\.|[^\\"])*\"     {
   yylval.stringconst =
      (char *) malloc (strlen(yytext) + 1 );
   sprintf(yylval.stringconst, "%s", yytext);
   return tString;
    
}

"*"       return '*';
"("       return '(';
")"       return ')';
";"       return ';'; 
":"       return ':'; 

"{"       return '{';

"}"       return '}';

"["       return '[';
"]"       return ']';

"\."      return '.';

","       return ',';


"<="      { yylval.stringconst =
      			(char *) malloc (strlen(yytext) + 1 ); 
						sprintf(yylval.stringconst, "%s", yytext);
						return tComparator; 
					} 
">="      { yylval.stringconst =
      			(char *) malloc (strlen(yytext) + 1 ); 
						sprintf(yylval.stringconst, "%s", yytext);
					  return tComparator; 
					} 
"="       {	yylval.stringconst =
      			(char *) malloc (strlen(yytext) + 1 );
			 			sprintf(yylval.stringconst,"%s",yytext);
						return tEquals; 
					} 
"<"       { yylval.stringconst =
      			(char *) malloc (strlen(yytext) + 1 ); 
						sprintf(yylval.stringconst,"%s",yytext);
						return tComparator; 
					} 
">"       { yylval.stringconst =
      			(char *) malloc (strlen(yytext) + 1 ); 
						sprintf(yylval.stringconst,"%s",yytext);
						return tComparator; 
					} 
"->"      return ARROW;
"|"       return '|';

"not"     return NOT;



"if"      return IF; 
"else"    return ELSE; 
"endif"   return ENDIF; 
"then"    return THEN; 


"member"     return MEMBER;
"in"         return IN;
"foreach"    return FOREACH;
"print"      return PRINT;
"patient"    return PATIENT;
"patients"   return PATIENTS;
"doctor"     return DOCTOR;
"diagnosis"  return DIAGNOSIS;
"gender"     return GENDER;
"age"        return AGE;
"id"         return ID;
"postalcode" return POSTALCODE;
"birthyear"  return BIRTHYEAR;
"years"      return YEARS;
"months"     return MONTHS;
"days"       return DAYS;
"hours"      return HOURS;
"start"      return START;
"end"        return END;
"timeline"   return TIMELINE;
"of"         return OF;
"is"         return IS;
"events"     return EVENTS;
"are"        return ARE;
"use"        return USE;

"group"      return GROUP;
"barchart"   return BARCHART;
"element"    return ELEMENT;

"sequences"  return SEQUENCES;
"sequence"   return SEQUENCE;

"like"       return LIKE;
"list"       return LIST;
"native"     return NATIVE;
"script"     return SCRIPT;


"count"      return COUNT;
"by"         return BY;

"table"      return TABLE;

"population" return POPULATION;
"period"     return PERIOD;

"to"         return TO;

"length"     return LENGTH;



0|([1-9][0-9]*)            { yylval.ival = atoi(yytext); return tInt; }
([01]?[0-9]|2[0-3])":"[0-5][0-9]    {
			yylval.hval  =
      (char *) malloc (strlen(yytext) + 1 );
   		sprintf(yylval.stringconst, "%s", yytext);
		 	return tHour; }


[a-zA-Z_][a-zA-Z0-9_]* {
   yylval.stringconst =
      (char *) malloc (strlen(yytext) + 1 );
   sprintf(yylval.stringconst, "%s", yytext);
   return tIdentifier;
}

. {yyerror("Scanner error. Invalid character."); exit(1);}

%%

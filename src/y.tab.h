/* A Bison parser, made by GNU Bison 3.0.3.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    tInt = 258,
    tFloat = 259,
    tIdentifier = 260,
    tExternalFile = 261,
    tString = 262,
    IF = 263,
    ELSE = 264,
    ENDIF = 265,
    THEN = 266,
    FOREACH = 267,
    PRINT = 268,
    PATIENTS = 269,
    TIMELINE = 270,
    OF = 271,
    IS = 272,
    EVENTS = 273,
    ARE = 274,
    USE = 275,
    GROUP = 276,
    BARCHART = 277,
    ELEMENT = 278,
    SEQUENCES = 279,
    LIKE = 280,
    LIST = 281,
    NATIVE = 282,
    SCRIPT = 283,
    NOT = 284,
    ARROW = 285,
    POPULATION = 286,
    TO = 287,
    PERIOD = 288,
    COMMENT = 289,
    PATIENT = 290,
    TYPE = 291,
    FUNC = 292,
    UMINUS = 293
  };
#endif
/* Tokens.  */
#define tInt 258
#define tFloat 259
#define tIdentifier 260
#define tExternalFile 261
#define tString 262
#define IF 263
#define ELSE 264
#define ENDIF 265
#define THEN 266
#define FOREACH 267
#define PRINT 268
#define PATIENTS 269
#define TIMELINE 270
#define OF 271
#define IS 272
#define EVENTS 273
#define ARE 274
#define USE 275
#define GROUP 276
#define BARCHART 277
#define ELEMENT 278
#define SEQUENCES 279
#define LIKE 280
#define LIST 281
#define NATIVE 282
#define SCRIPT 283
#define NOT 284
#define ARROW 285
#define POPULATION 286
#define TO 287
#define PERIOD 288
#define COMMENT 289
#define PATIENT 290
#define TYPE 291
#define FUNC 292
#define UMINUS 293

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 9 "oncotime.y" /* yacc.c:1915  */

    int ival;
    float fval;
    char *stringconst;

#line 136 "y.tab.h" /* yacc.c:1915  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

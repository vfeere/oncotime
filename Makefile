all:
	flex oncotime.l
	/usr/local/Cellar/bison/3.0.3/bin/bison --yacc --defines oncotime.y # 
	gcc lex.yy.c y.tab.c y.tab.h main.c -ll

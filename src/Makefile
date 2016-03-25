all: oncotime

debug:
	scanner.c parser.debug.c parser.h main.c
	gcc $^ -ll
	mv a.out $@

clean:
	rm scanner.c parser.c parser.h parser.h.gch oncotime memory.h.gch tree.h.gch pretty.h.gch symtab.h.gch type.h.gch filterRefinements.h.gch preprocessor.h.gch importFiles.h.gch regex.h.gch codegen.h.gch parser.output
	rm -rf a.out.dSYM

parser.c: oncotime.y
	bison --yacc -v --defines -o $@ $^

parser.debug.c: oncotime.y
		bison --yacc --defines --debug -o $@ $^

scanner.c: oncotime.l
	flex -o $@ $^

oncotime: scanner.c parser.c parser.h tree.h tree.c memory.h memory.c pretty.h pretty.c symtab.h symtab.c type.h type.c regex.h regex.c filterRefinements.h filterRefinements.c importFiles.h importFiles.c codegen.h codegen.c preprocessor.h preprocessor.c events.c main.c
	gcc $^ -ggdb -ll
	mv a.out oncotime	

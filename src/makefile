run:
	./scanner test.c

flex: lexical.l
	flex lexical.l

scanner: lex.yy.c
	gcc main.c lex.yy.c -lfl -o scanner
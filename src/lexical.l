%{
    #include <stdio.h>
%}
%option yylineno

INT 0|[1-9][0-9]{0,9}
FLOAT [0-9]+\.[0-9]+
ID [_a-zA-Z][_a-zA-Z0-9]{0,30}
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
WS [ \r\t]+
%%
{INT} { printf("Integer value %d", atoi(yytext)); }
{OR} { printf("OR\n"); }
{ID} { printf("ID\n"); }
. { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%
int main(int argc, char** argv) {
    int i;
    if (argc < 2) {
        yylex();
        return 0;
    }
    for (i = 1; i < argc; i++) {
        FILE *f = fopen(argv[i], "r");
        if (!f) {
            perror(argv[1]);
            return 1;
        }
        yyrestart(f);
        yylex();
        fclose(f);
    }
    if (argc > 1) {
    }
    return 0;
}

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
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
TYPE int|float
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
WS [ \n\r\t]+
%%
{INT} { printf("INT\n"); }
{FLOAT} { printf("FLOAT\n"); }
{SEMI} { printf("SEMI\n"); }
{COMMA} { printf("COMMA\n"); }
{ASSIGNOP} { printf("ASSIGNOP\n"); }
{RELOP} { printf("RELOP\n"); }
{PLUS} { printf("PLUS\n"); }
{MINUS} { printf("MINUS\n"); }
{STAR} { printf("STAR\n"); }
{DIV} { printf("DIV\n"); }
{AND} { printf("AND\n"); }
{OR} { printf("OR\n"); }
{DOT} { printf("DOT\n"); }
{NOT} { printf("NOT\n"); }
{TYPE} { printf("TYPE\n"); }
{LP} { printf("LP\n"); }
{RP} { printf("RP\n"); }
{LB} { printf("LB\n"); }
{RB} { printf("RB\n"); }
{LC} { printf("LC\n"); }
{RC} { printf("RC\n"); }
{STRUCT} { printf("STRUCT\n"); }
{RETURN} { printf("RETURN\n"); }
{IF} { printf("IF\n"); }
{ELSE} { printf("ELSE\n"); }
{WHILE} { printf("WHILE\n"); }
{ID} { printf("ID\n"); }
{WS} { printf("WHITESAPCE\n"); }
. { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%

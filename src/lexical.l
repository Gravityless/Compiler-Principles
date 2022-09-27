%{
#include <stdio.h>
#include "syntax.tab.h"

int yycolumn = 1;
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
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
WS [ \r\t]+
%%
{INT} { 
    yylval = atoi(yytext);
    return INT;
    }
{FLOAT} { printf("FLOAT\n"); }
{SEMI} { printf("SEMI\n"); }
{COMMA} { printf("COMMA\n"); }
{ASSIGNOP} { printf("ASSIGNOP\n"); }
{RELOP} { printf("RELOP\n"); }
{PLUS} { printf("PLUS\n"); }
{MINUS} { return MINUS; }
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
\n { yycolumn = 1; }
. { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%

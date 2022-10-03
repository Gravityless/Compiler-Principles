%{
#define YYSTYPE Node*
#include "syntax.tab.h"
#include "node.h"

extern int errornum;
int yycolumn = 1;

#define buildNode(name, val) \
    Node* node = newNode(name, val, yylineno, true, 0); \
    yylval = node;

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

{INT} { buildNode("INT", yytext); return INT; }
{FLOAT} { buildNode("FLOAT", yytext); return FLOAT; }
{SEMI} { buildNode("SEMI", yytext); return SEMI; }
{COMMA} { buildNode("COMMA", yytext); return COMMA; }
{ASSIGNOP} { buildNode("ASSIGNOP", yytext); return ASSIGNOP; }
{RELOP} { buildNode("RELOP", yytext); return RELOP; }
{PLUS} { buildNode("PLUS", yytext); return PLUS; }
{MINUS} { buildNode("MINUS", yytext); return MINUS; }
{STAR} { buildNode("STAR", yytext); return STAR; }
{DIV} { buildNode("DIV", yytext); return DIV; }
{AND} { buildNode("AND", yytext); return AND; }
{OR} { buildNode("OR", yytext); return OR; }
{DOT} { buildNode("DOT", yytext); return DOT; }
{NOT} { buildNode("NOT", yytext); return NOT; }
{TYPE} { buildNode("TYPE", yytext); return TYPE; }
{LP} { buildNode("LP", yytext); return LP; }
{RP} { buildNode("RP", yytext); return RP; }
{LB} { buildNode("LB", yytext); return LB; }
{RB} { buildNode("RB", yytext); return RB; }
{LC} { buildNode("LC", yytext); return LC; }
{RC} { buildNode("RC", yytext); return RC; }
{STRUCT} { buildNode("STRUCT", yytext); return STRUCT; }
{RETURN} { buildNode("RETURN", yytext); return RETURN; }
{IF} { buildNode("IF", yytext); return IF; }
{ELSE} { buildNode("ELSE", yytext); return ELSE; }
{WHILE} { buildNode("WHILE", yytext); return WHILE; }
{ID} { buildNode("ID", yytext); return ID; }
{WS} { }
\n { yycolumn = 1; }
. { errornum++; printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }

%%

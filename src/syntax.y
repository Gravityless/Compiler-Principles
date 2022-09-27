%{
#include <stdio.h>
#include "lex.yy.c"
void yyerror(char* msg);
%}

%token INT
%token ADD MINUS MUL DIV

%%
Calc :
    | Exp { printf("= %d\n", $1); }
    ;
Exp : Factor
    | Exp ADD Factor { $$ = $1 + $3; }
    | Exp MINUS Factor { $$ = $1 - $3; }
    ;
Factor : Term
    | Factor MUL Term { $$ = $1 * $3; }
    | Factor DIV Term { $$ = $1 / $3; }
    ;
Term : INT
    ;
%%
void yyerror(char* msg) {
    fprintf(stderr, "error: %s\n", msg);
}
int main() {
    yyparse();
}

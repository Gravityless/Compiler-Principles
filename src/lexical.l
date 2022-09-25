%{
    #include <stdio.h>
%}
digit [0-9]
letter[_a-zA-z]
%%
{digit}+ { printf("Integer value %d\n", atoi(yytext)); }
. { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%
int main(int argc, char** argv) {
    int i;
    if (argc < 2) {
        yylex();
        printf("hello, world");
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
        printf("hello, world");
    }
    if (argc > 1) {
        printf("hello, world");
    }
    return 0;
}

#include <stdio.h>

// extern FILE *yyin;
extern int yylex();
extern int yyrestart();

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

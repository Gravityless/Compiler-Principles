#include "node.h"
#include "semantic.h"

// extern FILE *yyin;
extern int yylex();
extern int yyrestart();
extern int yyparse();

extern Node* root;
int errornum = 0;

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
            return -1;
        }
        yyrestart(f);
        // yylex();
        yyparse();
        fclose(f);

        if (errornum == 0) {
            generateTree(root);
            semanticAnalysis(root);
        }
    }
    return 0;
}

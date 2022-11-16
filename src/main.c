#include "node.h"
#include "semantic.h"
#include "intercode.h"

// extern FILE *yyin;
extern int yylex();
extern int yyrestart();
extern int yyparse();

extern Node* root;
int errornum = 0;

int main(int argc, char** argv) {    
    FILE *codeList = fopen(argv[1], "r");
    FILE *irList = fopen(argv[2], "w+");
    if (!codeList) {
        perror(argv[1]);
        return -1;
    }
    yyrestart(codeList);
    // yylex();
    yyparse();
    fclose(codeList);

    if (errornum == 0) {
        // printf("start generating syntax tree\n");
        generateTree(root);

        // printf("start semantic analysis\n");
        semanticAnalysis(root);
        
        // printf("start generating intercodes\n");
        intercodeGenerate(root, irList);

        delTable(table);
    }
    return 0;
}

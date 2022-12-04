#include "node.h"
#include "semantic.h"
#include "intercode.h"
#include "assembly.h"

// extern FILE *yyin;
extern int yylex();
extern int yyrestart();
extern int yyparse();

extern Node* root;
int errornum = 0;

int main(int argc, char** argv) {    
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w+");
    if (!input) {
        perror(argv[1]);
        return -1;
    }
    yyrestart(input);
    // yylex();
    yyparse();
    fclose(input);

    if (errornum == 0) {
        // printf("start generating syntax tree\n");
        generateTree(root);

        // printf("start semantic analysis\n");
        semanticAnalysis(root);
        
        // printf("start generating intercodes\n");
        // intercodeGenerate(root, output);
        // FILE *irOut = fopen("./test_intercode.txt", "w+");
        // intercodeGenerate(root, irOut);
        intercodeGenerate(root, NULL);

        // printf("start assemblying\n");
        genAssemblyCode(output);

        delTable(table);
    }
    return 0;
}

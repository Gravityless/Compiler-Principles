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

    if (argc < 2) {
        yylex();
        return 0;
    }
    
    FILE *fp = fopen("./test_intercode.txt", "w+");
    for (int i = 1; i < argc; i++) {
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
            printf("start generating syntax tree\n");
            generateTree(root);

            printf("start semantic analysis\n");
            semanticAnalysis(root);
            
            printf("start generating intercodes\n");
            interCodeList = newInterCodeList();
            genInterCodes(root);
            if (!interError)
                printInterCode(fp, interCodeList);
            delTable(table);
        }
    }
    return 0;
}

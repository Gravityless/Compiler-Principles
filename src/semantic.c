#include "semantic.h"

void semanticAnalysis(Node* node) {
    Table table = initTable();
    traverseTree(node);
    deleteTable(table);
}
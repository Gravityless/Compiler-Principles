#include "semantic.h"

Type newType(int kind, ...) {
    Type t = (Type)malloc(sizeof(struct Type_));
    t->kind = kind;
    va_list ap;
    va_start(ap, kind);
    switch (kind)
    {
    case BASIC:
        t->u.basic = va_arg(ap, int);
        break;
    case ARRAY:
        t->u.array.elem = va_arg(ap, Type);
        t->u.array.size = va_arg(ap, int);
        break;
    case STRUCTURE:
        t->u.structure->name = va_arg(ap, char*);
        t->u.structure->tail = va_arg(ap, FieldList);
        break;
    case FUNCTION:
        t->u.function->argc = va_arg(ap, int);
        t->u.function->argv = va_arg(ap, FieldList);
        t->u.function->returnType = va_arg(ap, Type);
        break;
    }
    va_end(ap);
    return t;
}

Type copyType(Type type);
void deleteType(Type type);
bool checkType(Type t1, Type t2);
void printType(Type type);

FieldList newFieldList(char* Name, Type type);
FieldList copyFieldList(FieldList fieldList);
void deleteFieldList(FieldList fieldList);
void setFieldListName(FieldList fieldList, char* Name);
void printFieldList(FieldList fieldList);

TableItem newItem(int depth, FieldList fieldList);
void deleteItem(TableItem item);
bool structDef(TableItem item);

HashTable newHashTable();
void deleteHashTable(HashTable hashTable);
TableItem getHashHead(HashTable h, int index);
void setHashHead(HashTable h, int index, TableItem item);

Stack newStack();
void deleteStack(Stack stack);
void increDepth(Stack stack);
void reduceDepth(Stack stack);
TableItem getStackHead(Stack stack);
void setStackHead(Stack stack, TableItem item);

Table initTable();
void deleteTable(Table table);
TableItem searchTableItem(Table table, char* name);
bool checkTableItem(Table table, TableItem item);
void addTableItem(Table table, TableItem item);
void deleteTableItem(Table table, TableItem item);
void clearCurDepthStackList(Table table);
void printTable(Table table);

void traverseTree(Node* node);
void genExtDef(Node* node);
void genExtDecList(Node* node, Type specifier);
Type genSpecifier(Node* node);
Type genStructSpecifier(Node* node);
TableItem genVarDec(Node* node, Type specifier);
void genFunDec(Node* node, Type returnType);
void genVarList(Node* node, TableItem func);
FieldList genParamDec(Node* node);
void genCompSt(Node* node, Type returnType);
void genStmtList(Node* node, Type returnType);
void genStmt(Node* node, Type returnType);
void genDefList(Node* node, TableItem structInfo);
void genDef(Node* node, TableItem structInfo);
void genDecList(Node* node, Type specifier, TableItem structInfo);
void genDec(Node* node, Type specifier, TableItem structInfo);
Type genExp(Node* node);
void genArgs(Node* node, TableItem funcInfo);

void semanticAnalysis(Node* node) {
    Table table = initTable();
    traverseTree(node);
    deleteTable(table);
}

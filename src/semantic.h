#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "node.h"
#include "semErr.h"

#define HASH_TABLE_SIZE 0x3fff
#define MAX_SCOPE_DEPTH 20
#define MAX_MSG_LENGTH 100

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct TableItem_* TableItem;
typedef struct HashTable_* HashTable;
typedef struct Scope_*     Scope;
typedef struct Table_*     Table;
typedef struct Function_*  Function;

extern Table table;
extern HashTable hashTable;
extern Scope scope;

struct FieldList_
{
    char* name;
    Type type;
    FieldList tail;
};

struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE, FUNCTION} kind;
    union
    {
        int basic;
        struct { Type elem; int size; } array;
        struct { char* name; FieldList fieldList; } structure;
        struct { int argc; FieldList argv; Type rType; } function;
    } u;
};

struct TableItem_
{
    int layerDepth;
    FieldList fieldList;
    TableItem sameScope;
    TableItem sameHash;
};

struct HashTable_
{
    TableItem* hashArray;
};

struct Scope_
{
    TableItem* scopeLayer;
    int layerDepth;
};

struct Table_
{
    HashTable hashTable;
    Scope scope;
    int anonymousNum;
};

Type newType(int kind, ...);
Type copyType(Type type);
void delType(Type type);
bool compareType(Type t1, Type t2);

FieldList newFieldList(char* name, Type type);
FieldList copyFieldList(FieldList fieldList);
void delFieldList(FieldList fieldList);
void setFieldListName(FieldList fieldList, char* name);

TableItem newItem(int layerDepth, FieldList fieldList);
void delItem(TableItem item);
bool isStructDef(TableItem item);

HashTable newHashTable();
void delHashTable();
TableItem getHashHead(int index);
void setHashHead(int index, TableItem item);

Scope newScope();
void delScope();
void innerLayer();
void outerLayer();
void exitLayer();
TableItem getLayerHead();
void setLayerHead(TableItem item);

Table initTable();
void delTable();
TableItem searchTableItem(char* name);
bool hasConfliction(TableItem item);
void addTableItem(TableItem item);
void delTableItem(TableItem item);

void tranverseTree(Node* node);
void ExtDef(Node* node);
void ExtDecList(Node* node, Type specifier);
Type Specifier(Node* node);
Type StructSpecifier(Node* node);
TableItem VarDec(Node* node, Type specifier);
void FunDec(Node* node, Type returnType);
void VarList(Node* node, TableItem func);
FieldList ParamDec(Node* node);
void CompSt(Node* node, Type returnType);
void StmtList(Node* node, Type returnType);
void Stmt(Node* node, Type returnType);
void DefList(Node* node, TableItem structInfo);
void Def(Node* node, TableItem structInfo);
void DecList(Node* node, Type specifier, TableItem structInfo);
void Dec(Node* node, Type specifier, TableItem structInfo);
Type Exp(Node* node);
void Args(Node* node, TableItem funcInfo);

void semanticAnalysis(Node* node);

static inline unsigned int getHashCode(char* name) {
    unsigned int val = 0, i;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        if (i = val & ~HASH_TABLE_SIZE)
            val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
    }
    return val;
}

static inline void Error(SEMERR type, int line, char* msg) {
    printf("Error type %d at Line %d: %s\n", type, line, msg);
}

static inline char* newString(char* s) {
    if (s == NULL) return NULL;
    int length = strlen(s) + 1;
    char* t = (char*)malloc(sizeof(char) * length);
    strncpy(t, s, length);
    return t;
}

#endif

#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "node.h"
#include "semErr.h"

#define HASH_TABLE_SIZE 0x3fff
#define MAX_MSG_LENGTH 100

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct TableItem_* TableItem;
typedef struct HashTable_* HashTable;
typedef struct Scope_*     Scope;
typedef struct Table_*     Table;
typedef struct Function_*  Function;

extern Table table;

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
    int depth;
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
    TableItem* scopeList;
    int depth;
};

struct Table_
{
    HashTable hashTable;
    Scope scope;
    int anonymousNum;
};

Type newType(int kind, ...);
Type copyType(Type type);
void deleteType(Type type);
bool checkType(Type t1, Type t2);

FieldList newFieldList(char* name, Type type);
FieldList copyFieldList(FieldList fieldList);
void deleteFieldList(FieldList fieldList);
void setFieldListName(FieldList fieldList, char* name);

TableItem newItem(int depth, FieldList fieldList);
void deleteItem(TableItem item);
bool structDef(TableItem item);

HashTable newHashTable();
void deleteHashTable(HashTable hashTable);
TableItem getHashHead(HashTable h, int index);
void setHashHead(HashTable h, int index, TableItem item);

Scope newScope();
void deleteScope(Scope scope);
void incrDepth(Scope scope);
void reduceDepth(Scope scope);
TableItem getScopeHead(Scope scope);
void setScopeHead(Scope scope, TableItem item);

Table initTable();
void deleteTable(Table table);
TableItem searchTableItem(Table table, char* name);
bool findTableItem(Table table, TableItem item);
void addTableItem(Table table, TableItem item);
void deleteTableItem(Table table, TableItem item);
void clearScope(Table table);

void tranverseTree(Node* node);
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

#ifndef _INTERCODE_H_
#define _INTERCODE_H_
#include "node.h"
#include "semantic.h"

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef struct InterCodes_* InterCodes;
typedef struct Arg_* Arg;
typedef struct ArgList_* ArgList;
typedef struct InterCodeList_* InterCodeList;

struct Operand_ {
    enum {
        VARIABLE,
        CONSTANT,
        ADDRESS,
        TEXT,
    } kind;

    union {
        char* name;
        int val;
    } u;
};

struct InterCode_ {
    enum {
        LABEL,
        FUNCTION,
        ASSIGN,
        ADD,
        SUB,
        MUL,
        DIV,
        GET_ADDR,
        READ_ADDR,
        WRITE_ADDR,
        GOTO,
        IF_GOTO,
        RETURN,
        DEC,
        ARG,
        CALL,
        PARAM,
        READ,
        WRITE,
    } kind;

    union {
        struct { Operand op; } sinOp;
        struct { Operand right, left; } assign;
        struct { Operand result, op1, op2; } binOp;
        struct { Operand x, relop, y, z; } ifGoto;
        struct { Operand op; int size; } dec;
    } u;
};

struct InterCodes_ {
    InterCode code;
    InterCodes prev, next;
};

struct Arg_ {
    Operand op;
    Arg next;
};

struct ArgList_ {
    Arg head;
    Arg cur;
};

struct InterCodeList_ {
    InterCodes head;
    InterCodes cur;
    char* arrayName;
    int tempVarNum;
    int labelNum;
};

extern bool interError;
extern InterCodeList interCodeList;

// Operand func
Operand newOperand(int kind, ...);
void deleteOperand(Operand p);
void setOperand(Operand p, int kind, char* val);
void printOperand(FILE* fp, Operand op);

// Arg and ArgList func
ArgList newArgList();
void deleteArgList(ArgList p);
void reverseArgList(ArgList argList);
Arg newArg(Operand op);
void addArg(ArgList argList, Arg arg);
void deleteArg(Arg p);


// InterCodes func
InterCodes newInterCodes();
void deleteInterCodes(InterCodes p);
void addInterCode(InterCodes newCode);

// InterCodeList func
InterCodeList newInterCodeList();
void deleteInterCodeList();
void printInterCodeList(FILE* fp);

// traverse func
Operand Temp();
Operand Label();
int getSize(Type type);
void genInterCodes(Node* node);
void genInterCode(int kind, ...);
void transExp(Node* node, Operand place);
void transArgs(Node* node, ArgList argList);
void transCond(Node* node, Operand labelTrue, Operand labelFalse);
void transVarDec(Node* node, Operand place);
void transDec(Node* node);
void transDecList(Node* node);
void transDef(Node* node);
void transDefList(Node* node);
void transCompSt(Node* node);
void transStmt(Node* node);
void transStmtList(Node* node);
void transFunDec(Node* node);
void transExtDef(Node* node);
void transExtDefList(Node* node);
void intercodeGenerate(Node* node, FILE* fp);

#endif

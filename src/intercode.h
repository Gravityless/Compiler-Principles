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
        OP_VARIABLE,
        OP_CONSTANT,
        OP_ADDRESS,
        OP_LABEL,
        OP_FUNCTION,
        OP_RELOP,
    } kind;

    union {
        char* name;
        int val;
    } u;
};

struct InterCode_ {
    enum {
        IR_LABEL,
        IR_FUNCTION,
        IR_ASSIGN,
        IR_ADD,
        IR_SUB,
        IR_MUL,
        IR_DIV,
        IR_GET_ADDR,
        IR_READ_ADDR,
        IR_WRITE_ADDR,
        IR_GOTO,
        IR_IF_GOTO,
        IR_RETURN,
        IR_DEC,
        IR_ARG,
        IR_CALL,
        IR_PARAM,
        IR_READ,
        IR_WRITE,
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
    char* lastArrayName;
    int tempVarNum;
    int labelNum;
};

extern bool interError;
extern InterCodeList interCodeList;

// Operand func
Operand newOperand(int kind, ...);
void deleteOperand(Operand p);
void setOperand(Operand p, int kind, char* val);
void printOp(FILE* fp, Operand op);

// InterCode func
InterCode newInterCode(int kind, ...);
void deleteInterCode(InterCode p);
void printInterCode(FILE* fp, InterCodeList interCodeList);

// InterCodes func
InterCodes newInterCodes(InterCode code);
void deleteInterCodes(InterCodes p);

// Arg and ArgList func
Arg newArg(Operand op);
ArgList newArgList();
void deleteArg(Arg p);
void deleteArgList(ArgList p);
void addArg(ArgList argList, Arg arg);

// InterCodeList func
InterCodeList newInterCodeList();
void deleteInterCodeList(InterCodeList p);
void addInterCode(InterCodeList interCodeList, InterCodes newCode);

// traverse func
Operand newTemp();
Operand newLabel();
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

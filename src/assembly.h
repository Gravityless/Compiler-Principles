#ifndef _ASSEMBLY_H_
#define _ASSEMBLY_H_

#include "intercode.h"

#define REG_NUM 32

typedef struct Register_* Register;
typedef struct Varible_* Varible;
typedef struct Registers_* Registers;
typedef struct VariableList_* VariableList;
typedef struct VarTable_* VarTable;

struct Register_ {
    bool isFree;
    char* name;
};

struct Varible_ {
    int regNo;
    Operand op;
    Varible next;
};

struct Registers_ {
    Register regLsit[REG_NUM];
    int lastChangedNo;
};

struct VariableList_ {
    Varible head;
    Varible cur;
};

struct VarTable_ {
    VariableList varListReg;  // 寄存器中的变量表
    VariableList varListMem;  // 内存中的变量表
    bool inFunc;
    char* curFuncName;
};

typedef enum _regNo {
    ZERO,
    AT,
    V0,
    V1,
    A0,
    A1,
    A2,
    A3,
    T0,
    T1,
    T2,
    T3,
    T4,
    T5,
    T6,
    T7,
    S0,
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    T8,
    T9,
    K0,
    K1,
    GP,
    SP,
    FP,
    RA,
} RegNo;

extern Registers registers;
extern VarTable varTable;

Registers initRegisters();
void resetRegisters(Registers registers);
void deleteRegisters(Registers registers);

VarTable newVarTable();
VariableList newVariableList();
void printVariableList(VariableList varList);
void addVarible(VariableList varList, int regNo, Operand op);
void delVarible(VariableList varList, Varible var);
void clearVariableList(VariableList varList);
int checkVarible(FILE* fp, VarTable varTable, Registers registers,
                 Operand op);

int allocReg(Registers registers, VarTable varTable, Operand op);

Register newRegister(char* regName);
Varible newVarible(int regNo, Operand op);

void genAssemblyCode(FILE* fp);
void initCode(FILE* fp);
void interToAssem(FILE* fp, InterCodes interCodes);
void pusha(FILE* fp);
void popa(FILE* fp);

#endif
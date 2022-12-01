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
    bool avail;
    char* name;
};

struct Varible_ {
    int regNo;
    Operand op;
    Varible next;
};

struct Registers_ {
    Register regList[REG_NUM];
    int lastUsed;
};

struct VariableList_ {
    Varible head;
    Varible cur;
};

struct VarTable_ {
    VariableList varListReg;            // 寄存器中的变量
    VariableList varListMem;            // 内存中的变量
    bool inFunc;
    char* funcName;
};

typedef enum _regNo {
    ZERO,                               // 接地
    AT,                                 // 汇编器预留
    V0, V1,                             // 函数返回值
    A0, A1, A2, A3,                     // 函数参数
    T0, T1, T2, T3, T4, T5, T6, T7,     // 调用者保存
    S0, S1, S2, S3, S4, S5, S6, S7,     // 被调用者保存
    T8, T9,                             // 调用者保存
    K0, K1,                             // 汇编器预留/中断处理保留
    GP,                                 // 静态数据段64K内存空间中部
    SP,                                 // 栈顶指针
    FP,                                 // 帧指针/s8
    RA,                                 // 返回地址
} RegNo;

extern Registers registers;
extern VarTable varTable;

Registers initRegisters();
void resetRegisters();
void deleteRegisters();
int allocReg(Operand op);
Register newRegister(char* regName);

VarTable newVarTable();
VariableList newVariableList();
Varible newVarible(int regNo, Operand op);
void addVarible(VariableList varList, int regNo, Operand op);
void delVarible(VariableList varList, Varible var);
void clearVariableList(VariableList varList);
int getVarible(FILE* fp, Operand op);

void genAssemblyCode(FILE* fp);
void initAsm(FILE* fp);
void Ir2Asm(FILE* fp, InterCodes interCodes);
void pusha(FILE* fp);
void popa(FILE* fp);

#endif
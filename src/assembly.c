#include "assembly.h"

char* REG_NAME[REG_NUM] = {
    "$0",  "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2",
    "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5",
    "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

Registers registers;
VarTable varTable;

Registers initRegisters() {
    Registers p = (Registers)malloc(sizeof(struct Registers_));
    for (int i = 0; i < REG_NUM; i++) {
        p->regList[i] = newRegister(REG_NAME[i]);
    }
    p->regList[0]->avail = false;  // $0不允许使用
    p->lastUsed = 0;
    return p;
}

void resetRegisters() {
    for (int i = 1; i < REG_NUM; i++) {
        registers->regList[i]->avail = true;
    }
}

void deleteRegisters() {
    for (int i = 0; i < REG_NUM; i++) {
        free(registers->regList[i]);
    }
    free(registers);
}

VarTable newVarTable() {
    VarTable p = (VarTable)malloc(sizeof(struct VarTable_));
    p->varListReg = newVariableList();
    p->varListMem = newVariableList();
    p->inFunc = false;
    p->curFuncName = NULL;
    return p;
}

void deleteVarTable() {
    clearVariableList(varTable->varListReg);
    clearVariableList(varTable->varListMem);
    free(varTable->varListReg);
    free(varTable->varListMem);
    free(varTable);
}

VariableList newVariableList() {
    VariableList p = (VariableList)malloc(sizeof(struct VariableList_));
    p->head = NULL;
    p->cur = NULL;
    return p;
}

void printVariableList(VariableList varList) {
    printf("----------VariableList------------\n");
    Varible temp = varList->head;
    while (temp) {
        if (temp->op->kind == CONSTANT)
            printf("reg: %s, value: %d\n", REG_NAME[temp->regNo],
                   temp->op->u.val);
        else
            printf("reg: %s, varName: %s\n", REG_NAME[temp->regNo],
                   temp->op->u.name);
        temp = temp->next;
    }
    printf("------------end--------------\n");
}

void addVarible(VariableList varList, int regNo, Operand op) {
    Varible newVar = newVarible(regNo, op);
    // reg_list[reg_num].is_free=0;
    if (varList->head == NULL) {
        varList->head = newVar;
        varList->cur = newVar;
    } else {
        varList->cur->next = newVar;
        varList->cur = newVar;
    }
}

void delVarible(VariableList varList, Varible var) {
    if (var == varList->head) {
        varList->head = varList->head->next;
    } else {
        Varible temp = varList->head;
        while (temp) {
            if (temp->next == var) break;
            temp = temp->next;
        }
        if (varList->cur == var) varList->cur = temp;
        temp->next = var->next;
        var->next = NULL;
        free(var);
    }
}

void clearVariableList(VariableList varList) {
    Varible temp = varList->head;
    while (temp) {
        Varible p = temp;
        temp = temp->next;
        free(p);
    }
    varList->head = NULL;
    varList->cur = NULL;
}

int checkVarible(FILE* fp, Operand op) {
    if (op->kind != CONSTANT) {
        // 若为变量，先看变量表里有没有，有就返回，没有新分配一个
        Varible temp = varTable->varListReg->head;
        while (temp) {
            if (temp->op->kind != CONSTANT &&
                !strcmp(temp->op->u.name, op->u.name))
                return temp->regNo;
            temp = temp->next;
        }
        int regNo = allocReg(op);
        return regNo;
    } else {
        // 立即数则找个寄存器放进去, 如果为0直接返回0号寄存器
        if (op->u.val == 0) return ZERO;
        int regNo = allocReg(op);
        fprintf(fp, "  li %s, %d\n", registers->regList[regNo]->name,
                op->u.val);
        return regNo;
    }
}

int allocReg(Operand op) {
    // 先看有无空闲，有就直接放
    // printf("allocNewReg\n");
    for (int i = T0; i <= T9; i++) {
        if (registers->regList[i]->avail) {
            registers->regList[i]->avail = 0;
            addVarible(varTable->varListReg, i, op);
            return i;
        }
    }
    // printf("nofree\n");
    // 无空闲了，需要找一个寄存器释放掉
    // 可以先释放掉最近没用过的立即数
    Varible temp = varTable->varListReg->head;
    while (temp) {
        if (temp->op->kind == CONSTANT &&
            temp->regNo != registers->lastUsed) {
            int regNo = temp->regNo;
            registers->lastUsed = regNo;
            delVarible(varTable->varListReg, temp);
            addVarible(varTable->varListReg, regNo, op);
            return regNo;
        }
        temp = temp->next;
    }

    // 如果没有立即数，就找一个最近没用过的临时变量的释放掉
    temp = varTable->varListReg->head;
    while (temp) {
        if (temp->op->kind != CONSTANT) {
            if (temp->op->u.name[0] == 't' &&
                temp->regNo != registers->lastUsed) {
                int regNo = temp->regNo;
                registers->lastUsed = regNo;
                delVarible(varTable->varListReg, temp);
                addVarible(varTable->varListReg, regNo, op);
                return regNo;
            }
        }
        temp = temp->next;
    }
}

Register newRegister(char* regName) {
    Register p = (Register)malloc(sizeof(struct Register_));
    p->avail = true;
    p->name = regName;
}

Varible newVarible(int regNo, Operand op) {
    Varible p = (Varible)malloc(sizeof(struct Varible_));
    p->regNo = regNo;
    p->op = op;
    p->next = NULL;
}

void genAssemblyCode(FILE* fp) {
    registers = initRegisters();
    varTable = newVarTable();
    initAsm(fp);
    InterCodes temp = interCodeList->head;
    while (temp) {
        Ir2Asm(fp, temp);
        // printVariableList(varTable->varListReg);
        temp = temp->next;
    }
    deleteRegisters();
    deleteVarTable();
    registers = NULL;
    varTable = NULL;
}

void initAsm(FILE* fp) {
    fprintf(fp, ".data\n");
    fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fp, "_ret: .asciiz \"\\n\"\n");
    fprintf(fp, ".globl main\n");

    // 无重复变量名，直接把数组当全局变量声明了
    InterCodes temp = interCodeList->head;
    while (temp) {
        if (temp->code->kind == DEC)
            fprintf(fp, "%s: .word %d\n", temp->code->u.dec.op->u.name,
                    temp->code->u.dec.size);
        temp = temp->next;
    }

    // read function
    fprintf(fp, ".text\n");
    fprintf(fp, "read:\n");
    fprintf(fp, "  li $v0, 4\n");
    fprintf(fp, "  la $a0, _prompt\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0, 5\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  jr $ra\n\n");

    // write function
    fprintf(fp, "write:\n");
    fprintf(fp, "  li $v0, 1\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0, 4\n");
    fprintf(fp, "  la $a0, _ret\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  move $v0, $0\n");
    fprintf(fp, "  jr $ra\n");
}

void printinter(InterCodes cur) {
    FILE* fp = NULL;
    switch (cur->code->kind) {
        case LABEL:
            printf("LABEL ");
            printOperand(fp, cur->code->u.sinOp.op);
            printf(" :");
            break;
        case FUNCTION:
            printf("FUNCTION ");
            printOperand(fp, cur->code->u.sinOp.op);
            printf(" :");
            break;
        case ASSIGN:
            printOperand(fp, cur->code->u.assign.left);
            printf(" := ");
            printOperand(fp, cur->code->u.assign.right);
            break;
        case ADD:
            printOperand(fp, cur->code->u.binOp.result);
            printf(" := ");
            printOperand(fp, cur->code->u.binOp.op1);
            printf(" + ");
            printOperand(fp, cur->code->u.binOp.op2);
            break;
        case SUB:
            printOperand(fp, cur->code->u.binOp.result);
            printf(" := ");
            printOperand(fp, cur->code->u.binOp.op1);
            printf(" - ");
            printOperand(fp, cur->code->u.binOp.op2);
            break;
        case MUL:
            printOperand(fp, cur->code->u.binOp.result);
            printf(" := ");
            printOperand(fp, cur->code->u.binOp.op1);
            printf(" * ");
            printOperand(fp, cur->code->u.binOp.op2);
            break;
        case DIV:
            printOperand(fp, cur->code->u.binOp.result);
            printf(" := ");
            printOperand(fp, cur->code->u.binOp.op1);
            printf(" / ");
            printOperand(fp, cur->code->u.binOp.op2);
            break;
        case GET_ADDR:
            printOperand(fp, cur->code->u.assign.left);
            printf(" := &");
            printOperand(fp, cur->code->u.assign.right);
            break;
        case READ_ADDR:
            printOperand(fp, cur->code->u.assign.left);
            printf(" := *");
            printOperand(fp, cur->code->u.assign.right);
            break;
        case WRITE_ADDR:
            printf("*");
            printOperand(fp, cur->code->u.assign.left);
            printf(" := ");
            printOperand(fp, cur->code->u.assign.right);
            break;
        case GOTO:
            printf("GOTO ");
            printOperand(fp, cur->code->u.sinOp.op);
            break;
        case IF_GOTO:
            printf("IF ");
            printOperand(fp, cur->code->u.ifGoto.x);
            printf(" ");
            printOperand(fp, cur->code->u.ifGoto.relop);
            printf(" ");
            printOperand(fp, cur->code->u.ifGoto.y);
            printf(" GOTO ");
            printOperand(fp, cur->code->u.ifGoto.z);
            break;
        case RETURN:
            printf("RETURN ");
            printOperand(fp, cur->code->u.sinOp.op);
            break;
        case DEC:
            printf("DEC ");
            printOperand(fp, cur->code->u.dec.op);
            printf(" ");
            printf("%d", cur->code->u.dec.size);
            break;
        case ARG:
            printf("ARG ");
            printOperand(fp, cur->code->u.sinOp.op);
            break;
        case CALL:
            printOperand(fp, cur->code->u.assign.left);
            printf(" := CALL ");
            printOperand(fp, cur->code->u.assign.right);
            break;
        case PARAM:
            printf("PARAM ");
            printOperand(fp, cur->code->u.sinOp.op);
            break;
        case READ:
            printf("READ ");
            printOperand(fp, cur->code->u.sinOp.op);
            break;
        case WRITE:
            printf("WRITE ");
            printOperand(fp, cur->code->u.sinOp.op);
            break;
    }
    printf("\n");
}

void Ir2Asm(FILE* fp, InterCodes interCodes) {
    InterCode interCode = interCodes->code;
    int kind = interCode->kind;

    if (kind == LABEL) {
        fprintf(fp, "%s:\n", interCode->u.sinOp.op->u.name);
    } else if (kind == FUNCTION) {
        fprintf(fp, "\n%s:\n", interCode->u.sinOp.op->u.name);

        // 新函数，寄存器重新变为可用，并清空变量表（因为假定没有全局变量）
        resetRegisters();
        clearVariableList(varTable->varListReg);
        clearVariableList(varTable->varListMem);

        // main函数单独处理一下，在main里调用函数不算函数嵌套调用
        if (!strcmp(interCode->u.sinOp.op->u.name, "main")) {
            varTable->inFunc = false;
            varTable->curFuncName = NULL;
        } else {
            varTable->inFunc = true;
            varTable->curFuncName = interCode->u.sinOp.op->u.name;

            // 处理形参 PARAM
            TableItem item = searchTableItem(interCode->u.sinOp.op->u.name);
            int argc = 0;
            InterCodes temp = interCodes->next;
            while (temp && temp->code->kind == PARAM) {
                // 前4个参数存到a0 到a3中
                if (argc < 4) {
                    addVarible(varTable->varListReg, A0 + argc,
                               temp->code->u.sinOp.op);
                } else {
                    // 剩下的要用栈存
                    int regNo = checkVarible(fp, temp->code->u.sinOp.op);
                    fprintf(
                        fp, "  lw %s, %d($fp)\n",
                        registers->regList[regNo]->name,
                        (item->fieldList->type->u.function.argc - 1 - argc) * 4);
                }
                argc++;
                temp = temp->next;
            }
        }
    } else if (kind == GOTO) {
        fprintf(fp, "  j %s\n", interCode->u.sinOp.op->u.name);
    } else if (kind == RETURN) {
        int RegNo = checkVarible(fp, interCode->u.sinOp.op);
        fprintf(fp, "  move $v0, %s\n", registers->regList[RegNo]->name);
        fprintf(fp, "  jr $ra\n");
    } else if (kind == ARG) {
        // 需要在call里处理
    } else if (kind == PARAM) {
        // 需要在function里处理
    } else if (kind == READ) {
        fprintf(fp, "  addi $sp, $sp, -4\n");
        fprintf(fp, "  sw $ra, 0($sp)\n");
        fprintf(fp, "  jal read\n");
        fprintf(fp, "  lw $ra, 0($sp)\n");
        fprintf(fp, "  addi $sp, $sp, 4\n");
        int RegNo = checkVarible(fp, interCode->u.sinOp.op);
        fprintf(fp, "  move %s, $v0\n", registers->regList[RegNo]->name);
    } else if (kind == WRITE) {
        int RegNo = checkVarible(fp, interCode->u.sinOp.op);
        if (varTable->inFunc == false) {
            fprintf(fp, "  move $a0, %s\n", registers->regList[RegNo]->name);
            fprintf(fp, "  addi $sp, $sp, -4\n");
            fprintf(fp, "  sw $ra, 0($sp)\n");
            fprintf(fp, "  jal write\n");
            fprintf(fp, "  lw $ra, 0($sp)\n");
            fprintf(fp, "  addi $sp, $sp, 4\n");
        } else {
            // 函数嵌套调用，先将a0压栈 调用结束以后需要恢复a0
            fprintf(fp, "  addi $sp, $sp, -8\n");
            fprintf(fp, "  sw $a0, 0($sp)\n");
            fprintf(fp, "  sw $ra, 4($sp)\n");
            fprintf(fp, "  move $a0, %s\n", registers->regList[RegNo]->name);
            fprintf(fp, "  jal write\n");
            fprintf(fp, "  lw $a0, 0($sp)\n");
            fprintf(fp, "  lw $ra, 4($sp)\n");
            fprintf(fp, "  addi $sp, $sp, 8\n");
        }
    } else if (kind == ASSIGN) {
        int leftRegNo = checkVarible(fp, interCode->u.assign.left);
        // 右值为立即数，直接放左值寄存器里
        if (interCode->u.assign.right->kind == CONSTANT) {
            fprintf(fp, "  li %s, %d\n", registers->regList[leftRegNo]->name,
                    interCode->u.assign.right->u.val);
        }
        // 右值为变量，先check再move赋值寄存器
        else {
            int rightRegNo = checkVarible(fp, interCode->u.assign.right);
            fprintf(fp, "  move %s, %s\n", registers->regList[leftRegNo]->name,
                    registers->regList[rightRegNo]->name);
        }
    } else if (kind == GET_ADDR) {
        int leftRegNo = checkVarible(fp, interCode->u.assign.left);
        fprintf(fp, "  la %s, %s\n", registers->regList[leftRegNo]->name,
                interCode->u.assign.right->u.name);
    } else if (kind == READ_ADDR) {
        int leftRegNo = checkVarible(fp, interCode->u.assign.left);
        int rightRegNo = checkVarible(fp, interCode->u.assign.right);
        fprintf(fp, "  lw %s, 0(%s)\n", registers->regList[leftRegNo]->name,
                registers->regList[rightRegNo]->name);
    } else if (kind == WRITE_ADDR) {
        int leftRegNo = checkVarible(fp, interCode->u.assign.left);
        int rightRegNo = checkVarible(fp, interCode->u.assign.right);
        fprintf(fp, "  sw %s, 0(%s)\n", registers->regList[rightRegNo]->name,
                registers->regList[leftRegNo]->name);
    } else if (kind == CALL) {
        TableItem calledFunc =
            searchTableItem(interCode->u.assign.right->u.name);
        int leftRegNo = checkVarible(fp, interCode->u.assign.left);
        // 函数调用前的准备
        fprintf(fp, "  addi $sp, $sp, -4\n");
        fprintf(fp, "  sw $ra, 0($sp)\n");
        pusha(fp);

        // 如果是函数嵌套调用，把前形参存到内存，腾出a0-a3寄存器给新调用使用
        if (varTable->inFunc) {
            fprintf(fp, "  addi $sp, $sp, -%d\n",
                    calledFunc->fieldList->type->u.function.argc * 4);
            TableItem curFunc = searchTableItem(varTable->curFuncName);
            for (int i = 0; i < curFunc->fieldList->type->u.function.argc; i++) {
                if (i > calledFunc->fieldList->type->u.function.argc) break;
                if (i < 4) {
                    fprintf(fp, "  sw %s, %d($sp)\n",
                            registers->regList[A0 + i]->name, i * 4);
                    Varible var = varTable->varListReg->head;
                    while (var && var->regNo != A0 + i) {
                        var = var->next;
                    }
                    delVarible(varTable->varListReg, var);
                    addVarible(varTable->varListMem, -1, var->op);
                    int regNo = checkVarible(fp, var->op);
                    fprintf(fp, "  move %s, %s\n",
                            registers->regList[regNo]->name,
                            registers->regList[A0 + i]->name);
                }
            }
        }

        // 处理实参 ARG
        InterCodes arg = interCodes->prev;
        int argc = 0;
        while (arg && argc < calledFunc->fieldList->type->u.function.argc) {
            if (arg->code->kind == ARG) {
                int argRegNo = checkVarible(fp, arg->code->u.sinOp.op);
                // 前4个参数直接用寄存器存
                if (argc < 4) {
                    fprintf(fp, "  move %s, %s\n",
                            registers->regList[A0 + argc]->name,
                            registers->regList[argRegNo]->name);
                    argc++;
                }
                // 4个以后的参数压栈
                else {
                    fprintf(fp, "  addi $sp, $sp, -4\n");
                    fprintf(fp, "  sw %s, 0($sp)\n",
                            registers->regList[argRegNo]->name);
                    fprintf(fp, "  move $fp, $sp\n");
                    argc++;
                }
            }
            arg = arg->prev;
        }

        // 函数调用
        fprintf(fp, "  jal %s\n", interCode->u.assign.right->u.name);

        // 调用完后恢复栈指针、形参，然后恢复之前保存入栈的寄存器信息
        if (argc > 4) fprintf(fp, "  addi $sp, $sp, %d\n", 4 * argc);
        if (varTable->inFunc) {
            TableItem curFunc = searchTableItem(varTable->curFuncName);
            for (int i = 0; i < curFunc->fieldList->type->u.function.argc; i++) {
                if (i > calledFunc->fieldList->type->u.function.argc) break;
                if (i < 4) {
                    fprintf(fp, "  lw %s, %d($sp)\n",
                            registers->regList[A0 + i]->name, i * 4);
                    Varible var = varTable->varListReg->head;
                    while (var) {
                        if (var->op->kind != CONSTANT &&
                            !strcmp(varTable->varListMem->head->op->u.name,
                                    var->op->u.name))
                            break;
                        var = var->next;
                    }
                    if (var) {
                        registers->regList[var->regNo]->avail = true;
                        var->regNo = A0 + i;
                    } else {
                        addVarible(varTable->varListReg, A0 + i,
                                   varTable->varListMem->head->op);
                    }
                    delVarible(varTable->varListMem,
                               varTable->varListMem->head);
                }
            }
            fprintf(fp, "  addi $sp, $sp, %d\n",
                    calledFunc->fieldList->type->u.function.argc * 4);
        }
        popa(fp);
        fprintf(fp, "  lw $ra, 0($sp)\n");
        fprintf(fp, "  addi $sp, $sp, 4\n");
        fprintf(fp, "  move %s, $v0\n", registers->regList[leftRegNo]->name);
    } else if (kind == ADD) {
        int resultRegNo = checkVarible(fp, interCode->u.binOp.result);
        // 常数 常数
        if (interCode->u.binOp.op1->kind == CONSTANT &&
            interCode->u.binOp.op2->kind == CONSTANT) {
            fprintf(fp, "  li %s, %d\n", registers->regList[resultRegNo]->name,
                    interCode->u.binOp.op1->u.val +
                        interCode->u.binOp.op2->u.val);
        }
        // 变量 常数
        else if (interCode->u.binOp.op1->kind != CONSTANT &&
                 interCode->u.binOp.op2->kind == CONSTANT) {
            int op1RegNo = checkVarible(fp, interCode->u.binOp.op1);
            fprintf(fp, "  addi %s, %s, %d\n",
                    registers->regList[resultRegNo]->name,
                    registers->regList[op1RegNo]->name,
                    interCode->u.binOp.op2->u.val);
        }
        // 变量 变量
        else {
            int op1RegNo = checkVarible(fp, interCode->u.binOp.op1);
            int op2RegNo = checkVarible(fp, interCode->u.binOp.op2);
            fprintf(fp, "  add %s, %s, %s\n",
                    registers->regList[resultRegNo]->name,
                    registers->regList[op1RegNo]->name,
                    registers->regList[op2RegNo]->name);
        }
    } else if (kind == SUB) {
        int resultRegNo = checkVarible(fp, interCode->u.binOp.result);
        // 常数 常数
        if (interCode->u.binOp.op1->kind == CONSTANT &&
            interCode->u.binOp.op2->kind == CONSTANT) {
            fprintf(fp, "  li %s, %d\n", registers->regList[resultRegNo]->name,
                    interCode->u.binOp.op1->u.val -
                        interCode->u.binOp.op2->u.val);
        }
        // 变量 常数
        else if (interCode->u.binOp.op1->kind != CONSTANT &&
                 interCode->u.binOp.op2->kind == CONSTANT) {
            int op1RegNo = checkVarible(fp, interCode->u.binOp.op1);
            fprintf(fp, "  addi %s, %s, %d\n",
                    registers->regList[resultRegNo]->name,
                    registers->regList[op1RegNo]->name,
                    -interCode->u.binOp.op2->u.val);
        }
        // 变量 变量
        else {
            int op1RegNo = checkVarible(fp, interCode->u.binOp.op1);
            int op2RegNo = checkVarible(fp, interCode->u.binOp.op2);
            fprintf(fp, "  sub %s, %s, %s\n",
                    registers->regList[resultRegNo]->name,
                    registers->regList[op1RegNo]->name,
                    registers->regList[op2RegNo]->name);
        }
    } else if (kind == MUL) {
        int resultRegNo = checkVarible(fp, interCode->u.binOp.result);
        int op1RegNo = checkVarible(fp, interCode->u.binOp.op1);
        int op2RegNo = checkVarible(fp, interCode->u.binOp.op2);
        fprintf(fp, "  mul %s, %s, %s\n", registers->regList[resultRegNo]->name,
                registers->regList[op1RegNo]->name,
                registers->regList[op2RegNo]->name);
    } else if (kind == DIV) {
        int resultRegNo = checkVarible(fp, interCode->u.binOp.result);
        int op1RegNo = checkVarible(fp, interCode->u.binOp.op1);
        int op2RegNo = checkVarible(fp, interCode->u.binOp.op2);
        fprintf(fp, "  div %s, %s\n", registers->regList[op1RegNo]->name,
                registers->regList[op2RegNo]->name);
        fprintf(fp, "  mflo %s\n", registers->regList[resultRegNo]->name);
    } else if (kind == DEC) {
        // init 时候添加到全局变量里了
    } else if (kind == IF_GOTO) {
        char* relopName = interCode->u.ifGoto.relop->u.name;
        int xRegNo = checkVarible(fp, interCode->u.ifGoto.x);
        int yRegNo = checkVarible(fp, interCode->u.ifGoto.y);
        if (!strcmp(relopName, "=="))
            fprintf(fp, "  beq %s, %s, %s\n", registers->regList[xRegNo]->name,
                    registers->regList[yRegNo]->name,
                    interCode->u.ifGoto.z->u.name);
        else if (!strcmp(relopName, "!="))
            fprintf(fp, "  bne %s, %s, %s\n", registers->regList[xRegNo]->name,
                    registers->regList[yRegNo]->name,
                    interCode->u.ifGoto.z->u.name);
        else if (!strcmp(relopName, ">"))
            fprintf(fp, "  bgt %s, %s, %s\n", registers->regList[xRegNo]->name,
                    registers->regList[yRegNo]->name,
                    interCode->u.ifGoto.z->u.name);
        else if (!strcmp(relopName, "<"))
            fprintf(fp, "  blt %s, %s, %s\n", registers->regList[xRegNo]->name,
                    registers->regList[yRegNo]->name,
                    interCode->u.ifGoto.z->u.name);
        else if (!!strcmp(relopName, ">="))
            fprintf(fp, "  bge %s, %s, %s\n", registers->regList[xRegNo]->name,
                    registers->regList[yRegNo]->name,
                    interCode->u.ifGoto.z->u.name);
        else if (strcmp(relopName, "<="))
            fprintf(fp, "  ble %s, %s, %s\n", registers->regList[xRegNo]->name,
                    registers->regList[yRegNo]->name,
                    interCode->u.ifGoto.z->u.name);
    }
}

void pusha(FILE* fp) {
    fprintf(fp, "  addi $sp, $sp, -72\n");
    for (int i = T0; i <= T9; i++) {
        fprintf(fp, "  sw %s, %d($sp)\n", registers->regList[i]->name,
                (i - T0) * 4);
    }
}

void popa(FILE* fp) {
    for (int i = T0; i <= T9; i++) {
        fprintf(fp, "  lw %s, %d($sp)\n", registers->regList[i]->name,
                (i - T0) * 4);
    }
    fprintf(fp, "  addi $sp, $sp, 72\n");
}
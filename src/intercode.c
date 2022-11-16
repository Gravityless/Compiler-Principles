#include "intercode.h"

bool interError = false;
InterCodeList interCodeList;

// Operand func
Operand newOperand(int kind, ...) {
    Operand p = (Operand)malloc(sizeof(struct Operand_));
    p->kind = kind;
    va_list ap;
    va_start(ap, kind);
    switch (kind) {
        case OP_CONSTANT:
            p->u.val = va_arg(ap, int);
            break;
        default:
            p->u.name = va_arg(ap, char*);
            break;
    }
    return p;
}

void setOperand(Operand p, int kind, char* val) {
    p->kind = kind;
    switch (kind) {
        case OP_CONSTANT:
            p->u.val = atoi(val);
            break;
        default:
            if (p->u.name) free(p->u.name);
            p->u.name = val;
            break;
    }
}

void deleteOperand(Operand p) {
    if (p == NULL) return;
    switch (p->kind) {
        case OP_CONSTANT:
            break;
        default:
            if (p->u.name) free(p->u.name);
            break;
    }
    free(p);
}

void printOp(FILE* fp, Operand op) {
    switch (op->kind) {
        case OP_CONSTANT:
            fprintf(fp, "#%d", op->u.val);
            break;
        default:
            fprintf(fp, "%s", op->u.name);
            break;
    }
}

// InterCode func
InterCode newInterCode(int kind, ...) {
    InterCode p = (InterCode)malloc(sizeof(struct InterCode_));
    p->kind = kind;
    va_list ap;
    switch (kind) {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_RETURN:
        case IR_ARG:
        case IR_PARAM:
        case IR_READ:
        case IR_WRITE:
            va_start(ap, kind);
            p->u.sinOp.op = va_arg(ap, Operand);
            break;
        case IR_ASSIGN:
        case IR_GET_ADDR:
        case IR_READ_ADDR:
        case IR_WRITE_ADDR:
        case IR_CALL:
            va_start(ap, kind);
            p->u.assign.left = va_arg(ap, Operand);
            p->u.assign.right = va_arg(ap, Operand);
            break;
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
            va_start(ap, kind);
            p->u.binOp.result = va_arg(ap, Operand);
            p->u.binOp.op1 = va_arg(ap, Operand);
            p->u.binOp.op2 = va_arg(ap, Operand);
            break;
        case IR_DEC:
            va_start(ap, kind);
            p->u.dec.op = va_arg(ap, Operand);
            p->u.dec.size = va_arg(ap, int);
            break;
        case IR_IF_GOTO:
            va_start(ap, kind);
            p->u.ifGoto.x = va_arg(ap, Operand);
            p->u.ifGoto.relop = va_arg(ap, Operand);
            p->u.ifGoto.y = va_arg(ap, Operand);
            p->u.ifGoto.z = va_arg(ap, Operand);
    }
    return p;
}

void deleteInterCode(InterCode p) {
    switch (p->kind) {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_RETURN:
        case IR_ARG:
        case IR_PARAM:
        case IR_READ:
        case IR_WRITE:
            deleteOperand(p->u.sinOp.op);
            break;
        case IR_ASSIGN:
        case IR_GET_ADDR:
        case IR_READ_ADDR:
        case IR_WRITE_ADDR:
        case IR_CALL:
            deleteOperand(p->u.assign.left);
            deleteOperand(p->u.assign.right);
            break;
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
            deleteOperand(p->u.binOp.result);
            deleteOperand(p->u.binOp.op1);
            deleteOperand(p->u.binOp.op2);
            break;
        case IR_DEC:
            deleteOperand(p->u.dec.op);
            break;
        case IR_IF_GOTO:
            deleteOperand(p->u.ifGoto.x);
            deleteOperand(p->u.ifGoto.relop);
            deleteOperand(p->u.ifGoto.y);
            deleteOperand(p->u.ifGoto.z);
    }
    free(p);
}

void printInterCode(FILE* fp, InterCodeList interCodeList) {
    for (InterCodes cur = interCodeList->head; cur != NULL; cur = cur->next) {
        switch (cur->code->kind) {
            case IR_LABEL:
                fprintf(fp, "LABEL ");
                printOp(fp, cur->code->u.sinOp.op);
                fprintf(fp, " :");
                break;
            case IR_FUNCTION:
                fprintf(fp, "FUNCTION ");
                printOp(fp, cur->code->u.sinOp.op);
                fprintf(fp, " :");
                break;
            case IR_ASSIGN:
                printOp(fp, cur->code->u.assign.left);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_ADD:
                printOp(fp, cur->code->u.binOp.result);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.binOp.op1);
                fprintf(fp, " + ");
                printOp(fp, cur->code->u.binOp.op2);
                break;
            case IR_SUB:
                printOp(fp, cur->code->u.binOp.result);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.binOp.op1);
                fprintf(fp, " - ");
                printOp(fp, cur->code->u.binOp.op2);
                break;
            case IR_MUL:
                printOp(fp, cur->code->u.binOp.result);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.binOp.op1);
                fprintf(fp, " * ");
                printOp(fp, cur->code->u.binOp.op2);
                break;
            case IR_DIV:
                printOp(fp, cur->code->u.binOp.result);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.binOp.op1);
                fprintf(fp, " / ");
                printOp(fp, cur->code->u.binOp.op2);
                break;
            case IR_GET_ADDR:
                printOp(fp, cur->code->u.assign.left);
                fprintf(fp, " := &");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_READ_ADDR:
                printOp(fp, cur->code->u.assign.left);
                fprintf(fp, " := *");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_WRITE_ADDR:
                fprintf(fp, "*");
                printOp(fp, cur->code->u.assign.left);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_GOTO:
                fprintf(fp, "GOTO ");
                printOp(fp, cur->code->u.sinOp.op);
                break;
            case IR_IF_GOTO:
                fprintf(fp, "IF ");
                printOp(fp, cur->code->u.ifGoto.x);
                fprintf(fp, " ");
                printOp(fp, cur->code->u.ifGoto.relop);
                fprintf(fp, " ");
                printOp(fp, cur->code->u.ifGoto.y);
                fprintf(fp, " GOTO ");
                printOp(fp, cur->code->u.ifGoto.z);
                break;
            case IR_RETURN:
                fprintf(fp, "RETURN ");
                printOp(fp, cur->code->u.sinOp.op);
                break;
            case IR_DEC:
                fprintf(fp, "DEC ");
                printOp(fp, cur->code->u.dec.op);
                fprintf(fp, " ");
                fprintf(fp, "%d", cur->code->u.dec.size);
                break;
            case IR_ARG:
                fprintf(fp, "ARG ");
                printOp(fp, cur->code->u.sinOp.op);
                break;
            case IR_CALL:
                printOp(fp, cur->code->u.assign.left);
                fprintf(fp, " := CALL ");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_PARAM:
                fprintf(fp, "PARAM ");
                printOp(fp, cur->code->u.sinOp.op);
                break;
            case IR_READ:
                fprintf(fp, "READ ");
                printOp(fp, cur->code->u.sinOp.op);
                break;
            case IR_WRITE:
                fprintf(fp, "WRITE ");
                printOp(fp, cur->code->u.sinOp.op);
                break;
        }
        fprintf(fp, "\n");
    }
}

// InterCodes func
InterCodes newInterCodes(InterCode code) {
    InterCodes p = (InterCodes)malloc(sizeof(struct InterCodes_));
    p->code = code;
    p->prev = NULL;
    p->next = NULL;
}

void deleteInterCodes(InterCodes p) {
    deleteInterCode(p->code);
    free(p);
}

// Arg and ArgList func
Arg newArg(Operand op) {
    Arg p = (Arg)malloc(sizeof(struct Arg_));
    p->op = op;
    p->next = NULL;
}

ArgList newArgList() {
    ArgList p = (ArgList)malloc(sizeof(struct ArgList_));
    p->head = NULL;
    p->cur = NULL;
}

void deleteArg(Arg p) {
    deleteOperand(p->op);
    free(p);
}

void deleteArgList(ArgList p) {
    Arg q = p->head;
    while (q) {
        Arg temp = q;
        q = q->next;
        deleteArg(temp);
    }
    free(p);
}

void addArg(ArgList argList, Arg arg) {
    if (argList->head == NULL) {
        argList->head = arg;
        argList->cur = arg;
    } else {
        argList->cur->next = arg;
        argList->cur = arg;
    }
}

// InterCodeList func
InterCodeList newInterCodeList() {
    InterCodeList p = (InterCodeList)malloc(sizeof(struct InterCodeList_));
    p->head = NULL;
    p->cur = NULL;
    p->lastArrayName = NULL;
    p->tempVarNum = 1;
    p->labelNum = 1;
}

void deleteInterCodeList(InterCodeList p) {
    InterCodes q = p->head;
    while (q) {
        InterCodes temp = q;
        q = q->next;
        deleteInterCodes(temp);
    }
    free(p);
}

void addInterCode(InterCodeList interCodeList, InterCodes newCode) {
    if (interCodeList->head == NULL) {
        interCodeList->head = newCode;
        interCodeList->cur = newCode;
    } else {
        interCodeList->cur->next = newCode;
        newCode->prev = interCodeList->cur;
        interCodeList->cur = newCode;
    }
}

// traverse func
Operand newTemp() {
    char tName[10] = {0};
    sprintf(tName, "t%d", interCodeList->tempVarNum);
    interCodeList->tempVarNum++;
    Operand temp = newOperand(OP_VARIABLE, newString(tName));
    return temp;
}

Operand newLabel() {
    char lName[10] = {0};
    sprintf(lName, "label%d", interCodeList->labelNum);
    interCodeList->labelNum++;
    Operand temp = newOperand(OP_LABEL, newString(lName));
    return temp;
}

int getSize(Type type) {
    if (type == NULL)
        return 0;
    else if (type->kind == BASIC)
        return 4;
    else if (type->kind == ARRAY)
        return type->u.array.size * getSize(type->u.array.elem);
    else if (type->kind == STRUCTURE) {
        int size = 0;
        FieldList temp = type->u.structure.fieldList;
        while (temp) {
            size += getSize(temp->type);
            temp = temp->tail;
        }
        return size;
    }
    return 0;
}

void genInterCodes(Node *node) {
    if (node == NULL) return;
    if (!strcmp(node->type, "ExtDefList"))
        transExtDefList(node);
    else {
        genInterCodes(node->child);
        genInterCodes(node->sibling);
    }
}

void genInterCode(int kind, ...) {
    va_list ap;
    Operand temp = NULL;
    Operand result = NULL, op1 = NULL, op2 = NULL, relop = NULL;
    int size = 0;
    InterCodes newCode = NULL;
    switch (kind) {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_RETURN:
        case IR_ARG:
        case IR_PARAM:
        case IR_READ:
        case IR_WRITE:
            va_start(ap, kind);
            op1 = va_arg(ap, Operand);
            if (op1->kind == OP_ADDRESS) {
                temp = newTemp();
                genInterCode(IR_READ_ADDR, temp, op1);
                op1 = temp;
            }
            newCode = newInterCodes(newInterCode(kind, op1));
            addInterCode(interCodeList, newCode);
            break;
        case IR_ASSIGN:
        case IR_GET_ADDR:
        case IR_READ_ADDR:
        case IR_WRITE_ADDR:
        case IR_CALL:
            va_start(ap, kind);
            op1 = va_arg(ap, Operand);
            op2 = va_arg(ap, Operand);
            if (kind == IR_ASSIGN &&
                (op1->kind == OP_ADDRESS || op2->kind == OP_ADDRESS)) {
                if (op1->kind == OP_ADDRESS && op2->kind != OP_ADDRESS)
                    genInterCode(IR_WRITE_ADDR, op1, op2);
                else if (op2->kind == OP_ADDRESS && op1->kind != OP_ADDRESS)
                    genInterCode(IR_READ_ADDR, op1, op2);
                else {
                    temp = newTemp();
                    genInterCode(IR_READ_ADDR, temp, op2);
                    genInterCode(IR_WRITE_ADDR, op1, temp);
                }
            } else {
                newCode = newInterCodes(newInterCode(kind, op1, op2));
                addInterCode(interCodeList, newCode);
            }
            break;
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
            va_start(ap, kind);
            result = va_arg(ap, Operand);
            op1 = va_arg(ap, Operand);
            op2 = va_arg(ap, Operand);
            if (op1->kind == OP_ADDRESS) {
                temp = newTemp();
                genInterCode(IR_READ_ADDR, temp, op1);
                op1 = temp;
            }
            if (op2->kind == OP_ADDRESS) {
                temp = newTemp();
                genInterCode(IR_READ_ADDR, temp, op2);
                op2 = temp;
            }
            newCode = newInterCodes(newInterCode(kind, result, op1, op2));
            addInterCode(interCodeList, newCode);
            break;
        case IR_DEC:
            va_start(ap, kind);
            op1 = va_arg(ap, Operand);
            size = va_arg(ap, int);
            newCode = newInterCodes(newInterCode(kind, op1, size));
            addInterCode(interCodeList, newCode);
            break;
        case IR_IF_GOTO:
            va_start(ap, kind);
            result = va_arg(ap, Operand);
            relop = va_arg(ap, Operand);
            op1 = va_arg(ap, Operand);
            op2 = va_arg(ap, Operand);
            newCode =
                newInterCodes(newInterCode(kind, result, relop, op1, op2));
            addInterCode(interCodeList, newCode);
            break;
    }
}
void transExtDefList(Node *node) {
    while (node) {
        transExtDef(node->child);
        node = node->child->sibling;
    }
}

void transExtDef(Node *node) {
    if (interError) return;
    if (!strcmp(node->child->sibling->type, "FunDec")) {
        transFunDec(node->child->sibling);
        transCompSt(node->child->sibling->sibling);
    }
}

void transFunDec(Node *node) {
    if (interError) return;
    genInterCode(IR_FUNCTION,
                 newOperand(OP_FUNCTION, newString(node->child->val)));

    TableItem funcItem = searchTableItem(node->child->val);
    FieldList temp = funcItem->fieldList->type->u.function.argv;
    while (temp) {
        genInterCode(IR_PARAM, newOperand(OP_VARIABLE, newString(temp->name)));
        temp = temp->tail;
    }
}

void transCompSt(Node *node) {
    if (interError) return;
    Node *temp = node->child->sibling;
    if (!strcmp(temp->type, "DefList")) {
        transDefList(temp);
        temp = temp->sibling;
    }
    if (!strcmp(temp->type, "StmtList")) {
        transStmtList(temp);
    }
}

void transDefList(Node *node) {
    if (interError) return;
    while (node) {
        transDef(node->child);
        node = node->child->sibling;
    }
}

void transDef(Node *node) {
    if (interError) return;
    transDecList(node->child->sibling);
}

void transDecList(Node *node) {
    if (interError) return;
    Node *temp = node;
    while (temp) {
        transDec(temp->child);
        if (temp->child->sibling)
            temp = temp->child->sibling->sibling;
        else
            break;
    }
}

void transDec(Node *node) {
    if (interError) return;
    if (node->child->sibling == NULL) {
        transVarDec(node->child, NULL);
    } else {
        Operand t1 = newTemp();
        transVarDec(node->child, t1);
        Operand t2 = newTemp();
        transExp(node->child->sibling->sibling, t2);
        genInterCode(IR_ASSIGN, t1, t2);
    }
}

void transVarDec(Node *node, Operand place) {
    if (interError) return;
    if (!strcmp(node->child->type, "ID")) {
        TableItem temp = searchTableItem(node->child->val);
        Type type = temp->fieldList->type;
        if (type->kind == BASIC) {
            if (place) {
                interCodeList->tempVarNum--;
                setOperand(place, OP_VARIABLE,
                           newString(temp->fieldList->name));
            }
        } else if (type->kind == ARRAY) {
            if (type->u.array.elem->kind == ARRAY) {
                interError = true;
                printf("Cannot translate multi-dimensional array type or parameters of array type.\n");
                return;
            } else {
                genInterCode(
                    IR_DEC,
                    newOperand(OP_VARIABLE, newString(temp->fieldList->name)),
                    getSize(type));
            }
        } else if (type->kind == STRUCTURE) {
            genInterCode(IR_DEC,
                         newOperand(OP_VARIABLE, newString(temp->fieldList->name)),
                         getSize(type));
        }
    } else {
        transVarDec(node->child, place);
    }
}

void transStmtList(Node *node) {
    if (interError) return;
    while (node) {
        transStmt(node->child);
        node = node->child->sibling;
    }
}

void transStmt(Node *node) {
    if (interError) return;
    if (!strcmp(node->child->type, "Exp")) {
        transExp(node->child, NULL);
    } else if (!strcmp(node->child->type, "CompSt")) {
        transCompSt(node->child);
    } else if (!strcmp(node->child->type, "RETURN")) {
        Operand t1 = newTemp();
        transExp(node->child->sibling, t1);
        genInterCode(IR_RETURN, t1);
    } else if (!strcmp(node->child->type, "IF")) {
        Node *exp = node->child->sibling->sibling;
        Node *stmt = exp->sibling->sibling;
        Operand label1 = newLabel();
        Operand label2 = newLabel();

        transCond(exp, label1, label2);
        genInterCode(IR_LABEL, label1);
        transStmt(stmt);
        if (stmt->sibling == NULL) {
            genInterCode(IR_LABEL, label2);
        } else {
            Operand label3 = newLabel();
            genInterCode(IR_GOTO, label3);
            genInterCode(IR_LABEL, label2);
            transStmt(stmt->sibling->sibling);
            genInterCode(IR_LABEL, label3);
        }

    } else if (!strcmp(node->child->type, "WHILE")) {
        Operand label1 = newLabel();
        Operand label2 = newLabel();
        Operand label3 = newLabel();

        genInterCode(IR_LABEL, label1);
        transCond(node->child->sibling->sibling, label2, label3);
        genInterCode(IR_LABEL, label2);
        transStmt(node->child->sibling->sibling->sibling->sibling);
        genInterCode(IR_GOTO, label1);
        genInterCode(IR_LABEL, label3);
    }
}

void transExp(Node *node, Operand place) {
    if (interError) return;
    if (!strcmp(node->child->type, "LP"))
        transExp(node->child->sibling, place);
    else if (!strcmp(node->child->type, "Exp") || !strcmp(node->child->type, "NOT")) {
        if (strcmp(node->child->sibling->type, "LB") &&
            strcmp(node->child->sibling->type, "DOT")) {
            if (!strcmp(node->child->sibling->type, "AND") ||
                !strcmp(node->child->sibling->type, "OR") ||
                !strcmp(node->child->sibling->type, "RELOP") ||
                !strcmp(node->child->type, "NOT")) {
                Operand label1 = newLabel();
                Operand label2 = newLabel();
                Operand true_num = newOperand(OP_CONSTANT, 1);
                Operand false_num = newOperand(OP_CONSTANT, 0);
                genInterCode(IR_ASSIGN, place, false_num);
                transCond(node, label1, label2);
                genInterCode(IR_LABEL, label1);
                genInterCode(IR_ASSIGN, place, true_num);
            } else {
                if (!strcmp(node->child->sibling->type, "ASSIGNOP")) {
                    Operand t2 = newTemp();
                    transExp(node->child->sibling->sibling, t2);
                    Operand t1 = newTemp();
                    transExp(node->child, t1);
                    genInterCode(IR_ASSIGN, t1, t2);
                } else {
                    Operand t1 = newTemp();
                    transExp(node->child, t1);
                    Operand t2 = newTemp();
                    transExp(node->child->sibling->sibling, t2);
                    if (!strcmp(node->child->sibling->type, "PLUS")) {
                        genInterCode(IR_ADD, place, t1, t2);
                    } else if (!strcmp(node->child->sibling->type, "MINUS")) {
                        genInterCode(IR_SUB, place, t1, t2);
                    } else if (!strcmp(node->child->sibling->type, "STAR")) {
                        genInterCode(IR_MUL, place, t1, t2);
                    } else if (!strcmp(node->child->sibling->type, "DIV")) {
                        genInterCode(IR_DIV, place, t1, t2);
                    }
                }
            }

        } else {
            if (!strcmp(node->child->sibling->type, "LB")) {
                if (node->child->child->sibling &&
                    !strcmp(node->child->child->sibling->type, "LB")) {
                    interError = true;
                    printf("Cannot translate multi-dimensional array type or parameters of array type.\n");
                    return;
                } else {
                    Operand idx = newTemp();
                    transExp(node->child->sibling->sibling, idx);
                    Operand base = newTemp();
                    transExp(node->child, base);

                    Operand width;
                    Operand offset = newTemp();
                    Operand target;
                    TableItem item = searchTableItem(base->u.name);
                    width = newOperand(
                        OP_CONSTANT, getSize(item->fieldList->type->u.array.elem));
                    genInterCode(IR_MUL, offset, idx, width);
                    if (base->kind == OP_VARIABLE) {
                        target = newTemp();
                        genInterCode(IR_GET_ADDR, target, base);
                    } else {
                        target = base;
                    }
                    genInterCode(IR_ADD, place, target, offset);
                    place->kind = OP_ADDRESS;
                    interCodeList->lastArrayName = base->u.name;
                }
            }
            else {
                Operand temp = newTemp();
                transExp(node->child, temp);
                Operand target;

                if (temp->kind == OP_ADDRESS) {
                    target = newOperand(temp->kind, temp->u.name);
                } else {
                    target = newTemp();
                    genInterCode(IR_GET_ADDR, target, temp);
                }

                Operand id = newOperand(
                    OP_VARIABLE, newString(node->child->sibling->sibling->val));
                int offset = 0;
                TableItem item = searchTableItem(temp->u.name);
                if (item == NULL) {
                    item = searchTableItem(interCodeList->lastArrayName);
                }

                FieldList tmp;
                if (item->fieldList->type->kind == ARRAY) {
                    tmp = item->fieldList->type->u.array.elem->u.structure.fieldList;
                }
                else {
                    tmp = item->fieldList->type->u.structure.fieldList;
                }
                while (tmp) {
                    if (!strcmp(tmp->name, id->u.name)) break;
                    offset += getSize(tmp->type);
                    tmp = tmp->tail;
                }

                Operand tOffset = newOperand(OP_CONSTANT, offset);
                if (place) {
                    genInterCode(IR_ADD, place, target, tOffset);
                    setOperand(place, OP_ADDRESS, newString(id->u.name));
                }
            }
        }
    }
    else if (!strcmp(node->child->type, "MINUS")) {
        Operand t1 = newTemp();
        transExp(node->child->sibling, t1);
        Operand zero = newOperand(OP_CONSTANT, 0);
        genInterCode(IR_SUB, place, zero, t1);
    }
    else if (!strcmp(node->child->type, "ID") && node->child->sibling) {
        Operand funcTemp =
            newOperand(OP_FUNCTION, newString(node->child->val));
        if (!strcmp(node->child->sibling->sibling->type, "Args")) {
            ArgList argList = newArgList();
            transArgs(node->child->sibling->sibling, argList);
            if (!strcmp(node->child->val, "write")) {
                genInterCode(IR_WRITE, argList->head->op);
            } else {
                Arg argTemp = argList->head;
                while (argTemp) {
                    if (argTemp->op == OP_VARIABLE) {
                        TableItem item =
                            searchTableItem(argTemp->op->u.name);

                        if (item && item->fieldList->type->kind == STRUCTURE) {
                            Operand varTemp = newTemp();
                            genInterCode(IR_GET_ADDR, varTemp, argTemp->op);
                            Operand varTempCopy =
                                newOperand(OP_ADDRESS, varTemp->u.name);
                            genInterCode(IR_ARG, varTempCopy);
                        }
                    }
                    else {
                        genInterCode(IR_ARG, argTemp->op);
                    }
                    argTemp = argTemp->next;
                }
                if (place) {
                    genInterCode(IR_CALL, place, funcTemp);
                } else {
                    Operand temp = newTemp();
                    genInterCode(IR_CALL, temp, funcTemp);
                }
            }
        }
        else {
            if (!strcmp(node->child->val, "read")) {
                genInterCode(IR_READ, place);
            } else {
                if (place) {
                    genInterCode(IR_CALL, place, funcTemp);
                } else {
                    Operand temp = newTemp();
                    genInterCode(IR_CALL, temp, funcTemp);
                }
            }
        }
    }
    else if (!strcmp(node->child->type, "ID")) {
        TableItem item = searchTableItem(node->child->val);
        interCodeList->tempVarNum--;
        if (item->fieldList->isArg && item->fieldList->type->kind == STRUCTURE) {
            setOperand(place, OP_ADDRESS, newString(node->child->val));
        }
        else {
            setOperand(place, OP_VARIABLE, newString(node->child->val));
        }
    } else {
        interCodeList->tempVarNum--;
        setOperand(place, OP_CONSTANT, node->child->val);
    }
}

void transCond(Node *node, Operand labelTrue, Operand labelFalse) {
    if (interError) return;

    if (!strcmp(node->child->type, "NOT")) {
        transCond(node->child->sibling, labelFalse, labelTrue);
    }
    else if (!strcmp(node->child->sibling->type, "RELOP")) {
        Operand t1 = newTemp();
        Operand t2 = newTemp();
        transExp(node->child, t1);
        transExp(node->child->sibling->sibling, t2);

        Operand relop =
            newOperand(OP_RELOP, newString(node->child->sibling->val));

        if (t1->kind == OP_ADDRESS) {
            Operand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        if (t2->kind == OP_ADDRESS) {
            Operand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t2);
            t2 = temp;
        }

        genInterCode(IR_IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(IR_GOTO, labelFalse);
    } else if (!strcmp(node->child->sibling->type, "AND")) {
        Operand label1 = newLabel();
        transCond(node->child, label1, labelFalse);
        genInterCode(IR_LABEL, label1);
        transCond(node->child->sibling->sibling, labelTrue, labelFalse);
    } else if (!strcmp(node->child->sibling->type, "OR")) {
        Operand label1 = newLabel();
        transCond(node->child, labelTrue, label1);
        genInterCode(IR_LABEL, label1);
        transCond(node->child->sibling->sibling, labelTrue, labelFalse);
    } else {
        Operand t1 = newTemp();
        transExp(node, t1);
        Operand t2 = newOperand(OP_CONSTANT, 0);
        Operand relop = newOperand(OP_RELOP, newString("!="));

        if (t1->kind == OP_ADDRESS) {
            Operand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        genInterCode(IR_IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(IR_GOTO, labelFalse);
    }
}

void transArgs(Node *node, ArgList argList) {
    if (interError) return;

    Arg temp = newArg(newTemp());
    transExp(node->child, temp->op);

    if (temp->op->kind == OP_VARIABLE) {
        TableItem item = searchTableItem(temp->op->u.name);
        if (item && item->fieldList->type->kind == ARRAY) {
            interError = true;
            printf("Cannot translate multi-dimensional array type or parameters of array type.\n");
            return;
        }
    }
    addArg(argList, temp);

    if (node->child->sibling != NULL) {
        transArgs(node->child->sibling->sibling, argList);
    }
}

void intercodeGenerate(Node* root, FILE* fp) {
    interCodeList = newInterCodeList();
    genInterCodes(root);
    if (!interError)
        printInterCode(fp, interCodeList);
}

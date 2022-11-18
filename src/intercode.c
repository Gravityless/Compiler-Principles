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
        case CONSTANT:
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
        case CONSTANT:
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
        case CONSTANT:
            break;
        default:
            if (p->u.name) free(p->u.name);
            break;
    }
    free(p);
}

void printOperand(FILE* fp, Operand op) {
    switch (op->kind) {
        case CONSTANT:
            fprintf(fp, "#%d", op->u.val);
            break;
        default:
            fprintf(fp, "%s", op->u.name);
            break;
    }
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


// InterCodes func
InterCodes newInterCodes() {
    InterCodes p = (InterCodes)malloc(sizeof(struct InterCodes_));
    p->code = (InterCode)malloc(sizeof(struct InterCode_));
    p->prev = NULL;
    p->next = NULL;
}

void deleteInterCodes(InterCodes p) {
    InterCode code = p->code;
    switch (code->kind) {
        case LABEL:
        case FUNCTION:
        case GOTO:
        case RETURN:
        case ARG:
        case PARAM:
        case READ:
        case WRITE:
            deleteOperand(code->u.sinOp.op);
            break;
        case ASSIGN:
        case GET_ADDR:
        case READ_ADDR:
        case WRITE_ADDR:
        case CALL:
            deleteOperand(code->u.assign.left);
            deleteOperand(code->u.assign.right);
            break;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
            deleteOperand(code->u.binOp.result);
            deleteOperand(code->u.binOp.op1);
            deleteOperand(code->u.binOp.op2);
            break;
        case DEC:
            deleteOperand(code->u.dec.op);
            break;
        case IF_GOTO:
            deleteOperand(code->u.ifGoto.x);
            deleteOperand(code->u.ifGoto.relop);
            deleteOperand(code->u.ifGoto.y);
            deleteOperand(code->u.ifGoto.z);
    }
    free(code);
    free(p);
}

void addInterCode(InterCodes newCode) {
    if (interCodeList->head == NULL) {
        interCodeList->head = newCode;
        interCodeList->cur = newCode;
    } else {
        interCodeList->cur->next = newCode;
        newCode->prev = interCodeList->cur;
        interCodeList->cur = newCode;
    }
}

// InterCodeList func
InterCodeList newInterCodeList() {
    InterCodeList p = (InterCodeList)malloc(sizeof(struct InterCodeList_));
    p->head = NULL;
    p->cur = NULL;
    p->arrayName = NULL;
    p->tempVarNum = 1;
    p->labelNum = 1;
}

void deleteInterCodeList() {
    InterCodes q = interCodeList->head;
    while (q) {
        InterCodes temp = q;
        q = q->next;
        deleteInterCodes(temp);
    }
    free(interCodeList);
}

void printInterCodeList(FILE* fp) {
    InterCode code = NULL;
    for (InterCodes cur = interCodeList->head; cur != NULL; cur = cur->next) {
        code = cur->code;
        switch (code->kind) {
            case LABEL:
                fprintf(fp, "LABEL ");
                printOperand(fp, code->u.sinOp.op);
                fprintf(fp, " :");
                break;
            case FUNCTION:
                fprintf(fp, "FUNCTION ");
                printOperand(fp, code->u.sinOp.op);
                fprintf(fp, " :");
                break;
            case ASSIGN:
                printOperand(fp, code->u.assign.left);
                fprintf(fp, " := ");
                printOperand(fp, code->u.assign.right);
                break;
            case ADD:
                printOperand(fp, code->u.binOp.result);
                fprintf(fp, " := ");
                printOperand(fp, code->u.binOp.op1);
                fprintf(fp, " + ");
                printOperand(fp, code->u.binOp.op2);
                break;
            case SUB:
                printOperand(fp, code->u.binOp.result);
                fprintf(fp, " := ");
                printOperand(fp, code->u.binOp.op1);
                fprintf(fp, " - ");
                printOperand(fp, code->u.binOp.op2);
                break;
            case MUL:
                printOperand(fp, code->u.binOp.result);
                fprintf(fp, " := ");
                printOperand(fp, code->u.binOp.op1);
                fprintf(fp, " * ");
                printOperand(fp, code->u.binOp.op2);
                break;
            case DIV:
                printOperand(fp, code->u.binOp.result);
                fprintf(fp, " := ");
                printOperand(fp, code->u.binOp.op1);
                fprintf(fp, " / ");
                printOperand(fp, code->u.binOp.op2);
                break;
            case GET_ADDR:
                printOperand(fp, code->u.assign.left);
                fprintf(fp, " := &");
                printOperand(fp, code->u.assign.right);
                break;
            case READ_ADDR:
                printOperand(fp, code->u.assign.left);
                fprintf(fp, " := *");
                printOperand(fp, code->u.assign.right);
                break;
            case WRITE_ADDR:
                fprintf(fp, "*");
                printOperand(fp, code->u.assign.left);
                fprintf(fp, " := ");
                printOperand(fp, code->u.assign.right);
                break;
            case GOTO:
                fprintf(fp, "GOTO ");
                printOperand(fp, code->u.sinOp.op);
                break;
            case IF_GOTO:
                fprintf(fp, "IF ");
                printOperand(fp, code->u.ifGoto.x);
                fprintf(fp, " ");
                printOperand(fp, code->u.ifGoto.relop);
                fprintf(fp, " ");
                printOperand(fp, code->u.ifGoto.y);
                fprintf(fp, " GOTO ");
                printOperand(fp, code->u.ifGoto.z);
                break;
            case RETURN:
                fprintf(fp, "RETURN ");
                printOperand(fp, code->u.sinOp.op);
                break;
            case DEC:
                fprintf(fp, "DEC ");
                printOperand(fp, code->u.dec.op);
                fprintf(fp, " ");
                fprintf(fp, "%d", code->u.dec.size);
                break;
            case ARG:
                fprintf(fp, "ARG ");
                printOperand(fp, code->u.sinOp.op);
                break;
            case CALL:
                printOperand(fp, code->u.assign.left);
                fprintf(fp, " := CALL ");
                printOperand(fp, code->u.assign.right);
                break;
            case PARAM:
                fprintf(fp, "PARAM ");
                printOperand(fp, code->u.sinOp.op);
                break;
            case READ:
                fprintf(fp, "READ ");
                printOperand(fp, code->u.sinOp.op);
                break;
            case WRITE:
                fprintf(fp, "WRITE ");
                printOperand(fp, code->u.sinOp.op);
                break;
        }
        fprintf(fp, "\n");
    }
}

// traverse func
Operand Temp() {
    char tName[10] = {0};
    sprintf(tName, "t%d", interCodeList->tempVarNum);
    interCodeList->tempVarNum++;
    Operand temp = newOperand(VARIABLE, newString(tName));
    return temp;
}

Operand Label() {
    char lName[10] = {0};
    sprintf(lName, "label%d", interCodeList->labelNum);
    interCodeList->labelNum++;
    Operand temp = newOperand(TEXT, newString(lName));
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
    Operand temp = NULL;
    Operand result = NULL, op1 = NULL, op2 = NULL, relop = NULL;
    int size = 0;
    InterCodes interCodes = newInterCodes();
    InterCode newCode = interCodes->code;
    va_list ap;
    va_start(ap, kind);
    interCodes->code->kind = kind;
    switch (kind) {
        case LABEL:
        case FUNCTION:
        case GOTO:
        case RETURN:
        case ARG:
        case PARAM:
        case READ:
        case WRITE:
            op1 = va_arg(ap, Operand);
            if (op1->kind == ADDRESS) {
                temp = Temp();
                genInterCode(READ_ADDR, temp, op1);
                op1 = temp;
            }
            newCode->u.sinOp.op = op1;
            addInterCode(interCodes);
            break;
        case ASSIGN:
        case GET_ADDR:
        case READ_ADDR:
        case WRITE_ADDR:
        case CALL:
            op1 = va_arg(ap, Operand);
            op2 = va_arg(ap, Operand);
            if (kind == ASSIGN &&
                (op1->kind == ADDRESS || op2->kind == ADDRESS)) {
                if (op1->kind == ADDRESS && op2->kind != ADDRESS)
                    genInterCode(WRITE_ADDR, op1, op2);
                else if (op2->kind == ADDRESS && op1->kind != ADDRESS)
                    genInterCode(READ_ADDR, op1, op2);
                else {
                    temp = Temp();
                    genInterCode(READ_ADDR, temp, op2);
                    genInterCode(WRITE_ADDR, op1, temp);
                }
            } else {
                newCode->u.assign.left = op1;
                newCode->u.assign.right = op2;
                addInterCode(interCodes);
            }
            break;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
            result = va_arg(ap, Operand);
            op1 = va_arg(ap, Operand);
            op2 = va_arg(ap, Operand);
            if (op1->kind == ADDRESS) {
                temp = Temp();
                genInterCode(READ_ADDR, temp, op1);
                op1 = temp;
            }
            if (op2->kind == ADDRESS) {
                temp = Temp();
                genInterCode(READ_ADDR, temp, op2);
                op2 = temp;
            }
            newCode->u.binOp.result = result;
            newCode->u.binOp.op1 = op1;
            newCode->u.binOp.op2 = op2;            
            addInterCode(interCodes);
            break;
        case DEC:
            newCode->u.dec.op = va_arg(ap, Operand);
            newCode->u.dec.size = va_arg(ap, int);
            addInterCode(interCodes);
            break;
        case IF_GOTO:
            newCode->u.ifGoto.x = va_arg(ap, Operand);
            newCode->u.ifGoto.relop = va_arg(ap, Operand);
            newCode->u.ifGoto.y = va_arg(ap, Operand);
            newCode->u.ifGoto.z = va_arg(ap, Operand);
            addInterCode(interCodes);
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
    genInterCode(FUNCTION, newOperand(TEXT, newString(node->child->val)));

    TableItem funcItem = searchTableItem(node->child->val);
    FieldList temp = funcItem->fieldList->type->u.function.argv;
    while (temp) {
        genInterCode(PARAM, newOperand(VARIABLE, newString(temp->name)));
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
        Operand t1 = Temp();
        transVarDec(node->child, t1);
        Operand t2 = Temp();
        transExp(node->child->sibling->sibling, t2);
        genInterCode(ASSIGN, t1, t2);
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
                setOperand(place, VARIABLE, newString(temp->fieldList->name));
            }
        } else if (type->kind == ARRAY) {
            if (type->u.array.elem->kind == ARRAY) {
                interError = true;
                printf("Cannot translate multi-dimensional array type or parameters of array type.\n");
                return;
            } else {
                genInterCode(DEC, newOperand(VARIABLE, newString(temp->fieldList->name)),
                    getSize(type));
            }
        } else if (type->kind == STRUCTURE) {
            genInterCode(DEC, newOperand(VARIABLE, newString(temp->fieldList->name)),
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
        Operand t1 = Temp();
        transExp(node->child->sibling, t1);
        genInterCode(RETURN, t1);
    } else if (!strcmp(node->child->type, "IF")) {
        Node *exp = node->child->sibling->sibling;
        Node *stmt = exp->sibling->sibling;
        Operand label1 = Label();
        Operand label2 = Label();

        transCond(exp, label1, label2);
        genInterCode(LABEL, label1);
        transStmt(stmt);
        if (stmt->sibling == NULL) {
            genInterCode(LABEL, label2);
        } else {
            Operand label3 = Label();
            genInterCode(GOTO, label3);
            genInterCode(LABEL, label2);
            transStmt(stmt->sibling->sibling);
            genInterCode(LABEL, label3);
        }

    } else if (!strcmp(node->child->type, "WHILE")) {
        Operand label1 = Label();
        Operand label2 = Label();
        Operand label3 = Label();

        genInterCode(LABEL, label1);
        transCond(node->child->sibling->sibling, label2, label3);
        genInterCode(LABEL, label2);
        transStmt(node->child->sibling->sibling->sibling->sibling);
        genInterCode(GOTO, label1);
        genInterCode(LABEL, label3);
    }
}

void transExp(Node *node, Operand place) {
    if (interError) return;
    if (!strcmp(node->child->type, "LP"))
        transExp(node->child->sibling, place);
    else if (!strcmp(node->child->type, "Exp") || !strcmp(node->child->type, "NOT")) {
        if (strcmp(node->child->sibling->type, "LB") && strcmp(node->child->sibling->type, "DOT")) {
            if (!strcmp(node->child->sibling->type, "AND") ||
                !strcmp(node->child->sibling->type, "OR") ||
                !strcmp(node->child->sibling->type, "RELOP") ||
                !strcmp(node->child->type, "NOT")) {
                Operand label1 = Label();
                Operand label2 = Label();
                Operand true_num = newOperand(CONSTANT, 1);
                Operand false_num = newOperand(CONSTANT, 0);
                genInterCode(ASSIGN, place, false_num);
                transCond(node, label1, label2);
                genInterCode(LABEL, label1);
                genInterCode(ASSIGN, place, true_num);
            } else {
                if (!strcmp(node->child->sibling->type, "ASSIGNOP")) {
                    Operand t2 = Temp();
                    transExp(node->child->sibling->sibling, t2);
                    Operand t1 = Temp();
                    transExp(node->child, t1);
                    genInterCode(ASSIGN, t1, t2);
                } else {
                    Operand t1 = Temp();
                    transExp(node->child, t1);
                    Operand t2 = Temp();
                    transExp(node->child->sibling->sibling, t2);
                    if (!strcmp(node->child->sibling->type, "PLUS")) {
                        genInterCode(ADD, place, t1, t2);
                    } else if (!strcmp(node->child->sibling->type, "MINUS")) {
                        genInterCode(SUB, place, t1, t2);
                    } else if (!strcmp(node->child->sibling->type, "STAR")) {
                        genInterCode(MUL, place, t1, t2);
                    } else if (!strcmp(node->child->sibling->type, "DIV")) {
                        genInterCode(DIV, place, t1, t2);
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
                    Operand idx = Temp();
                    transExp(node->child->sibling->sibling, idx);
                    Operand base = Temp();
                    transExp(node->child, base);

                    Operand width;
                    Operand offset = Temp();
                    Operand target;
                    TableItem item = searchTableItem(base->u.name);
                    width = newOperand(
                        CONSTANT, getSize(item->fieldList->type->u.array.elem));
                    genInterCode(MUL, offset, idx, width);
                    if (base->kind == VARIABLE) {
                        target = Temp();
                        genInterCode(GET_ADDR, target, base);
                    } else {
                        target = base;
                    }
                    genInterCode(ADD, place, target, offset);
                    place->kind = ADDRESS;
                    interCodeList->arrayName = base->u.name;
                }
            }
            else {
                Operand temp = Temp();
                transExp(node->child, temp);
                Operand target;

                if (temp->kind == ADDRESS) {
                    // 当Exp是一个地址时，将它视为一个变量用作偏移计算
                    target = newOperand(VARIABLE, temp->u.name);
                } else {
                    target = Temp();
                    genInterCode(GET_ADDR, target, temp);
                }

                Operand id = newOperand(VARIABLE, newString(node->child->sibling->sibling->val));
                int offset = 0;
                TableItem item = searchTableItem(temp->u.name);
                if (item == NULL) {
                    item = searchTableItem(interCodeList->arrayName);
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

                Operand tOffset = newOperand(CONSTANT, offset);
                if (place) {
                    genInterCode(ADD, place, target, tOffset);
                    setOperand(place, ADDRESS, newString(id->u.name));
                }
            }
        }
    }
    else if (!strcmp(node->child->type, "MINUS")) {
        Operand t1 = Temp();
        transExp(node->child->sibling, t1);
        Operand zero = newOperand(CONSTANT, 0);
        genInterCode(SUB, place, zero, t1);
    }
    else if (!strcmp(node->child->type, "ID") && node->child->sibling) {
        Operand funcTemp = newOperand(TEXT, newString(node->child->val));

        if (!strcmp(node->child->sibling->sibling->type, "Args")) {
            ArgList argList = newArgList();
            transArgs(node->child->sibling->sibling, argList);
            if (!strcmp(node->child->val, "write")) {
                genInterCode(WRITE, argList->head->op);
            } else {
                // 逆序排列参数
                reverseArgList(argList);
                Arg argTemp = argList->head;
                while (argTemp) {
                    if (argTemp->op->kind == VARIABLE) {
                        TableItem item = searchTableItem(argTemp->op->u.name);
                        if (item && item->fieldList->type->kind == STRUCTURE) {
                            Operand varTemp = Temp();
                            // 结构体变量取地址
                            genInterCode(GET_ADDR, varTemp, argTemp->op);
                            genInterCode(ARG, varTemp);
                        } else {
                            // 普通变量直接传递
                            genInterCode(ARG, argTemp->op);
                        }
                    } else {
                        // 常数或其他
                        genInterCode(ARG, argTemp->op);
                    }
                    argTemp = argTemp->next;
                }
                if (place) {
                    genInterCode(CALL, place, funcTemp);
                } else {
                    Operand temp = Temp();
                    genInterCode(CALL, temp, funcTemp);
                }
            }
        }
        else {
            if (!strcmp(node->child->val, "read")) {
                genInterCode(READ, place);
            } else {
                if (place) {
                    genInterCode(CALL, place, funcTemp);
                } else {
                    Operand temp = Temp();
                    genInterCode(CALL, temp, funcTemp);
                }
            }
        }
    }
    else if (!strcmp(node->child->type, "ID")) {
        TableItem item = searchTableItem(node->child->val);
        interCodeList->tempVarNum--;
        if (item->fieldList->isArg && item->fieldList->type->kind == STRUCTURE) {
            setOperand(place, ADDRESS, newString(node->child->val));
            // setOperand(place, VARIABLE, newString(node->child->val));
        }
        else {
            setOperand(place, VARIABLE, newString(node->child->val));
        }
    } else {
        interCodeList->tempVarNum--;
        setOperand(place, CONSTANT, node->child->val);
    }
}

void reverseArgList(ArgList argList) {
    Arg head = argList->head;
    Arg next;
    Arg prev = NULL;

    while(head) {
        next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    argList->head = prev;
}

void transCond(Node *node, Operand labelTrue, Operand labelFalse) {
    if (interError) return;

    if (!strcmp(node->child->type, "NOT")) {
        transCond(node->child->sibling, labelFalse, labelTrue);
    }
    else if (!strcmp(node->child->sibling->type, "RELOP")) {
        Operand t1 = Temp();
        Operand t2 = Temp();
        transExp(node->child, t1);
        transExp(node->child->sibling->sibling, t2);

        Operand relop =
            newOperand(TEXT, newString(node->child->sibling->val));

        if (t1->kind == ADDRESS) {
            Operand temp = Temp();
            genInterCode(READ_ADDR, temp, t1);
            t1 = temp;
        }
        if (t2->kind == ADDRESS) {
            Operand temp = Temp();
            genInterCode(READ_ADDR, temp, t2);
            t2 = temp;
        }

        genInterCode(IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(GOTO, labelFalse);
    } else if (!strcmp(node->child->sibling->type, "AND")) {
        Operand label1 = Label();
        transCond(node->child, label1, labelFalse);
        genInterCode(LABEL, label1);
        transCond(node->child->sibling->sibling, labelTrue, labelFalse);
    } else if (!strcmp(node->child->sibling->type, "OR")) {
        Operand label1 = Label();
        transCond(node->child, labelTrue, label1);
        genInterCode(LABEL, label1);
        transCond(node->child->sibling->sibling, labelTrue, labelFalse);
    } else {
        Operand t1 = Temp();
        transExp(node, t1);
        Operand t2 = newOperand(CONSTANT, 0);
        Operand relop = newOperand(TEXT, newString("!="));

        if (t1->kind == ADDRESS) {
            Operand temp = Temp();
            genInterCode(READ_ADDR, temp, t1);
            t1 = temp;
        }
        genInterCode(IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(GOTO, labelFalse);
    }
}

void transArgs(Node *node, ArgList argList) {
    if (interError) return;

    Arg temp = newArg(Temp());
    transExp(node->child, temp->op);

    if (temp->op->kind == VARIABLE) {
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
        printInterCodeList(fp);
}

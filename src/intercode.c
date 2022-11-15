#include "intercode.h"

bool interError = false;
InterCodeList interCodeList;

// Operand func
Operand newOperand(int kind, ...) {
    Operand p = (Operand)malloc(sizeof(struct Operand_));
    assert(p != NULL);
    p->kind = kind;
    va_list vaList;
    assert(kind >= 0 && kind < 6);
    va_start(vaList, kind);
    switch (kind) {
        case OP_CONSTANT:
            p->u.value = va_arg(vaList, int);
            break;
        case OP_VARIABLE:
        case OP_ADDRESS:
        case OP_LABEL:
        case OP_FUNCTION:
        case OP_RELOP:
            p->u.name = va_arg(vaList, char*);
            break;
    }

    return p;
}

void setOperand(Operand p, int kind, void* val) {
    assert(p != NULL);
    assert(kind >= 0 && kind < 6);
    p->kind = kind;
    switch (kind) {
        case OP_CONSTANT:
            p->u.value = (int)val;
            break;
        case OP_VARIABLE:
        case OP_ADDRESS:
        case OP_LABEL:
        case OP_FUNCTION:
        case OP_RELOP:
            if (p->u.name) free(p->u.name);
            p->u.name = (char*)val;
            break;
    }
}

void deleteOperand(Operand p) {
    if (p == NULL) return;
    assert(p->kind >= 0 && p->kind < 6);
    switch (p->kind) {
        case OP_CONSTANT:
            break;
        case OP_VARIABLE:
        case OP_ADDRESS:
        case OP_LABEL:
        case OP_FUNCTION:
        case OP_RELOP:
            if (p->u.name) {
                free(p->u.name);
                p->u.name = NULL;
            }
            break;
    }
    free(p);
}

void printOp(FILE* fp, Operand op) {
    assert(op != NULL);
    if (fp == NULL) {
        switch (op->kind) {
            case OP_CONSTANT:
                printf("#%d", op->u.value);
                break;
            case OP_VARIABLE:
            case OP_ADDRESS:
            case OP_LABEL:
            case OP_FUNCTION:
            case OP_RELOP:
                printf("%s", op->u.name);
                break;
        }
    } else {
        switch (op->kind) {
            case OP_CONSTANT:
                fprintf(fp, "#%d", op->u.value);
                break;
            case OP_VARIABLE:
            case OP_ADDRESS:
            case OP_LABEL:
            case OP_FUNCTION:
            case OP_RELOP:
                fprintf(fp, "%s", op->u.name);
                break;
        }
    }
}

// InterCode func
InterCode newInterCode(int kind, ...) {
    InterCode p = (InterCode)malloc(sizeof(struct InterCode_));
    assert(p != NULL);
    p->kind = kind;
    va_list vaList;
    assert(kind >= 0 && kind < 19);
    switch (kind) {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_RETURN:
        case IR_ARG:
        case IR_PARAM:
        case IR_READ:
        case IR_WRITE:
            va_start(vaList, kind);
            p->u.sinOp.op = va_arg(vaList, Operand);
            break;
        case IR_ASSIGN:
        case IR_GET_ADDR:
        case IR_READ_ADDR:
        case IR_WRITE_ADDR:
        case IR_CALL:
            va_start(vaList, kind);
            p->u.assign.left = va_arg(vaList, Operand);
            p->u.assign.right = va_arg(vaList, Operand);
            break;
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
            va_start(vaList, kind);
            p->u.binOp.result = va_arg(vaList, Operand);
            p->u.binOp.op1 = va_arg(vaList, Operand);
            p->u.binOp.op2 = va_arg(vaList, Operand);
            break;
        case IR_DEC:
            va_start(vaList, kind);
            p->u.dec.op = va_arg(vaList, Operand);
            p->u.dec.size = va_arg(vaList, int);
            break;
        case IR_IF_GOTO:
            va_start(vaList, kind);
            p->u.ifGoto.x = va_arg(vaList, Operand);
            p->u.ifGoto.relop = va_arg(vaList, Operand);
            p->u.ifGoto.y = va_arg(vaList, Operand);
            p->u.ifGoto.z = va_arg(vaList, Operand);
    }
    return p;
}

void deleteInterCode(InterCode p) {
    assert(p != NULL);
    assert(p->kind >= 0 && p->kind < 19);
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
        assert(cur->code->kind >= 0 && cur->code->kind < 19);
        if (fp == NULL) {
            switch (cur->code->kind) {
                case IR_LABEL:
                    printf("LABEL ");
                    printOp(fp, cur->code->u.sinOp.op);
                    printf(" :");
                    break;
                case IR_FUNCTION:
                    printf("FUNCTION ");
                    printOp(fp, cur->code->u.sinOp.op);
                    printf(" :");
                    break;
                case IR_ASSIGN:
                    printOp(fp, cur->code->u.assign.left);
                    printf(" := ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_ADD:
                    printOp(fp, cur->code->u.binOp.result);
                    printf(" := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    printf(" + ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_SUB:
                    printOp(fp, cur->code->u.binOp.result);
                    printf(" := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    printf(" - ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_MUL:
                    printOp(fp, cur->code->u.binOp.result);
                    printf(" := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    printf(" * ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_DIV:
                    printOp(fp, cur->code->u.binOp.result);
                    printf(" := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    printf(" / ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_GET_ADDR:
                    printOp(fp, cur->code->u.assign.left);
                    printf(" := &");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_READ_ADDR:
                    printOp(fp, cur->code->u.assign.left);
                    printf(" := *");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_WRITE_ADDR:
                    printf("*");
                    printOp(fp, cur->code->u.assign.left);
                    printf(" := ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_GOTO:
                    printf("GOTO ");
                    printOp(fp, cur->code->u.sinOp.op);
                    break;
                case IR_IF_GOTO:
                    printf("IF ");
                    printOp(fp, cur->code->u.ifGoto.x);
                    printf(" ");
                    printOp(fp, cur->code->u.ifGoto.relop);
                    printf(" ");
                    printOp(fp, cur->code->u.ifGoto.y);
                    printf(" GOTO ");
                    printOp(fp, cur->code->u.ifGoto.z);
                    break;
                case IR_RETURN:
                    printf("RETURN ");
                    printOp(fp, cur->code->u.sinOp.op);
                    break;
                case IR_DEC:
                    printf("DEC ");
                    printOp(fp, cur->code->u.dec.op);
                    printf(" ");
                    printf("%d", cur->code->u.dec.size);
                    break;
                case IR_ARG:
                    printf("ARG ");
                    printOp(fp, cur->code->u.sinOp.op);
                    break;
                case IR_CALL:
                    printOp(fp, cur->code->u.assign.left);
                    printf(" := CALL ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_PARAM:
                    printf("PARAM ");
                    printOp(fp, cur->code->u.sinOp.op);
                    break;
                case IR_READ:
                    printf("READ ");
                    printOp(fp, cur->code->u.sinOp.op);
                    break;
                case IR_WRITE:
                    printf("WRITE ");
                    printOp(fp, cur->code->u.sinOp.op);
                    break;
            }
            printf("\n");
        } else {
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
}

// InterCodes func
InterCodes newInterCodes(InterCode code) {
    InterCodes p = (InterCodes)malloc(sizeof(struct InterCodes_));
    assert(p != NULL);
    p->code = code;
    p->prev = NULL;
    p->next = NULL;
}

void deleteInterCodes(InterCodes p) {
    assert(p != NULL);
    deleteInterCode(p->code);
    free(p);
}

// Arg and ArgList func
Arg newArg(Operand op) {
    Arg p = (Arg)malloc(sizeof(struct Arg_));
    assert(p != NULL);
    p->op = op;
    p->next = NULL;
}

ArgList newArgList() {
    ArgList p = (ArgList)malloc(sizeof(struct ArgList_));
    assert(p != NULL);
    p->head = NULL;
    p->cur = NULL;
}

void deleteArg(Arg p) {
    assert(p != NULL);
    deleteOperand(p->op);
    free(p);
}

void deleteArgList(ArgList p) {
    assert(p != NULL);
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
    assert(p != NULL);
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
    // printf("newTemp() tempVal:%d\n", interCodeList->tempVarNum);
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
    va_list vaList;
    Operand temp = NULL;
    Operand result = NULL, op1 = NULL, op2 = NULL, relop = NULL;
    int size = 0;
    InterCodes newCode = NULL;
    assert(kind >= 0 && kind < 19);
    switch (kind) {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_RETURN:
        case IR_ARG:
        case IR_PARAM:
        case IR_READ:
        case IR_WRITE:
            va_start(vaList, kind);
            op1 = va_arg(vaList, Operand);
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
            va_start(vaList, kind);
            op1 = va_arg(vaList, Operand);
            op2 = va_arg(vaList, Operand);
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
            va_start(vaList, kind);
            result = va_arg(vaList, Operand);
            op1 = va_arg(vaList, Operand);
            op2 = va_arg(vaList, Operand);
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
            va_start(vaList, kind);
            op1 = va_arg(vaList, Operand);
            size = va_arg(vaList, int);
            newCode = newInterCodes(newInterCode(kind, op1, size));
            addInterCode(interCodeList, newCode);
            break;
        case IR_IF_GOTO:
            va_start(vaList, kind);
            result = va_arg(vaList, Operand);
            relop = va_arg(vaList, Operand);
            op1 = va_arg(vaList, Operand);
            op2 = va_arg(vaList, Operand);
            newCode =
                newInterCodes(newInterCode(kind, result, relop, op1, op2));
            addInterCode(interCodeList, newCode);
            break;
    }
}
void transExtDefList(Node *node) {
    // ExtDefList -> ExtDef ExtDefList
    //             | e
    while (node) {
        transExtDef(node->child);
        node = node->child->sibling;
    }
}

void transExtDef(Node *node) {
    assert(node != NULL);
    if (interError) return;
    // ExtDef -> Specifier ExtDecList SEMI
    //         | Specifier SEMI
    //         | Specifier FunDec CompSt

    // 因为没有全局变量使用，
    // ExtDecList不涉及中间代码生成，类型声明也不涉及，所以只需要处理FunDec和CompSt
    if (!strcmp(node->child->sibling->type, "FunDec")) {
        transFunDec(node->child->sibling);
        transCompSt(node->child->sibling->sibling);
    }
}

void transFunDec(Node *node) {
    assert(node != NULL);
    if (interError) return;
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    genInterCode(IR_FUNCTION,
                 newOperand(OP_FUNCTION, newString(node->child->val)));
    // InterCodes func = newInterCodes(newInterCode(
    //     IR_FUNCTION, newOperand(OP_FUNCTION, newString(node->child->val))));
    // addInterCode(interCodeList, func);

    TableItem funcItem = searchTableItem(node->child->val);
    FieldList temp = funcItem->fieldList->type->u.function.argv;
    while (temp) {
        genInterCode(IR_PARAM, newOperand(OP_VARIABLE, newString(temp->name)));
        // InterCodes arg = newInterCodes(newInterCode(
        //     IR_PARAM, newOperand(OP_VARIABLE, newString(temp->name))));
        // addInterCode(interCodeList, arg);
        temp = temp->tail;
    }
}

void transCompSt(Node *node) {
    assert(node != NULL);
    if (interError) return;
    printf("debuger: translate compst\n");
    // CompSt -> LC DefList StmtList RC
    Node *temp = node->child->sibling;
    printf("debuger: translate deflist\n");
    if (!strcmp(temp->type, "DefList")) {
        transDefList(temp);
    }
    printf("debuger: translate stmtlist\n");
    if (!strcmp(temp->type, "StmtList")) {
        transStmtList(temp);
    }
}

void transDefList(Node *node) {
    if (interError) return;
    // DefList -> Def DefList
    //          | e
    while (node) {
        transDef(node->child);
        node = node->child->sibling;
    }
}

void transDef(Node *node) {
    assert(node != NULL);
    if (interError) return;
    // Def -> Specifier DecList SEMI
    transDecList(node->child->sibling);
}

void transDecList(Node *node) {
    assert(node != NULL);
    if (interError) return;
    // DecList -> Dec
    //          | Dec COMMA DecList
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
    assert(node != NULL);
    if (interError) return;
    // Dec -> VarDec
    //      | VarDec ASSIGNOP Exp

    // Dec -> VarDec
    if (node->child->sibling == NULL) {
        transVarDec(node->child, NULL);
    }
    // Dec -> VarDec ASSIGNOP Exp
    else {
        Operand t1 = newTemp();
        transVarDec(node->child, t1);
        Operand t2 = newTemp();
        transExp(node->child->sibling->sibling, t2);
        genInterCode(IR_ASSIGN, t1, t2);
    }
}

void transVarDec(Node *node, Operand place) {
    assert(node != NULL);
    if (interError) return;
    // VarDec -> ID
    //         | VarDec LB INT RB

    if (!strcmp(node->child->type, "ID")) {
        printf("translate id\n");
        printf("search id\n");
        TableItem temp = searchTableItem(node->child->val);
        if (temp) printf("debuger: founded\n");
        else printf("debuger: unfound\n");
        Type type = temp->fieldList->type;
        if (type->kind == BASIC) {
            if (place) {
                interCodeList->tempVarNum--;
                printf("debuger: set operand\n");
                setOperand(place, OP_VARIABLE,
                           (void*)newString(temp->fieldList->name));
            }
        } else if (type->kind == ARRAY) {
            // 不需要完成高维数组情况
            if (type->u.array.elem->kind == ARRAY) {
                interError = true;
                printf(
                    "Cannot translate: Code containsvariables of "
                    "multi-dimensional array type or parameters of array "
                    "type.\n");
                return;
            } else {
                genInterCode(
                    IR_DEC,
                    newOperand(OP_VARIABLE, newString(temp->fieldList->name)),
                    getSize(type));
            }
        } else if (type->kind == STRUCTURE) {
            // 3.1选做
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
    // StmtList -> Stmt StmtList
    //           | e
    while (node) {
        transStmt(node->child);
        node = node->child->sibling;
    }
}

void transStmt(Node *node) {
    assert(node != NULL);
    if (interError) return;
    // Stmt -> Exp SEMI
    //       | CompSt
    //       | RETURN Exp SEMI
    //       | IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    //       | WHILE LP Exp RP Stmt

    // Stmt -> Exp SEMI

    if (!strcmp(node->child->type, "Exp")) {
        transExp(node->child, NULL);
    }

    // Stmt -> CompSt
    else if (!strcmp(node->child->type, "CompSt")) {
        transCompSt(node->child);
    }

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->child->type, "RETURN")) {
        Operand t1 = newTemp();
        transExp(node->child->sibling, t1);
        genInterCode(IR_RETURN, t1);
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->child->type, "IF")) {
        Node *exp = node->child->sibling->sibling;
        Node *stmt = exp->sibling->sibling;
        Operand label1 = newLabel();
        Operand label2 = newLabel();

        transCond(exp, label1, label2);
        genInterCode(IR_LABEL, label1);
        transStmt(stmt);
        if (stmt->sibling == NULL) {
            genInterCode(IR_LABEL, label2);
        }
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        else {
            Operand label3 = newLabel();
            genInterCode(IR_GOTO, label3);
            genInterCode(IR_LABEL, label2);
            transStmt(stmt->sibling->sibling);
            genInterCode(IR_LABEL, label3);
        }

    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->child->type, "WHILE")) {
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
    assert(node != NULL);
    if (interError) return;
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp

    //      | MINUS Exp
    //      | NOT Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT

    // Exp -> LP Exp RP
    if (!strcmp(node->child->type, "LP"))
        transExp(node->child->sibling, place);

    else if (!strcmp(node->child->type, "Exp") ||
             !strcmp(node->child->type, "NOT")) {
        // 条件表达式 和 基本表达式
        if (strcmp(node->child->sibling->type, "LB") &&
            strcmp(node->child->sibling->type, "DOT")) {
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | NOT Exp
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
                // Exp -> Exp ASSIGNOP Exp
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
                    // Exp -> Exp PLUS Exp
                    if (!strcmp(node->child->sibling->type, "PLUS")) {
                        genInterCode(IR_ADD, place, t1, t2);
                    }
                    // Exp -> Exp MINUS Exp
                    else if (!strcmp(node->child->sibling->type, "MINUS")) {
                        genInterCode(IR_SUB, place, t1, t2);
                    }
                    // Exp -> Exp STAR Exp
                    else if (!strcmp(node->child->sibling->type, "STAR")) {
                        genInterCode(IR_MUL, place, t1, t2);
                    }
                    // Exp -> Exp DIV Exp
                    else if (!strcmp(node->child->sibling->type, "DIV")) {
                        genInterCode(IR_DIV, place, t1, t2);
                    }
                }
            }

        }
        // 数组和结构体访问
        else {
            // Exp -> Exp LB Exp RB
            if (!strcmp(node->child->sibling->type, "LB")) {
                //数组
                if (node->child->child->sibling &&
                    !strcmp(node->child->child->sibling->type, "LB")) {
                    //多维数组，报错
                    interError = true;
                    printf(
                        "Cannot translate: Code containsvariables of "
                        "multi-dimensional array type or parameters of array "
                        "type.\n");
                    return;
                } else {
                    Operand idx = newTemp();
                    transExp(node->child->sibling->sibling, idx);
                    Operand base = newTemp();
                    transExp(node->child, base);

                    Operand width;
                    Operand offset = newTemp();
                    Operand target;
                    // 根据假设，Exp1只会展开为 Exp DOT ID 或 ID
                    // 我们让前一种情况把ID作为name回填进place返回到这里的base处，在语义分析时将结构体变量也填进表（因为假设无重名），这样两种情况都可以查表得到。
                    TableItem item = searchTableItem(base->u.name);
                    assert(item->fieldList->type->kind == ARRAY);
                    width = newOperand(
                        OP_CONSTANT, getSize(item->fieldList->type->u.array.elem));
                    genInterCode(IR_MUL, offset, idx, width);
                    // 如果是ID[Exp],
                    // 则需要对ID取址，如果前面是结构体内访问，则会返回一个地址类型，不需要再取址
                    if (base->kind == OP_VARIABLE) {
                        // printf("非结构体数组访问\n");
                        target = newTemp();
                        genInterCode(IR_GET_ADDR, target, base);
                    } else {
                        // printf("结构体数组访问\n");
                        target = base;
                    }
                    genInterCode(IR_ADD, place, target, offset);
                    place->kind = OP_ADDRESS;
                    interCodeList->lastArrayName = base->u.name;
                }
            }
            // Exp -> Exp DOT ID
            else {
                //结构体
                Operand temp = newTemp();
                transExp(node->child, temp);
                // 两种情况，Exp直接为一个变量，则需要先取址，若Exp为数组或者多层结构体访问或结构体形参，则target会被填成地址，可以直接用。
                Operand target;

                if (temp->kind == OP_ADDRESS) {
                    target = newOperand(temp->kind, temp->u.name);
                    // target->isAddr = true;
                } else {
                    target = newTemp();
                    genInterCode(IR_GET_ADDR, target, temp);
                }

                Operand id = newOperand(
                    OP_VARIABLE, newString(node->child->sibling->sibling->val));
                int offset = 0;
                TableItem item = searchTableItem(temp->u.name);
                //结构体数组，temp是临时变量，查不到表，需要用处理数组时候记录下的数组名老查表
                if (item == NULL) {
                    item = searchTableItem(interCodeList->lastArrayName);
                }

                FieldList tmp;
                // 结构体数组 eg: a[5].b
                if (item->fieldList->type->kind == ARRAY) {
                    tmp = item->fieldList->type->u.array.elem->u.structure.fieldList;
                }
                // 一般结构体
                else {
                    tmp = item->fieldList->type->u.structure.fieldList;
                }
                // 遍历获得offset
                while (tmp) {
                    if (!strcmp(tmp->name, id->u.name)) break;
                    offset += getSize(tmp->type);
                    tmp = tmp->tail;
                }

                Operand tOffset = newOperand(OP_CONSTANT, offset);
                if (place) {
                    genInterCode(IR_ADD, place, target, tOffset);
                    // 为了处理结构体里的数组把id名通过place回传给上层
                    setOperand(place, OP_ADDRESS, (void*)newString(id->u.name));
                    // place->isAddr = true;
                }
            }
        }
    }
    //单目运算符
    // Exp -> MINUS Exp
    else if (!strcmp(node->child->type, "MINUS")) {
        Operand t1 = newTemp();
        transExp(node->child->sibling, t1);
        Operand zero = newOperand(OP_CONSTANT, 0);
        genInterCode(IR_SUB, place, zero, t1);
    }
    // // Exp -> NOT Exp
    // else if (!strcmp(node->child->type, "NOT")) {
    //     Operand label1 = newLabel();
    //     Operand label2 = newLabel();
    //     Operand true_num = newOperand(OP_CONSTANT, 1);
    //     Operand false_num = newOperand(OP_CONSTANT, 0);
    //     genInterCode(IR_ASSIGN, place, false_num);
    //     transCond(node, label1, label2);
    //     genInterCode(IR_LABEL, label1);
    //     genInterCode(IR_ASSIGN, place, true_num);
    // }
    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(node->child->type, "ID") && node->child->sibling) {
        Operand funcTemp =
            newOperand(OP_FUNCTION, newString(node->child->val));
        // Exp -> ID LP Args RP
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

                        // 结构体作为参数需要传址
                        if (item && item->fieldList->type->kind == STRUCTURE) {
                            Operand varTemp = newTemp();
                            genInterCode(IR_GET_ADDR, varTemp, argTemp->op);
                            Operand varTempCopy =
                                newOperand(OP_ADDRESS, varTemp->u.name);
                            // varTempCopy->isAddr = true;
                            genInterCode(IR_ARG, varTempCopy);
                        }
                    }
                    // 一般参数直接传值
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
        // Exp -> ID LP RP
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
    // Exp -> ID
    else if (!strcmp(node->child->type, "ID")) {
        TableItem item = searchTableItem(node->child->val);
        // 根据讲义，因为结构体不允许赋值，结构体做形参时是传址的方式
        interCodeList->tempVarNum--;
        if (item->fieldList->isArg && item->fieldList->type->kind == STRUCTURE) {
            setOperand(place, OP_ADDRESS, (void*)newString(node->child->val));
            // place->isAddr = true;
        }
        // 非结构体参数情况都当做变量处理
        else {
            setOperand(place, OP_VARIABLE, (void*)newString(node->child->val));
        }

        // Operand t1 = newOperand(OP_VARIABLE, id_name->field->name);
        // genInterCode(IR_ASSIGN, place, t1);
    } else {
        // // Exp -> FLOAT
        // 无浮点数常数
        // if (!strcmp(node->child->type, "FLOAT")) {
        //     Operand t1 = newOperand(OP_CONSTANT, node->child->val);
        //     genInterCode(IR_ASSIGN, place, t1);
        // }

        // Exp -> INT
        interCodeList->tempVarNum--;
        setOperand(place, OP_CONSTANT, (void*)atoi(node->child->val));
        // Operand t1 = newOperand(OP_CONSTANT, node->child->val);
        // genInterCode(IR_ASSIGN, place, t1);
    }
}

void transCond(Node *node, Operand labelTrue, Operand labelFalse) {
    assert(node != NULL);
    if (interError) return;
    // Exp -> Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | NOT Exp

    // Exp -> NOT Exp
    if (!strcmp(node->child->type, "NOT")) {
        transCond(node->child->sibling, labelFalse, labelTrue);
    }
    // Exp -> Exp RELOP Exp
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
    }
    // Exp -> Exp AND Exp
    else if (!strcmp(node->child->sibling->type, "AND")) {
        Operand label1 = newLabel();
        transCond(node->child, label1, labelFalse);
        genInterCode(IR_LABEL, label1);
        transCond(node->child->sibling->sibling, labelTrue, labelFalse);
    }
    // Exp -> Exp OR Exp
    else if (!strcmp(node->child->sibling->type, "OR")) {
        Operand label1 = newLabel();
        transCond(node->child, labelTrue, label1);
        genInterCode(IR_LABEL, label1);
        transCond(node->child->sibling->sibling, labelTrue, labelFalse);
    }
    // other cases
    else {
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
    assert(node != NULL);
    assert(argList != NULL);
    if (interError) return;
    // Args -> Exp COMMA Args
    //       | Exp

    // Args -> Exp
    Arg temp = newArg(newTemp());
    transExp(node->child, temp->op);

    if (temp->op->kind == OP_VARIABLE) {
        TableItem item = searchTableItem(temp->op->u.name);
        if (item && item->fieldList->type->kind == ARRAY) {
            interError = true;
            printf(
                "Cannot translate: Code containsvariables of "
                "multi-dimensional array type or parameters of array "
                "type.\n");
            return;
        }
    }
    addArg(argList, temp);

    // Args -> Exp COMMA Args
    if (node->child->sibling != NULL) {
        transArgs(node->child->sibling->sibling, argList);
    }
}
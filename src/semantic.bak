#include "semantic.h"

#define DEBUG 0

Table table = NULL;
HashTable hashTable  = NULL;
Scope scope = NULL;

Type newType(int kind, ...) {
    Type p = (Type)malloc(sizeof(struct Type_));
    p->kind = kind;
    va_list ap;
    va_start(ap, kind);
    switch (kind) {
        case BASIC:
            p->u.basic = va_arg(ap, int);
            break;
        case ARRAY:
            p->u.array.elem = va_arg(ap, Type);
            p->u.array.size = va_arg(ap, int);
            break;
        case STRUCTURE:
            p->u.structure.name = va_arg(ap, char*);
            p->u.structure.fieldList = va_arg(ap, FieldList);
            break;
        case FUNCTION:
            p->u.function.argc = va_arg(ap, int);
            p->u.function.argv = va_arg(ap, FieldList);
            p->u.function.rType = va_arg(ap, Type);
            break;
    }
    va_end(ap);
    return p;
}

Type copyType(Type src) {
    if (src == NULL) return NULL;
    Type p = (Type)malloc(sizeof(struct Type_));
    p->kind = src->kind;
    switch (p->kind) {
        case BASIC:
            p->u.basic = src->u.basic;
            break;
        case ARRAY:
            p->u.array.elem = copyType(src->u.array.elem);
            p->u.array.size = src->u.array.size;
            break;
        case STRUCTURE:
            p->u.structure.name = newString(src->u.structure.name);
            p->u.structure.fieldList = copyFieldList(src->u.structure.fieldList);
            break;
        case FUNCTION:
            p->u.function.argc = src->u.function.argc;
            p->u.function.argv = copyFieldList(src->u.function.argv);
            p->u.function.rType = copyType(src->u.function.rType);
            break;
    }

    return p;
}

void delType(Type type) {
    FieldList temp = NULL;
    // printType(type);
    switch (type->kind) {
        case BASIC:
            break;
        case ARRAY:
            delType(type->u.array.elem);
            break;
        case STRUCTURE:
            // printf("debuger: free type-struct-name\n");
            if (type->u.structure.name)
                free(type->u.structure.name);
            temp = type->u.structure.fieldList;
            while (temp) {
                FieldList del = temp;
                temp = temp->tail;
                delFieldList(del);
            }
            break;
        case FUNCTION:
            delType(type->u.function.rType);
            temp = type->u.function.argv;
            while (temp) {
                FieldList del = temp;
                temp = temp->tail;
                delFieldList(del);
            }
            break;
    }
    // printf("debuger: free type\n");
    if(type)
        free(type);
}

bool compareType(Type t1, Type t2) {
    if (t1 == NULL || t2 == NULL) return true;
    if (t1->kind == FUNCTION || t2->kind == FUNCTION) return false;
    if (t1->kind != t2->kind)
        return false;
    else {
        switch (t1->kind) {
            case BASIC:
                return t1->u.basic == t2->u.basic;
            case ARRAY:
                return compareType(t1->u.array.elem, t2->u.array.elem);
            case STRUCTURE:
                return !strcmp(t1->u.structure.name, t2->u.structure.name);
        }
    }
}

void printType(Type type) {
    if (type == NULL) {
        printf("type is NULL.\n");
    } else {
        printf("type kind: %d\n", type->kind);
        switch (type->kind) {
            case BASIC:
                printf("type basic: %d\n", type->u.basic);
                break;
            case ARRAY:
                printf("array size: %d\n", type->u.array.size);
                printType(type->u.array.elem);
                break;
            case STRUCTURE:
                if (!type->u.structure.name)
                    printf("struct name is NULL\n");
                else {
                    printf("struct name is %s\n", type->u.structure.name);
                }
                printFieldList(type->u.structure.fieldList);
                break;
            case FUNCTION:
                printf("function argc is %d\n", type->u.function.argc);
                printf("function args:\n");
                printFieldList(type->u.function.argv);
                printf("function return type:\n");
                printType(type->u.function.rType);
                break;
        }
    }
}

// FieldList functions
FieldList newFieldList(char* newName, Type newType) {
    FieldList p = (FieldList)malloc(sizeof(struct FieldList_));
    p->name = newString(newName);
    p->type = newType;
    p->tail = NULL;
    return p;
}

FieldList copyFieldList(FieldList src) {
    FieldList head = NULL, cur = NULL;
    FieldList temp = src;

    while (temp) {
        if (!head) {
            head = newFieldList(temp->name, copyType(temp->type));
            cur = head;
            temp = temp->tail;
        } else {
            cur->tail = newFieldList(temp->name, copyType(temp->type));
            cur = cur->tail;
            temp = temp->tail;
        }
    }
    return head;
}

void delFieldList(FieldList fieldList) {
    // printf("debuger: free fieldList-name\n");
    if (fieldList->name)
        free(fieldList->name);
    
    if (fieldList->type) 
        delType(fieldList->type);
    
    // printf("debuger: free fieldList\n");
    if(fieldList)
        free(fieldList);
}

void setFieldListName(FieldList p, char* newName) {
    // printf("debuger: free fieldList-name\n");
    if (p->name != NULL) 
        free(p->name);
    p->name = newString(newName);
}

void printFieldList(FieldList fieldList) {
    if (fieldList == NULL)
        printf("fieldList is NULL\n");
    else {
        printf("fieldList name is: %s\n", fieldList->name);
        printf("FieldList Type:\n");
        printType(fieldList->type);
        printFieldList(fieldList->tail);
    }
}

// tableItem functions
TableItem newItem(int layerDepth, FieldList pfield) {
    TableItem p = (TableItem)malloc(sizeof(struct TableItem_));
    p->layerDepth = layerDepth;
    p->fieldList = pfield;
    p->sameHash = NULL;
    p->sameScope = NULL;
    return p;
}

void delItem(TableItem item) {
    if (item->fieldList != NULL) 
        delFieldList(item->fieldList);
    // printf("debuger: free tableItem\n");
    if (item)
        free(item);
}

// Hash functions
HashTable newHashTable() {
    HashTable p = (HashTable)malloc(sizeof(struct HashTable_));
    p->hashArray = (TableItem*)malloc(sizeof(TableItem) * HASH_TABLE_SIZE);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        p->hashArray[i] = NULL;
    }
    return p;
}

void delHashTable() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        TableItem temp = hashTable->hashArray[i];
        while (temp) {
            TableItem del = temp;
            temp = temp->sameHash;
            delItem(del);
        }
    }
    // printf("debuger: free hash-hashArray\n");
    if (hashTable->hashArray)
        free(hashTable->hashArray);
    // printf("debuger: free hashTable\n");
    if (hashTable)
        free(hashTable);
}

TableItem getHashHead(int index) {
    return hashTable->hashArray[index];
}

void setHashHead(int index, TableItem newVal) {
    hashTable->hashArray[index] = newVal;
}

// Scope functions
Scope newScope() {
    Scope p = (Scope)malloc(sizeof(struct Scope_));
    p->scopeLayer = (TableItem*)malloc(sizeof(TableItem) * MAX_SCOPE_DEPTH);
    for (int i = 0; i < MAX_SCOPE_DEPTH; i++) {
        p->scopeLayer[i] = NULL;
    }
    p->layerDepth = 0;
    return p;
}

void delScope() {
    // printf("debuger: free scope-scopeLayer\n");
    if (scope->scopeLayer)
        free(scope->scopeLayer);

    // printf("debuger: free scope\n");
    if (scope)
        free(scope);
}

void innerLayer() {
    scope->layerDepth++;
}

void outerLayer() {
    scope->layerDepth--;
}

void exitLayer() {
    TableItem temp = getLayerHead();
    while (temp) {
        TableItem del = temp;
        temp = temp->sameScope;
        delTableItem(del);
    }
    setLayerHead(NULL);
    outerLayer();
}

TableItem getLayerHead() {
    return scope->scopeLayer[scope->layerDepth];
}

void setLayerHead(TableItem newVal) {
    scope->scopeLayer[scope->layerDepth] = newVal;
}

// Table functions
Table initTable() {
    Table table = (Table)malloc(sizeof(struct Table_));
    table->hashTable = newHashTable();
    table->scope = newScope();
    table->anonymousNum = 0;
    return table;
};

void delTable() {
    delHashTable(hashTable);
    delScope();
    // printf("debuger: free table\n");
    if (table)
        free(table);
};

TableItem searchTableItem(char* name) {
    unsigned hashCode = getHashCode(name);
    TableItem temp = getHashHead(hashCode);
    if (temp == NULL) return NULL;
    while (temp) {
        if (!strcmp(temp->fieldList->name, name)) return temp;
        temp = temp->sameHash;
    }
    return NULL;
}

// Return false -> no confliction, true -> has confliction
bool hasConfliction(TableItem item) {
    TableItem temp = searchTableItem(item->fieldList->name);
    if (temp == NULL) return false;
    while (temp) {
        if (!strcmp(temp->fieldList->name, item->fieldList->name)) {
            if (temp->fieldList->type->kind == STRUCTURE ||
                item->fieldList->type->kind == STRUCTURE)
                return true;
            if (temp->layerDepth == scope->layerDepth) return true;
        }
        temp = temp->sameHash;
    }
    return false;
}

void addTableItem(TableItem item) {
    unsigned hashCode = getHashCode(item->fieldList->name);

    item->sameScope = getLayerHead();
    setLayerHead(item);

    item->sameHash = getHashHead(hashCode);
    setHashHead(hashCode, item);
}

void delTableItem(TableItem item) {
    unsigned hashCode = getHashCode(item->fieldList->name);
    if (item == getHashHead(hashCode))
        setHashHead(hashCode, item->sameHash);
    else {
        TableItem cur = getHashHead(hashCode);
        TableItem last = cur;
        while (cur != item) {
            last = cur;
            cur = cur->sameHash;
        }
        last->sameHash = cur->sameHash;
    }
    delItem(item);
}

bool isStructDef(TableItem src) {
    if (src == NULL || src->fieldList->type->kind != STRUCTURE 
        || src->fieldList->type->u.structure.name)
        return false;
    return true;
}

// for Debug
void printTable(Table table) {
    printf("----------------hash_table----------------\n");
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        TableItem item = getHashHead(i);
        if (item) {
            printf("[%d]", i);
            while (item) {
                printf(" -> name: %s layerDepth: %d\n", item->fieldList->name,
                       item->layerDepth);
                printf("========FiledList========\n");
                printFieldList(item->fieldList);
                printf("===========End===========\n");
                item = item->sameHash;
            }
            printf("\n");
        }
    }
    printf("-------------------end--------------------\n");
}

// Generate symbol table functions
void ExtDef(Node* node) {
    // ExtDef -> Specifier ExtDecList SEMI
    //         | Specifier SEMI
    //         | Specifier FunDec CompSt
    Type specifierType = Specifier(node->child);
    char* secondName = node->child->sibling->type;

    // ExtDef -> Specifier ExtDecList SEMI
    if (!strcmp(secondName, "ExtDecList")) {
        // TODO: process first situation
        ExtDecList(node->child->sibling, specifierType);
    }
    // ExtDef -> Specifier FunDec CompSt
    else if (!strcmp(secondName, "FunDec")) {
        // TODO: process third situation
        FunDec(node->child->sibling, specifierType);
        CompSt(node->child->sibling->sibling, specifierType);
    }
    if (specifierType) delType(specifierType);
    // Specifier SEMI
    // this situation has no meaning
    // or is struct define(have been processe inSpecifier())
}

void ExtDecList(Node* node, Type specifier) {
    // ExtDecList -> VarDec
    //             | VarDec COMMA ExtDecList
    Node* temp = node;
    while (temp) {
        TableItem item = VarDec(temp->child, specifier);
        if (hasConfliction(item)) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "Redefined variable \"%s\".", item->fieldList->name);
            Error(VAR_REDEF, temp->lineno, msg);
            delItem(item);
        } else {
            addTableItem(item);
        }
        if (temp->child->sibling) {
            temp = temp->sibling->sibling->child;
        } else {
            break;
        }
    }
}

Type Specifier(Node* node) {
    // Specifier -> TYPE
    //            | StructSpecifier

    Node* t = node->child;
    // Specifier -> TYPE
    if (!strcmp(t->type, "TYPE")) {
        if (!strcmp(t->val, "float")) {
            return newType(BASIC, 1);
        } else {
            return newType(BASIC, 0);
        }
    }
    // Specifier -> StructSpecifier
    else {
        return StructSpecifier(t);
    }
}

Type StructSpecifier(Node* node) {
    // StructSpecifier -> STRUCT OptTag LC DefList RC
    //                  | STRUCT Tag

    // OptTag -> ID | e
    // Tag -> ID
    Type returnType = NULL;
    Node* t = node->child->sibling;
    // StructSpecifier->STRUCT OptTag LC DefList RC
    if (strcmp(t->type, "Tag")) {
        TableItem structItem =
            newItem(scope->layerDepth,
                    newFieldList("", newType(STRUCTURE, NULL, NULL)));
        if (!strcmp(t->type, "OptTag")) {
            setFieldListName(structItem->fieldList, t->child->val);
            t = t->sibling;
        }
        // unnamed struct
        else {
            table->anonymousNum ++;
            char structName[20] = {0};
            sprintf(structName, "%d", table->anonymousNum );
            setFieldListName(structItem->fieldList, structName);
        }

        if (!strcmp(t->sibling->type, "DefList")) {
            DefList(t->sibling, structItem);
        }

        if (hasConfliction(structItem)) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "Duplicated name \"%s\".", structItem->fieldList->name);
            Error(STRUCT_REDEF, node->lineno, msg);
            delItem(structItem);
        } else {
            returnType = newType(
                STRUCTURE, newString(structItem->fieldList->name),
                copyFieldList(structItem->fieldList->type->u.structure.fieldList));

            if (!strcmp(node->child->sibling->type, "OptTag")) {
                addTableItem(structItem);
            }
            // OptTag -> e
            else {
                delItem(structItem);
            }
        }
    }

    // StructSpecifier->STRUCT Tag
    else {
        TableItem structItem = searchTableItem(t->child->val);
        if (structItem == NULL || !isStructDef(structItem)) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "Undefined structure \"%s\".", t->child->val);
            Error(STRUCT_UNDEF, node->lineno, msg);
        } else
            returnType = newType(
                STRUCTURE, newString(structItem->fieldList->name),
                copyFieldList(structItem->fieldList->type->u.structure.fieldList));
    }
    return returnType;
}

TableItem VarDec(Node* node, Type specifier) {
    // VarDec -> ID
    //         | VarDec LB INT RB
    Node* id = node;
    // get ID
    while (id->child) id = id->child;
    TableItem p = newItem(scope->layerDepth, newFieldList(id->val, NULL));

    // VarDec -> ID
    if (!strcmp(node->child->type, "ID")) {
        p->fieldList->type = copyType(specifier);
    }
    // VarDec -> VarDec LB INT RB
    else {
        Node* varDec = node->child;
        Type temp = specifier;
        while (varDec->sibling) {
            p->fieldList->type =
                newType(ARRAY, copyType(temp), atoi(varDec->sibling->sibling->val));
            temp = p->fieldList->type;
            varDec = varDec->child;
        }
    }
    return p;
}

void FunDec(Node* node, Type returnType) {
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    TableItem p =
        newItem(scope->layerDepth,
                newFieldList(node->child->val,
                             newType(FUNCTION, 0, NULL, copyType(returnType))));

    // FunDec -> ID LP VarList RP
    if (!strcmp(node->child->sibling->sibling->type, "VarList")) {
        VarList(node->child->sibling->sibling, p);
    }

    // FunDec -> ID LP RP don't need process

    // check redefine
    if (hasConfliction(p)) {
        char msg[MAX_MSG_LENGTH] = {0};
        sprintf(msg, "Redefined function \"%s\".", p->fieldList->name);
        Error(FUN_REDEF, node->lineno, msg);
        delItem(p);
        p = NULL;
    } else {
        addTableItem(p);
    }
}

void VarList(Node* node, TableItem func) {
    // VarList -> ParamDec COMMA VarList
    //          | ParamDec
    innerLayer();
    int argc = 0;
    Node* temp = node->child;
    FieldList cur = NULL;

    // VarList -> ParamDec
    FieldList paramDec = ParamDec(temp);
    func->fieldList->type->u.function.argv = copyFieldList(paramDec);
    cur = func->fieldList->type->u.function.argv;
    argc++;

    // VarList -> ParamDec COMMA VarList
    while (temp->sibling) {
        temp = temp->sibling->sibling->child;
        paramDec = ParamDec(temp);
        if (paramDec) {
            cur->tail = copyFieldList(paramDec);
            cur = cur->tail;
            argc++;
        }
    }

    func->fieldList->type->u.function.argc = argc;

    outerLayer();
}

FieldList ParamDec(Node* node) {
    // ParamDec -> Specifier VarDec
    Type specifierType = Specifier(node->child);
    TableItem p = VarDec(node->child->sibling, specifierType);
    if (specifierType) delType(specifierType);
    if (hasConfliction(p)) {
        char msg[MAX_MSG_LENGTH] = {0};
        sprintf(msg, "Redefined variable \"%s\".", p->fieldList->name);
        Error(VAR_REDEF, node->lineno, msg);
        delItem(p);
        return NULL;
    } else {
        addTableItem(p);
        return p->fieldList;
    }
}

void CompSt(Node* node, Type returnType) {
    // CompSt -> LC DefList StmtList RC
    innerLayer();
    Node* temp = node->child->sibling;
    if (!strcmp(temp->type, "DefList")) {
        DefList(temp, NULL);
        temp = temp->sibling;
    }
    if (!strcmp(temp->type, "StmtList")) {
        StmtList(temp, returnType);
    }

    exitLayer();
}

void StmtList(Node* node, Type returnType) {
    // StmtList -> Stmt StmtList
    //           | e
    while (node) {
        Stmt(node->child, returnType);
        node = node->child->sibling;
    }
}

void Stmt(Node* node, Type returnType) {
    // Stmt -> Exp SEMI
    //       | CompSt
    //       | RETURN Exp SEMI
    //       | IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    //       | WHILE LP Exp RP Stmt

    Type expType = NULL;
    // Stmt -> Exp SEMI
    if (!strcmp(node->child->type, "Exp")) expType = Exp(node->child);

    // Stmt -> CompSt
    else if (!strcmp(node->child->type, "CompSt"))
        CompSt(node->child, returnType);

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->child->type, "RETURN")) {
        expType = Exp(node->child->sibling);

        // check return type
        if (!compareType(returnType, expType))
            Error(RETURN_TYPE, node->lineno,
                   "Type mismatched for return.");
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->child->type, "IF")) {
        Node* stmt = node->child->sibling->sibling->sibling->sibling;
        expType = Exp(node->child->sibling->sibling);
        Stmt(stmt, returnType);
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        if (stmt->sibling != NULL) Stmt(stmt->sibling->sibling, returnType);
    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->child->type, "WHILE")) {
        expType = Exp(node->child->sibling->sibling);
        Stmt(node->child->sibling->sibling->sibling->sibling, returnType);
    }

    if (expType) delType(expType);
}

void DefList(Node* node, TableItem structInfo) {
    // DefList -> Def DefList
    //          | e
    while (node) {
        Def(node->child, structInfo);
        node = node->child->sibling;
    }
}

void Def(Node* node, TableItem structInfo) {
    // Def -> Specifier DecList SEMI
    Type dectype = Specifier(node->child);
    DecList(node->child->sibling, dectype, structInfo);
    if (dectype) delType(dectype);
}

void DecList(Node* node, Type specifier, TableItem structInfo) {
    // DecList -> Dec
    //          | Dec COMMA DecList
    Node* temp = node;
    while (temp) {
        Dec(temp->child, specifier, structInfo);
        if (temp->child->sibling)
            temp = temp->child->sibling->sibling;
        else
            break;
    }
}

void Dec(Node* node, Type specifier, TableItem structInfo) {
    // Dec -> VarDec
    //      | VarDec ASSIGNOP Exp

    // Dec -> VarDec
    if (node->child->sibling == NULL) {
        if (structInfo != NULL) {
            // 结构体内，将VarDec返回的Item中的filedList
            // Copy判断是否重定义，无错则到结构体链表尾 记得del掉Item
            TableItem decitem = VarDec(node->child, specifier);
            FieldList payload = decitem->fieldList;
            FieldList structField = structInfo->fieldList->type->u.structure.fieldList;
            FieldList last = NULL;
            while (structField != NULL) {
                // then we have to check
                if (!strcmp(payload->name, structField->name)) {
                    //出现重定义，报错
                    char msg[MAX_MSG_LENGTH] = {0};
                    sprintf(msg, "Redefined field \"%s\".",
                            decitem->fieldList->name);
                    Error(FIELD_REDEF, node->lineno, msg);
                    delItem(decitem);
                    return;
                } else {
                    last = structField;
                    structField = structField->tail;
                }
            }
            //新建一个fieldlist,删除之前的item
            if (last == NULL) {
                // that is good
                structInfo->fieldList->type->u.structure.fieldList =
                    copyFieldList(decitem->fieldList);
            } else {
                last->tail = copyFieldList(decitem->fieldList);
            }
            delItem(decitem);
        } else {
            // 非结构体内，判断返回的item有无冲突，无冲突放入表中，有冲突报错del
            TableItem decitem = VarDec(node->child, specifier);
            if (hasConfliction(decitem)) {
                //出现冲突，报错
                char msg[MAX_MSG_LENGTH] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->fieldList->name);
                Error(VAR_REDEF, node->lineno, msg);
                delItem(decitem);
            } else {
                addTableItem(decitem);
            }
        }
    }
    // Dec -> VarDec ASSIGNOP Exp
    else {
        if (structInfo != NULL) {
            // 结构体内不能赋值，报错
            Error(FIELD_REDEF, node->lineno,
                   "Illegal initialize variable in struct.");
        } else {
            // 判断赋值类型是否相符
            //如果成功，注册该符号
            TableItem decitem = VarDec(node->child, specifier);
            Type exptype = Exp(node->child->sibling->sibling);
            if (hasConfliction(decitem)) {
                //出现冲突，报错
                char msg[MAX_MSG_LENGTH] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->fieldList->name);
                Error(VAR_REDEF, node->lineno, msg);
                delItem(decitem);
            }
            if (!compareType(decitem->fieldList->type, exptype)) {
                //类型不相符
                //报错
                Error(ASSIGN_TYPE, node->lineno,
                       "Type mismatchedfor assignment.");
                delItem(decitem);
            }
            if (decitem->fieldList->type && decitem->fieldList->type->kind == ARRAY) {
                //报错，对非basic类型赋值
                Error(ASSIGN_TYPE, node->lineno,
                       "Illegal initialize variable.");
                delItem(decitem);
            } else {
                addTableItem(decitem);
            }
            // exp不出意外应该返回一个无用的type，删除
            if (exptype) delType(exptype);
        }
    }
}

Type Exp(Node* node) {
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp
    //      | LP Exp RP
    //      | MINUS Exp
    //      | NOT Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT
    Node* t = node->child;
    // exp will only check if the cal is right
    //  printTable(table);
    //二值运算
    if (!strcmp(t->type, "Exp")) {
        // 基本数学运算符
        if (strcmp(t->sibling->type, "LB") && strcmp(t->sibling->type, "DOT")) {
            Type p1 = Exp(t);
            Type p2 = Exp(t->sibling->sibling);
            Type returnType = NULL;

            // Exp -> Exp ASSIGNOP Exp
            if (!strcmp(t->sibling->type, "ASSIGNOP")) {
                //检查左值
                Node* tchild = t->child;

                if (!strcmp(tchild->type, "FLOAT") ||
                    !strcmp(tchild->type, "INT")) {
                    //报错，左值
                    Error(ASSIGN_RVAL, t->lineno,
                           "The left-hand side of an assignment must be "
                           "avariable.");

                } else if (!strcmp(tchild->type, "ID") ||
                           !strcmp(tchild->sibling->type, "LB") ||
                           !strcmp(tchild->sibling->type, "DOT")) {
                    if (!compareType(p1, p2)) {
                        //报错，类型不匹配
                        Error(ASSIGN_TYPE, t->lineno,
                               "Type mismatched for assignment.");
                    } else
                        returnType = copyType(p1);
                } else {
                    //报错，左值
                    Error(ASSIGN_RVAL, t->lineno,
                           "The left-hand side of an assignment must be "
                           "avariable.");
                }

            }
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | Exp PLUS Exp
            //      | Exp MINUS Exp
            //      | Exp STAR Exp
            //      | Exp DIV Exp
            else {
                if (p1 && p2 && (p1->kind == ARRAY || p2->kind == ARRAY)) {
                    //报错，数组，结构体运算
                    Error(OP_TYPE, t->lineno,
                           "Type mismatched for operands.");
                } else if (!compareType(p1, p2)) {
                    //报错，类型不匹配
                    Error(OP_TYPE, t->lineno,
                           "Type mismatched for operands.");
                } else {
                    if (p1 && p2) {
                        returnType = copyType(p1);
                    }
                }
            }

            if (p1) delType(p1);
            if (p2) delType(p2);
            return returnType;
        }
        // 数组和结构体访问
        else {
            // Exp -> Exp LB Exp RB
            if (!strcmp(t->sibling->type, "LB")) {
                //数组
                Type p1 = Exp(t);
                Type p2 = Exp(t->sibling->sibling);
                Type returnType = NULL;

                if (!p1) {
                    // 第一个exp为null，上层报错，这里不用再管
                } else if (p1 && p1->kind != ARRAY) {
                    //报错，非数组使用[]运算符
                    char msg[MAX_MSG_LENGTH] = {0};
                    sprintf(msg, "\"%s\" is not an array.", t->child->val);
                    Error(NOT_ARRAY, t->lineno, msg);
                } else if (!p2 || p2->kind != BASIC ||
                           p2->u.basic != 0) {
                    //报错，不用int索引[]
                    char msg[MAX_MSG_LENGTH] = {0};
                    sprintf(msg, "\"%s\" is not an integer.",
                            t->sibling->sibling->child->val);
                    Error(NOT_INT, t->lineno, msg);
                } else {
                    returnType = copyType(p1->u.array.elem);
                }
                if (p1) delType(p1);
                if (p2) delType(p2);
                return returnType;
            }
            // Exp -> Exp DOT ID
            else {
                Type p1 = Exp(t);
                Type returnType = NULL;
                if (!p1 || p1->kind != STRUCTURE ||
                    !p1->u.structure.name) {
                    //报错，对非结构体使用.运算符
                    Error(NOT_STRUCT, t->lineno, "Illegal use of \".\".");
                } else {
                    Node* ref_id = t->sibling->sibling;
                    FieldList structfield = p1->u.structure.fieldList;
                    while (structfield != NULL) {
                        if (!strcmp(structfield->name, ref_id->val)) {
                            break;
                        }
                        structfield = structfield->tail;
                    }
                    if (structfield == NULL) {
                        //报错，没有可以匹配的域名
                        printf("Error type %d at Line %d: %s.\n", 14, t->lineno,
                               "non-existed field");
                        ;
                    } else {
                        returnType = copyType(structfield->type);
                    }
                }
                if (p1) delType(p1);
                return returnType;
            }
        }
    }
    //单目运算符
    // Exp -> MINUS Exp
    //      | NOT Exp
    else if (!strcmp(t->type, "MINUS") || !strcmp(t->type, "NOT")) {
        Type p1 = Exp(t->sibling);
        Type returnType = NULL;
        if (!p1 || p1->kind != BASIC) {
            //报错，数组，结构体运算
            printf("Error type %d at Line %d: %s.\n", 7, t->lineno,
                   "OP_TYPE");
        } else {
            returnType = copyType(p1);
        }
        if (p1) delType(p1);
        return returnType;
    } else if (!strcmp(t->type, "LP")) {
        return Exp(t->sibling);
    }
    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(t->type, "ID") && t->sibling) {
        TableItem funcInfo = searchTableItem(t->val);

        // function not find
        if (funcInfo == NULL) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "Undefined function \"%s\".", t->val);
            Error(FUN_UNDEF, node->lineno, msg);
            return NULL;
        } else if (funcInfo->fieldList->type->kind != FUNCTION) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "\"%s\" is not a function.", t->val);
            Error(NOT_FUN, node->lineno, msg);
            return NULL;
        }
        // Exp -> ID LP Args RP
        else if (!strcmp(t->sibling->sibling->type, "Args")) {
            Args(t->sibling->sibling, funcInfo);
            return copyType(funcInfo->fieldList->type->u.function.rType);
        }
        // Exp -> ID LP RP
        else {
            if (funcInfo->fieldList->type->u.function.argc != 0) {
                char msg[MAX_MSG_LENGTH] = {0};
                sprintf(msg,
                        "too few arguments to function \"%s\", except %d args.",
                        funcInfo->fieldList->name,
                        funcInfo->fieldList->type->u.function.argc);
                Error(ARG_INCOMPATIBLE, node->lineno, msg);
            }
            return copyType(funcInfo->fieldList->type->u.function.rType);
        }
    }
    // Exp -> ID
    else if (!strcmp(t->type, "ID")) {
        TableItem tp = searchTableItem(t->val);
        if (tp == NULL || isStructDef(tp)) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "Undefined variable \"%s\".", t->val);
            Error(VAR_UNDEF, t->lineno, msg);
            return NULL;
        } else {
            return copyType(tp->fieldList->type);
        }
    } else {
        // Exp -> FLOAT
        if (!strcmp(t->type, "FLOAT")) {
            return newType(BASIC, 1);
        }
        // Exp -> INT
        else {
            return newType(BASIC, 0);
        }
    }
}

void Args(Node* node, TableItem funcInfo) {
    // Args -> Exp COMMA Args
    //       | Exp

    Node* temp = node;
    FieldList arg = funcInfo->fieldList->type->u.function.argv;

    while (temp) {
        if (arg == NULL) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(
                msg, "too many arguments to function \"%s\", except %d args.",
                funcInfo->fieldList->name, funcInfo->fieldList->type->u.function.argc);
            Error(ARG_INCOMPATIBLE, node->lineno, msg);
            break;
        }
        Type realType = Exp(temp->child);

        if (!compareType(realType, arg->type)) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "Function \"%s\" is not applicable for arguments.",
                    funcInfo->fieldList->name);
            Error(ARG_INCOMPATIBLE, node->lineno, msg);
            if (realType) delType(realType);
            return;
        }
        if (realType) delType(realType);

        arg = arg->tail;
        if (temp->child->sibling) {
            temp = temp->child->sibling->sibling;
        } else {
            break;
        }
    }
    if (arg != NULL) {
        char msg[MAX_MSG_LENGTH] = {0};
        sprintf(msg, "too few arguments to function \"%s\", except %d args.",
                funcInfo->fieldList->name, funcInfo->fieldList->type->u.function.argc);
        Error(ARG_INCOMPATIBLE, node->lineno, msg);
    }
}


// Global function
void tranverseTree(Node *node)
{
    if (node == NULL)
        return;
#if DEBUG
    printf("start tranverse %s.\n", node->type);
#endif
    if (!strcmp(node->type, "ExtDef"))
        ExtDef(node);
    tranverseTree(node->child);
    tranverseTree(node->sibling);
}

void semanticAnalysis(Node *node)
{
    table = initTable();
    hashTable = table->hashTable;
    scope = table->scope;
    tranverseTree(node);
    delTable(table);
}

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
    FieldList tmp = NULL;
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
            tmp = type->u.structure.fieldList;
            while (tmp) {
                FieldList del = tmp;
                tmp = tmp->tail;
                delFieldList(del);
            }
            break;
        case FUNCTION:
            delType(type->u.function.rType);
            tmp = type->u.function.argv;
            while (tmp) {
                FieldList del = tmp;
                tmp = tmp->tail;
                delFieldList(del);
            }
            break;
    }
    // printf("debuger: free type\n");
    if(type)
        free(type);
}

bool compareType(Type t1, Type t2) {
    if (t1 == NULL && t2 == NULL) return true;
    if ((t1 != NULL && t2 == NULL) || (t1 == NULL && t2 != NULL)) return false;
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

// FieldList functions
FieldList newFieldList(char* newName, Type newType) {
    FieldList p = (FieldList)malloc(sizeof(struct FieldList_));
    p->name = newString(newName);
    p->type = newType;
    p->isArg = false;
    p->tail = NULL;
    return p;
}

FieldList copyFieldList(FieldList src) {
    FieldList head = NULL, cur = NULL;
    FieldList tmp = src;

    while (tmp) {
        if (!head) {
            head = newFieldList(tmp->name, copyType(tmp->type));
            cur = head;
            tmp = tmp->tail;
        } else {
            cur->tail = newFieldList(tmp->name, copyType(tmp->type));
            cur = cur->tail;
            tmp = tmp->tail;
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
        TableItem tmp = hashTable->hashArray[i];
        while (tmp) {
            TableItem del = tmp;
            tmp = tmp->sameHash;
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
    TableItem tmp = getLayerHead();
    while (tmp) {
        TableItem del = tmp;
        tmp = tmp->sameScope;
        delTableItem(del);
    }
    setLayerHead(NULL);
    outerLayer();
}

TableItem getLayerHead() {
    printf("debugger: enter getLayerHead\n");
    if (scope) printf("scope exist\n");
    if (scope->scopeLayer) printf("scopeLayer exist\n");
    return scope->scopeLayer[scope->layerDepth];
}

void setLayerHead(TableItem newVal) {
    scope->scopeLayer[scope->layerDepth] = newVal;
}

// Table functions
Table initTable() {
    Table table = (Table)malloc(sizeof(struct Table_));
    hashTable = newHashTable();
    scope = newScope();
    table->hashTable = hashTable;
    table->scope = scope;
    table->anonymousNum = 0;
    printf("debugger: createing read & write\n");
    TableItem read = newItem(
        0, newFieldList(newString("read"),
                        newType(FUNCTION, 0, NULL, newType(BASIC, 0))));

    TableItem write = newItem(
        0, newFieldList(newString("write"),
                        newType(FUNCTION, 1,
                                newFieldList("arg1", newType(BASIC, 0)),
                                newType(BASIC, 0))));
    printf("debugger: adding read & write\n");
    addTableItem(read);
    addTableItem(write);
    printf("debugger: added read & write\n");
    return table;
};

void delTable() {
    delHashTable();
    delScope();
    // printf("debuger: free table\n");
    if (table)
        free(table);
};

TableItem searchTableItem(char* name) {
    unsigned hashCode = getHashCode(name);
    TableItem tmp = getHashHead(hashCode);
    if (tmp == NULL) return NULL;
    while (tmp) {
        if (!strcmp(tmp->fieldList->name, name)) return tmp;
        tmp = tmp->sameHash;
    }
    return NULL;
}

bool hasConfliction(TableItem item) {
    TableItem tmp = searchTableItem(item->fieldList->name);
    if (tmp == NULL) return false;
    while (tmp) {
        if (!strcmp(tmp->fieldList->name, item->fieldList->name)) {
            if (tmp->fieldList->type->kind == STRUCTURE ||
                item->fieldList->type->kind == STRUCTURE)
                return true;
            if (tmp->layerDepth == scope->layerDepth) return true;
        }
        tmp = tmp->sameHash;
    }
    return false;
}

void addTableItem(TableItem item) {
    printf("debugger: get hash code\n");
    unsigned hashCode = getHashCode(item->fieldList->name);

    printf("debugger: get layer head\n");
    item->sameScope = getLayerHead();
    printf("debugger: add to layer\n");
    setLayerHead(item);

    printf("debugger: add to hashtable\n");
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

bool isStructDef(TableItem item) {
    if (item == NULL || item->fieldList->type->kind != STRUCTURE 
        || item->fieldList->type->u.structure.name)
        return false;
    return true;
}

// Generate symbol table functions
void ExtDef(Node* node) {
    Type specifierType = Specifier(node->child);
    char* secondName = node->child->sibling->type;

    if (!strcmp(secondName, "ExtDecList")) {
        ExtDecList(node->child->sibling, specifierType);
    }
    else if (!strcmp(secondName, "FunDec")) {
        FunDec(node->child->sibling, specifierType);
        CompSt(node->child->sibling->sibling, specifierType);
    }
    if (specifierType) delType(specifierType);
}

void ExtDecList(Node* node, Type specifier) {
    Node* tmp = node;
    while (tmp) {
        TableItem item = VarDec(tmp->child, specifier);
        if (hasConfliction(item)) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "Redefined variable \"%s\".", item->fieldList->name);
            Error(VAR_REDEF, tmp->lineno, msg);
            delItem(item);
        } else {
            addTableItem(item);
        }
        if (tmp->child->sibling) {
            tmp = tmp->sibling->sibling->child;
        } else {
            break;
        }
    }
}

Type Specifier(Node* node) {
    Node* t = node->child;
    if (!strcmp(t->type, "TYPE")) {
        if (!strcmp(t->val, "int")) {
            return newType(BASIC, 0);
        } else {
            return newType(BASIC, 1);
        }
    } else {
        return StructSpecifier(t);
    }
}

Type StructSpecifier(Node* node) {
    Type returnType = NULL;
    Node* t = node->child->sibling;

    if (strcmp(t->type, "Tag")) {
        TableItem structItem =
            newItem(scope->layerDepth, newFieldList("", newType(STRUCTURE, NULL, NULL)));
        if (!strcmp(t->type, "OptTag")) {
            setFieldListName(structItem->fieldList, t->child->val);
            t = t->sibling;
        } else {
            table->anonymousNum ++;
            char structName[20] = {0};
            sprintf(structName, "%d", table->anonymousNum);
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
            returnType = newType(STRUCTURE, newString(structItem->fieldList->name),
                copyFieldList(structItem->fieldList->type->u.structure.fieldList));

            if (!strcmp(node->child->sibling->type, "OptTag")) {
                addTableItem(structItem);
            } else {
                delItem(structItem);
            }
        }
    } else {
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
    Node* id = node;
    while (id->child) id = id->child;
    TableItem p = newItem(scope->layerDepth, newFieldList(id->val, NULL));

    if (!strcmp(node->child->type, "ID")) {
        p->fieldList->type = copyType(specifier);
    } else {
        Node* varDec = node->child;
        Type tmp = specifier;
        while (varDec->sibling) {
            tmp = newType(ARRAY, copyType(tmp), atoi(varDec->sibling->sibling->val));
            varDec = varDec->child;
        }
        p->fieldList->type = tmp;
    }
    return p;
}

void FunDec(Node* node, Type returnType) {
    TableItem p = newItem(scope->layerDepth, newFieldList(node->child->val,
        newType(FUNCTION, 0, NULL, copyType(returnType))));

    if (!strcmp(node->child->sibling->sibling->type, "VarList")) {
        VarList(node->child->sibling->sibling, p);
    }

    if (hasConfliction(p)) {
        char msg[MAX_MSG_LENGTH] = {0};
        sprintf(msg, "Redefined function \"%s\".", p->fieldList->name);
        Error(FUN_REDEF, node->lineno, msg);
        delItem(p);
    } else {
        addTableItem(p);
    }
}

void VarList(Node* node, TableItem fun) {
    innerLayer();

    int argc = 0;
    Node* tmp = node->child;
    FieldList cur = NULL;

    FieldList paramDec = ParamDec(tmp);
    fun->fieldList->type->u.function.argv = copyFieldList(paramDec);
    cur = fun->fieldList->type->u.function.argv;
    argc++;

    while (tmp->sibling) {
        tmp = tmp->sibling->sibling->child;
        paramDec = ParamDec(tmp);
        if (paramDec) {
            cur->tail = copyFieldList(paramDec);
            cur = cur->tail;
            argc++;
        }
    }
    fun->fieldList->type->u.function.argc = argc;

    outerLayer();
}

FieldList ParamDec(Node* node) {
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
        p->fieldList->isArg = true;
        addTableItem(p);
        return p->fieldList;
    }
}

void CompSt(Node* node, Type returnType) {
    innerLayer();

    Node* tmp = node->child->sibling;
    if (!strcmp(tmp->type, "DefList")) {
        DefList(tmp, NULL);
        tmp = tmp->sibling;
    }
    if (!strcmp(tmp->type, "StmtList")) {
        StmtList(tmp, returnType);
    }

    // exitLayer();
}

void StmtList(Node* node, Type returnType) {
    while (node) {
        Stmt(node->child, returnType);
        node = node->child->sibling;
    }
}

void Stmt(Node* node, Type returnType) {
    Type expType = NULL;
    if (!strcmp(node->child->type, "Exp")) 
        expType = Exp(node->child);
    else if (!strcmp(node->child->type, "CompSt"))
        CompSt(node->child, returnType);
    else if (!strcmp(node->child->type, "RETURN")) {
        expType = Exp(node->child->sibling);
        if (!compareType(returnType, expType))
            Error(RETURN_TYPE, node->lineno,
                   "Type mismatched for return.");
    } else if (!strcmp(node->child->type, "IF")) {
        Node* stmt = node->child->sibling->sibling->sibling->sibling;
        expType = Exp(node->child->sibling->sibling);
        Stmt(stmt, returnType);
        if (stmt->sibling != NULL) 
            Stmt(stmt->sibling->sibling, returnType);
    } else if (!strcmp(node->child->type, "WHILE")) {
        expType = Exp(node->child->sibling->sibling);
        Stmt(node->child->sibling->sibling->sibling->sibling, returnType);
    }

    if (expType) delType(expType);
}

void DefList(Node* node, TableItem structInfo) {
    while (node) {
        Def(node->child, structInfo);
        node = node->child->sibling;
    }
}

void Def(Node* node, TableItem structInfo) {
    Type dectype = Specifier(node->child);
    DecList(node->child->sibling, dectype, structInfo);
    if (dectype) delType(dectype);
}

void DecList(Node* node, Type specifier, TableItem structInfo) {
    Node* tmp = node;
    while (tmp) {
        Dec(tmp->child, specifier, structInfo);
        if (tmp->child->sibling)
            tmp = tmp->child->sibling->sibling;
        else
            break;
    }
}

void Dec(Node* node, Type specifier, TableItem structInfo) {
    if (node->child->sibling == NULL) {
        if (structInfo != NULL) {
            TableItem decItem = VarDec(node->child, specifier);
            FieldList structField = structInfo->fieldList->type->u.structure.fieldList;
            FieldList last = NULL;
            while (structField != NULL) {
                if (!strcmp(decItem->fieldList->name, structField->name)) {
                    char msg[MAX_MSG_LENGTH] = {0};
                    sprintf(msg, "Redefined field \"%s\".",
                            decItem->fieldList->name);
                    Error(FIELD_REDEF, node->lineno, msg);
                    delItem(decItem);
                    return;
                } else {
                    last = structField;
                    structField = structField->tail;
                }
            }
            
            if (last == NULL) {
                structInfo->fieldList->type->u.structure.fieldList =
                    copyFieldList(decItem->fieldList);
            } else {
                last->tail = copyFieldList(decItem->fieldList);
            }
            // delItem(decItem);
            addTableItem(decItem);
        } else {
            TableItem decItem = VarDec(node->child, specifier);
            if (hasConfliction(decItem)) {
                char msg[MAX_MSG_LENGTH] = {0};
                sprintf(msg, "Redefined variable \"%s\".", decItem->fieldList->name);
                Error(VAR_REDEF, node->lineno, msg);
                delItem(decItem);
            } else {
                addTableItem(decItem);
            }
        }
    } else {
        // TODO: STRUCT & ARRAY ASSIGN
        if (structInfo != NULL) {
            Error(ASSIGN_TYPE, node->lineno,
                   "Cannot initiate in a struct definition.");
        } else {
            TableItem decItem = VarDec(node->child, specifier);
            Type exptype = Exp(node->child->sibling->sibling);
            if (hasConfliction(decItem)) {
                char msg[MAX_MSG_LENGTH] = {0};
                sprintf(msg, "Redefined variable \"%s\".", decItem->fieldList->name);
                Error(VAR_REDEF, node->lineno, msg);
                delItem(decItem);
            } else if (!compareType(decItem->fieldList->type, exptype)) {
                Error(ASSIGN_TYPE, node->lineno, "Cannot cast for assignment.");
                delItem(decItem);
            } else {
                addTableItem(decItem);
            }

            if (exptype) delType(exptype);
        }
    }
}

Type Exp(Node* node) {
    Node* t = node->child;
    if (!strcmp(t->type, "Exp")) {
        if (strcmp(t->sibling->type, "LB") && strcmp(t->sibling->type, "DOT")) {
            Type p1 = Exp(t);
            Type p2 = Exp(t->sibling->sibling);
            Type returnType = NULL;

            if (!strcmp(t->sibling->type, "ASSIGNOP")) {
                Node* tchild = t->child;

                if (!strcmp(tchild->type, "ID") ||
                    !strcmp(tchild->sibling->type, "LB") ||
                    !strcmp(tchild->sibling->type, "DOT")) {
                    if (!compareType(p1, p2))
                            Error(ASSIGN_TYPE, t->lineno, "Cannot cast type for assignment.");
                    else
                            returnType = copyType(p1);
                } else {
                    Error(ASSIGN_RVAL, t->lineno,
                        "The left-hand side of an assignment cannot be a right-value.");
                }
            } else {
                if (p1 && p2 && (p1->kind == ARRAY || p2->kind == ARRAY))
                    Error(OP_TYPE, t->lineno, "Cannot operate two array.");
                else if (!compareType(p1, p2))
                    Error(OP_TYPE, t->lineno, "Cannot operate two different type.");
                else if (p1 && p2)
                    returnType = copyType(p1);
            }

            if (p1) delType(p1);
            if (p2) delType(p2);
            return returnType;
        } else {
            if (!strcmp(t->sibling->type, "LB")) {
                Type p1 = Exp(t);
                Type p2 = Exp(t->sibling->sibling);
                Type returnType = NULL;

                if (p1 && p1->kind != ARRAY) {
                    char msg[MAX_MSG_LENGTH] = {0};
                    sprintf(msg, "\"%s\" is not an array.", t->child->val);
                    Error(NOT_ARRAY, t->lineno, msg);
                } else if (!p2 || p2->kind != BASIC || p2->u.basic != 0) {
                    char msg[MAX_MSG_LENGTH] = {0};
                    sprintf(msg, "\"%s\" is not an integer.", t->sibling->sibling->child->val);
                    Error(NOT_INT, t->lineno, msg);
                } else {
                    returnType = copyType(p1->u.array.elem);
                }
                if (p1) delType(p1);
                if (p2) delType(p2);
                return returnType;
            } else {
                Type p1 = Exp(t);
                Type returnType = NULL;
                if (!p1 || p1->kind != STRUCTURE || !p1->u.structure.name) {
                    char msg[MAX_MSG_LENGTH] = {0};
                    sprintf(msg, "\"%s\" is not a struct.", t->child->val);
                    Error(NOT_STRUCT, t->lineno, msg);
                } else {
                    Node* field = t->sibling->sibling;
                    FieldList structfield = p1->u.structure.fieldList;
                    while (structfield != NULL) {
                        if (!strcmp(structfield->name, field->val)) {
                            break;
                        }
                        structfield = structfield->tail;
                    }
                    if (structfield == NULL) {
                        char msg[MAX_MSG_LENGTH] = {0};
                        sprintf(msg, "field %s is non-existed.", field->val);
                        Error(FIELD_UNDEF, t->lineno, msg);
                    } else {
                        returnType = copyType(structfield->type);
                    }
                }
                if (p1) delType(p1);
                return returnType;
            }
        }
    } else if (!strcmp(t->type, "MINUS") || !strcmp(t->type, "NOT")) {
        Type p1 = Exp(t->sibling);
        Type returnType = NULL;
        if (!p1 || p1->kind != BASIC) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "Cannot operate non-basic type %s.", t->val);
            Error(OP_TYPE, t->lineno, msg);
        } else {
            returnType = copyType(p1);
        }
        if (p1) delType(p1);
        return returnType;
    } else if (!strcmp(t->type, "LP")) {
        return Exp(t->sibling);
    } else if (!strcmp(t->type, "ID") && t->sibling) {
        TableItem funInfo = searchTableItem(t->val);

        if (funInfo == NULL) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "Undefined function \"%s\".", t->val);
            Error(FUN_UNDEF, node->lineno, msg);
            return NULL;
        } else if (funInfo->fieldList->type->kind != FUNCTION) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "\"%s\" is not a function.", t->val);
            Error(NOT_FUN, node->lineno, msg);
            return NULL;
        } else if (!strcmp(t->sibling->sibling->type, "Args")) {
            Args(t->sibling->sibling, funInfo);
            return copyType(funInfo->fieldList->type->u.function.rType);
        } else {
            if (funInfo->fieldList->type->u.function.argc != 0) {
                char msg[MAX_MSG_LENGTH] = {0};
                sprintf(msg, "too few arguments to function \"%s\", except %d args.",
                    funInfo->fieldList->name, funInfo->fieldList->type->u.function.argc);
                Error(ARG_INCOMPATIBLE, node->lineno, msg);
            }
            return copyType(funInfo->fieldList->type->u.function.rType);
        }
    } else if (!strcmp(t->type, "ID")) {
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
        if (!strcmp(t->type, "INT")) {
            return newType(BASIC, 0);
        }
        else {
            return newType(BASIC, 1);
        }
    }
}

void Args(Node* node, TableItem funInfo) {
    Node* tmp = node;
    FieldList arg = funInfo->fieldList->type->u.function.argv;

    while (tmp) {
        if (arg == NULL) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "too many arguments to function \"%s\", except %d args.",
                funInfo->fieldList->name, funInfo->fieldList->type->u.function.argc);
            Error(ARG_INCOMPATIBLE, node->lineno, msg);
            break;
        }
        Type realType = Exp(tmp->child);

        if (!compareType(realType, arg->type)) {
            char msg[MAX_MSG_LENGTH] = {0};
            sprintf(msg, "Function \"%s\" is not applicable for arguments.",
                    funInfo->fieldList->name);
            Error(ARG_INCOMPATIBLE, node->lineno, msg);
            if (realType) delType(realType);
            return;
        }
        if (realType) delType(realType);

        arg = arg->tail;
        if (tmp->child->sibling)
            tmp = tmp->child->sibling->sibling;
        else
            break;
    }

    if (arg != NULL) {
        char msg[MAX_MSG_LENGTH] = {0};
        sprintf(msg, "too few arguments to function \"%s\", except %d args.",
                funInfo->fieldList->name, funInfo->fieldList->type->u.function.argc);
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
    // delTable(table);
}

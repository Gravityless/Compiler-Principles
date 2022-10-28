#include "semantic.h"

Table table = NULL;

Type newType(int kind, ...)
{
    Type t = (Type)malloc(sizeof(struct Type_));
    t->kind = kind;
    va_list ap;
    va_start(ap, kind);
    switch (kind)
    {
    case BASIC:
        t->u.basic = va_arg(ap, int);
        break;
    case ARRAY:
        t->u.array.elem = va_arg(ap, Type);
        t->u.array.size = va_arg(ap, int);
        break;
    case STRUCTURE:
        t->u.structure->name = va_arg(ap, char *);
        t->u.structure->type = va_arg(ap, FieldList);
        break;
    case FUNCTION:
        t->u.function->argc = va_arg(ap, int);
        t->u.function->argv = va_arg(ap, FieldList);
        t->u.function->returnType = va_arg(ap, Type);
        break;
    }
    va_end(ap);
    return t;
}

Type copyType(Type type)
{
    if (type == NULL)
        return NULL;
    Type t = (Type)malloc(sizeof(struct Type_));
    t->kind = type->kind;
    switch (t->kind)
    {
    case BASIC:
        t->u.basic = type->u.basic;
        break;
    case ARRAY:
        t->u.array.elem = copyType(type->u.array.elem);
        t->u.array.size = type->u.array.size;
        break;
    case STRUCTURE:
        t->u.structure->name = newString(type->u.structure->name);
        t->u.structure->type = copyFieldList(type->u.structure->type);
        break;
    case FUNCTION:
        t->u.function->argc = type->u.function->argc;
        t->u.function->argv = copyFieldList(type->u.function->argv);
        t->u.function->returnType = copyType(type->u.function->returnType);
        break;
    }
}

void deleteType(Type type)
{
    FieldList t = NULL;
    switch (type->kind)
    {
    case BASIC:
        break;
    case ARRAY:
        deleteType(type->u.array.elem);
        break;
    case STRUCTURE:
        if (type->u.structure->name)
            free(type->u.structure->name);
        t = type->u.structure->type;
        while (t)
        {
            FieldList fieldList = t;
            t = t->tail;
            deleteFieldList(fieldList);
        }
        break;
    case FUNCTION:
        deleteType(type->u.function->returnType);
        t = type->u.function->argv;
        while (t)
        {
            FieldList fieldList = t;
            t = t->tail;
            deleteFieldList(fieldList);
        }
        break;
    }
    free(type);
}

bool checkType(Type t1, Type t2)
{
    if (t1->kind != t2->kind)
        return false;

    switch (t1->kind)
    {
    case BASIC:
        return t1->u.basic == t2->u.basic;
    case ARRAY:
        return checkType(t1->u.array.elem, t2->u.array.elem);
    case STRUCTURE:
        return !strcmp(t1->u.structure->name, t2->u.structure->name);
    case FUNCTION:
        return false;
    }

    return false;
}

void printType(Type type)
{
    if (type == NULL)
    {
        printf("type is NULL.\n");
        return;
    }

    printf("type kind: %d\n", type->kind);
    switch (type->kind)
    {
    case BASIC:
        printf("type basic: %d\n", type->u.basic);
        break;
    case ARRAY:
        printf("array size: %d\n", type->u.array.size);
        printType(type->u.array.elem);
        break;
    case STRUCTURE:
        if (!type->u.structure->name)
            printf("struct name is NULL\n");
        else
        {
            printf("struct name is %s\n", type->u.structure->name);
        }
        printFieldList(type->u.structure->type);
        break;
    case FUNCTION:
        printf("function argc is %d\n", type->u.function->argc);
        printf("function args:\n");
        printFieldList(type->u.function->argv);
        printf("function return type:\n");
        printType(type->u.function->returnType);
        break;
    }
}

FieldList newFieldList(char *name, Type type)
{
    FieldList p = (FieldList)malloc(sizeof(struct FieldList_));
    p->name = newString(name);
    p->type = type;
    p->tail = NULL;
    return p;
}

FieldList copyFieldList(FieldList fieldList)
{
    FieldList p = NULL, q = NULL;
    FieldList t = fieldList;
    p = newFieldList(t->name, copyType(t->type));
    q = p;
    t = t->tail;

    while (t)
    {
        q->tail = newFieldList(t->name, copyType(t->type));
        q = q->tail;
        t = t->tail;
    }
    return p;
}

void deleteFieldList(FieldList fieldList)
{
    if (fieldList->name)
        free(fieldList->name);
    if (fieldList->type)
        deleteType(fieldList->type);
    free(fieldList);
}

void setFieldListName(FieldList fieldList, char *name)
{
    if (fieldList->name != NULL)
        free(fieldList->name);
    fieldList->name = newString(name);
}

void printFieldList(FieldList fieldList)
{
    if (fieldList == NULL)
        printf("FieldList is NULL\n");
    else
    {
        printf("FieldList name is: %s\n", fieldList->name);
        printf("FieldList Type:\n");
        printType(fieldList->type);
        printFieldList(fieldList->tail);
    }
}

TableItem newItem(int depth, FieldList fieldList)
{
    TableItem t = (TableItem)malloc(sizeof(struct TableItem_));
    t->depth = depth;
    t->fieldList = fieldList;
    t->nextHash = NULL;
    t->nextStack = NULL;
    return t;
}

void deleteItem(TableItem item)
{
    if (item->fieldList != NULL)
        deleteFieldList(item->fieldList);
    free(item);
}

HashTable newHashTable()
{
    HashTable t = (HashTable)malloc(sizeof(struct HashTable_));
    t->hashArray = (TableItem *)malloc(sizeof(struct TableItem_) * HASH_TABLE_SIZE);

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        t->hashArray[i] = NULL;
    }
    return t;
}

void deleteHashTable(HashTable hashTable)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        TableItem t = hashTable->hashArray[i];
        while (t)
        {
            TableItem item = t;
            t = t->nextHash;
            deleteItem(item);
        }
    }
    free(hashTable->hashArray);
    free(hashTable);
}

TableItem getHashHead(HashTable h, int index)
{
    return h->hashArray[index];
}
void setHashHead(HashTable h, int index, TableItem item)
{
    h->hashArray[index] = item;
}

Stack newStack()
{
    Stack t = (Stack)malloc(sizeof(struct Stack_));
    t->stackArray = (TableItem *)malloc(sizeof(TableItem) * HASH_TABLE_SIZE);
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        t->stackArray[i] = NULL;
    }
    t->depth = 0;
    return t;
}

void deleteStack(Stack stack)
{
    free(stack->stackArray);
    free(stack);
}

void increDepth(Stack stack)
{
    stack->depth++;
}

void reduceDepth(Stack stack)
{
    stack->depth--;
}

TableItem getStackHead(Stack stack)
{
    return stack->stackArray[stack->depth];
}

void setStackHead(Stack stack, TableItem item)
{
    stack->stackArray[stack->depth] = item;
}

Table initTable()
{
    Table t = (Table)malloc(sizeof(struct Table_));
    t->hashTable = newHashTable();
    t->stack = newStack();
    t->anonyNum = 0;
    return t;
}

void deleteTable(Table table)
{
    deleteHashTable(table->hashTable);
    deleteStack(table->stack);
    free(table);
}

TableItem searchTableItem(Table table, char *name)
{
    TableItem t = getHashHead(table->hashTable, getHashCode(name));
    if (t == NULL)
        return NULL;
    while (t)
    {
        if (!strcmp(t->fieldList->name, name))
            return t;
        t = t->nextHash;
    }
    return NULL;
}

bool checkTableItem(Table table, TableItem item)
{
    TableItem t = searchTableItem(table, item->fieldList->name);
    if (t == NULL)
        return false;
    while (t)
    {
        if (!strcmp(t->fieldList->name, item->fieldList->name))
        {
            if (t->fieldList->type->kind == STRUCTURE ||
                item->fieldList->type->kind == STRUCTURE)
                return true;
            if (t->depth == table->stack->depth)
                return true;
        }
        t = t->nextHash;
    }
    return false;
}

void addTableItem(Table table, TableItem item)
{
    int hashCode = getHashCode(item->fieldList->name);
    HashTable h = table->hashTable;
    Stack s = table->stack;
    item->nextStack = getStackHead(s);
    setStackHead(s, item);
    item->nextHash = getHashHead(h, hashCode);
    setHashHead(h, hashCode, item);
}

void deleteTableItem(Table table, TableItem item)
{
    int hashCode = getHashCode(item->fieldList->name);
    HashTable h = table->hashTable;
    Stack s = table->stack;
    if (item == getHashHead(h, hashCode))
        setHashHead(h, hashCode, item->nextHash);
    else
    {
        TableItem p = getHashHead(h, hashCode);
        TableItem q = p;
        while (p != item)
        {
            q = p;
            p = p->nextStack;
        }
        q->nextStack = p->nextStack;
    }
    deleteItem(item);
}

bool structDef(TableItem item)
{
    if (item == NULL || item->fieldList->type->kind != STRUCTURE)
        return false;
    return true;
}

void clearCurDepthStackList(Table table)
{
    Stack s = table->stack;
    TableItem t = getStackHead(s);
    while (t)
    {
        TableItem p = t;
        t = t->nextStack;
        deleteTableItem(table, p);
    }
    reduceDepth(s);
}

void printTable(Table table)
{
    printf("----------------hash_table----------------\n");
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        TableItem item = getHashHead(table->hashTable, i);
        if (item)
        {
            printf("[%d]", i);
            while (item)
            {
                printf(" -> name: %s depth: %d\n", item->fieldList->name,
                       item->depth);
                printf("========FiledList========\n");
                printFieldList(item->fieldList);
                printf("===========End===========\n");
                item = item->nextHash;
            }
            printf("\n");
        }
    }
    printf("-------------------end--------------------\n");
}

void genExtDef(Node *node)
{
    Type specifierType = genSpecifier(node->child);
    char *nextType = node->child->sibling->type;

    if (!strcmp(nextType, "ExtDecList"))
    {
        genExtDecList(node->child->sibling, specifierType);
    }
    else if (!strcmp(nextType, "FunDec"))
    {
        genFunDec(node->child->sibling, specifierType);
        genCompSt(node->child->sibling->sibling, specifierType);
    }

    if (specifierType)
        deleteType(specifierType);
}

// TODO: line 398 if?
void genExtDecList(Node *node, Type specifier)
{
    Node *pNode = node;
    while (pNode)
    {
        TableItem pItem = genVarDec(pNode->child, specifier);
        if (checkTableItem(table, pItem))
        {
            char msg[80] = {0};
            sprintf(msg, "Redefine variable \"%s\".", pItem->fieldList->name);
            Error(VAR_REDEF, pNode->lineno, msg);
            deleteItem(pItem);
        }
        else
        {
            addTableItem(table, pItem);
        }
        if (pNode->child->sibling != NULL)
            pNode = pNode->sibling->sibling->child;
        else
            break;
    }
}

Type genSpecifier(Node *node)
{
    Node *pNode = node->child;
    if (!strcmp(pNode->type, "TYPE"))
    {
        if (!strcmp(pNode->val, "int"))
            return newType(BASIC, 0);
        else
            return newType(BASIC, 1);
    }
    else
    {
        return genStructSpecifier(pNode);
    }
}

// TODO: line 435 where is LB?
Type genStructSpecifier(Node *node)
{
    Type result = NULL;
    Node *pNode = node->child->sibling;

    if (strcmp(pNode->type, "Tag"))
    {
        TableItem item = newItem(table->stack->depth,
                                 newFieldList("", newType(STRUCTURE, NULL, NULL)));
        if (!strcmp(pNode->type, "OptTag"))
        {
            setFieldListName(item->fieldList, pNode->child->val);
            pNode = pNode->sibling;
        }
        else
        {
            table->anonyNum++;
            char name[40] = {0};
            sprintf(name, "%d", table->anonyNum);
            setFieldListName(item->fieldList, name);
        }

        if (!strcmp(pNode->sibling->type, "DefList"))
            genDefList(pNode->sibling, item);
        if (checkTableItem(table, item))
        {
            char msg[80] = {0};
            sprintf(msg, "Duplicated name \"%s\".", item->fieldList->name);
            Error(STRUCT_REDEF, node->lineno, msg);
            deleteItem(item);
        }
        else
        {
            result = newType(STRUCTURE, newString(item->fieldList->name),
                             copyFieldList(item->fieldList->type->u.structure->type));
            if (!strcmp(node->child->sibling->type, "OptTag"))
                addTableItem(table, item);
            else
                deleteItem(item);
        }
    }
    else
    {
        TableItem item = searchTableItem(table, pNode->child->val);
        if (item == NULL || !structDef(item))
        {
            char msg[80] = {0};
            sprintf(msg, "Undefined structure \"%s\".", pNode->child->val);
            Error(STRUCT_UNDEF, node->lineno, msg);
        }
        else
        {
            result = newType(STRUCTURE, newString(item->fieldList->name),
                             copyFieldList(item->fieldList->type->u.structure->type));
        }
    }

    return result;
}

TableItem genVarDec(Node *node, Type specifier)
{
    Node *id = node;
    while (id->child != NULL)
        id = id->child;
    TableItem item = newItem(table->stack->depth, newFieldList(id->val, NULL));
    if (!strcmp(node->child->type, "ID"))
        item->fieldList->type = copyType(specifier);
    else
    {
        Node *varDec = node->child;
        Type type = specifier;
        while (varDec->sibling)
        {
            item->fieldList->type = newType(ARRAY, copyType(type), atoi(varDec->sibling->sibling->val));
            type = item->fieldList->type;
            varDec = varDec->child;
        }
    }
    return item;
}

void genFunDec(Node *node, Type returnType)
{
    TableItem item = newItem(table->stack->depth, newFieldList(node->child->val,
                                                               newType(FUNCTION, 0, NULL, copyType(returnType))));
    if (!strcmp(node->child->sibling->sibling->type, "VarList"))
        VarList(node->child->sibling->sibling, item);
    if (checkTableItem(table, item))
    {
        char msg[80] = {0};
        sprintf(msg, "Redefine function \"%s\".", item->fieldList->name);
        Error(FUN_REDEF, node->lineno, msg);
        deleteItem(item);
    }
    else
    {
        addTableItem(table, item);
    }
}

void genVarList(Node *node, TableItem func)
{
    increDepth(table->stack);
    int argc = 0;
    Node *pNode = node->child;
    FieldList fieldList = NULL;

    FieldList paramDec = genParamDec(pNode);
    func->fieldList->type->u.function->argv = copyFieldList(paramDec);
    fieldList = func->fieldList->type->u.function->argv;
    argc++;

    while (pNode->sibling)
    {
        pNode = pNode->sibling->sibling->child;
        paramDec = genParamDec(pNode);
        if (paramDec)
        {
            fieldList->tail = copyFieldList(paramDec);
            fieldList = fieldList->tail;
            argc++;
        }
    }

    func->fieldList->type->u.function->argc = argc;
    reduceDepth(table->stack);
}

FieldList genParamDec(Node *node)
{
    Type specifierType = genSpecifier(node->child);
    TableItem item = genVarDec(node->child->sibling, specifierType);
    if (specifierType)
        deleteType(specifierType);
    if (checkTableItem(table, item))
    {
        char msg[80] = {0};
        sprintf(msg, "Redefine variable \"%s\".", item->fieldList->name);
        Error(VAR_REDEF, node->lineno, msg);
        deleteItem(item);
        return NULL;
    }
    else
    {
        addTableItem(table, item);
        return item->fieldList;
    }
}

void genCompSt(Node *node, Type returnType)
{
    increDepth(table->stack);
    Node *pNode = node->child->sibling;
    if (!strcmp(pNode->type, "DefList"))
    {
        DefList(pNode, NULL);
        pNode = pNode->sibling;
    }
    if (!strcmp(pNode->type, "StmtList"))
    {
        StmtList(pNode, returnType);
    }

    clearCurDepthStackList(table);
}

void genStmtList(Node *node, Type returnType)
{
    while (node)
    {
        Stmt(node->child, returnType);
        node = node->child->sibling;
    }
}

void genStmt(Node *node, Type returnType)
{
    Type expType = NULL;

    if (!strcmp(node->child->type, "Exp"))
        expType = genExp(node->child);
    else if (!strcmp(node->child->type, "CompSt"))
        genCompSt(node->child, returnType);
    else if (!strcmp(node->child->type, "RETURN"))
    {
        expType = genExp(node->child->sibling);
        if (!checkType(returnType, expType))
            Error(RETURN_TYPE, node->lineno, "Type mismatched for return.");
    }
    else if (!strcmp(node->child->type, "IF"))
    {
        Node *stmt = node->child->sibling->sibling->sibling->sibling;
        expType = genExp(node->child->sibling->sibling);
        genStmt(stmt, returnType);
        if (stmt->sibling != NULL)
            genStmt(stmt->sibling->sibling, returnType);
    }
    else if (!strcmp(node->child->type, "WHILE"))
    {
        expType = genExp(node->child->sibling->sibling);
        genStmt(node->child->sibling->sibling->sibling->sibling, returnType);
    }

    if (expType)
        deleteType(expType);
}

void genDefList(Node *node, TableItem structInfo)
{
    while (node)
    {
        genDef(node->child, structInfo);
        node = node->child->sibling;
    }
}

void genDef(Node *node, TableItem structInfo)
{
    Type dectype = genSpecifier(node->child);
    genDecList(node->child->sibling, dectype, structInfo);
    if (dectype)
        deleteType(dectype);
}

void genDecList(Node *node, Type specifier, TableItem structInfo)
{
    Node *pNode = node;
    while (pNode)
    {
        Dec(pNode->child, specifier, structInfo);
        if (pNode->child->sibling)
            pNode = pNode->child->sibling->sibling;
        else
            break;
    }
}

void genDec(Node *node, Type specifier, TableItem structInfo)
{
    if (node->child->sibling == NULL)
    {
        if (structInfo != NULL)
        {
            TableItem decitem = VarDec(node->child, specifier);
            FieldList payload = decitem->fieldList;
            FieldList structField = structInfo->fieldList->type->u.structure->type;
            FieldList last = NULL;
            while (structField != NULL)
            {
                if (!strcmp(payload->name, structField->name))
                {
                    char msg[80] = {0};
                    sprintf(msg, "Redefined field \"%s\".",
                            decitem->fieldList->name);
                    Error(FEILD_REDEF, node->lineno, msg);
                    deleteItem(decitem);
                    return;
                }
                else
                {
                    last = structField;
                    structField = structField->tail;
                }
            }
            if (last == NULL)
            {
                structInfo->fieldList->type->u.structure->type =
                    copyFieldList(decitem->fieldList);
            }
            else
            {
                last->tail = copyFieldList(decitem->fieldList);
            }
            deleteItem(decitem);
        }
        else
        {
            TableItem decitem = VarDec(node->child, specifier);
            if (checkTableItemConflict(table, decitem))
            {
                char msg[80] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->fieldList->name);
                Error(VAR_REDEF, node->lineno, msg);
                deleteItem(decitem);
            }
            else
            {
                addTableItem(table, decitem);
            }
        }
    }
    else
    {
        if (structInfo != NULL)
        {
            Error(FEILD_REDEF, node->lineno,
                  "Illegal initialize variable in struct.");
        }
        else
        {
            TableItem decitem = VarDec(node->child, specifier);
            Type exptype = Exp(node->child->sibling->sibling);
            if (checkTableItemConflict(table, decitem))
            {
                char msg[80] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->fieldList->name);
                Error(VAR_REDEF, node->lineno, msg);
                deleteItem(decitem);
            }
            if (!checkType(decitem->fieldList->type, exptype))
            {
                Error(ASSIGN_TYPE, node->lineno,
                      "Type mismatchedfor assignment.");
                deleteItem(decitem);
            }
            if (decitem->fieldList->type && decitem->fieldList->type->kind == ARRAY)
            {
                Error(ASSIGN_TYPE, node->lineno,
                      "Illegal initialize variable.");
                deleteItem(decitem);
            }
            else
            {
                addTableItem(table, decitem);
            }
            if (exptype)
                deleteType(exptype);
        }
    }
}

Type genExp(Node *node)
{
    if (!strcmp(node->type, "Exp"))
    {
        if (strcmp(node->sibling->type, "LB") && strcmp(node->sibling->type, "DOT"))
        {
            Type p1 = genExp(node);
            Type p2 = genExp(node->sibling->sibling);
            Type returnType = NULL;

            if (!strcmp(node->sibling->type, "ASSIGNOP"))
            {

                Node *tchild = node->child;

                if (!strcmp(tchild->type, "FLOAT") ||
                    !strcmp(tchild->type, "INT"))
                {

                    Error(ASSIGN_RVAL, node->lineno,
                          "The left-hand side of an assignment must be "
                          "avariable.");
                }
                else if (!strcmp(tchild->type, "ID") ||
                         !strcmp(tchild->sibling->type, "LB") ||
                         !strcmp(tchild->sibling->type, "DOT"))
                {
                    if (!checkType(p1, p2))
                    {

                        Error(ASSIGN_TYPE, node->lineno,
                              "Type mismatched for assignment.");
                    }
                    else
                        returnType = copyType(p1);
                }
                else
                {

                    Error(ASSIGN_RVAL, node->lineno,
                          "The left-hand side of an assignment must be "
                          "avariable.");
                }
            }
            else
            {
                if (p1 && p2 && (p1->kind == ARRAY || p2->kind == ARRAY))
                {
                    Error(OP_TYPE, node->lineno,
                          "Type mismatched for operands.");
                }
                else if (!checkType(p1, p2))
                {
                    Error(OP_TYPE, node->lineno,
                          "Type mismatched for operands.");
                }
                else
                {
                    if (p1 && p2)
                    {
                        returnType = copyType(p1);
                    }
                }
            }

            if (p1)
                deleteType(p1);
            if (p2)
                deleteType(p2);
            return returnType;
        }
        else
        {
            if (!strcmp(node->sibling->type, "LB"))
            {
                Type p1 = genExp(node);
                Type p2 = genExp(node->sibling->sibling);
                Type returnType = NULL;

                if (!p1)
                {
                }
                else if (p1 && p1->kind != ARRAY)
                {
                    char msg[80] = {0};
                    sprintf(msg, "\"%s\" is not an array.", node->child->val);
                    Error(NOT_ARRAY, node->lineno, msg);
                }
                else if (!p2 || p2->kind != BASIC ||
                         p2->u.basic != 0)
                {
                    char msg[80] = {0};
                    sprintf(msg, "\"%s\" is not an integer.",
                            node->sibling->sibling->child->val);
                    Error(NOT_INT, node->lineno, msg);
                }
                else
                {
                    returnType = copyType(p1->u.array.elem);
                }
                if (p1)
                    deleteType(p1);
                if (p2)
                    deleteType(p2);
                return returnType;
            }
            else
            {
                Type p1 = genExp(node);
                Type returnType = NULL;
                if (!p1 || p1->kind != STRUCTURE ||
                    !p1->u.structure->name)
                {
                    Error(NOT_STRUCT, node->lineno, "Illegal use of \".\".");
                    if (p1)
                        deleteType(p1);
                }
                else
                {
                    Node *ref_id = node->sibling->sibling;
                    FieldList structfield = p1->u.structure->type;
                    while (structfield != NULL)
                    {
                        if (!strcmp(structfield->name, ref_id->val))
                        {
                            break;
                        }
                        structfield = structfield->tail;
                    }
                    if (structfield == NULL)
                    {
                        printf("Error type %d at Line %d: %s.\n", 14, node->lineno,
                               "NONEXISTFIELD");
                        ;
                    }
                    else
                    {
                        returnType = copyType(structfield->type);
                    }
                }
                if (p1)
                    deleteType(p1);
                return returnType;
            }
        }
    }
    else if (!strcmp(node->type, "MINUS") || !strcmp(node->type, "NOT"))
    {
        Type p1 = genExp(node->sibling);
        Type returnType = NULL;
        if (!p1 || p1->kind != BASIC)
        {
            printf("Error type %d at Line %d: %s.\n", 7, node->lineno,
                   "OP_TYPE");
        }
        else
        {
            returnType = copyType(p1);
        }
        if (p1)
            deleteType(p1);
        return returnType;
    }
    else if (!strcmp(node->type, "LP"))
    {
        return Exp(node->sibling);
    }
    else if (!strcmp(node->type, "ID") && node->sibling)
    {
        TableItem funcInfo = searchTableItem(table, node->val);

        if (funcInfo == NULL)
        {
            char msg[80] = {0};
            sprintf(msg, "Undefined function \"%s\".", node->val);
            Error(FUN_UNDEF, node->lineno, msg);
            return NULL;
        }
        else if (funcInfo->fieldList->type->kind != FUNCTION)
        {
            char msg[80] = {0};
            sprintf(msg, "\"i\" is not a function.", node->val);
            Error(NOT_FUN, node->lineno, msg);
            return NULL;
        }
        else if (!strcmp(node->sibling->sibling->type, "Args"))
        {
            genArgs(node->sibling->sibling, funcInfo);
            return copyType(funcInfo->fieldList->type->u.function->returnType);
        }
        else
        {
            if (funcInfo->fieldList->type->u.function->argc != 0)
            {
                char msg[80] = {0};
                sprintf(msg,
                        "too few arguments to function \"%s\", except %d args.",
                        funcInfo->fieldList->name,
                        funcInfo->fieldList->type->u.function->argc);
                Error(ARG_INCOMPATIBLE, node->lineno, msg);
            }
            return copyType(funcInfo->fieldList->type->u.function->returnType);
        }
    }
    else if (!strcmp(node->type, "ID"))
    {
        TableItem tp = searchTableItem(table, node->val);
        if (tp == NULL || isStructDef(tp))
        {
            char msg[80] = {0};
            sprintf(msg, "Undefined variable \"%s\".", node->val);
            Error(VAR_UNDEF, node->lineno, msg);
            return NULL;
        }
        else
        {
            return copyType(tp->fieldList->type);
        }
    }
    else
    {
        if (!strcmp(node->type, "FLOAT"))
        {
            return newType(BASIC, 1);
        }
        else
        {
            return newType(BASIC, 0);
        }
    }
}

void genArgs(Node *node, TableItem funcInfo)
{
    Node *temp = node;
    FieldList arg = funcInfo->fieldList->type->u.function->argv;
    while (temp)
    {
        if (arg == NULL)
        {
            char msg[80] = {0};
            sprintf(
                msg, "too many arguments to function \"%s\", except %d args.",
                funcInfo->fieldList->name, funcInfo->fieldList->type->u.function->argc);
            Error(ARG_INCOMPATIBLE, node->lineno, msg);
            break;
        }
        Type realType = genExp(temp->child);
        if (!checkType(realType, arg->type))
        {
            char msg[80] = {0};
            sprintf(msg, "Function \"%s\" is not applicable for arguments.",
                    funcInfo->fieldList->name);
            Error(ARG_INCOMPATIBLE, node->lineno, msg);
            if (realType)
                deleteType(realType);
            return;
        }
        if (realType)
            deleteType(realType);

        arg = arg->tail;
        if (temp->child->sibling)
        {
            temp = temp->child->sibling->sibling;
        }
        else
        {
            break;
        }
    }
    if (arg != NULL)
    {
        char msg[80] = {0};
        sprintf(msg, "too few arguments to function \"%s\", except %d args.",
                funcInfo->fieldList->name, funcInfo->fieldList->type->u.function->argc);
        Error(ARG_INCOMPATIBLE, node->lineno, msg);
    }
}

void traverseTree(Node *node)
{
    if (node == NULL)
        return;
    if (!strcmp(node->type, "ExtDef"))
        genExtDef(node);

    tranverseTree(node->child);
    tranverseTree(node->sibling);
}

void semanticAnalysis(Node *node)
{
    table = initTable();
    traverseTree(node);
    deleteTable(table);
}

#include "node.h"

Node* newNode(char* type, char* val, int lineno, bool is_token, int argc, ...) {
    Node* p = (Node*) malloc(sizeof(Node));
    strcpy(p->type, type);
    strcpy(p->val, val);
    p->lineno = lineno;
    p->token = is_token;
    p->sibling = NULL;
    p->child = NULL;

    if (!is_token) {
        va_list vargs;
        va_start(vargs, argc);
        Subtree child = NULL;
        while (child == NULL && argc > 0) {
            child = va_arg(vargs, Subtree);
            argc--;
        }
        p->child = child;

        for (; argc > 0; argc--) {
            child->sibling = va_arg(vargs, Subtree);
            if (child->sibling != NULL) child = child->sibling;
        }
        
        va_end(vargs);
    }

    return p;
}

void printTree(Node* node, int space) {
    if (node == NULL) return;

    for (int i = 0; i < space; i++) printf("  ");

    if (!node->token) 
        printf("%s (%d)\n", node->type, node->lineno);
    else if (strcmp("INT", node->type) == 0)
        printf("%s: %d\n", node->type, atoi(node->val));
    else if (strcmp("FLOAT", node->type) == 0)
        printf("%s: %f\n", node->type, atof(node->val));
    else if(strcmp("ID", node->type) == 0 \
        || strcmp("TYPE", node->type) == 0)
        printf("%s: %s\n", node->type, node->val);
    else
        printf("%s\n", node->type);

    printTree(node->child, space + 1);
    printTree(node->sibling, space);

    return;  
}

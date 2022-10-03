#include "node.h"

Node* newNode(char* type, char* val, int lineno, bool is_token, int argc, ...) {
    Node* p = (Node*) malloc(sizeof(Node));
    strcpy(p->type, type);
    strcpy(p->val, val);
    p->lineno = lineno;
    p->token = is_token;
    p->sib = NULL;
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
            child->sib = va_arg(vargs, Subtree);
            if (child->sib != NULL) child = child->sib;
        }
        
        va_end(vargs);
    }

    return p;
}

void printSubtree(Node* node, int space) {
    if (node == NULL) return;

    for (int i = 0; i < space; i++) printf("  ");

    if (!node->token) 
        printf("%s (%d)\n", node->type, node->lineno);
    else if(strcmp("ID", node->type) == 0 \
        || strcmp("TYPE", node->type) == 0 \
        || strcmp("INT", node->type) == 0 \
        || strcmp("FLOAT", node->type) == 0)
        printf("%s: %s\n", node->type, node->val);
    else
        printf("%s\n", node->type);

    printSubtree(node->child, space + 1);
    printSubtree(node->sib, space);

    return;  
}

void printTree(Node* root) {
    printSubtree(root, 0);
    return;
}

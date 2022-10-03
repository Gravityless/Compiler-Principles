#include "node.h"

Node* newNode(char* name, char* val, int lineno, bool is_token, int argc, ...) {
    Node* p = (Node*) malloc(sizeof(Node));
    strcpy(p->name, name);
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
#ifndef _NODE_H_
#define _NODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef struct node {
    char type[40];
    char val[40];
    int lineno;
    bool token;

    struct node* sibling;
    struct node* child;
} Node, *Subtree;

Node* newNode(char* type, char* val, int lineno, bool is_token, int argc, ...);

void printTree(Node* node, int space);

void printTreetoFile(Node* node, int space, FILE *fp);

void generateTree(Node* node);

#endif
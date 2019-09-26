#include "header.h"
#include <stdlib.h>

extern int linenumber;

AST_NODE *Allocate(AST_TYPE type){
    AST_NODE *temp;
    temp = (AST_NODE*)malloc(sizeof(struct AST_NODE));
    temp->nodeType = type;
    temp->dataType = NONE_TYPE;
    temp->child = NULL;
    temp->rightSibling = NULL;
    temp->parent = NULL;
    // Notice that leftmostSibling is not initialized as NULL
    temp->leftmostSibling = temp;
    temp->linenumber = linenumber;
    return temp;
}

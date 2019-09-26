#include "header.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char *AST_TYPE_string[] = {"PROGRAM", "GLOBAL_DECL_LIST", "GLOBAL_DECL", "DECL_LIST", "FUNCTION_DECL", "PARAM_LIST", "PARAM", "DIM_FN", "DIMFN1", "EXPR_NULL", "BLOCK", "DECL", "TYPE_DECL", "VAR_DECL",
    "TYPE", "STRUCT_TYPE", "DEF_LIST", "DEF", "OPT_TAG", "TAG", "ID_LIST", "DIM_DECL", "CEXPR", "MCEXPR", "CFACTOR", "INIT_ID_LIST", "INIT_ID", "STMT_LIST", "STMT", "ASSIGN_EXPR_LIST",
    "NONEMPTY_ASSIGN_EXPR_LIST", "TEST", "ASSIGN_EXPR", "RELOP_EXPR", "RELOP_TERM", "RELOP_FACTOR", "REL_OP", "RELOP_EXPR_LIST", "NONEMPTY_RELOP_EXPR_LIST", "EXPR", "ADD_OP", "TERM",
    "MUL_OP", "FACTOR", "VAR_REF", "DIM", "STRUCT_TAIL", "NUL","ID_value", "CONST_value"};

int printGVNode(FILE *fp, AST_NODE* node, int count);

char *printLabelString(FILE *fp, AST_NODE *astNode)
{
    char *binaryOpString[] = {
        "+",
        "-",
        "*",
        "/",
        "==",
        ">=",
        "<=",
        "!=",
        ">",
        "<",
        "&&",
        "||"
    };
    char *unaryOpString[] = {
        "+",
        "-",
        "!"
    };
//    fprintf(fp, "%d ", astNode->linenumber);
    switch (astNode->nodeType) {
        case PROGRAM_NODE:
            fprintf(fp, "PROGRAM_NODE");
            break;
        case DECLARATION_NODE:
            fprintf(fp, "DECLARATION_NODE ");
            switch (astNode->semantic_value.declSemanticValue.kind) {
                case VARIABLE_DECL:
                    fprintf(fp, "VARIABLE_DECL");
                    break;
                case TYPE_DECL:
                    fprintf(fp, "TYPE_DECL");
                    break;
                case FUNCTION_DECL:
                    fprintf(fp, "FUNCTION_DECL");
                    break;
                case FUNCTION_PARAMETER_DECL:
                    fprintf(fp, "FUNCTION_PARAMETER_DECL");
                    break;
            }
            break;
        case IDENTIFIER_NODE:
            fprintf(fp, "IDENTIFIER_NODE ");
            fprintf(fp, "%s ", astNode->semantic_value.identifierSemanticValue.identifierName);
            switch (astNode->semantic_value.identifierSemanticValue.kind) {
                case NORMAL_ID:
                    fprintf(fp, "NORMAL_ID");
                    break;
                case ARRAY_ID:
                    fprintf(fp, "ARRAY_ID");
                    break;
                case WITH_INIT_ID:
                    fprintf(fp, "WITH_INIT_ID");
                    break;
            }
            break;
        case PARAM_LIST_NODE:
            fprintf(fp, "PARAM_LIST_NODE");
            break;
        case NUL_NODE:
            fprintf(fp, "NUL_NODE");
            break;
        case BLOCK_NODE:
            fprintf(fp, "BLOCK_NODE");
            break;
        case VARIABLE_DECL_LIST_NODE:
            fprintf(fp, "VARIABLE_DECL_LIST_NODE");
            break;
        case STMT_LIST_NODE:
            fprintf(fp, "STMT_LIST_NODE");
            break;
        case STMT_NODE:
            fprintf(fp, "STMT_NODE ");
            switch (astNode->semantic_value.stmtSemanticValue.kind) {
                case WHILE_STMT:
                    fprintf(fp, "WHILE_STMT");
                    break;
                case FOR_STMT:
                    fprintf(fp, "FOR_STMT");
                    break;
                case ASSIGN_STMT:
                    fprintf(fp, "ASSIGN_STMT");
                    break;
                case IF_STMT:
                    fprintf(fp, "IF_STMT");
                    break;
                case FUNCTION_CALL_STMT:
                    fprintf(fp, "FUNCTION_CALL_STMT");
                    break;
                case RETURN_STMT:
                    fprintf(fp, "RETURN_STMT");
                    break;
            }
            break;
        case EXPR_NODE:
            fprintf(fp, "EXPR_NODE ");
            switch (astNode->semantic_value.exprSemanticValue.kind) {
                case BINARY_OPERATION:
                    fprintf(fp, "%s", binaryOpString[astNode->semantic_value.exprSemanticValue.op.binaryOp]);
                    break;
                case UNARY_OPERATION:
                    fprintf(fp, "%s", unaryOpString[astNode->semantic_value.exprSemanticValue.op.unaryOp]);
                    break;
            }
            break;
        case CONST_VALUE_NODE:
            fprintf(fp, "CONST_VALUE_NODE ");
            switch (astNode->semantic_value.const1->const_type) {
                case  INTEGERC:
                    fprintf(fp, "%d", astNode->semantic_value.const1->const_u.intval);
                    break;
                case FLOATC:
                    fprintf(fp, "%f", astNode->semantic_value.const1->const_u.fval);
                    break;
                case STRINGC:
                    astNode->semantic_value.const1->const_u.sc[strlen(astNode->semantic_value.const1->const_u.sc) - 1] = 0;
                    fprintf(fp, "\\\"%s\\\"", astNode->semantic_value.const1->const_u.sc + 1);
                    astNode->semantic_value.const1->const_u.sc[strlen(astNode->semantic_value.const1->const_u.sc)] = '"';
                    break;
            }
            break;
        case NONEMPTY_ASSIGN_EXPR_LIST_NODE:
            fprintf(fp, "NONEMPTY_ASSIGN_EXPR_LIST_NODE");
            break;
        case NONEMPTY_RELOP_EXPR_LIST_NODE:
            fprintf(fp, "NONEMPTY_RELOP_EXPR_LIST_NODE");
            break;
        default:
            fprintf(fp, "default case in char *getLabelString(AST_TYPE astType)");
            break;
    }
}

void printGV(AST_NODE *root, char* fileName)
{
    if (fileName == NULL) {
        fileName = "AST_Graph.gv";
    }
    FILE *fp;
    fp = fopen(fileName, "w");
    if (!fp) {
        printf("Cannot open file \"%s\"\n", fileName);
        return;
    }
    fprintf(fp , "Digraph AST\n");
    fprintf(fp , "{\n");
    fprintf(fp , "label = \"%s\"\n", fileName);

    int nodeCount = 0;
    printGVNode(fp, root, nodeCount);

    fprintf(fp , "}\n");
    fclose(fp);
}

// count: the (unused) id number to be used
// return: then next unused id number
int printGVNode(FILE *fp, AST_NODE* node, int count)
{
    if (node == NULL) {
        return count;
    }

    int currentNodeCount = count;
    fprintf(fp, "node%d [label =\"", count);
    printLabelString(fp, node);
    fprintf(fp, "\"]\n");
    ++count;
    int countAfterCheckChildren = count;
    if (node->child) {
        countAfterCheckChildren = printGVNode(fp, node->child, count);
        fprintf(fp, "node%d -> node%d [style = bold]\n", currentNodeCount, count);
    }

    int countAfterCheckSibling = countAfterCheckChildren;
    if (node->rightSibling) {
        countAfterCheckSibling = printGVNode(fp, node->rightSibling, countAfterCheckChildren);
        fprintf(fp, "node%d -> node%d [style = dashed]\n", currentNodeCount, countAfterCheckChildren);
    }

    return countAfterCheckSibling;
}


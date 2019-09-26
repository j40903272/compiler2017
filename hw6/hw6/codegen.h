#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "header.h"
#include "symbolTable.h"

char *idName(AST_NODE * node);
void gen_head(char *name);
void gen_prologue();
void gen_epilogue(char *name);
int get_reg();
void free_reg(int reg);
void gen_Alignment();
void gen_data(int reg, int offset, char type, int flag);

void codegen(AST_NODE *root);
void genProgram(AST_NODE *root);
void genGlobalVarDecl(AST_NODE *varDeclListNode, int flag);
void genFunctionDecl(AST_NODE *funcDeclNode);
void genLocalVarDecl(AST_NODE *DeclListNode);
void genVarDeclInit(AST_NODE * declNode);
void genStmtList(AST_NODE *stmtListNode);
void genStmt(AST_NODE *stmtNode);
void genBlock(AST_NODE *blockNode);
void genWhileStmt(AST_NODE *whileNode);
void genIfStmt(AST_NODE *ifNode);
void genForStmt(AST_NODE *forNode);
void genAssignStmt(AST_NODE *assignNode);
void genFuncCall(AST_NODE *funcNode);
void genReturnStmt(AST_NODE *returnNode);
int genExprRelated(AST_NODE *exprRNode);
int genExpr(AST_NODE *exprNode);
void genWrite(AST_NODE *node);
void genIntBinaryOp(AST_NODE *exprNode, int reg1, int reg2, char *op);
void genFloatBinaryOp(AST_NODE *exprNode, int reg1, int reg2, char *op);

#endif
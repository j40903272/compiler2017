#include "codegen.h"
#include "header.h"
#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *output;
int regs[32], g_cnt, g_offset;

void gen_head(char *name){
	fprintf(output, ".text\n");
	fprintf(output, "_start_%s:\n", name);
}

void gen_prologue(){
	fprintf(output, "str x30, [sp, #0]\n");
	fprintf(output, "str x29, [sp, #-8]\n");
	fprintf(output, "add x29, sp, #-8\n");
	fprintf(output, "add sp, sp, #-16\n");
	int offset = 0;
	for(int i = 19; i <= 29; ++i){
		offset += 8;
		fprintf(output, "str x%d, [x29, #%d]\n", i, -offset);
	}
	fprintf(output, ".data\n");
	fprintf(output, "_AR_%d: .word %d\n", g_cnt, offset);
	gen_Alignment();
	fprintf(output, ".text\n");
	fprintf(output, "ldr w19, _AR_%d\n", g_cnt++);
	fprintf(output, "sub sp, sp, w19\n");
	g_offset = 0;
}

void gen_epilogue(char *name){
	int offset = 0;
	fprintf(output, "_end_%s:\n", name);
	for(int i = 19; i <= 29; ++i){
		offset += 8;
		fprintf(output, "ldr x%d, [x29, #%d]\n", i, -offset);
	}
	fprintf(output, "ldr x30, [x29, #8]\n");
	fprintf(output, "add sp, x29, #8\n");
	fprintf(output, "ldr x29, [x29, #0]\n");
	fprintf(output, "ret x30\n");
}

int get_reg(){
	for(int i = 19 ; i <= 29 ; i++){
		if(!regs[i]){
			regs[i] = 1;
			return i;
		}
	}
	printf("out of register!!!!\n");
	exit(0);
}

void free_reg(int reg){
	regs[reg] = 0;
}

void gen_Alignment(){
	fprintf(output, ".align 3\n");
}

void genGlobalVarDecl(AST_NODE *DeclListNode){
	fprintf(output, ".data\n");
	for (AST_NODE *declNode = DeclListNode->child ; declNode != NULL; declNode = declNode->rightSibling)
	{
		if(declNode->semantic_value.declSemanticValue.kind == VARIABLE_DECL)
		{
			for (AST_NODE *idNode = declNode->child->rightSibling ; idNode != NULL; idNode = idNode->rightSibling)
			{
				char name[512];
				sprintf(name, "_%s", idNode->semantic_value.identifierSemanticValue.identifierName);
				TypeDescriptor *TD = idNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor;
				DATA_TYPE type = TD->properties.dataType;

				if(TD->kind != SCALAR_TYPE_DESCRIPTOR){
					int size = 4;
					for(int i = 0 ; i < TD->properties.arrayProperties.dimension ; i++)
						size *= TD->properties.arrayProperties.sizeInEachDimension[i];
					fprintf(output, "%s: .skip %d\n", name, size);
				}
				else{// NORMAL_ID || WITH_INIT_ID
					if(type == INT_TYPE){
						int value = idNode->child != NULL ? idNode->child->semantic_value.const1->const_u.intval : 0;
						fprintf(output, "%s: .word %d\n", name, value);
					}
					else if (type == FLOAT_TYPE){
						double value = idNode->child != NULL ? idNode->child->semantic_value.const1->const_u.fval : 0.0;
						fprintf(output, "%s: .word %d\n", name, 0);
					}
				}
			}
		}
	}
	gen_Alignment();
}

void genFunctionDecl(AST_NODE *funcDeclNode){
	AST_NODE *typeNode, *idNode, *paramListNode, *blockNode;
	typeNode = funcDeclNode->child;
	idNode = typeNode->rightSibling;
	paramListNode = idNode->rightSibling;
	blockNode = paramListNode->rightSibling;
	
	gen_head(idNode->semantic_value.identifierSemanticValue.identifierName);
	gen_prologue();
	for(AST_NODE *node = blockNode->child ; node != NULL ; node = node->rightSibling){
		if(node->nodeType == VARIABLE_DECL_LIST_NODE)
			genLocalVarDecl(node);
		else if(node->nodeType == STMT_LIST_NODE)
			genStmtList(node);
	}
	gen_epilogue(idNode->semantic_value.identifierSemanticValue.identifierName);
}

void genLocalVarDecl(AST_NODE *DeclListNode){
	int _offset = g_offset;
	for(AST_NODE *declNode = DeclListNode->child ; declNode != NULL ; declNode = declNode->rightSibling){
		if(declNode->semantic_value.declSemanticValue.kind == VARIABLE_DECL){
			int offset = 0;
			for (AST_NODE *idNode = declNode->child->rightSibling ; idNode != NULL; idNode = idNode->rightSibling)
			{
				SymbolTableEntry *entry = idNode->semantic_value.identifierSemanticValue.symbolTableEntry;
				if(entry->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR)
					offset += 4;
				else{
					int size = 4;
					for(int i = 0 ; i < entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension ; i++)
						size *= entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[i];
					offset += size;
				}
				entry->offset = g_offset + 88 + offset;
			}
			g_offset += offset;
		}
	}
	fprintf(output, ".data\n");
	fprintf(output, "_int_const_%d: .word %d\n", g_cnt, g_offset - _offset);
	gen_Alignment();
	fprintf(output, ".text\n");
	int reg = get_reg();
	fprintf(output, "ldr w%d, _int_const_%d\n", reg, g_cnt++);
	free_reg(reg);
	fprintf(output, "sub sp, sp, w%d\n", reg);
}

void genBlock(AST_NODE *blockNode){
	int id = g_cnt++;
	int reg1 = get_reg(), reg2 = get_reg();
	free_reg(reg1), free_reg(reg2);

	fprintf(output, ".data\n");
	fprintf(output, "_SP_%d: .skip 8\n", id);
	fprintf(output, ".text\n");
	fprintf(output, "ldr x%d, =_SP_%d\n", reg1, id);
	fprintf(output, "mov x%d, sp\n", reg2);
	fprintf(output, "str x%d, [x%d, #0]\n", reg2, reg1);
	for(AST_NODE *node = blockNode->child ; node != NULL ; node = node->rightSibling){
		if(node->nodeType == VARIABLE_DECL_LIST_NODE)
			genLocalVarDecl(node);
		else if(node->nodeType == STMT_LIST_NODE)
			genStmtList(node);
	}
	reg1 = get_reg(), reg2 = get_reg();
	free_reg(reg1), free_reg(reg2);
	fprintf(output, "ldr x%d, =_SP_%d\n", reg1, id);
	fprintf(output, "ldr x%d, [x%d, #0]\n", reg2, reg1);
	fprintf(output, "mov sp, x%d\n", reg2);
}

void genStmtList(AST_NODE *stmtListNode){
	for(AST_NODE *stmtNode = stmtListNode->child ; stmtNode != NULL ; stmtNode = stmtNode->rightSibling)
		genStmt(stmtNode);
}

void genWhileStmt(AST_NODE *whileNode){
	int cnt = g_cnt++;
	AST_NODE *condition = whileNode->child, *stmtNode = condition->rightSibling;
	fprintf(output, "_WHILE_%d:\n", cnt);
	if(condition->nodeType == STMT_NODE && condition->semantic_value.stmtSemanticValue.kind == ASSIGN_STMT){
		genAssignStmt(condition);
		condition = condition->child;
	}
	int reg = genExprRelated(condition);
	free_reg(reg);
	if(condition->dataType == FLOAT_TYPE)
		fprintf(output, "fcvtzs w%d, s%d\n", reg, reg);
	fprintf(output, "cmp w%d, #0\n", reg);
	fprintf(output, "beq _END_WHILE_%d\n", cnt);
	genStmt(stmtNode);
	fprintf(output, "b _WHILE_%d\n", cnt);
	fprintf(output, "_END_WHILE_%d:\n", cnt);
}

void genIfStmt(AST_NODE *ifNode){
	AST_NODE *condition = ifNode->child, *stmtNode = condition->rightSibling, *elseNode = stmtNode->rightSibling;
	int cnt = g_cnt++;
	fprintf(output, "_IF_%d:\n", cnt);
	if(condition->nodeType == STMT_NODE && condition->semantic_value.stmtSemanticValue.kind == ASSIGN_STMT){
		genAssignStmt(condition);
		condition = condition->child;
	}
	int reg = genExprRelated(condition);
	free_reg(reg);
	if(condition->dataType == FLOAT_TYPE)
		fprintf(output, "fcvtzs w%d, s%d\n", reg, reg);
	fprintf(output, "cmp w%d, #0\n", reg);
	fprintf(output, "beq _ELSE_%d\n", cnt);
	genStmt(stmtNode);
	fprintf(output, "b _END_IF_%d\n", cnt);
	fprintf(output, "_ELSE_%d:\n", cnt);
	genStmt(elseNode);
	fprintf(output, "_END_IF_%d:\n", cnt);
}

void genForStmt(AST_NODE *forNode){
	//TODO
	return;
}

void gen_data(int reg, int offset, char type){
	fprintf(output, ".data\n");
	fprintf(output, "_const_%d: .word %d\n", g_cnt, offset);
	gen_Alignment();
	fprintf(output, ".text\n");
	fprintf(output, "ldr %c%d, _const_%d\n", type, reg, g_cnt++);
}

void genAssignStmt(AST_NODE *assignNode){
	AST_NODE *idNode = assignNode->child, *rightNode = idNode->rightSibling;
	int resultReg = genExprRelated(rightNode);
	SymbolTableEntry *entry = idNode->semantic_value.identifierSemanticValue.symbolTableEntry;
	TypeDescriptor *TD = entry->attribute->attr.typeDescriptor;

	if(idNode->semantic_value.identifierSemanticValue.kind == NORMAL_ID){
		idNode->dataType = TD->properties.dataType;
		int reg = get_reg();
		free_reg(reg);
		if(entry->nestingLevel == 0)//global
			fprintf(output, "ldr x%d, =_%s\n", reg, idNode->semantic_value.identifierSemanticValue.identifierName);
		else{
			gen_data(reg, entry->offset, 'w');
			fprintf(output, "sub x%d, x29, x%d\n", reg, reg);
		}
		if(idNode->dataType == INT_TYPE){
			if(rightNode->dataType == FLOAT_TYPE)
				fprintf(output, "fcvtzs w%d, s%d\n", resultReg, resultReg);
			fprintf(output, "str w%d, [x%d, #0]\n", resultReg, reg);
		}
		else{
			if(rightNode->dataType == INT_TYPE)
				fprintf(output, "scvtf s%d, w%d\n", resultReg, resultReg);
			fprintf(output, "str s%d, [x%d, #0]\n", resultReg, reg);
		}
	}
	else{//ARRAY
		idNode->dataType = entry->attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
		int dim = 0;
		int reg = get_reg();
		fprintf(output, "mov x%d, #0\n", reg);
		for(AST_NODE *dimListNode = idNode->child ; dimListNode != NULL ; dimListNode = dimListNode->rightSibling){
			int idxreg = genExprRelated(dimListNode);
			int reg2 = get_reg();
			gen_data(reg2, entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim], 'x');
			fprintf(output, "mul x%d, x%d, x%d\n", reg, reg, reg2);
			fprintf(output, "lsl x%d, x%d, #2\n", idxreg, idxreg);
			fprintf(output, "add x%d, x%d, x%d\n", reg, reg, idxreg);
			free_reg(idxreg);
			free_reg(reg2);
			++dim;
		}
		int offset = entry->offset;
		int reg2 = get_reg();

		if(entry->nestingLevel == 0)//global
			fprintf(output, "ldr x%d, =_%s\n", reg2, idNode->semantic_value.identifierSemanticValue.identifierName);
		else{
			fprintf(output, "add x%d, x%d, x%d\n", reg, reg, 29);
			gen_data(reg2, entry->offset, 'x');
			fprintf(output, "neg x%d, x%d\n", reg2, reg2);
		}
		fprintf(output, "add x%d, x%d, x%d\n", reg, reg, reg2);
		if(idNode->dataType == INT_TYPE){
			if(rightNode->dataType == FLOAT_TYPE)
				fprintf(output, "fcvtzs w%d, s%d\n", resultReg, resultReg);
			fprintf(output, "str w%d, [x%d, #0]\n", resultReg, reg);
		}
		else{
			if(rightNode->dataType == INT_TYPE)
				fprintf(output, "scvtf s%d, w%d\n", resultReg, resultReg);
			fprintf(output, "str s%d, [x%d, #0]\n", resultReg, reg);
		}
		free_reg(reg);
		free_reg(reg2);
	}
	free_reg(resultReg);
	assignNode->dataType = (idNode->dataType == FLOAT_TYPE || rightNode->dataType == FLOAT_TYPE) ? FLOAT_TYPE : INT_TYPE;
}

void genFuncCall(AST_NODE *funcNode){
	AST_NODE *idNode = funcNode->child;
	if(strcmp(idNode->semantic_value.identifierSemanticValue.identifierName, "write") == 0){
		genWrite(idNode);
		return;
	}
	//Parameterless procedure calls
	if(strcmp(idNode->semantic_value.identifierSemanticValue.identifierName, "read") == 0)
		fprintf(output, "bl _read_int\n");
	else if(strcmp(idNode->semantic_value.identifierSemanticValue.identifierName, "fread") == 0)
		fprintf(output, "bl _read_float\n");
	else
		fprintf(output, "bl _start_%s\n", idNode->semantic_value.identifierSemanticValue.identifierName);
	funcNode->dataType = idNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.functionSignature->returnType;
}

void genReturnStmt(AST_NODE *returnNode){
	AST_NODE *node;
	for(node = returnNode->parent ; node != NULL ; node = node->parent){
		if(node->nodeType == DECLARATION_NODE){
			if(node->semantic_value.declSemanticValue.kind == FUNCTION_DECL)
				returnNode->dataType = node->child->dataType;
			break;
		}
	}
	int reg = genExprRelated(returnNode->child);
	if(returnNode->dataType == INT_TYPE){
		if(returnNode->child->dataType == INT_TYPE)
			fprintf(output, "mov w0, w%d\n", reg);
		else{
			fprintf(output, "fmov s0, s%d\n", reg);
			fprintf(output, "fcvtzs w0, s0\n");
		}
	}
	else if(returnNode->dataType == FLOAT_TYPE){
		if(returnNode->child->dataType == INT_TYPE){
			fprintf(output, "mov w0, w%d\n", reg);
			fprintf(output, "scvtf s0, w0\n");
		}
		else
			fprintf(output, "fmov s0, s%d\n", reg);
	}
	fprintf(output, "b _end_%s\n", node->child->rightSibling->semantic_value.identifierSemanticValue.identifierName);
	//free_reg(reg);
}

void genStmt(AST_NODE *stmtNode){
	if(stmtNode->nodeType == NUL_NODE)
		return;
	else if(stmtNode->nodeType == BLOCK_NODE)
		genBlock(stmtNode);
	else{
		switch(stmtNode->semantic_value.stmtSemanticValue.kind){
			case WHILE_STMT:
				genWhileStmt(stmtNode);
				break;
			case FOR_STMT:
				genForStmt(stmtNode);
				break;
			case ASSIGN_STMT:
				genAssignStmt(stmtNode);
				break;
			case IF_STMT:
				genIfStmt(stmtNode);
				break;
			case FUNCTION_CALL_STMT:
				genFuncCall(stmtNode);
				break;
			case RETURN_STMT:
				genReturnStmt(stmtNode);
				break;
		}
	}
}

int genExprRelated(AST_NODE *exprRelatedNode){
	int reg = get_reg();
	SymbolTableEntry *entry = exprRelatedNode->semantic_value.identifierSemanticValue.symbolTableEntry;
	switch(exprRelatedNode->nodeType){
		case EXPR_NODE:
			free_reg(reg);
			reg = genExpr(exprRelatedNode);
			return reg;
		case STMT_NODE:
			free_reg(reg);
			genFuncCall(exprRelatedNode);
			reg = get_reg();
			if(exprRelatedNode->dataType == INT_TYPE)
				fprintf(output, "mov w%d, w0\n", reg);
			else if(exprRelatedNode->dataType == FLOAT_TYPE)
				fprintf(output, "fmov s%d, s0\n", reg);
			return reg;
		case IDENTIFIER_NODE:
			
			if(exprRelatedNode->semantic_value.identifierSemanticValue.kind == NORMAL_ID){
				exprRelatedNode->dataType = entry->attribute->attr.typeDescriptor->properties.dataType;
				if(entry->nestingLevel == 0){
					if(exprRelatedNode->dataType == INT_TYPE)
						fprintf(output, "ldr w%d, _%s\n", reg, exprRelatedNode->semantic_value.identifierSemanticValue.identifierName);
					else
						fprintf(output, "ldr s%d, _%s\n", reg, exprRelatedNode->semantic_value.identifierSemanticValue.identifierName);
				}
				else{
					gen_data(reg, entry->offset, 'w');
					fprintf(output, "sub x%d, x29, x%d\n", reg, reg);
					if(exprRelatedNode->dataType == INT_TYPE)
						fprintf(output, "ldr w%d, [x%d, #0]\n", reg, reg);
					else
						fprintf(output, "ldr s%d, [x%d, #0]\n", reg, reg);
				}
			}
			else{
				exprRelatedNode->dataType = entry->attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
				int dim = 0;
				fprintf(output, "mov x%d, #0\n", reg);
				for(AST_NODE *dimListNode = exprRelatedNode->child ; dimListNode != NULL ; dimListNode = dimListNode->rightSibling){
					int reg2 = get_reg();
					gen_data(reg2, entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim], 'x');
					fprintf(output, "mul x%d, x%d, x%d\n", reg, reg, reg2);
					free_reg(reg2);
					reg2 = genExprRelated(dimListNode);
					fprintf(output, "lsl x%d, x%d, #2\n", reg2, reg2);
					fprintf(output, "add x%d, x%d, x%d\n", reg, reg, reg2);
					free_reg(reg2);
					++dim;
				}
				int reg2 = get_reg();
				if(entry->nestingLevel == 0)
					fprintf(output, "ldr x%d, =_%s\n", reg2, exprRelatedNode->semantic_value.identifierSemanticValue.identifierName);
				else{
					fprintf(output, "add x%d, x%d, x%d\n", reg, reg, 29);
					gen_data(reg2, entry->offset, 'x');
					fprintf(output, "neg x%d, x%d\n", reg2, reg2);
				}
				free_reg(reg2);
				fprintf(output, "add x%d, x%d, x%d\n", reg, reg, reg2);
				if(exprRelatedNode->dataType == INT_TYPE)
					fprintf(output, "ldr w%d, [x%d, #0]\n", reg, reg);
				else
					fprintf(output, "ldr s%d, [x%d, #0]\n", reg, reg);
			}
			break;
		case CONST_VALUE_NODE:
			if(exprRelatedNode->semantic_value.const1->const_type == INTEGERC){
				exprRelatedNode->dataType = INT_TYPE;
				exprRelatedNode->semantic_value.exprSemanticValue.constEvalValue.iValue = exprRelatedNode->semantic_value.const1->const_u.intval;
				fprintf(output, ".data\n");
				fprintf(output, "_int_const_%d: .word %d\n", g_cnt, exprRelatedNode->semantic_value.const1->const_u.intval);
				gen_Alignment();
				fprintf(output, ".text\n");
				fprintf(output, "ldr w%d, _int_const_%d\n", reg, g_cnt++);
			}
			else if(exprRelatedNode->semantic_value.const1->const_type == FLOATC){
				exprRelatedNode->dataType = FLOAT_TYPE;
				exprRelatedNode->semantic_value.exprSemanticValue.constEvalValue.fValue = exprRelatedNode->semantic_value.const1->const_u.fval;
				fprintf(output, ".data\n");
				fprintf(output, "_float_const_%d: .float %lf\n", g_cnt, exprRelatedNode->semantic_value.const1->const_u.fval);
				gen_Alignment();
				fprintf(output, ".text\n");
				fprintf(output, "ldr s%d, _float_const_%d\n", reg, g_cnt++);
			}
			else if(exprRelatedNode->semantic_value.const1->const_type == STRINGC){
				exprRelatedNode->dataType = CONST_STRING_TYPE;
				fprintf(output, ".data\n");
				fprintf(output, "_string_const_%d: .asciz %s\n", g_cnt, exprRelatedNode->semantic_value.const1->const_u.sc);
				gen_Alignment();
				fprintf(output, ".text\n");
				fprintf(output, "ldr x%d, =_string_const_%d\n", reg, g_cnt++);
			}
			break;
	}
	return reg;
}

void genIntBinaryOp(AST_NODE *exprNode, int reg1, int reg2, char *op){
	exprNode->dataType = INT_TYPE;
	fprintf(output, "cmp w%d, w%d\n", reg1, reg2);
	fprintf(output, "%s _ELSE_%d\n", op, g_cnt);
	fprintf(output, "mov w%d, #0\n", reg1);
	fprintf(output, "b _END_%d\n", g_cnt);
	fprintf(output, "_ELSE_%d:\n", g_cnt);
	fprintf(output, "mov w%d, #1\n", reg1);
	fprintf(output, "_END_%d:\n", g_cnt++);
}

void genFloatBinaryOp(AST_NODE *exprNode, int reg1, int reg2, char *op){
	exprNode->dataType = INT_TYPE;
	fprintf(output, "fcmp s%d, s%d\n", reg1, reg2);
	fprintf(output, "%s _ELSE_%d\n", op, g_cnt);
	fprintf(output, "mov w%d, #0\n", reg1);
	fprintf(output, "b _END_%d\n", g_cnt);
	fprintf(output, "_ELSE_%d:\n", g_cnt);
	fprintf(output, "mov w%d, #1\n", reg1);
	fprintf(output, "_END_%d:\n", g_cnt++);
}

int genExpr(AST_NODE *exprNode){
	if(exprNode->semantic_value.exprSemanticValue.kind == BINARY_OPERATION){
		AST_NODE *leftNode = exprNode->child, *rightNode = leftNode->rightSibling;
		exprNode->dataType = (leftNode->dataType == FLOAT_TYPE || rightNode->dataType == FLOAT_TYPE) ? FLOAT_TYPE : INT_TYPE;
		if(exprNode->semantic_value.exprSemanticValue.isConstEval){
			int reg = get_reg();
			fprintf(output, ".data\n");
			if(exprNode->dataType == INT_TYPE){
				fprintf(output, "_int_const_%d: .word %d\n", g_cnt, exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue);
				gen_Alignment();
				fprintf(output, ".text\n");
				fprintf(output, "ldr w%d, _int_const_%d\n", reg, g_cnt++);
			}else{
				fprintf(output, "_float_const_%d: .float %lf\n", g_cnt, exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue);
				gen_Alignment();
				fprintf(output, ".text\n");
				fprintf(output, "ldr s%d, _float_const_%d\n", reg, g_cnt++);
			}
			return reg;
		}
		else{
			int reg1 = genExprRelated(leftNode);
			free_reg(reg1);
			if(leftNode->dataType == INT_TYPE)
				fprintf(output, "str w%d, [sp, #0]\n", reg1);
			else
				fprintf(output, "str s%d, [sp, #0]\n", reg1);
			fprintf(output, "sub sp, sp, #8\n");
			int reg2 = genExprRelated(rightNode);
			fprintf(output, "add sp, sp, #8\n");
			reg1 = get_reg();
			if(leftNode->dataType == INT_TYPE)
				fprintf(output, "ldr w%d, [sp, #0]\n", reg1);
			else
				fprintf(output, "ldr s%d, [sp, #0]\n", reg1);
			if(leftNode->dataType == INT_TYPE && rightNode->dataType == INT_TYPE){
				exprNode->dataType = INT_TYPE;
				switch(exprNode->semantic_value.exprSemanticValue.op.binaryOp){
					case BINARY_OP_ADD:
						fprintf(output, "add w%d, w%d, w%d\n", reg1, reg1, reg2);
						break;
					case BINARY_OP_SUB:
						fprintf(output, "sub w%d, w%d, w%d\n", reg1, reg1, reg2);
						break;
					case BINARY_OP_MUL:
						fprintf(output, "mul w%d, w%d, w%d\n", reg1, reg1, reg2);
						break;
					case BINARY_OP_DIV:
						fprintf(output, "sdiv w%d, w%d, w%d\n", reg1, reg1, reg2);
						break;
					case BINARY_OP_EQ:
						genIntBinaryOp(exprNode, reg1, reg2, "beq");
						break;
					case BINARY_OP_GE:
						genIntBinaryOp(exprNode, reg1, reg2, "bge");
						break;
					case BINARY_OP_LE:
						genIntBinaryOp(exprNode, reg1, reg2, "ble");
						break;
					case BINARY_OP_NE:
						genIntBinaryOp(exprNode, reg1, reg2, "bne");
						break;
					case BINARY_OP_GT:
						genIntBinaryOp(exprNode, reg1, reg2, "bgt");
						break;
					case BINARY_OP_LT:
						genIntBinaryOp(exprNode, reg1, reg2, "blt");
						break;
					case BINARY_OP_AND:
						fprintf(output, "cmp w%d, #0\n", reg1);
						fprintf(output, "beq _ELSE_%d\n", g_cnt);
						fprintf(output, "cmp w%d, #0\n", reg2);
						fprintf(output, "beq _ELSE_%d\n", g_cnt);
						fprintf(output, "mov w%d, #1\n", reg1);
						fprintf(output, "b _END_%d\n", g_cnt);
						fprintf(output, "_ELSE_%d:\n", g_cnt);
						fprintf(output, "mov w%d, #0\n", reg1);
						fprintf(output, "_END_%d:\n", g_cnt++);
						break;
					case BINARY_OP_OR:;
						fprintf(output, "cmp w%d, #0\n", reg1);
						fprintf(output, "bne _ELSE_%d\n", g_cnt);
						fprintf(output, "cmp w%d, #0\n", reg2);
						fprintf(output, "bne _ELSE_%d\n", g_cnt);
						fprintf(output, "mov w%d, #0\n", reg1);
						fprintf(output, "b _END_%d\n", g_cnt);
						fprintf(output, "_ELSE_%d:\n", g_cnt);
						fprintf(output, "mov w%d, #1\n", reg1);
						fprintf(output, "_END_%d:\n", g_cnt++);
						break;	
				}
				free_reg(reg2);
				return reg1;
			}
			else{//float
				exprNode->dataType = FLOAT_TYPE;
				if(leftNode->dataType == INT_TYPE)
					fprintf(output, "scvtf s%d, w%d\n", reg1, reg1);
				if(rightNode->dataType == INT_TYPE)
				fprintf(output, "scvtf s%d, w%d\n", reg2, reg2);

				switch(exprNode->semantic_value.exprSemanticValue.op.binaryOp){
					case BINARY_OP_ADD:
						fprintf(output, "fadd s%d, s%d, s%d\n", reg1, reg1, reg2);
						break;
					case BINARY_OP_SUB:
						fprintf(output, "fsub s%d, s%d, s%d\n", reg1, reg1, reg2);
						break;
					case BINARY_OP_MUL:
						fprintf(output, "fmul s%d, s%d, s%d\n", reg1, reg1, reg2);
						break;
					case BINARY_OP_DIV:
						fprintf(output, "fdiv s%d, s%d, s%d\n", reg1, reg1, reg2);
						break;
					case BINARY_OP_EQ:
						genFloatBinaryOp(exprNode, reg1, reg2, "beq");
						break;
					case BINARY_OP_GE:
						genFloatBinaryOp(exprNode, reg1, reg2, "bge");
						break;
					case BINARY_OP_LE:
						genFloatBinaryOp(exprNode, reg1, reg2, "ble");
						break;
					case BINARY_OP_NE:
						genFloatBinaryOp(exprNode, reg1, reg2, "bne");
						break;
					case BINARY_OP_GT:
						genFloatBinaryOp(exprNode, reg1, reg2, "bgt");
						break;
					case BINARY_OP_LT:
						genFloatBinaryOp(exprNode, reg1, reg2, "blt");
						break;
					case BINARY_OP_AND:
						exprNode->dataType = INT_TYPE;
						fprintf(output, "fcmp s%d, #0\n", reg1);
						fprintf(output, "beq _ELSE_%d\n", g_cnt);
						fprintf(output, "cmp s%d, #0\n", reg2);
						fprintf(output, "beq _ELSE_%d\n", g_cnt);
						fprintf(output, "mov w%d, #1\n", reg1);
						fprintf(output, "b _END_%d\n", g_cnt);
						fprintf(output, "_ELSE_%d:\n", g_cnt);
						fprintf(output, "mov w%d, #0\n", reg1);
						fprintf(output, "_END_%d:\n", g_cnt);
						++g_cnt;
						break;
					case BINARY_OP_OR:
						exprNode->dataType = INT_TYPE;
						fprintf(output, "fcmp s%d, #0\n", reg1);
						fprintf(output, "bne _ELSE_%d\n", g_cnt);
						fprintf(output, "cmp s%d, #0\n", reg2);
						fprintf(output, "bne _ELSE_%d\n", g_cnt);
						fprintf(output, "mov w%d, #0\n", reg1);
						fprintf(output, "b _END_%d\n", g_cnt);
						fprintf(output, "_ELSE_%d:\n", g_cnt);
						fprintf(output, "mov w%d, #1\n", reg1);
						fprintf(output, "_END_%d:\n", g_cnt);
						++g_cnt;
						break;
				}
				free_reg(reg2);
				return reg1;
			}
		}
	}
	else{//Unary
		AST_NODE* operand = exprNode->child;
		exprNode->dataType = operand->dataType;
		if(exprNode->semantic_value.exprSemanticValue.isConstEval){
			fprintf(output, ".data\n");
			if(exprNode->dataType == INT_TYPE)
				fprintf(output, "_int_const_%d: .word %d\n", g_cnt, exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue);
			else
				fprintf(output, "_float_const_%d: .float %lf\n", g_cnt, exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue);
			gen_Alignment();
			fprintf(output, ".text\n");
			int reg = get_reg();
			if(exprNode->dataType == INT_TYPE)
				fprintf(output, "ldr w%d, _int_const_%d\n", reg, g_cnt++);
			else
				fprintf(output, "ldr s%d, _float_const_%d\n", reg, g_cnt++);
			return reg;
		}
		else{
			int reg = genExprRelated(operand);
			if(operand->dataType == INT_TYPE){
				if(exprNode->semantic_value.exprSemanticValue.op.unaryOp == UNARY_OP_NEGATIVE)
					fprintf(output, "neg w%d, w%d", reg, reg);
				else if(exprNode->semantic_value.exprSemanticValue.op.unaryOp == UNARY_OP_LOGICAL_NEGATION){
					fprintf(output, "cmp w%d, #0\n", reg);
					fprintf(output, "beq _ELSE_%d\n", g_cnt);
					fprintf(output, "mov w%d, #0\n", reg);
					fprintf(output, "b _END_%d\n", g_cnt);
					fprintf(output, "_ELSE_%d:\n", g_cnt);
					fprintf(output, "mov w%d, #1\n", reg);
					fprintf(output, "_END_%d:\n", g_cnt++);
				}
			}
			else{
				exprNode->dataType = FLOAT_TYPE;
				if(exprNode->semantic_value.exprSemanticValue.op.unaryOp == UNARY_OP_NEGATIVE)
					fprintf(output, "fneg s%d, s%d", reg, reg);
				else if(exprNode->semantic_value.exprSemanticValue.op.unaryOp == UNARY_OP_LOGICAL_NEGATION){
					fprintf(output, "fcvtzs w%d, s%d\n", reg, reg);
					fprintf(output, "cmp w%d, #0\n", reg);
					fprintf(output, "beq _ELSE_%d\n", g_cnt);
					fprintf(output, "mov w%d, #0\n", reg);
					fprintf(output, "b _END_%d\n", g_cnt);
					fprintf(output, "_ELSE_%d:\n", g_cnt);
					fprintf(output, "mov w%d, #1\n", reg);
					fprintf(output, "_END_%d:\n", g_cnt++);
				}
			}
			return reg;
		}
	}
}

void genWrite(AST_NODE *node){
	AST_NODE *paramListNode = node->rightSibling, *paramNode = paramListNode->child;
	int reg = genExprRelated(paramNode);
	if(paramNode->dataType == INT_TYPE){
		fprintf(output, "mov w0, w%d\n", reg);
		fprintf(output, "bl _write_int\n");
	}
	else if(paramNode->dataType == FLOAT_TYPE){
		fprintf(output, "fmov s0, s%d\n", reg);
		fprintf(output, "bl _write_float\n");
	}
	else if(paramNode->dataType == CONST_STRING_TYPE){
		fprintf(output, "mov x0, x%d\n", reg);
		fprintf(output, "bl _write_str\n");
	}
	free_reg(reg);
}

void genProgram(AST_NODE *root){
	for (AST_NODE *decl = root->child ; decl != NULL; decl = decl->rightSibling){
		if (decl->nodeType == VARIABLE_DECL_LIST_NODE)
			genGlobalVarDecl(decl);
		else
			genFunctionDecl(decl);
	}
}

void codegen(AST_NODE *root){
	output = fopen("output.s", "w");
	genProgram(root);
	fclose(output);
}
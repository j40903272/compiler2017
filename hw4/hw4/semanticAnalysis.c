#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "symbolTable.h"
// This file is for reference only, you are not required to follow the implementation. //
// You only need to check for errors stated in the hw4 assignment document. //
int g_anyErrorOccur = 0;
DATA_TYPE currentReturnType;
extern SymbolTable symbolTable;
int ignoreDim = 0;

DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2);
void processProgramNode(AST_NODE *programNode);
void processDeclarationNode(AST_NODE* declarationNode);
void declareIdList(AST_NODE* typeNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize);
void declareFunction(AST_NODE* returnTypeNode);
void processDeclDimList(AST_NODE* variableDeclDimList, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize);
DATA_TYPE processTypeNode(AST_NODE* typeNode);
void processBlockNode(AST_NODE* blockNode);
void processStmtNode(AST_NODE* stmtNode);
void processGeneralNode(AST_NODE *node);
void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode);
void checkWhileStmt(AST_NODE* whileNode);
void checkForStmt(AST_NODE* forNode);
void checkAssignmentStmt(AST_NODE* assignmentNode);
void checkIfStmt(AST_NODE* ifNode);
void checkWriteFunction(AST_NODE* functionCallNode);
void checkFunctionCall(AST_NODE* functionCallNode);
void processExprRelatedNode(AST_NODE* exprRelatedNode);
void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter);
void checkReturnStmt(AST_NODE* returnNode);
void processExprNode(AST_NODE* exprNode);
void processVariableLValue(AST_NODE* idNode);
void processVariableRValue(AST_NODE* idNode);
void processConstValueNode(AST_NODE* constValueNode);
void getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue);
void evaluateExprValue(AST_NODE* exprNode);

void traverse(AST_NODE *node);
void checkIdentifierNode(AST_NODE* identifierNode);

void printErrorMsgSpecial(AST_NODE* node1, char* name2, ErrorMsgKind errorMsgKind)
{
    g_anyErrorOccur = 1;
    printf("Error found in line %d\n", node1->linenumber);
    char name1[256];
    if(node1->nodeType == EXPR_NODE)
        strcpy(name1, "expression");
    else
        strcpy(name1, IdName(node1));
    switch(errorMsgKind)
    {
        case PASS_ARRAY_TO_SCALAR:
            printf("Array %s passed to scalar parameter %s.\n", name1, name2);
            break;
        case PASS_SCALAR_TO_ARRAY:
            printf("Scalar %s passed to array parameter %s.\n", name1, name2);
            break;
        default:
            printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
            break;
    }
}


void printErrorMsg(AST_NODE* node, ErrorMsgKind errorMsgKind)
{
    g_anyErrorOccur = 1;
    printf("Error found in line %d\n", node->linenumber);
    switch(errorMsgKind)
    {
        case SYMBOL_IS_NOT_TYPE:
        printf("ID %s is not a type.\n", IdName(node));
        break;
        case SYMBOL_REDECLARE:
            printf("ID %s redeclared.\n", IdName(node));
            break;
        case SYMBOL_UNDECLARED:
            printf("ID %s undeclared.\n", IdName(node));
            break;
        case NOT_FUNCTION_NAME:
            printf("ID %s is not a function.\n", IdName(node));
            break;
        case RETURN_ARRAY:
            printf("Function returns array.\n");
            break;
        case TOO_FEW_ARGUMENTS:
            printf("Too few arguments to function %s.\n", IdName(node));
            break;
        case TOO_MANY_ARGUMENTS:
            printf("Too many arguments to function %s.\n", IdName(node));
            break;
        case RETURN_TYPE_UNMATCH:
            printf("Incompatible return type.\n");
            break;
        case INCOMPATIBLE_ARRAY_DIMENSION:
            printf("Incompatible array dimensions for array ID %s.\n", IdName(node));
            break;
        case IS_TYPE_NOT_VARIABLE:
            printf("ID %s is type, not variable.\n", IdName(node));
            break;
        case IS_FUNCTION_NOT_VARIABLE:
            printf("ID %s is function, not variable.\n", IdName(node));
            break;
        case ARRAY_SIZE_NOT_INT:
            printf("Array dimensions of ID %s is not integer.\n", IdName(node));
            break;
        case ARRAY_SIZE_NEGATIVE:
            printf("Array dimensions of ID %s is negative.\n", IdName(node));
            break;
        case ARRAY_SUBSCRIPT_NOT_INT:
            printf("Array subscript of ID %s is not integer.\n", IdName(node));
            break;
        case NOT_ASSIGNABLE:
            printf("ID %s not assignable\n", IdName(node));
            break;
        case PARAMETER_TYPE_UNMATCH:
            printf("parameter %s has incompatible type\n", IdName(node));
            break;
        default:
            printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
            break;
    }
}


void semanticAnalysis(AST_NODE *root)
{
    processProgramNode(root);
}


DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2)
{
    if(dataType1 == FLOAT_TYPE || dataType2 == FLOAT_TYPE) {
        return FLOAT_TYPE;
    } else {
        return INT_TYPE;
    }
}

void traverse(AST_NODE *node){
    //puts("into traverse");
    for(AST_NODE* i = node->child ; i != NULL ; i = i->rightSibling){
        switch (i->nodeType) {
            case PROGRAM_NODE:
                //puts("PROGRAM_NODE");
                processProgramNode(i);
                break;
            case DECLARATION_NODE:
                //puts("DECLARATION_NODE");
                processDeclarationNode(i);
                break;
            case IDENTIFIER_NODE:
                //puts("IDENTIFIER_NODE");
                checkIdentifierNode(i);
                break;
            case PARAM_LIST_NODE:
                //!!
                //puts("PARAM_LIST_NODE");
                traverse(i);
                break;
            case NUL_NODE:
                //puts("NUL_NODE");
                break;
            case BLOCK_NODE:
                //puts("BLOCK_NODE");
                processBlockNode(i);
                break;
            case VARIABLE_DECL_LIST_NODE:
                //!!
                //puts("VARIABLE_DECL_LIST_NODE");
                traverse(i);
                break;
            case STMT_LIST_NODE:
                //!!
                //puts("STMT_LIST_NODE");
                traverse(i);
                break;
            case STMT_NODE:
                //puts("STMT_NODE");
                processStmtNode(i);
                break;
            case EXPR_NODE:
                //puts("EXPR_NODE");
                processExprNode(i);
                break;
            case CONST_VALUE_NODE:
                //puts("CONST_VALUE_NODE");
                processConstValueNode(i);
                break;
            case NONEMPTY_ASSIGN_EXPR_LIST_NODE:
                //puts("NONEMPTY_ASSIGN_EXPR_LIST_NODE");
                traverse(i);
                break;
            case NONEMPTY_RELOP_EXPR_LIST_NODE:
                //puts("NONEMPTY_RELOP_EXPR_LIST_NODE");
                traverse(i);
                break;
            default:
                //puts("Unknown nodetype in traverse");
                ;
        }
    }
}

void processProgramNode(AST_NODE *programNode)
{
    traverse(programNode);
}

void processDeclarationNode(AST_NODE* declNode)
{
    //puts("into processDeclarationNode");
    switch(declNode->semantic_value.declSemanticValue.kind){
        case VARIABLE_DECL:
            //puts("VARIABLE_DECL");
            declareIdList(declNode->child, VARIABLE_ATTRIBUTE, 0);
            break;
        case TYPE_DECL:
            //puts("TYPE_DECL");
            declareIdList(declNode->child, TYPE_ATTRIBUTE, 0);
            break;
        case FUNCTION_DECL:
            //puts("FUNCTION_DECL");
            declareFunction(declNode->child);
            break;
        case FUNCTION_PARAMETER_DECL:
            //puts("FUNCTION_PARAMETER_DECL");
            declareIdList(declNode->child, VARIABLE_ATTRIBUTE, 1);
            break;
        default:
            ;//puts("!?!?!?!?");
    }
}

char* IdName(AST_NODE *node){
    return node->semantic_value.identifierSemanticValue.identifierName;
}

DATA_TYPE processTypeNode(AST_NODE* idNodeAsType)
{
    //puts("into processTypeNode");
    SymbolTableEntry* ptr = retrieveSymbol(IdName(idNodeAsType));
    if(ptr == NULL){
        printErrorMsg(idNodeAsType, SYMBOL_UNDECLARED);
        idNodeAsType->semantic_value.identifierSemanticValue.symbolTableEntry = retrieveSymbol("void");
        return VOID_TYPE;
    }
    else{
        if(ptr->attribute->attributeKind != TYPE_ATTRIBUTE)
            printErrorMsg(idNodeAsType, SYMBOL_IS_NOT_TYPE);
        return ptr->attribute->attr.typeDescriptor->properties.dataType;
    }
}

void declareIdList(AST_NODE* typeNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize)
{
    //puts("into declareIdList");
    AST_NODE* Ids = typeNode->rightSibling;
    DATA_TYPE type =  processTypeNode(typeNode);
    SymbolAttribute *Attr;
    while(Ids != NULL){
        SymbolTableEntry* ptr = retrieveSymbol(IdName(Ids));
        if(ptr != NULL && ptr->nestingLevel == symbolTable.currentLevel){
            printErrorMsg(Ids, SYMBOL_REDECLARE);
            Ids->semantic_value.identifierSemanticValue.symbolTableEntry = ptr;
        }
        else{
            if(Ids->semantic_value.identifierSemanticValue.kind == ARRAY_ID){
                Attr = createAttrType(isVariableOrTypeAttribute, ARRAY_TYPE_DESCRIPTOR, type);
                processDeclDimList(Ids, Attr->attr.typeDescriptor, ignoreArrayFirstDimSize);//deal with array properties
            }
            else{
                Attr = createAttrType(isVariableOrTypeAttribute, SCALAR_TYPE_DESCRIPTOR, type);
                if(Ids->semantic_value.identifierSemanticValue.kind == WITH_INIT_ID){
                    //don't need to handle
                    traverse(Ids);
                }
            }
            Ids->semantic_value.identifierSemanticValue.symbolTableEntry = InsertSymbol(IdName(Ids), Attr);
        }
        Ids = Ids->rightSibling;
    }
}

void processDeclDimList(AST_NODE* idNode, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize)
{
    //puts("into processDeclDimList");
    int dim = 0;
    for(AST_NODE* node = idNode->child ; node != NULL ; node = node->rightSibling){
        if(dim == 0 && ignoreFirstDimSize)
            typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim] = -1;
        else if(node->nodeType != CONST_VALUE_NODE || node->semantic_value.const1->const_type != INTEGERC){
            typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim] = -1;
            printErrorMsg(node, ARRAY_SIZE_NOT_INT);
        }
        else{
            int size = node->semantic_value.const1->const_u.intval;
            if (size < 0)
                printErrorMsg(node, ARRAY_SIZE_NEGATIVE);
            else
                typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim] = size;
        }
        ++dim;
    }
    typeDescriptor->properties.arrayProperties.dimension = dim;
    if(dim > MAX_ARRAY_DIMENSION)
        printErrorMsg(idNode, EXCESSIVE_ARRAY_DIM_DECLARATION);
}


void declareFunction(AST_NODE* TypeNode)
{
    //puts("into declareFunction");
    AST_NODE *IdNode = TypeNode->rightSibling, *paramListNode = IdNode->rightSibling, *blockNode = paramListNode->rightSibling;
    DATA_TYPE returnType = currentReturnType = processTypeNode(TypeNode);

    SymbolAttribute *Attr = (SymbolAttribute*)malloc(sizeof(SymbolAttribute*));
    *Attr = (SymbolAttribute){(SymbolAttributeKind)FUNCTION_SIGNATURE, (void*)malloc(sizeof(FunctionSignature*))};
    *(Attr->attr.functionSignature) = (FunctionSignature){0, NULL, returnType};
    
    SymbolTableEntry* ptr = retrieveSymbol(IdName(IdNode));
    if(ptr != NULL){
        IdNode->linenumber = TypeNode->linenumber;
        printErrorMsg(IdNode, SYMBOL_REDECLARE);
        IdNode->semantic_value.identifierSemanticValue.symbolTableEntry = ptr;
    }
    else
        IdNode->semantic_value.identifierSemanticValue.symbolTableEntry = InsertSymbol(IdName(IdNode), Attr);

    //printf("%s %d\n", IdName(IdNode), IdNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.functionSignature->returnType);
    openScope();
    traverse(paramListNode);
    Parameter* last = NULL;
    for(AST_NODE* paramDeclNode = paramListNode->child ; paramDeclNode != NULL ; paramDeclNode = paramDeclNode->rightSibling)
    {
        Parameter* tmp = (Parameter*)malloc(sizeof(Parameter*));
        tmp->type = retrieveSymbol(IdName(paramDeclNode->child->rightSibling))->attribute->attr.typeDescriptor;
        tmp->parameterName = IdName(paramDeclNode->child->rightSibling);
        ++Attr->attr.functionSignature->parametersCount;
        tmp->next = NULL;
        if(last == NULL)
            Attr->attr.functionSignature->parameterList = tmp;
        else
            last->next = tmp;
        last = tmp;
    }
    traverse(blockNode);
    closeScope();
}

void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode)
{
}

void checkWhileStmt(AST_NODE* whileNode)
{
    //puts("checkWhileStmt");
    traverse(whileNode);
}


void checkForStmt(AST_NODE* forNode)
{
    //puts("checkForStmt");
    traverse(forNode);
}


void checkAssignmentStmt(AST_NODE* assignmentNode)
{
    //puts("into checkAssignmentStmt");
    traverse(assignmentNode);
    AST_NODE *l = assignmentNode->child, *r = l->rightSibling;
    processVariableLValue(l);
    processVariableRValue(r);
    //Handle Assignment Conversions
}

void processVariableLValue(AST_NODE* idNode)
{
    //puts("into processVariableLValue");
    if(idNode->nodeType != IDENTIFIER_NODE)
        printErrorMsg(idNode, NOT_ASSIGNABLE);
}

void processVariableRValue(AST_NODE* idNode)
{
    //puts("into processVariableRValue");
    if(idNode->nodeType == IDENTIFIER_NODE)
        return;
    else if(idNode->nodeType == STMT_NODE && idNode->semantic_value.stmtSemanticValue.kind)
        return;
    else if(idNode->nodeType == EXPR_NODE)
        return;
    else if(idNode->nodeType == CONST_VALUE_NODE)
        return;
    printErrorMsg(idNode, NOT_ASSIGNABLE);
}

void checkIfStmt(AST_NODE* ifNode)
{
    //puts("checkIfStmt");
    traverse(ifNode);
}

void checkWriteFunction(AST_NODE* functionCallNode)
{
}

void checkFunctionCall(AST_NODE* functionCallNode)
{
    //puts("into checkFunctionCall");
    ignoreDim = 1;
    traverse(functionCallNode);
    ignoreDim = 0;
    AST_NODE *idNode = functionCallNode->child, *paramListNode = idNode->rightSibling;
    SymbolTableEntry *ptr = retrieveSymbol(IdName(idNode));
    
    if(ptr == NULL)
        return;
    else if(ptr->attribute->attributeKind != FUNCTION_SIGNATURE){
        printErrorMsg(idNode, NOT_FUNCTION_NAME);
        return;
    }

    functionCallNode->dataType = ptr->attribute->attr.functionSignature->returnType;
    Parameter *paramPtr = ptr->attribute->attr.functionSignature->parameterList;

    for(AST_NODE *paramNode = paramListNode->child ; ; paramNode = paramNode->rightSibling, paramPtr = paramPtr->next)
    {
        if(paramNode == NULL || paramPtr == NULL){
            if(paramNode != NULL)
                printErrorMsg(idNode, TOO_MANY_ARGUMENTS);
            else if(paramPtr != NULL)
                printErrorMsg(idNode, TOO_FEW_ARGUMENTS);
            break;
        }
        checkParameterPassing(paramPtr, paramNode);
    }
}

void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter)
{
    //puts("into checkParameterPassing");
    if(formalParameter->type->kind == SCALAR_TYPE_DESCRIPTOR){
        if(formalParameter->type->properties.dataType != actualParameter->dataType)
            printErrorMsg(actualParameter, PARAMETER_TYPE_UNMATCH);
        if(actualParameter->semantic_value.identifierSemanticValue.kind == ARRAY_ID)
            printErrorMsgSpecial(actualParameter, formalParameter->parameterName, PASS_ARRAY_TO_SCALAR);
    }
    else if(formalParameter->type->kind == ARRAY_TYPE_DESCRIPTOR){
        if(formalParameter->type->properties.arrayProperties.elementType != actualParameter->dataType)
            printErrorMsg(actualParameter, PARAMETER_TYPE_UNMATCH);
        if(actualParameter->semantic_value.identifierSemanticValue.kind == NORMAL_ID)
            printErrorMsgSpecial(actualParameter, formalParameter->parameterName, PASS_SCALAR_TO_ARRAY);
    }
}


void processExprRelatedNode(AST_NODE* exprRelatedNode)
{
}

void getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue)
{
}

void evaluateExprValue(AST_NODE* exprNode)
{
}


void processExprNode(AST_NODE* exprNode)
{
    //puts("into processExprNode");
    traverse(exprNode);
    AST_NODE *l = exprNode->child, *r = l->rightSibling;
    if(exprNode->semantic_value.exprSemanticValue.kind == BINARY_OPERATION){
        if(exprNode->semantic_value.exprSemanticValue.op.binaryOp == BINARY_OP_ADD || \
           exprNode->semantic_value.exprSemanticValue.op.binaryOp == BINARY_OP_SUB || \
           exprNode->semantic_value.exprSemanticValue.op.binaryOp == BINARY_OP_MUL || \
           exprNode->semantic_value.exprSemanticValue.op.binaryOp == BINARY_OP_DIV)
            exprNode->dataType = getBiggerType(l->dataType, r->dataType);
        else
            exprNode->dataType = INT_TYPE;
    }
    else if(exprNode->semantic_value.exprSemanticValue.kind == UNARY_OPERATION)
        exprNode->dataType = l->dataType;
}

void processConstValueNode(AST_NODE* constValueNode)
{
    //puts("into processConstValueNode");
    switch (constValueNode->semantic_value.const1->const_type) {
        case INTEGERC:
            constValueNode->dataType = INT_TYPE;
            break;
        case FLOATC:
            constValueNode->dataType = FLOAT_TYPE;
            break;
        default:
            ;//puts("!?!?!? const value type");
    }
}


void checkReturnStmt(AST_NODE* returnNode)
{
    //puts("into checkReturnStmt");
    traverse(returnNode);
    if(returnNode->child->dataType != currentReturnType){
        if(returnNode->child->dataType != NONE_TYPE || currentReturnType != VOID_TYPE)
            printErrorMsg(returnNode, RETURN_TYPE_UNMATCH);
    }
    if(returnNode->child->nodeType == IDENTIFIER_NODE && \
            returnNode->child->semantic_value.identifierSemanticValue.kind == ARRAY_ID)
    {
        printErrorMsg(returnNode, RETURN_ARRAY);
    }
}


void processBlockNode(AST_NODE* blockNode)
{
    //puts("processBlockNode");
    openScope();
    traverse(blockNode);
    closeScope();
}


void processStmtNode(AST_NODE* stmtNode)
{
    //puts("into processStmtNode");
    switch (stmtNode->semantic_value.stmtSemanticValue.kind) {
        case WHILE_STMT:
            //puts("WHILE_STMT");
            checkWhileStmt(stmtNode);
            break;
        case FOR_STMT:
            //puts("FOR_STMT");
            checkForStmt(stmtNode);
            break;
        case ASSIGN_STMT:
            //puts("ASSIGN_STMT");
            checkAssignmentStmt(stmtNode);
            break;
        case IF_STMT:
            //puts("IF_STMT");
            checkIfStmt(stmtNode);
            break;
        case FUNCTION_CALL_STMT:
            //puts("FUNCTION_CALL_STMT");
            checkFunctionCall(stmtNode);
            break;
        case RETURN_STMT:
            //puts("RETURN_STMT");
            checkReturnStmt(stmtNode);
            break;
        default:
            ;//puts("!?!?!?!?!? STMT");
    }
}


void processGeneralNode(AST_NODE *node)
{
}

void checkIdentifierNode(AST_NODE* idNode)
{
    //printf("into checkIdentifierNode %s\n", idNode->semantic_value.identifierSemanticValue.identifierName);
    traverse(idNode);
    SymbolTableEntry* ptr = retrieveSymbol(IdName(idNode));
    idNode->semantic_value.identifierSemanticValue.symbolTableEntry = ptr;
    if(ptr == NULL)
        printErrorMsg(idNode, SYMBOL_UNDECLARED);
    else if(ptr->attribute->attributeKind == TYPE_ATTRIBUTE)
        printErrorMsg(idNode, IS_TYPE_NOT_VARIABLE);
    else if(ptr->attribute->attributeKind == FUNCTION_SIGNATURE){
        //puts("function");
        idNode->dataType = ptr->attribute->attr.functionSignature->returnType;
        //printf("%d\n", idNode->dataType);
        //printErrorMsg(idNode, IS_FUNCTION_NOT_VARIABLE);
    }
    else if(ptr->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR){
        //puts("scalar");
        idNode->dataType = ptr->attribute->attr.typeDescriptor->properties.dataType;
        idNode->semantic_value.identifierSemanticValue.kind = NORMAL_ID;
    }
    else if(ptr->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR){
        //puts("array");
        idNode->dataType = ptr->attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
        //printf("%d\n", idNode->dataType);
        idNode->semantic_value.identifierSemanticValue.kind = ARRAY_ID;
        int dim = 0;
        for(AST_NODE* i = idNode->child ; i != NULL ; i = i->rightSibling){
            if(i->dataType != INT_TYPE)
                printErrorMsg(idNode, ARRAY_SUBSCRIPT_NOT_INT);
            ++dim;
        }
        //printf("ID %s %d %d\n", IdName(idNode), dim, ptr->attribute->attr.typeDescriptor->properties.arrayProperties.dimension);
        if(ignoreDim == 0){//should match
            if(dim != ptr->attribute->attr.typeDescriptor->properties.arrayProperties.dimension)
                printErrorMsg(idNode, INCOMPATIBLE_ARRAY_DIMENSION);
            else
                idNode->semantic_value.identifierSemanticValue.kind = NORMAL_ID;
        }
        else{//param passing
            if(dim == ptr->attribute->attr.typeDescriptor->properties.arrayProperties.dimension)
                idNode->semantic_value.identifierSemanticValue.kind = NORMAL_ID;
            else if(dim < ptr->attribute->attr.typeDescriptor->properties.arrayProperties.dimension)
                idNode->semantic_value.identifierSemanticValue.kind = ARRAY_ID;
            else
                printErrorMsg(idNode, INCOMPATIBLE_ARRAY_DIMENSION);
        }
    }
}
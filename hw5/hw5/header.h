#ifndef __HEADER_H__
#define __HEADER_H__

#define MAX_ARRAY_DIMENSION 10

typedef enum DATA_TYPE
{
    INT_TYPE,
    FLOAT_TYPE,
    VOID_TYPE,
    INT_PTR_TYPE,//for parameter passing
    FLOAT_PTR_TYPE,//for parameter passing
    CONST_STRING_TYPE,//for "const string"
    NONE_TYPE,//for nodes like PROGRAM_NODE which has no type
    ERROR_TYPE
} DATA_TYPE;

typedef enum IDENTIFIER_KIND
{
    NORMAL_ID, //function Name, uninitialized scalar variable
    ARRAY_ID, //ID_NODE->child = dim
    WITH_INIT_ID, //ID_NODE->child = initial value
} IDENTIFIER_KIND;

typedef enum BINARY_OPERATOR
{
    BINARY_OP_ADD,
    BINARY_OP_SUB,
    BINARY_OP_MUL,
    BINARY_OP_DIV,
    BINARY_OP_EQ,
    BINARY_OP_GE,
    BINARY_OP_LE,
    BINARY_OP_NE,
    BINARY_OP_GT,
    BINARY_OP_LT,
    BINARY_OP_AND,
    BINARY_OP_OR
} BINARY_OPERATOR;

typedef enum UNARY_OPERATOR
{
    UNARY_OP_POSITIVE,
    UNARY_OP_NEGATIVE,
    UNARY_OP_LOGICAL_NEGATION
} UNARY_OPERATOR;

//C_type= type of constant ex: 1, 3.3, "const string"
//do not modify, or lexer might break
typedef enum C_type {INTEGERC,FLOATC,STRINGC} C_type;

typedef enum STMT_KIND
{
    WHILE_STMT,
    FOR_STMT,
    ASSIGN_STMT, //TODO:for simpler implementation, assign_expr also uses this
    IF_STMT,
    FUNCTION_CALL_STMT,
    RETURN_STMT,
} STMT_KIND;

typedef enum EXPR_KIND
{
    BINARY_OPERATION,
    UNARY_OPERATION
} EXPR_KIND;

typedef enum DECL_KIND
{
    VARIABLE_DECL,
    TYPE_DECL,
    FUNCTION_DECL,
    FUNCTION_PARAMETER_DECL
} DECL_KIND;

typedef enum AST_TYPE
{
    PROGRAM_NODE,
    DECLARATION_NODE,
    IDENTIFIER_NODE,
    PARAM_LIST_NODE,
    NUL_NODE,
    BLOCK_NODE,
    VARIABLE_DECL_LIST_NODE,
    STMT_LIST_NODE,
    STMT_NODE,
    EXPR_NODE,
    CONST_VALUE_NODE, //ex:1, 2, "constant string"
    NONEMPTY_ASSIGN_EXPR_LIST_NODE,
    NONEMPTY_RELOP_EXPR_LIST_NODE
} AST_TYPE;

//*************************
// AST_NODE's semantic value
//*************************

typedef struct STMTSemanticValue
{
    STMT_KIND kind;
} STMTSemanticValue;

typedef struct EXPRSemanticValue
{
    EXPR_KIND kind;
    
    int isConstEval;

    union
    {
        int iValue;
        float fValue;
    } constEvalValue;

    union
    {
        BINARY_OPERATOR binaryOp;
        UNARY_OPERATOR unaryOp;
    } op;
} EXPRSemanticValue;

typedef struct DECLSemanticValue
{
    DECL_KIND kind;
} DECLSemanticValue;

struct SymbolAttribute;

typedef struct IdentifierSemanticValue
{
    char *identifierName;
    struct SymbolTableEntry *symbolTableEntry;
    IDENTIFIER_KIND kind;
} IdentifierSemanticValue;

typedef struct TypeSpecSemanticValue
{
    char *typeName;
} TypeSpecSemanticValue;

//don't modify or lexer may break
typedef struct CON_Type{
        C_type  const_type;
	union {
		int     intval;
		double  fval;
		char    *sc; }
		const_u;
} CON_Type;


struct AST_NODE {
	struct AST_NODE *child;
	struct AST_NODE *parent;
	struct AST_NODE *rightSibling;
	struct AST_NODE *leftmostSibling;
	AST_TYPE nodeType;
    DATA_TYPE dataType;
	int linenumber;
	union {
        IdentifierSemanticValue identifierSemanticValue;
        STMTSemanticValue stmtSemanticValue;
        DECLSemanticValue declSemanticValue;
        EXPRSemanticValue exprSemanticValue;
		CON_Type *const1;
	} semantic_value;
};
typedef struct AST_NODE AST_NODE;

AST_NODE *Allocate(AST_TYPE type);
void semanticAnalysis(AST_NODE *root);

#endif

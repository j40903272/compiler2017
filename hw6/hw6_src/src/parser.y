/* ===== Definition Section ===== */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "header.h"
#include "symbolTable.h"
#include "myRegister.h"
#include "printSourceFile.h"
int linenumber = 1;
AST_NODE *prog;

extern int g_anyErrorOccur;

static inline AST_NODE* makeSibling(AST_NODE *a, AST_NODE *b)
{ 
    while (a->rightSibling) {
        a = a->rightSibling;
    }
    if (b == NULL) {
        return a;
    }
    b = b->leftmostSibling;
    a->rightSibling = b;
    
    b->leftmostSibling = a->leftmostSibling;
    b->parent = a->parent;
    while (b->rightSibling) {
        b = b->rightSibling;
        b->leftmostSibling = a->leftmostSibling;
        b->parent = a->parent;
    }
    return b;
}

static inline AST_NODE* makeChild(AST_NODE *parent, AST_NODE *child)
{
    if (child == NULL) {
        return parent;
    }
    if (parent->child) {
        makeSibling(parent->child, child);
    } else {
        child = child->leftmostSibling;
        parent->child = child;
        while (child) {
            child->parent = parent;
            child = child->rightSibling;
        }
    }
    return parent;
}

static AST_NODE* makeFamily(AST_NODE *parent, int childrenCount, ...)
{
    va_list childrenList;
    va_start(childrenList, childrenCount);
    AST_NODE* child = va_arg(childrenList, AST_NODE*);
    makeChild(parent, child);
    AST_NODE* tmp = child;
    int index = 1;
    for (index = 1; index < childrenCount; ++index) {
        child = va_arg(childrenList, AST_NODE*);
        tmp = makeSibling(tmp, child);
    }
    va_end(childrenList);
    return parent;
}

static inline AST_NODE* makeIDNode(char *lexeme, IDENTIFIER_KIND idKind)
{
    AST_NODE* identifier = Allocate(IDENTIFIER_NODE);
    identifier->semantic_value.identifierSemanticValue.identifierName = lexeme;
    identifier->semantic_value.identifierSemanticValue.kind = idKind;
    identifier->semantic_value.identifierSemanticValue.symbolTableEntry = NULL;
    return identifier;                        
}

static inline AST_NODE* makeStmtNode(STMT_KIND stmtKind)
{
    AST_NODE* stmtNode = Allocate(STMT_NODE);
    stmtNode->semantic_value.stmtSemanticValue.kind = stmtKind;
    return stmtNode;                        
}

static inline AST_NODE* makeDeclNode(DECL_KIND declKind)
{
    AST_NODE* declNode = Allocate(DECLARATION_NODE);
    declNode->semantic_value.declSemanticValue.kind = declKind;
    return declNode;                        
}

static inline AST_NODE* makeExprNode(EXPR_KIND exprKind, int operationEnumValue)
{
    AST_NODE* exprNode = Allocate(EXPR_NODE);
    exprNode->semantic_value.exprSemanticValue.isConstEval = 0;
    exprNode->semantic_value.exprSemanticValue.kind = exprKind;
    if (exprKind == BINARY_OPERATION) {
        exprNode->semantic_value.exprSemanticValue.op.binaryOp = operationEnumValue;
    } else if (exprKind == UNARY_OPERATION) {
        exprNode->semantic_value.exprSemanticValue.op.unaryOp = operationEnumValue;
    } else {
        printf("Error in static inline AST_NODE* makeExprNode(EXPR_KIND exprKind, int operationEnumValue)\n");
    }
    return exprNode;                        
}

%}



%union{
	char *lexeme;
	CON_Type  *const1;
	AST_NODE  *node;
};

%token <lexeme>ID
%token <const1>CONST
%token VOID    
%token INT     
%token FLOAT   
%token IF      
%token ELSE    
%token WHILE   
%token FOR 
%token TYPEDEF 
%token OP_ASSIGN  
%token OP_OR   
%token OP_AND  
%token OP_NOT  
%token OP_EQ   
%token OP_NE   
%token OP_GT   
%token OP_LT   
%token OP_GE   
%token OP_LE   
%token OP_PLUS 
%token OP_MINUS        
%token OP_TIMES        
%token OP_DIVIDE       
%token MK_LB 
%token MK_RB 
%token MK_LPAREN       
%token MK_RPAREN       
%token MK_LBRACE       
%token MK_RBRACE       
%token MK_COMMA        
%token MK_SEMICOLON    
%token MK_DOT  
%token ERROR
%token RETURN

%type <node> program global_decl_list global_decl function_head function_decl block stmt_list decl_list decl var_decl type init_id_list init_id  stmt relop_expr relop_term relop_factor expr term factor var_ref
%type <node> opt_param_list param_list param dim_fn expr_null id_list dim_decl cexpr mcexpr cfactor assign_expr_list test assign_expr rel_op relop_expr_list nonempty_relop_expr_list
%type <node> add_op mul_op dim_list type_decl nonempty_assign_expr_list


%start program

%%

/* ==== Grammar Section ==== */

/* Productions */               /* Semantic actions */
program		: global_decl_list { $$=Allocate(PROGRAM_NODE);  makeChild($$,$1); prog=$$;}
		| { $$=Allocate(PROGRAM_NODE); prog=$$;}
		;

global_decl_list: global_decl_list global_decl 
                    {
                        $$ = makeSibling($1, $2);
                    }	
                | global_decl
                    {
                        $$ = $1;
                    }
                ; 

global_decl	: decl_list function_decl
                {
                    $$ = makeSibling(makeChild(Allocate(VARIABLE_DECL_LIST_NODE), $1), $2);
                }
            | function_decl 
                {
                    $$ = $1;
                }
            ;

function_decl	: function_head MK_LPAREN opt_param_list MK_RPAREN MK_LBRACE block MK_RBRACE
                    {
                        $$ = $1;
                        makeChild($$, $3);
                        makeChild($$, $6);
                    }
                ;
function_head   : type ID
                    {
                        $$ = makeDeclNode(FUNCTION_DECL);
                        makeFamily($$, 2, $1, makeIDNode($2, NORMAL_ID));
                    }
                | VOID ID
                    {
                        $$ = makeDeclNode(FUNCTION_DECL);
                        AST_NODE* voidNode = makeIDNode("void", NORMAL_ID);
                        makeFamily($$, 2, voidNode, makeIDNode($2, NORMAL_ID));
                    }
                | ID ID
                    {
                        $$ = makeDeclNode(FUNCTION_DECL);
                        AST_NODE* idNode = makeIDNode($1, NORMAL_ID);
                        makeFamily($$, 2, idNode, makeIDNode($2, NORMAL_ID));
                    }
                ;
opt_param_list  : param_list
                    {
                        $$ = Allocate(PARAM_LIST_NODE);
                        makeChild($$, $1);
                    }
                |
                    {
                        $$ = Allocate(PARAM_LIST_NODE);
                    }
                ;

param_list	: param_list MK_COMMA  param 
                {
                    $$ = makeSibling($1, $3);
                }
            | param	
                {
                    $$ = $1; 
                }
            ;

param		: type ID 
                {
                    $$ = makeDeclNode(FUNCTION_PARAMETER_DECL);
                    makeFamily($$, 2, $1, makeIDNode($2, NORMAL_ID));
                }
            | ID ID
                {
                    $$ = makeDeclNode(FUNCTION_PARAMETER_DECL);
                    makeFamily($$, 2, makeIDNode($1, NORMAL_ID), makeIDNode($2, NORMAL_ID));
                }
            | type ID dim_fn 
                {
                    $$ = makeDeclNode(FUNCTION_PARAMETER_DECL);
                    makeFamily($$, 2, $1, makeChild(makeIDNode($2, ARRAY_ID), $3));
                }
            | ID ID dim_fn 
                {
                    $$ = makeDeclNode(FUNCTION_PARAMETER_DECL);
                    makeFamily($$, 2, makeIDNode($1, NORMAL_ID), makeChild(makeIDNode($2, ARRAY_ID), $3));
                }
            ;
dim_fn		: MK_LB expr_null MK_RB 
                {
                    $$ = $2;
                }
            | dim_fn MK_LB expr MK_RB
                {
                    $$ = makeSibling($1, $3);
                }
		;

expr_null	:expr 
                {
                    $$ = $1;
                }
            |
                {
                    $$ = Allocate(NUL_NODE); 
                }
            ;

block           : decl_list stmt_list 
                    {
                        $$ = Allocate(BLOCK_NODE);
                        makeFamily($$, 2, makeChild(Allocate(VARIABLE_DECL_LIST_NODE), $1), makeChild(Allocate(STMT_LIST_NODE), $2));
                    }
                | stmt_list  
                    {
                        $$ = Allocate(BLOCK_NODE);
                        makeChild($$, makeChild(Allocate(STMT_LIST_NODE), $1));
                    }
                | decl_list 
                    {
                        $$ = Allocate(BLOCK_NODE);
                        makeChild($$, makeChild(Allocate(VARIABLE_DECL_LIST_NODE), $1));
                    }
                |   {
                        $$ = Allocate(BLOCK_NODE);
                    }
                ;
 
decl_list	: decl_list decl 
                {
                    $$ = makeSibling($1, $2);
                }
            | decl 
                {
                    $$ = $1;
                }
            ;

decl		: type_decl 
                {
                    $$ = $1;
                }
            | var_decl 
                {
                    $$ = $1;
                }
            ;

type_decl 	: TYPEDEF type id_list MK_SEMICOLON  
                {
                    $$ = makeDeclNode(TYPE_DECL);
                    makeFamily($$, 2, $2, $3);
                }
            | TYPEDEF VOID id_list MK_SEMICOLON 
                {
                    $$ = makeDeclNode(TYPE_DECL);
                    AST_NODE* voidNode = makeIDNode("void", NORMAL_ID);
                    makeFamily($$, 2, voidNode, $3);
                }
            ;

var_decl	: type init_id_list MK_SEMICOLON 
                {
                    $$ = makeDeclNode(VARIABLE_DECL);
                    makeFamily($$, 2, $1, $2);
                }
            | ID init_id_list MK_SEMICOLON
                {
                    $$ = makeDeclNode(VARIABLE_DECL);
                    makeFamily($$, 2, makeIDNode($1, NORMAL_ID), $2);
                }
            ;

type		: INT 
                {
                    $$ = makeIDNode("int", NORMAL_ID);  
                }
            | FLOAT 
                {
                    $$ = makeIDNode("float", NORMAL_ID);
                }
            ;

id_list		: ID 
                {
                    $$ = makeIDNode($1, NORMAL_ID);
                }
            | id_list MK_COMMA ID 
                {
                    $$ = makeSibling($1, makeIDNode($3, NORMAL_ID));
                }
            | id_list MK_COMMA ID dim_decl
                {
                    $$ = makeSibling($1, makeChild(makeIDNode($3, ARRAY_ID), $4));
                }
            | ID dim_decl
                {
                    $$ = makeChild(makeIDNode($1, ARRAY_ID), $2);
                }
		;
dim_decl	: MK_LB cexpr MK_RB 
                {
                    $$ = $2;
                } 
            | dim_decl MK_LB cexpr MK_RB 
                {
                    $$ = makeSibling($1, $3);
                } 
            ;
cexpr		: cexpr OP_PLUS mcexpr 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_ADD);
                    makeFamily($$, 2, $1, $3);
                } 
            | cexpr OP_MINUS mcexpr
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_SUB);
                    makeFamily($$, 2, $1, $3);
                } 
            | mcexpr 
                {
                    $$ = $1;
                }
            ;  
mcexpr		: mcexpr OP_TIMES cfactor 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_MUL);
                    makeFamily($$, 2, $1, $3);
                }
            | mcexpr OP_DIVIDE cfactor 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_DIV);
                    makeFamily($$, 2, $1, $3);
                }
            | cfactor 
                {
                    $$ = $1;
                }
            ;
        
cfactor:	CONST 
                {
                    $$ = Allocate(CONST_VALUE_NODE);
                    $$->semantic_value.const1 = $1;
                }
            |MK_LPAREN cexpr MK_RPAREN 
                {
                    $$ = $2;
                }
            ;

init_id_list	: init_id 
                    {
                        $$ = $1; 
                    }
                | init_id_list MK_COMMA init_id 
                    {
                        $$ = makeSibling($1, $3);
                    }
                ;

init_id		: ID 
                {
                    $$ = makeIDNode($1, NORMAL_ID);
                }
            | ID dim_decl 
                {
                    $$ = makeChild(makeIDNode($1, ARRAY_ID), $2);
                }
            | ID OP_ASSIGN relop_expr 
                {
                    $$ = makeChild(makeIDNode($1, WITH_INIT_ID), $3);
                }
            ;

stmt_list	: stmt_list stmt 
                {
                    $$ = makeSibling($1, $2);
                }
            | stmt
                {
                    $$ = $1;
                }
            ;



stmt		: MK_LBRACE block MK_RBRACE 
                {
                    $$ = $2;
                }
            | WHILE MK_LPAREN test MK_RPAREN stmt 
                {
                    $$ = makeStmtNode(WHILE_STMT);
                    makeFamily($$, 2, $3, $5);
                }
            | FOR MK_LPAREN assign_expr_list MK_SEMICOLON relop_expr_list MK_SEMICOLON assign_expr_list MK_RPAREN stmt
                {
                    $$ = makeStmtNode(FOR_STMT);
                    makeFamily($$, 4, $3, $5, $7, $9);
                }
            | var_ref OP_ASSIGN relop_expr MK_SEMICOLON
                {
                    $$ = makeStmtNode(ASSIGN_STMT);
                    makeFamily($$, 2, $1, $3);
                }
            | IF MK_LPAREN test MK_RPAREN stmt
                {
                    $$ = makeStmtNode(IF_STMT);
                    makeFamily($$, 3, $3, $5, Allocate(NUL_NODE));
                }
            | IF MK_LPAREN test MK_RPAREN stmt ELSE stmt
                {
                    $$ = makeStmtNode(IF_STMT);
                    makeFamily($$, 3, $3, $5, $7);
                }
            | ID MK_LPAREN relop_expr_list MK_RPAREN MK_SEMICOLON
                {
                    $$ = makeStmtNode(FUNCTION_CALL_STMT);
                    makeFamily($$, 2, makeIDNode($1, NORMAL_ID), $3);
                }
            | MK_SEMICOLON 
                {
                    $$ = Allocate(NUL_NODE);
                }
            | RETURN MK_SEMICOLON  
                {
                    $$ = makeStmtNode(RETURN_STMT);
                    makeChild($$, Allocate(NUL_NODE));
                }
            | RETURN relop_expr MK_SEMICOLON
                {
                    $$ = makeStmtNode(RETURN_STMT);
                    makeChild($$, $2);
                }
            ;

assign_expr_list : nonempty_assign_expr_list 
                     {
                        $$ = makeChild(Allocate(NONEMPTY_ASSIGN_EXPR_LIST_NODE), $1);
                     }
                 |  
                     {
                         $$ = Allocate(NUL_NODE); 
                     }
                 ;

nonempty_assign_expr_list        : nonempty_assign_expr_list MK_COMMA assign_expr 
                                    {
                                        $$ = makeSibling($1, $3);
                                    }
                                 | assign_expr
                                    {
                                        $$ = $1;
                                    }
                                 ;

test		: assign_expr
                {
                    $$ = $1;
                }
            ;

assign_expr     : var_ref OP_ASSIGN relop_expr 
                    {
                        //TODO: for simpler implementation, use ASSIGN_STMT for now
                        $$ = makeStmtNode(ASSIGN_STMT);
                        makeFamily($$, 2, $1, $3);
                    }
                | relop_expr
                    {
                        $$ = $1;
                    }
		;

relop_expr	: relop_term 
                {
                    $$ = $1;
                }
            | relop_expr OP_OR relop_term
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_OR);
                    makeFamily($$, 2, $1, $3);
                }
            ;

relop_term	: relop_factor 
                {
                    $$ = $1;
                }
            | relop_term OP_AND relop_factor
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_AND);
                    makeFamily($$, 2, $1, $3);
                }
            ;

relop_factor	: expr
                    {
                        $$ = $1;
                    }
                | expr rel_op expr 
                    {
                        $$ = makeFamily($2, 2, $1, $3);
                    }
                ;

rel_op		: OP_EQ
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_EQ);
                }
            | OP_GE 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_GE);
                }
            | OP_LE 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_LE);
                }
            | OP_NE 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_NE);
                }
            | OP_GT 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_GT);
                }
            | OP_LT 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_LT);
                }
            ;


relop_expr_list	: nonempty_relop_expr_list 
                    {
                        $$ = makeChild(Allocate(NONEMPTY_RELOP_EXPR_LIST_NODE), $1);
                    }
                | 
                    {
                        $$ = Allocate(NUL_NODE);
                    }
                ;

nonempty_relop_expr_list	: nonempty_relop_expr_list MK_COMMA relop_expr
                                {
                                    $$ = makeSibling($1, $3);
                                }
                            | relop_expr 
                                {
                                    $$ = $1;
                                }
                            ;

expr		: expr add_op term 
                {
                    $$ = makeFamily($2, 2, $1, $3);
                }
            | term 
                {
                    $$ = $1;
                }
            ;

add_op		: OP_PLUS
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_ADD);
                }
            | OP_MINUS 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_SUB);
                }
            ;

term		: term mul_op factor
                {
                    $$ = makeFamily($2, 2, $1, $3);
                }
            | factor
                {
                    $$ = $1;
                }
            ;

mul_op		: OP_TIMES
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_MUL);
                }
            | OP_DIVIDE 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_DIV);
                }
            ;

factor		: MK_LPAREN relop_expr MK_RPAREN
                {
                    $$ = $2;
                }
            | OP_MINUS MK_LPAREN relop_expr MK_RPAREN 
                {
                    $$ = makeExprNode(UNARY_OPERATION, UNARY_OP_NEGATIVE);
                    makeChild($$, $3);
                }
            | OP_NOT MK_LPAREN relop_expr MK_RPAREN
                {   
                    $$ = makeExprNode(UNARY_OPERATION, UNARY_OP_LOGICAL_NEGATION);
                    makeChild($$, $3);
                }
            | CONST 
                {
                    $$ = Allocate(CONST_VALUE_NODE);
                    $$->semantic_value.const1=$1;
                }
            | OP_MINUS CONST 
                {
                    $$ = makeExprNode(UNARY_OPERATION, UNARY_OP_NEGATIVE);
                    AST_NODE *constNode = Allocate(CONST_VALUE_NODE);
                    constNode->semantic_value.const1 = $2;
                    makeChild($$, constNode);
                }
            | OP_NOT CONST
                {
                    $$ = makeExprNode(UNARY_OPERATION, UNARY_OP_LOGICAL_NEGATION);
                    AST_NODE *constNode = Allocate(CONST_VALUE_NODE);
                    constNode->semantic_value.const1 = $2;
                    makeChild($$, constNode);
                }
            | ID MK_LPAREN relop_expr_list MK_RPAREN 
                {
                    $$ = makeStmtNode(FUNCTION_CALL_STMT);
                    makeFamily($$, 2, makeIDNode($1, NORMAL_ID), $3);
                }
            | OP_MINUS ID MK_LPAREN relop_expr_list MK_RPAREN 
                {
                    $$ = makeExprNode(UNARY_OPERATION, UNARY_OP_NEGATIVE);
                    AST_NODE* functionCallNode = makeStmtNode(FUNCTION_CALL_STMT);
                    makeFamily(functionCallNode, 2, makeIDNode($2, NORMAL_ID), $4);
                    makeChild($$, functionCallNode);
                }
            | OP_NOT ID MK_LPAREN relop_expr_list MK_RPAREN
                {
                    $$ = makeExprNode(UNARY_OPERATION, UNARY_OP_LOGICAL_NEGATION);
                    AST_NODE* functionCallNode = makeStmtNode(FUNCTION_CALL_STMT);
                    makeFamily(functionCallNode, 2, makeIDNode($2, NORMAL_ID), $4);
                    makeChild($$, functionCallNode);
                }
            | var_ref 
                {
                    $$ = $1;
                }
            | OP_MINUS var_ref 
                {
                    $$ = makeExprNode(UNARY_OPERATION, UNARY_OP_NEGATIVE);
                    makeChild($$, $2);
                }
            | OP_NOT var_ref 
                {
                    $$ = makeExprNode(UNARY_OPERATION, UNARY_OP_LOGICAL_NEGATION);
                    makeChild($$, $2);
                }
            ;

var_ref		: ID 
                {
                    $$ = makeIDNode($1, NORMAL_ID);
                }
            | ID dim_list 
                {
                    $$ = makeIDNode($1, ARRAY_ID);
                    makeChild($$,$2);
                }
            ;


dim_list	: dim_list MK_LB expr MK_RB 
                {
                    $$ = makeSibling($1, $3);
                }
            | MK_LB expr MK_RB
                {
                    $$ = $2;
                }
		;


%%

#include "lex.yy.c"

void initSystem()
{
     initializeSymbolTable();
     initializeRegisterTable();
}

void closeSystem()
{
     symbolTableEnd();
}

main (argc, argv)
int argc;
char *argv[];
  {
     yyin = fopen(argv[1],"r");
     yyparse();
     // printGV(prog, NULL);
     
     initSystem();
     readSourceFile(argv[1]);
     
     semanticAnalysis(prog);
     
     codeGenerate(prog);
     
     closeSystem();
     
     if(!g_anyErrorOccur) {
        printf("Parsing completed. No errors found.\n");
     }
  } /* main */


int yyerror (mesg)
char *mesg;
  {
  printf("%s\t%d\t%s\t%s\n", "Error found in Line ", linenumber, "next token: ", yytext );
  exit(1);
  }

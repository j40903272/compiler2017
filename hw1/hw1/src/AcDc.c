#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "header.h"

SymbolTable *globaltable; 
char tokentype[32][32] = { "FloatDeclaration", "IntegerDeclaration", "PrintOp", "AssignmentOp", "PlusOp", "MinusOp",
             "MulOp", "DivOp", "Alphabet", "IntValue", "FloatValue", "EOFsymbol" };
char datatype[8][8] = { "Int", "Float", "Notype" };
char stmttype[4][32] = { "Print", "Assignment" };
char valuetype[16][32] = { "Identifier", "IntConst", "FloatConst", "PlusNode", "MinusNode", "MulNode", "DivNode", "IntToFloatConvertNode" };
char operation[16][32] = { "Plus", "Minus", "Mul", "Div", "Assign", "IntToFloatConvert" };

int main( int argc, char *argv[] )
{
    FILE *source, *target;
    Program program;
    SymbolTable symtab;

    if( argc == 3){
        source = fopen(argv[1], "r");
        target = fopen(argv[2], "w");
        if( !source ){
            printf("can't open the source file\n");
            exit(2);
        }
        else if( !target ){
            printf("can't open the target file\n");
            exit(2);
        }
        else{
            program = parser(source);
            fclose(source);
            symtab = build(program);
            check(&program, &symtab);
            //printf("\nAFTER CONSTANT FOLDING\n\n");
            //check(&program, &symtab);
            globaltable = &symtab;
            gencode(program, target);
        }
    }
    else{
        printf("Usage: %s source_file target_file\n", argv[0]);
    }


    return 0;
}


/********************************************* 
  Scanning 
 *********************************************/
Token getNumericToken( FILE *source, char c )
{
    Token token;
    int i = 0;

    while( isdigit(c) ) {
        token.tok[i++] = c;
        c = fgetc(source);
    }

    if( c != '.' ){
        ungetc(c, source);
        token.tok[i] = '\0';
        token.type = IntValue;
        return token;
    }

    token.tok[i++] = '.';

    c = fgetc(source);
    if( !isdigit(c) ){
        ungetc(c, source);
        printf("Expect a digit : %c\n", c);
        exit(1);
    }

    while( isdigit(c) ){
        token.tok[i++] = c;
        c = fgetc(source);
    }

    ungetc(c, source);
    token.tok[i] = '\0';
    token.type = FloatValue;
    return token;
}

Token scanner( FILE *source )
{
    char c;
    Token token;

    while( !feof(source) ){
        c = fgetc(source);

        while( isspace(c) ) c = fgetc(source);

        if( isdigit(c) )
            return getNumericToken(source, c);

        int cnt = 0;
        
        if( islower(c) ){
            while( islower(c) ) {
                token.tok[cnt++] = c;
                c = fgetc(source);
            }
            ungetc(c, source);
            token.tok[cnt] = '\0';
            if( strcmp(token.tok, "f") == 0 )
                token.type = FloatDeclaration;
            else if( strcmp(token.tok, "i") == 0 )
                token.type = IntegerDeclaration;
            else if( strcmp(token.tok, "p") == 0 )
                token.type = PrintOp;
            else
                token.type = Alphabet;
            return token;
        }
        token.tok[0] = c;
        token.tok[1] = '\0';
        switch(c){
            case '=':
                token.type = AssignmentOp;
                return token;
            case '+':
                token.type = PlusOp;
                return token;
            case '-':
                token.type = MinusOp;
                return token;
            case '*':
                token.type = MulOp;
                return token;
            case '/':
                token.type = DivOp;
                return token;
            case EOF:
                token.type = EOFsymbol;
                token.tok[0] = '\0';
                return token;
            default:
                printf("Invalid character : %c\n", c);
                exit(1);
        }
    }

    token.tok[0] = '\0';
    token.type = EOFsymbol;
    return token;
}


/********************************************************
  Parsing
 *********************************************************/
Declaration parseDeclaration( FILE *source, Token token )
{
    Token token2;
    switch(token.type){
        case FloatDeclaration:
        case IntegerDeclaration:
            token2 = scanner(source);
            //printf("[%d] return from scanner: %s, type : %s\n", __LINE__, token2.tok, tokentype[token2.type]);
            if (strcmp(token2.tok, "f") == 0 ||
                    strcmp(token2.tok, "i") == 0 ||
                    strcmp(token2.tok, "p") == 0) {
                printf("Syntax Error: %s cannot be used as id\n", token2.tok);
                exit(1);
            }
            return makeDeclarationNode( token, token2 );
        default:
            printf("Syntax Error: Expect Declaration %s\n", token.tok);
            exit(1);
    }
}

Declarations *parseDeclarations( FILE *source )
{
    Token token = scanner(source);
    //printf("[%d] return from scanner: %s, type : %s\n", __LINE__, token.tok, tokentype[token.type]);
    Declaration decl;
    Declarations *decls;
    switch(token.type){
        case FloatDeclaration:
        case IntegerDeclaration:
            decl = parseDeclaration(source, token);
            decls = parseDeclarations(source);
            return makeDeclarationTree( decl, decls );
        case PrintOp:
        case Alphabet:
            for(int i = strlen(token.tok)-1 ; i >= 0 ; i--)
                ungetc(token.tok[i], source);
            return NULL;
        case EOFsymbol:
            return NULL;
        default:
            printf("Syntax Error: Expect declarations %s\n", token.tok);
            exit(1);
    }
}

Expression *parseValue( FILE *source )
{
    Token token = scanner(source);
    //printf("[%d] return from scanner: %s, type : %s\n", __LINE__, token.tok, tokentype[token.type]);
    Expression *value = (Expression *)malloc( sizeof(Expression) );
    value->leftOperand = value->rightOperand = NULL;

    switch(token.type){
        case Alphabet:
            (value->v).type = Identifier;
            strcpy((value->v).val.id, token.tok);
            break;
        case IntValue:
            (value->v).type = IntConst;
            (value->v).val.ivalue = atoi(token.tok);
            break;
        case FloatValue:
            (value->v).type = FloatConst;
            (value->v).val.fvalue = atof(token.tok);
            break;
        default:
            printf("Syntax Error: Expect Identifier or a Number %s\n", token.tok);
            exit(1);
    }

    return value;
}

Expression *parseExpressionTail( FILE *source, Expression *lvalue )
{
    Token token = scanner(source);
    //printf("[%d] return from scanner: %s, type : %s\n", __LINE__, token.tok, tokentype[token.type]);
    Expression *expr;
    
    switch(token.type){
        case PlusOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = PlusNode;
            (expr->v).val.op = Plus;
            expr->leftOperand = lvalue;
            expr->rightOperand = parseValue(source);
            return parseExpressionTail(source, expr);
        case MinusOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = MinusNode;
            (expr->v).val.op = Minus;
            expr->leftOperand = lvalue;
            expr->rightOperand = parseValue(source);
            return parseExpressionTail(source, expr);
        case MulOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = MulNode;
            (expr->v).val.op = Mul;
            if(lvalue->v.type == PlusNode || lvalue->v.type == MinusNode){
                expr->leftOperand = lvalue->rightOperand;
                lvalue->rightOperand = expr;
                expr->rightOperand = parseValue(source);
                return parseExpressionTail(source, lvalue);
            }
            else{
                expr->leftOperand = lvalue;
                expr->rightOperand = parseValue(source);
                return parseExpressionTail(source, expr);
            }
        case DivOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = DivNode;
            (expr->v).val.op = Div;
            if(lvalue->v.type == PlusNode || lvalue->v.type == MinusNode){
                expr->leftOperand = lvalue->rightOperand;
                lvalue->rightOperand = expr;
                expr->rightOperand = parseValue(source);
                return parseExpressionTail(source, lvalue);
            }
            else{
                expr->leftOperand = lvalue;
                expr->rightOperand = parseValue(source);
                return parseExpressionTail(source, expr);
            }
        case Alphabet:
        case PrintOp:
            for(int i = strlen(token.tok)-1 ; i >= 0 ; i--)
                ungetc(token.tok[i], source);
            return lvalue;
        case EOFsymbol:
            return lvalue;
        default:
            printf("Syntax Error: Expect a numeric value or an identifier %s\n", token.tok);
            exit(1);
    }
}

Expression *parseExpression( FILE *source, Expression *lvalue )
{
    Token token = scanner(source);
    //printf("[%d] return from scanner: %s, type : %s\n", __LINE__, token.tok, tokentype[token.type]);
    Expression *expr;

    switch(token.type){
        case PlusOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = PlusNode;
            (expr->v).val.op = Plus;
            expr->leftOperand = lvalue;
            expr->rightOperand = parseValue(source);
            return parseExpressionTail(source, expr);
        case MinusOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = MinusNode;
            (expr->v).val.op = Minus;
            expr->leftOperand = lvalue;
            expr->rightOperand = parseValue(source);
            return parseExpressionTail(source, expr);
        case MulOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = MulNode;
            (expr->v).val.op = Mul;
            expr->leftOperand = lvalue;
            expr->rightOperand = parseValue(source);
            return parseExpressionTail(source, expr);
        case DivOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = DivNode;
            (expr->v).val.op = Div;
            expr->leftOperand = lvalue;
            expr->rightOperand = parseValue(source);
            return parseExpressionTail(source, expr);
        case Alphabet:
        case PrintOp:
            for(int i = strlen(token.tok)-1 ; i >= 0 ; i--)
                ungetc(token.tok[i], source);
            return lvalue;
        case EOFsymbol:
            return lvalue;
        default:
            printf("Syntax Error: Expect a numeric value or an identifier %s\n", token.tok);
            exit(1);
    }
}

Statement parseStatement( FILE *source, Token token )
{
    Token next_token;
    Expression *value, *expr;

    switch(token.type){
        case Alphabet:
            next_token = scanner(source);
            //printf("[%d] return from scanner: %s, type : %s\n", __LINE__, next_token.tok, tokentype[next_token.type]);
            if(next_token.type == AssignmentOp){
                value = parseValue(source);
                expr = parseExpression(source, value);
                return makeAssignmentNode(token.tok, value, expr);
            }
            else{
                printf("Syntax Error: Expect an assignment op %s\n", next_token.tok);
                exit(1);
            }
        case PrintOp:
            next_token = scanner(source);
            //printf("[%d] return from scanner: %s, type : %s\n", __LINE__, next_token.tok, tokentype[next_token.type]);
            if(next_token.type == Alphabet)
                return makePrintNode(next_token.tok);
            else{
                printf("Syntax Error: Expect an identifier %s\n", next_token.tok);
                exit(1);
            }
            break;
        default:
            printf("Syntax Error: Expect a statement %s\n", token.tok);
            exit(1);
    }
}

Statements *parseStatements( FILE * source )
{

    Token token = scanner(source);
    //printf("[%d] return from scanner: %s, type : %s\n", __LINE__, token.tok, tokentype[token.type]);
    Statement stmt;
    Statements *stmts;

    switch(token.type){
        case Alphabet:
        case PrintOp:
            stmt = parseStatement(source, token);
            stmts = parseStatements(source);
            return makeStatementTree(stmt , stmts);
        case EOFsymbol:
            return NULL;
        default:
            printf("Syntax Error: Expect statements %s\n", token.tok);
            exit(1);
    }
}


/*********************************************************************
  Build AST
 **********************************************************************/
Declaration makeDeclarationNode( Token declare_type, Token identifier )
{
    Declaration tree_node;

    switch(declare_type.type){
        case FloatDeclaration:
            tree_node.type = Float;
            break;
        case IntegerDeclaration:
            tree_node.type = Int;
            break;
        default:
            break;
    }
    strcpy(tree_node.name, identifier.tok);

    return tree_node;
}

Declarations *makeDeclarationTree( Declaration decl, Declarations *decls )
{
    Declarations *new_tree = (Declarations *)malloc( sizeof(Declarations) );
    new_tree->first = decl;
    new_tree->rest = decls;

    return new_tree;
}


Statement makeAssignmentNode( char *id, Expression *v, Expression *expr_tail )
{
    Statement stmt;
    AssignmentStatement assign;

    stmt.type = Assignment;
    strcpy(assign.id, id);
    if(expr_tail == NULL)
        assign.expr = v;
    else
        assign.expr = expr_tail;
    stmt.stmt.assign = assign;

    return stmt;
}

Statement makePrintNode( char *id )
{
    Statement stmt;
    stmt.type = Print;
    strcpy(stmt.stmt.variable, id);

    return stmt;
}

Statements *makeStatementTree( Statement stmt, Statements *stmts )
{
    Statements *new_tree = (Statements *)malloc( sizeof(Statements) );
    new_tree->first = stmt;
    new_tree->rest = stmts;

    return new_tree;
}

/* parser */
Program parser( FILE *source )
{
    Program program;

    program.declarations = parseDeclarations(source);
    program.statements = parseStatements(source);

    return program;
}


/********************************************************
  Build symbol table
 *********************************************************/
void InitializeTable( SymbolTable *table )
{
    table->var_num = 0;
    memset(table->table, 0, sizeof(table->table));
    for(int i = 0 ; i < 26; i++)
        table->typetable[i] = Notype;
}

void add_table( SymbolTable *table, char *c, DataType t )
{
    for(int i = 0 ; i < table->var_num ; i++)
        if(strcmp(c, table->table[i]) == 0)
            printf("Error : id %s has been declared\n", c);//error
    
    table->typetable[table->var_num] = t;
    strcpy(table->table[table->var_num], c);
    table->var_num += 1;
}

void printtable(SymbolTable *table){
    printf("\nprint SymbolTable\n");
    for(int i = 0 ; i < table->var_num ; i++){
        printf("name : %s, type : %s\n", table->table[i], datatype[table->typetable[i]]);
    }
    printf("==========================================\n");
}

SymbolTable build( Program program )
{
    SymbolTable table;
    Declarations *decls = program.declarations;
    Declaration current;

    InitializeTable(&table);

    while(decls !=NULL){
        current = decls->first;
        add_table(&table, current.name, current.type);
        decls = decls->rest;
    }
    //printtable(&table);
    return table;
}


/********************************************************************
  Type checking
 *********************************************************************/

void convertType( Expression *old, DataType type )
{
    if(old->type == Float && type == Int){
        printf("error : can't convert float to integer\n");
        return;
    }
    if(old->type == Int && type == Float){
        if(old->v.type == Identifier)
            printf("convert identifier[%s] to float\n",old->v.val.id);
        else if(old->v.type == IntConst){
            printf("convert int[%d] to float\n", old->v.val.ivalue);
            old->v.val.fvalue = (float)old->v.val.ivalue;
            old->v.type = FloatConst;
        }
        else
            printf("convert %s to float\n", valuetype[old->v.type]);

        Expression *tmp = (Expression *)malloc( sizeof(Expression) );
        *tmp = *old;
        old->leftOperand = tmp;
        old->rightOperand = NULL;

        old->type = Float;
        return;
    }
}

DataType generalize( Expression *left, Expression *right )
{
    if(left->type == Float || right->type == Float){
        printf("generalize : float\n");
        return Float;
    }
    printf("generalize : int\n");
    return Int;
}

DataType lookup_table( SymbolTable *table, char *c )
{
    int id = -1;
    for(int i = 0 ; i < table->var_num ; i++)
        if(strcmp(c, table->table[i]) == 0)
            id = i;
    if(id == -1)
        printf("Error : identifier %c is not declared\n", c);//error
    //printf("\nlookup table\n%s, %s\n", table->table[id], datatype[table->typetable[id]]);
    return table->typetable[id];
}

int checkexpression( Expression * expr, SymbolTable * table )
{
    char c[64];
    if(expr->leftOperand == NULL && expr->rightOperand == NULL){
        switch(expr->v.type){
            case Identifier:
                strcpy(c, expr->v.val.id);
                printf("identifier : %s\n",c);
                expr->type = lookup_table(table, c);
                return 0;
            case IntConst:
                //printf("constant : int\n");
                printf("constant : int[%d]\n", expr->v.val.ivalue);
                expr->type = Int;
                return 1;
            case FloatConst:
                //printf("constant : float\n");
                printf("constant : float[%f]\n", expr->v.val.fvalue);
                expr->type = Float;
                return 1;
                //case PlusNode: case MinusNode: case MulNode: case DivNode:
            default:
                return 0;
        }
    }
    else{
        Expression *left = expr->leftOperand;
        Expression *right = expr->rightOperand;

        int l = checkexpression(left, table);
        int r = checkexpression(right, table);
        printf("%s\n", valuetype[expr->v.type]);

        DataType type = generalize(left, right);
        convertType(left, type);//left->type = type;//converto
        convertType(right, type);//right->type = type;//converto
        expr->type = type;

        if(l && r){
            if(expr->type == Int){
                switch(expr->v.type){
                    case PlusNode:
                        expr->v.val.ivalue = left->v.val.ivalue + right->v.val.ivalue;
                        break;
                    case MinusNode:
                        expr->v.val.ivalue = left->v.val.ivalue - right->v.val.ivalue;
                        break;
                    case MulNode:
                        expr->v.val.ivalue = left->v.val.ivalue * right->v.val.ivalue;
                        break;
                    case DivNode:
                        expr->v.val.ivalue = left->v.val.ivalue / right->v.val.ivalue;
                        break;
                }
                printf("fold %d %s %d to %d\n", left->v.val.ivalue, valuetype[expr->v.type], right->v.val.ivalue, expr->v.val.ivalue);
                expr->v.type = IntConst;
            }
            else if(expr->type == Float){
                switch(expr->v.type){
                    case PlusNode:
                        expr->v.val.fvalue = left->v.val.fvalue + right->v.val.fvalue;
                        break;
                    case MinusNode:
                        expr->v.val.fvalue = left->v.val.fvalue - right->v.val.fvalue;
                        break;
                    case MulNode:
                        expr->v.val.fvalue = left->v.val.fvalue * right->v.val.fvalue;
                        break;
                    case DivNode:
                        expr->v.val.fvalue = left->v.val.fvalue / right->v.val.fvalue;
                        break;
                }
                printf("fold %f %s %f to %f\n", left->v.val.fvalue, valuetype[expr->v.type], right->v.val.fvalue, expr->v.val.fvalue);
                expr->v.type = FloatConst;
            }
            else printf("LINE :%d : CONSTANT FOLDING ERROR\n", __LINE__);
            expr->leftOperand = expr->rightOperand = NULL;
            return 1;
        }
    }
    return 0;
}

void checkstmt( Statement *stmt, SymbolTable * table )
{
    if(stmt->type == Assignment){
        AssignmentStatement assign = stmt->stmt.assign;
        printf("assignment : %s \n",assign.id);
        checkexpression(assign.expr, table);
        stmt->stmt.assign.type = lookup_table(table, assign.id);
        if (assign.expr->type == Float && stmt->stmt.assign.type == Int) {
            printf("error : can't convert float to integer\n");
        } else {
            convertType(assign.expr, stmt->stmt.assign.type);
        }
    }
    else if (stmt->type == Print){
        printf("print : %s \n",stmt->stmt.variable);
        lookup_table(table, stmt->stmt.variable);
    }
    else printf("error : statement error\n");//error
}

void check( Program *program, SymbolTable * table )
{
    Statements *stmts = program->statements;
    while(stmts != NULL){
        checkstmt(&stmts->first,table);
        stmts = stmts->rest;
    }
}


/***********************************************************************
  Code generation
 ************************************************************************/
char find_id(char str[]){
    for(int i = 0 ; i < globaltable->var_num ; i++){
        if(strcmp(str, globaltable->table[i]) == 0)
            return 'a' + i;
    }
    printf("line : %d, id not found\n", __LINE__);
    return 'a';
}

void fprint_op( FILE *target, ValueType op )
{
    switch(op){
        case MinusNode:
            fprintf(target,"-\n");
            break;
        case PlusNode:
            fprintf(target,"+\n");
            break;
        case MulNode:
            fprintf(target,"*\n");
            break;
        case DivNode:
            fprintf(target,"/\n");
            break;
        default:
            fprintf(target,"Error in fprintf_op ValueType = %d\n",op);
            break;
    }
}

void fprint_expr( FILE *target, Expression *expr)
{
    char id;
    if(expr->leftOperand == NULL){
        switch( (expr->v).type ){
            case Identifier:
                id = find_id((expr->v).val.id);
                fprintf(target,"l%c\n", id);
                break;
            case IntConst:
                fprintf(target,"%d\n",(expr->v).val.ivalue);
                break;
            case FloatConst:
                fprintf(target,"%f\n", (expr->v).val.fvalue);
                break;
            default:
                fprintf(target,"Error In fprint_left_expr. (expr->v).type=%s\n",valuetype[(expr->v).type]);
                break;
        }
    }
    else{
        fprint_expr(target, expr->leftOperand);
        if(expr->rightOperand == NULL)
            fprintf(target,"5k\n");
        else{
            fprint_expr(target, expr->rightOperand);
            fprint_op(target, (expr->v).type);
        }
    }
}

void gencode(Program prog, FILE * target)
{
    Statements *stmts = prog.statements;
    Statement stmt;
    char id;
    while(stmts != NULL){
        stmt = stmts->first;
        switch(stmt.type){
            case Print:
                id = find_id(stmt.stmt.variable);
                fprintf(target,"l%c\n", id);
                fprintf(target,"p\n");
                break;
            case Assignment:
                fprint_expr(target, stmt.stmt.assign.expr);
                /*
                   if(stmt.stmt.assign.type == Int){
                   fprintf(target,"0 k\n");
                   }
                   else if(stmt.stmt.assign.type == Float){
                   fprintf(target,"5 k\n");
                   }*/
                id = find_id(stmt.stmt.variable);
                fprintf(target,"s%c\n", id);
                fprintf(target,"0 k\n");
                break;
        }
        stmts=stmts->rest;
    }

}


/***************************************
  For our debug,
  you can omit them.
 ****************************************/
void print_expr(Expression *expr)
{
    if(expr == NULL)
        return;
    else{
        print_expr(expr->leftOperand);
        switch((expr->v).type){
            case Identifier:
                printf("%c ", (expr->v).val.id);
                break;
            case IntConst:
                printf("%d ", (expr->v).val.ivalue);
                break;
            case FloatConst:
                printf("%f ", (expr->v).val.fvalue);
                break;
            case PlusNode:
                printf("+ ");
                break;
            case MinusNode:
                printf("- ");
                break;
            case MulNode:
                printf("* ");
                break;
            case DivNode:
                printf("/ ");
                break;
            case IntToFloatConvertNode:
                printf("(float) ");
                break;
            default:
                printf("error ");
                break;
        }
        print_expr(expr->rightOperand);
    }
}

void test_parser( FILE *source )
{
    Declarations *decls;
    Statements *stmts;
    Declaration decl;
    Statement stmt;
    Program program = parser(source);

    decls = program.declarations;

    while(decls != NULL){
        decl = decls->first;
        if(decl.type == Int)
            printf("i ");
        if(decl.type == Float)
            printf("f ");
        printf("%c ",decl.name);
        decls = decls->rest;
    }

    stmts = program.statements;

    while(stmts != NULL){
        stmt = stmts->first;
        if(stmt.type == Print){
            printf("p %c ", stmt.stmt.variable);
        }

        if(stmt.type == Assignment){
            printf("%c = ", stmt.stmt.assign.id);
            print_expr(stmt.stmt.assign.expr);
        }
        stmts = stmts->rest;
    }

}

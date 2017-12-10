%option noyywrap 
%{
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
int linenumber, identifier_num;
symtab * lookup();
symtab * ptr;
void     insertID();
void 	 printSymTab();
int 	 is_reserved();
void 	 printIdentifiers();
int 	 main();
/* You need to define for all tokens in C--, here are some examples */
#define INT		10
#define FLOAT 		11 
#define OP_ASSIGN 	12 
#define OP_OR 		13 
#define MK_LPAREN 	14 
#define MK_RPAREN 	15 
#define ERROR 		100 

%}

letter   		[A-Za-z]
digit    		[0-9]
ID	 			{letter}({letter}|{digit}|"_")*
WS	 			[ \t]+
Int_const 		{digit}+
Float_const 	{digit}*\.?{digit}+([eE][-+]?[0-9]+)?
String_const 	\"[^"]*\"
comment			\/\*([^*]|(\*+([^*/])))*\*+\/


/* You need to define the following RE's 
Float_constant
String_constant
comment
*/

OP_assign       "="
OP_or   		"||"
OP_and			"&&"
OP_not			"!"

OP_plus			"+"
OP_minus		"-"
OP_mul			"*"
OP_div			"/"

OP_less			"<"
OP_greater		">"
OP_lessequal	"<="
OP_greaterequal	">="
OP_equal 		"=="
OP_notequal		"!="


/* Other operators appear here */ 


newline  "\n"

DL_lbracket "["
DL_rbracket "]"
DL_lparen "("
DL_rparen ")"
DL_lbrace "{"
DL_rbrace "}"
DL_comma  ","
DL_semicol ";"
DL_dot  "."

/* Other separators appear here */

error    .

%%

{WS}            {}
{comment}		printf("%s\n", yytext);
{Int_const}		{}
{Float_const}   {}
{String_const}	{}
{ID}            { 
					if(is_reserved(yytext))
						;//printf("reserved word %s\n", yytext);
					else{
						//printf("identifier %s\n", yytext);
						ptr = lookup(yytext);
						if (ptr == NULL)
						     insertID(yytext);	
						else 
						     ptr->counter++;
					}
				}



{OP_assign}     	{}
{OP_or}         	{}
{OP_and}			{}
{OP_not}			{}
{OP_plus}			{}
{OP_minus}			{}
{OP_mul}			{}
{OP_div}			{}
{OP_less}			{}
{OP_greater}		{}
{OP_lessequal}		{}
{OP_greaterequal}	{}
{OP_equal}			{}
{OP_notequal}		{}

{DL_lbracket} {}
{DL_rbracket} {}
{DL_lparen}     {}
{DL_rparen}     {}
{DL_lbrace}     {}
{DL_rbrace}     {}
{DL_comma}      {}
{DL_semicol}    {}
{newline}       linenumber += 1;
{error}         printf("ERR %s\n", yytext);


%%

int main(int argc, char **argv)
{
    argc--; ++argv;
    if (argc > 0)
    	yyin = fopen(argv[0], "r");
    else
    	yyin = stdin;
    yylex();
    //printSymTab();
    printIdentifiers();
}
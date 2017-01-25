%token IDENTIFIER I_CONSTANT F_CONSTANT STRING_LITERAL
%token IHASA WEHASA
%token ITZA ITZSRSLYA ITZLOTZA ITZSRSLYLOTZA 
%token TROOF NUMBR NUMBAR YARN BUKKIT NOOB
%token TROOFS NUMBRS NUMBARS YARNS BUKKITS NOOBS
%token WIN FAIL
%token ITZ THARIZ
%token R AN MKAY
%token SUMOF DIFFOF PRODUKTOF QUOSHUNTOF MODOF BIGGROF SMALLROF
%token FLIPOF SQUAROF UNSQUAROF
%token BOTHOF EITHEROF WONOF NOT ALLOF ANYOF
%token BOTHSAEM DIFFRINT
%token SMOOSH MAEK APOSTROPHEZ
%token VISIBLE GIMMEH
%token ORLY YARLY MEBBE NOWAI OCI
%token WTF OMG OMGWTF
%token IMINYR UPPIN NERFIN YR TIL WILE
%token HOWIZI IFUSAYSO FOUNDYR GTFO IIZ
%token MAHFRENZ ME HUGZ TXTMAHBFF ANSTUFF TTYL UR MAH IMSHARINIT
%token IMMESINWIF IMSRSLYMESINWIF DUNMESINWIF

%token EOL ELLIPSIS BANG

%{

#include "compiler.h"
#include "compiler_node.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void yyerror(const char*);

%}

%start program;

%union {
   int ival;
   float fval;
   node_t* node;
}

%type <ival> IDENTIFIER I_CONSTANT F_CONSTANT STRING_LITERAL
%type <ival> IHASA WEHASA
%type <ival> ITZA ITZSRSLYA ITZLOTZA ITZSRSLYLOTZA 
%type <ival> TROOF NUMBR NUMBAR YARN BUKKIT NOOB
%type <ival> TROOFS NUMBRS NUMBARS YARNS BUKKITS NOOBS
%type <ival> WIN FAIL
%type <ival> ITZ THARIZ
%type <ival> R AN MKAY
%type <ival> SUMOF DIFFOF PRODUKTOF QUOSHUNTOF MODOF BIGGROF SMALLROF
%type <ival> FLIPOF SQUAROF UNSQUAROF
%type <ival> BOTHOF EITHEROF WONOF NOT ALLOF ANYOF
%type <ival> BOTHSAEM DIFFRINT
%type <ival> SMOOSH MAEK APOSTROPHEZ
%type <ival> VISIBLE GIMMEH
%type <ival> ORLY YARLY MEBBE NOWAI OCI
%type <ival> WTF OMG OMGWTF
%type <ival> IMINYR UPPIN NERFIN YR TIL WILE
%type <ival> HOWIZI IFUSAYSO FOUNDYR GTFO IIZ
%type <ival> MAHFRENZ ME HUGZ TXTMAHBFF ANSTUFF TTYL UR MAH IMSHARINIT
%type <ival> IMMESINWIF IMSRSLYMESINWIF DUNMESINWIF

%type <ival> EOL ELLIPSIS BANG

%type <node> program;
%type <node> block statement
%type <node> declaration_statement
%type <node> declaration
%type <node> expression_statement
%type <node> expression
%type <node> primary_expression target
%type <ival> unary_op unary_arithmetic_op unary_logical_op
%type <ival> binary_op binary_arithmetic_op binary_logical_op
%type <ival> relational_op
%type <ival> typespec type types init
%type <node> assignment_statement
%type <node> print_statement print

%%

program
	: block { cur_nptr=node_create_program($1);  printf("program\n"); }
	;

block
	: statement { $$=node_create_block($1);  printf("<block>\n"); }
	| block statement { $$=node_block_add_stmt($1,$2); printf("<block-add>\n"); }
	;

statement
	: declaration_statement { printf("statement\n"); }
	| expression_statement 
	| assignment_statement 
	| print_statement 
	;

declaration_statement
	: declaration EOL { $$=$1; }
	;

declaration
	: IHASA IDENTIFIER 
		{ $$=node_create_decl_stmt($2,0,0,0,0); printf("here 1\n"); }
	| IHASA IDENTIFIER typespec
		{ $$=node_create_decl_stmt($2,$3,0,0,0); printf("here 2\n"); }
	| IHASA IDENTIFIER init
		{ $$=node_create_decl_stmt($2,0,$3,0,0); printf("here 3\n"); }
	| IHASA IDENTIFIER IMSHARINIT
		{ $$=node_create_decl_stmt($2,0,0,0,1); printf("here 4\n"); }
	| WEHASA IDENTIFIER 
		{ $$=node_create_decl_stmt($2,0,0,1,0); printf("here 11\n"); }
	| WEHASA IDENTIFIER typespec
		{ $$=node_create_decl_stmt($2,$3,0,1,0); printf("here 12\n"); }
	| WEHASA IDENTIFIER init
		{ $$=node_create_decl_stmt($2,0,$3,1,0); printf("here 13\n"); }
	| WEHASA IDENTIFIER IMSHARINIT
		{ $$=node_create_decl_stmt($2,0,0,1,1); printf("here 14\n"); }
	| declaration AN typespec 
		{ $$=node_update_decl_stmt($1,$3,0,0,0); printf("here 5\n"); }
	| declaration AN init 
		{ $$=node_update_decl_stmt($1,0,$3,0,0); printf("here 6\n"); }
	| declaration AN IMSHARINIT 
		{ $$=node_update_decl_stmt($1,0,0,0,1); printf("here 7\n"); }
	;

typespec
	: ITZA type { $$=$2; }
	| ITZSRSLYA type { $$=$2; }
	| ITZLOTZA types { $$=$2; }
	| ITZSRSLYLOTZA types { $$=$2; }
	;

type
	: NOOB { $$=T_VOID; printf("T_VOID_ARRAY\n"); }
	| TROOF { $$=T_BOOLEAN; printf("T_BOOLEAN\n"); }
	| NUMBR { $$=T_INTEGER; printf("T_INTGEGER\n"); }
	| NUMBAR { $$=T_FLOAT; printf("T_FLOAT\n"); }
	| YARN { $$=T_STRING; printf("T_STRING\n"); }
	| BUKKIT { $$=T_STRUCT; printf("T_STRUCT\n"); }
	;

types
	: NOOBS { $$=T_VOID_ARRAY; printf("T_VOID_ARRAY\n"); }
	| TROOFS { $$=T_BOOLEAN_ARRAY; printf("T_BOOLEAN_ARRAY\n"); }
	| NUMBRS { $$=T_INTEGER_ARRAY; printf("T_INTEGER_ARRAY\n"); }
	| NUMBARS { $$=T_FLOAT_ARRAY; printf("T_FLOAT_ARRAY\n"); }
	| YARNS { $$=T_STRING_ARRAY; printf("T_STRING_ARRAY\n"); }
	| BUKKITS { $$=T_STRUCT_ARRAY; printf("T_STRUCT_ARRAY\n"); }
	;

init
	: ITZ expression { $$=$2; }
	| THARIZ expression { $$=$2; }
	;

expression_statement
	: expression EOL { $$=node_create_expr_stmt($1); }
	;

expression
   : primary_expression 
	| unary_op expression { $$=node_create_expr1($1,$2); }
	| binary_op expression AN expression { $$=node_create_expr2($1,$2,$4); }
	| relational_op expression AN expression { $$=node_create_expr2($1,$2,$4); }
   ;

primary_expression
   : target { $$=$1; }
	| I_CONSTANT { $$=node_create_integer($1); printf("I_CONSTANT %d\n",$1); }
	| F_CONSTANT { $$=node_create_float($1); }
	| STRING_LITERAL { $$=node_create_string($1); }
   ;

target
	: IDENTIFIER { $$=node_create_identifier($1,0); }
	;

unary_op
   : unary_arithmetic_op
   | unary_logical_op
   ;

unary_arithmetic_op
   : FLIPOF { $$=OP_INV; }
   | SQUAROF { $$=OP_POW2; }
   | UNSQUAROF { $$=OP_SQRT; }
   ;

unary_logical_op
   : NOT { $$=OP_NOT; }
   ;

binary_op
   : binary_arithmetic_op
   | binary_logical_op
   ;

binary_arithmetic_op
   : SUMOF { $$=OP_ADD; }
   | DIFFOF { $$=OP_SUB; }
   | PRODUKTOF { $$=OP_MUL; }
   | QUOSHUNTOF { $$=OP_DIV; }
   | MODOF { $$=OP_MOD; }
   | BIGGROF { $$=OP_MAX; }
   | SMALLROF { $$=OP_MIN; }
   ;

binary_logical_op
   : BOTHOF { $$=OP_AND; }
   | EITHEROF { $$=OP_OR; }
   | WONOF { $$=OP_XOR; }
   ;


relational_op
   : BOTHSAEM { $$=OP_EQ; }
   | DIFFRINT { $$=OP_NEQ; }
   ;

assignment_statement
	: target R expression EOL { $$=node_create_assign_stmt($1,$3); }
	;

print_statement
	: print EOL
	;

print
	: VISIBLE primary_expression { $$=node_create_print_stmt($2); }
	| print primary_expression { $$=node_update_print_stmt($1,$2); }
	;

%%
#include <stdio.h>

void yyerror(const char *s)
{
	fflush(stdout);
	fprintf(stderr, "*** %s\n", s);
}


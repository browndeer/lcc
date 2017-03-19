/* compiler_gram.y
 *
 * Copyright (c) 2017 Brown Deer Technology, LLC.  All Rights Reserved.
 *
 * This software was developed by Brown Deer Technology, LLC.
 * For more information contact info@browndeertechnology.com
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 (GPLv3)
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* DAR */

%token HAI KTHXBYE
%token CANHAS
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
%token ORLY YARLY MEBBE NOWAI OIC
%token WTF OMG OMGWTF
%token IMINYR UPPIN NERFIN YR TIL WILE IMOUTTAYR
%token HOWIZI IFUSAYSO FOUNDYR GTFO IIZ
%token WHATEVR WHATEVAR
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
int yylex(void);

%}

%start program;

%union {
   int ival;
   float fval;
   node_t* node;
}

%type <ival> HAI KTHXBYE
%type <ival> CANHAS
%type <ival> IDENTIFIER I_CONSTANT STRING_LITERAL
%type <fval> F_CONSTANT 
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
%type <ival> ORLY YARLY MEBBE NOWAI OIC
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
%type <ival> typespec type types 
%type <node> init
%type <node> assignment_statement
%type <node> print_statement print
%type <node> input_statement input
%type <node> if_statement
%type <node> if_block else_block elsif_clauses elsif_clause
%type <node> switch_statement case_statement
%type <node> break_statement
%type <node> for_statement while_statement
%type <ival> iter
%type <node> barrier_statement
%type <node> remote_statement
%type <node> lock_statement

%type <ival> includes

%%

program
	: HAI expression EOL block KTHXBYE EOL 
		{ cur_nptr=node_create_program($4); }
	| HAI expression EOL includes block KTHXBYE EOL 
		{ cur_nptr=node_create_program($5); }
	;

includes
	: CANHAS IDENTIFIER '?' EOL
	| includes CANHAS IDENTIFIER '?' EOL
	;

block
	: statement { $$=node_create_block($1); }
	| block statement { $$=node_block_add_stmt($1,$2); }
	;

statement
	: declaration_statement 
	| expression_statement 
	| assignment_statement 
	| print_statement 
	| input_statement 
	| if_statement 
	| switch_statement 
	| case_statement 
	| break_statement 
	| for_statement 
	| while_statement 
	| barrier_statement 
	| remote_statement 
	| lock_statement
	;

declaration_statement
	: declaration EOL { $$=$1; }
	;

declaration
	: IHASA IDENTIFIER 
		{ $$=node_create_decl_stmt($2,0,0,0,0); }
	| IHASA IDENTIFIER typespec
		{ $$=node_create_decl_stmt($2,$3,0,0,0); }
	| IHASA IDENTIFIER init
		{ $$=node_create_decl_stmt($2,0,$3,0,0); }
	| IHASA IDENTIFIER IMSHARINIT
		{ $$=node_create_decl_stmt($2,0,0,0,1); }
	| WEHASA IDENTIFIER 
		{ $$=node_create_decl_stmt($2,0,0,1,0); }
	| WEHASA IDENTIFIER typespec
		{ $$=node_create_decl_stmt($2,$3,0,1,0); }
	| WEHASA IDENTIFIER init
		{ $$=node_create_decl_stmt($2,0,$3,1,0); }
	| WEHASA IDENTIFIER IMSHARINIT
		{ $$=node_create_decl_stmt($2,0,0,1,1); }
	| declaration AN typespec 
		{ $$=node_update_decl_stmt($1,$3,0,0,0); }
	| declaration AN init 
		{ $$=node_update_decl_stmt($1,0,$3,0,0); }
	| declaration AN IMSHARINIT 
		{ $$=node_update_decl_stmt($1,0,0,0,1); }
	;

typespec
	: ITZA type { $$=$2; }
	| ITZSRSLYA type { $$=$2; }
	| ITZLOTZA types { $$=$2; }
	| ITZSRSLYLOTZA types { $$=$2; }
	;

type
	: NOOB { $$=T_VOID; }
	| TROOF { $$=T_BOOLEAN; }
	| NUMBR { $$=T_INTEGER; }
	| NUMBAR { $$=T_FLOAT; }
	| YARN { $$=T_STRING; }
	| BUKKIT { $$=T_STRUCT; }
	;

types
	: NOOBS { $$=T_VOID_ARRAY; }
	| TROOFS { $$=T_BOOLEAN_ARRAY; }
	| NUMBRS { $$=T_INTEGER_ARRAY; }
	| NUMBARS { $$=T_FLOAT_ARRAY; }
	| YARNS { $$=T_STRING_ARRAY; }
	| BUKKITS { $$=T_STRUCT_ARRAY; }
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
	| WHATEVR { $$=node_create_expr0(OP_RAND); }
	| WHATEVAR { $$=node_create_expr0(OP_RANDF); }
	| MAHFRENZ { $$=node_create_expr0(OP_SHMEM_NPES); }
	| ME { $$=node_create_expr0(OP_SHMEM_PE); }
	| unary_op expression { $$=node_create_expr1($1,$2); }
	| binary_op expression AN expression { $$=node_create_expr2($1,$2,$4); }
	| relational_op expression AN expression { $$=node_create_expr2($1,$2,$4); }
   ;

primary_expression
   : target { $$=$1; }
	| I_CONSTANT { $$=node_create_integer($1); }
	| F_CONSTANT { $$=node_create_float($1); }
	| STRING_LITERAL { $$=node_create_string($1); }
   ;

target
	: IDENTIFIER { $$=node_create_identifier($1,0,0,0); }
	| target APOSTROPHEZ expression { $$=node_update_identifier($1,0,$3,0); }
	| UR target 
		{ $$=node_update_identifier($2,0,0,1); }
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
	: expression R expression EOL { $$=node_create_assign_stmt($1,$3); }
	;

print_statement
	: print EOL
	;

print
	: VISIBLE expression { $$=node_create_print_stmt($2); }
	| print expression { $$=node_update_print_stmt($1,$2); }
	;

input_statement
	: input EOL
	;

input
	: GIMMEH target { $$=node_create_input_stmt($2); }
	| input target { $$=node_update_input_stmt($1,$2); }
	;

if_statement
	: ORLY EOL if_block OIC EOL
		{ $$=node_create_if_stmt($3,0,0); }
	| ORLY EOL if_block else_block OIC EOL
		{ $$=node_create_if_stmt($3,0,$4); }
	| ORLY EOL if_block elsif_clauses OIC EOL
		{ $$=node_create_if_stmt($3,$4,0); }
	| ORLY EOL if_block elsif_clauses else_block OIC EOL
		{ $$=node_create_if_stmt($3,$4,$5); }
	| ORLY EOL elsif_clauses else_block OIC EOL
		{ $$=node_create_if_stmt(0,$3,$4); }
	| ORLY EOL else_block OIC EOL
		{ $$=node_create_if_stmt(0,0,$3); }
	;

if_block
	: YARLY EOL block { $$=$3; }
	;

else_block
	: NOWAI EOL block { $$=$3; }
	;

elsif_clauses
	: elsif_clause { $$=$1; }
	| elsif_clauses elsif_clause { $$=node_insert_tail($1,$2); }
	;

elsif_clause
	: MEBBE expression EOL block { $$=node_create_elsif_clause($2,$4); }
	;

switch_statement
	: WTF EOL block OIC EOL { $$=node_create_switch_stmt($3); }
	;

case_statement
	: OMG expression EOL { $$=node_create_case_stmt($2); }
	| OMGWTF EOL { $$=node_create_case_stmt(0); }
	;

break_statement
	: GTFO EOL { $$=node_create_break_stmt(); }
	;

for_statement
	: IMINYR IDENTIFIER iter YR IDENTIFIER TIL expression EOL block IMOUTTAYR IDENTIFIER EOL
		{ $$=node_create_for_stmt($2,$3,$5,$7,$9); }
	;

while_statement
	: IMINYR IDENTIFIER iter YR IDENTIFIER WILE expression EOL block IMOUTTAYR IDENTIFIER EOL
		{ $$=node_create_while_stmt($2,$3,$5,$7,$9); }
	;

iter
	: UPPIN { $$=+1; }
	| NERFIN { $$=-1; }
	;

barrier_statement
	: HUGZ EOL { $$=node_create_barrier_stmt(); }
	;

remote_statement
	: TXTMAHBFF expression EOL { $$=node_create_remote_stmt($2,0); }
	| TXTMAHBFF expression ANSTUFF EOL { $$=node_create_remote_stmt($2,1); }
	| TTYL EOL { $$=node_create_remote_stmt(0,0); }
	;

lock_statement
	: IMMESINWIF target EOL { $$=node_create_lock_stmt($2,L_TRYLOCK); }
	| IMSRSLYMESINWIF target EOL { $$=node_create_lock_stmt($2,L_LOCK); }
	| DUNMESINWIF target EOL { $$=node_create_lock_stmt($2,L_UNLOCK); }
	;


%%
#include <stdio.h>

extern int __linenum;

void yyerror(const char *s)
{
	fflush(stdout);
	fprintf(stderr, "*** %d: %s\n", __linenum,s);
}


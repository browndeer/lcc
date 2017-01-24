%token IDENTIFIER I_CONSTANT F_CONSTANT STRING_LITERAL
%token IHASA WEHASA
%token ITZA ITZSRSLYA ITZLOTZA ITZSRSLYLOTZA
%token TROOF NUMBR NUMBAR YARN BUKKIT NOOB
%token TROOFS NUMBRS NUMBARS YARNS BUKKITS NOOBS
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

%start program;

%%

program
	: block
	;

block
	: statement
	| block statement
	;

statement
	: declaration_statement
	| expression_statement
	;

declaration_statement
	: IHASA IDENTIFIER EOL
	;


expression_statement
	: expression EOL
	;

expression
   : primary_expression
	| unary_op expression
	| binary_op expression AN expression
	| relational_op expression AN expression
   ;

primary_expression
   : IDENTIFIER
	| constant
	| string
   ;

constant
   : I_CONSTANT
   | F_CONSTANT
   ;

string
   : STRING_LITERAL
   ;

unary_op
   : unary_arithmetic_op
   | unary_logical_op
   ;

unary_arithmetic_op
   : FLIPOF
   | SQUAROF
   | UNSQUAROF
   ;

unary_logical_op
   : NOT
   ;

binary_op
   : binary_arithmetic_op
   | binary_logical_op
   ;

binary_arithmetic_op
   : SUMOF
   | DIFFOF
   | PRODUKTOF
   | QUOSHUNTOF
   | MODOF
   | BIGGROF
   | SMALLROF
   ;

binary_logical_op
   : BOTHOF
   | EITHEROF
   | WONOF
   ;


relational_op
   : BOTHSAEM
   | DIFFRINT
   ;

%%
#include <stdio.h>

void yyerror(const char *s)
{
	fflush(stdout);
	fprintf(stderr, "*** %s\n", s);
}


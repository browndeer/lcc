/* compiler_scan.l
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

%e number of parsed tree nodes  
%p number of positions  
%n number of states  
%k number of packed character classes  
%a number of transitions  
%o size of output array 

%e  1019
%p  2807
%n  371
%k  284
%a  1213
%o  1117

O   [0-7]
D   [0-9]
NZ  [-1-9]
L   [a-zA-Z]
A   [a-zA-Z_0-9]
H   [a-fA-F0-9]
HP  (0[xX])
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
CP  (u|U|L)
SP  (u8|u|U|L)
ES  (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS  [ \t\v\f]
BLANK ^[ \t\v\f]*\n

%{

#include <stdio.h>
#include "compiler.h"
#include "compiler_node.h"
#include "compiler_gram.h"

extern void __compiler_error(const char *);  /* prints grammar violation message */
extern int __compiler_lex(void);

extern int sym_type(const char *);  /* returns type from symbol table */

#define sym_type(identifier) IDENTIFIER /* with no symbol table, fake it */

static void comment(void);
static int check_type(void);
static void eol(void);

int __linenum = 1;

%}

%%

^[ ]*\n		{ ++__linenum; }

"OBTW"{WS}*"\n"      { ++__linenum; comment(); }
"BTW"{WS}+.*"\n"             { ++__linenum; /* consume //-comment */ }

"HAI"						{ return(HAI); }
"KTHXBYE"				{ return(KTHXBYE); }

"CAN HAS"				{ return(CANHAS); }

"I HAS A"				{ return(IHASA); }
"WE HAS A"				{ return(WEHASA); }

"ITZ"						{ return(ITZ); }
"THAR IZ"				{ return(THARIZ); }

"ITZ A"					{ return(ITZA); }
"ITZ SRSLY A"			{ return(ITZSRSLYA); }
"ITZ LOTZ A"			{ return(ITZLOTZA); }
"ITZ SRSLY LOTZ A"	{ return(ITZSRSLYLOTZA); }

"TROOF"					{ return(TROOF); }
"NUMBR"					{ return(NUMBR); }
"NUMBAR"					{ return(NUMBAR); }
"YARN"					{ return(YARN); }
"BUKKIT"					{ return(BUKKIT); }
"NOOB"					{ return(NOOB); }
"TROOFS"					{ return(TROOFS); }
"NUMBRS"					{ return(NUMBRS); }
"NUMBARS"				{ return(NUMBARS); }
"YARNS"					{ return(YARNS); }
"BUKKITS"				{ return(BUKKITS); }
"NOOBS"					{ return(NOOBS); }

"WIN"						{ __compiler_lval.ival = 1; return I_CONSTANT; }
"FAIL"					{ __compiler_lval.ival = 0; return I_CONSTANT; }

"R"						{ return(R); }
"AN"						{ return(AN); }
"MKAY"					{ return(MKAY); }

"SUM OF"					{ return(SUMOF); }
"DIFF OF"				{ return(DIFFOF); }
"PRODUKT OF"			{ return(PRODUKTOF); }
"QUOSHUNT OF"			{ return(QUOSHUNTOF); }
"MOD OF"					{ return(MODOF); }
"BIGGR OF"				{ return(BIGGROF); }
"SMALLR OF"				{ return(SMALLROF); }
"FLIP OF"				{ return(FLIPOF); }
"SQUAR OF"				{ return(SQUAROF); }
"UNSQUAR OF"			{ return(UNSQUAROF); }
"POWR OF"				{ return(POWROF); }
"ROOT OF"				{ return(ROOTOF); }

"BOTH OF"				{ return(BOTHOF); }
"EITHER OF"				{ return(EITHEROF); }
"WON OF"					{ return(WONOF); }
"NOT"						{ return(NOT); }
"ALL OF"					{ return(ALLOF); }
"ANY OF"					{ return(ANYOF); }

"BOTH SAEM"				{ return(BOTHSAEM); }
"DIFFRINT"				{ return(DIFFRINT); }

"SMOOSH"      		   { return(SMOOSH); }
"MAEK"					{ return(MAEK); }
"'Z"						{ return(APOSTROPHEZ); }

"VISIBLE"				{ return(VISIBLE); }
"GIMMEH"					{ return(GIMMEH); }

"O RLY?"					{ return(ORLY); }
"YA RLY"					{ return(YARLY); }
"MEBBE"					{ return(MEBBE); }
"NO WAI"					{ return(NOWAI); }
"OIC"						{ return(OIC); }

"WTF?"					{ return(WTF); }
"OMG"						{ return(OMG); }
"OMGWTF"					{ return(OMGWTF); }

"IM IN YR"				{ return(IMINYR); }
"UPPIN"					{ return(UPPIN); }
"NERFIN"					{ return(NERFIN); }
"YR"						{ return(YR); }
"TIL"						{ return(TIL); }
"WILE"					{ return(WILE); }
"IM OUTTA YR"			{ return(IMOUTTAYR); }
"IM IN YR LOOP"		{ return(IMINYRLOOP); }
"IM OUTTA YR LOOP"	{ return(IMOUTTAYRLOOP); }
"FRUM"					{ return(FRUM); }
"BY"						{ return(BY); }

"HOW IZ I"				{ return(HOWIZI); }
"IF U SAY SO"			{ return(IFUSAYSO); }
"FOUND YR"				{ return(FOUNDYR); }
"GTFO"					{ return(GTFO); }
"I IZ"					{ return(IIZ); }

"WHATEVR"				{ return(WHATEVR); }
"WHATEVAR"				{ return(WHATEVAR); }

"MAH FRENZ"				{ return(MAHFRENZ); }
"ME"						{ return(ME); }
"HUGZ"					{ return(HUGZ); }
"TXT MAH BFF"			{ return(TXTMAHBFF); }
"AN STUFF"				{ return(ANSTUFF); }
"TTYL"					{ return(TTYL); }
"UR"						{ return(UR); }
"MAH"						{ return(MAH); }
"IM SHARIN IT"			{ return(IMSHARINIT); }
"IM MESIN WIF"			{ return(IMMESINWIF); }
"IM SRSLY MESIN WIF"	{ return(IMSRSLYMESINWIF); }
"DUN MESIN WIF"		{ return(DUNMESINWIF); }

{L}{A}*					{ 
								__compiler_lval.ival 
									= add_str(symbuf,&symbuf_sz,yytext);
                        return(IDENTIFIER);
							}

{HP}{H}+{IS}?				{ 
									__compiler_lval.ival = atoi(yytext); 
									return I_CONSTANT;
								}

{NZ}{D}*{IS}?				{ 
									__compiler_lval.ival = atoi(yytext); 
									return I_CONSTANT;
								}

"0"{O}*{IS}?				{ 
									__compiler_lval.ival = atoi(yytext); 
									return I_CONSTANT;
								}

{D}+{E}{FS}?				{ 
									__compiler_lval.fval = atof(yytext); 
									return F_CONSTANT;
								}

{D}*"."{D}+{E}?{FS}?		{ 
									__compiler_lval.fval = atof(yytext); 
									return F_CONSTANT;
								}

{D}+"."{E}?{FS}?			{
									__compiler_lval.fval = atof(yytext); 
									return F_CONSTANT;
								}

{HP}{H}+{P}{FS}?			{
									__compiler_lval.fval = atof(yytext); 
									return F_CONSTANT;
								}

{HP}{H}*"."{H}+{P}{FS}?			{
											__compiler_lval.fval = atof(yytext); 
											return F_CONSTANT;
										}

{HP}{H}+"."{P}{FS}?			{
										__compiler_lval.fval = atof(yytext); 
										return F_CONSTANT;
									}

({SP}?\"([^"\\\n]|{ES})*\"{WS}*)+	{
													__compiler_lval.ival 
														= add_str(symbuf,&symbuf_sz,yytext);
													return STRING_LITERAL;
												}

"..."{WS}*"\n"				{ ++__linenum; }
"\n"				{ ++__linenum; eol(); return EOL; }
","				{ eol(); return EOL; }
"!"				{ return BANG; }
"?"				{ return '?'; }

{WS}+					{ /* whitespace separates tokens */ }
.					{ /* discard bad characters */ }

%%

int yywrap(void)        /* called at end of input */
{
    return 1;           /* terminate now */
}

static void comment(void)
{

    int c;
	int count = 0;
	int newline = 0;

    while ((c = input()) != 0) {

		if (c==' ' || c=='\t') continue;

		switch (count) {

			default:
				if (newline && c=='T') ++count;
				else count = 0;
				break;

			case 1:
				if (c=='L') ++count;
				else count = 0;
				break;

			case 2:
				if (c=='D') ++count;
				else count = 0;
				break;

			case 3:
				if (c=='R') ++count;
				else count = 0;
				break;

			case 4:
				if (c=='\n') { ++__linenum; return; }
				else count = 0;
				break;

		}

		if (c=='\n') { ++__linenum; newline=1; }

	}

    __compiler_error("unterminated comment");
}

static int check_type(void)
{
	return IDENTIFIER;
}


static void eol(void)
{

   int c;
	int count = 0;
	int newline = 0;

   while ((c = input()) != 0) {

		if (c == ' ') continue;
		if (c == '\t') continue;
		if (c == '\n') { ++__linenum; continue; }
		if (c == ',') continue;

		unput(c);

		return;		
		
	}

}


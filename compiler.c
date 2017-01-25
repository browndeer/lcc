/* lc2c.c
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

#define COMPILER_VERSION_STRING "0.7"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "compiler_node.h"
#include "compiler_gram.h"

/*
struct type_entry {
	int ival;
	size_t sz;
	char* c_type;
	char* cl_type;
};

struct type_entry type_table[] = {
	{ TYPEID_OPAQUE, 8, "void", "void" },
	{ TYPEID_VOID, 0, "void", "void" },
	{ TYPEID_CHAR, 1, "char", "char" },
	{ TYPEID_SHORT, 2, "short", "short" },
	{ TYPEID_INT, 4, "int", "int" },
	{ TYPEID_LONG, 8, "long", "long" },
	{ TYPEID_UCHAR, 1, "unsigned char", "uchar" },
	{ TYPEID_USHORT, 2, "unsigned short", "ushort" },
	{ TYPEID_UINT, 4, "unsigned int", "uint" },
	{ TYPEID_ULONG, 8, "unsigned long", "ulong" },
	{ TYPEID_FLOAT, 4, "float", "float" },
	{ TYPEID_DOUBLE, 8, "double", "double" },
};

int ntypes = sizeof(type_table)/sizeof(struct type_entry);
*/

char* symbuf = 0;
int symbuf_sz = 0;
node_t* cur_nptr;

char* typbuf[16384];
int ntypbuf = 0;

char* locbuf;
size_t locbufsz;


int 
add_str( char* buf, int* sz, const char* s)
{
	int i;
	if (STRBUFSIZE-(*sz)-1 < strnlen(s,STRBUFSIZE-1)) return(0);
	for(i=0; i< (*sz); i+=strnlen(buf+i,STRBUFSIZE-i-1)+1) 
		if (!strncmp(buf+i,s,STRBUFSIZE-i)) return(i);
	strncpy(buf+i,s,STRBUFSIZE-i);
	(*sz) += strnlen(s,STRBUFSIZE-i)+1;
	return(i);
}


int
add_typedef( char* sym )
{ typbuf[ntypbuf++] = sym; }

int
is_type(char* s)
{
	int i;
	for(i=0;i<ntypbuf;i++) 
		if (!strncmp(typbuf[i],s,256)) return(1);
	return(0);
}



void fprintf_sym(FILE*, char*, int ll_style, int c_style);
void fprintf_type( FILE*, node_t* arg ); 


void cgen_program( FILE* fp, node_t* root );


//int type_ival2index(int ival);

extern FILE* __compiler_in;

int main( int argc, char** argv)
{
	int i,j,k;
	char sbuf[1024];

	char* in_filename = 0;
	char* out_filename = 0;

	int opt_debug = 0;
	int opt_verbose = 0;
	int opt_version = 0;
	int opt_help = 0;

	int n = 1;
	while(n<argc) {

		char* arg = argv[n++];

		if (!strncmp(arg,"-v",2)) opt_verbose = 1;

		else if (!strncmp(arg,"-q",2)) opt_verbose = 0;

		else if (!strncmp(arg,"--debug",7)) {
			opt_debug = 1;
		}

		else if ( !strncmp(arg,"--version",9)
			|| !strncmp(arg,"-V",2) ) opt_version=1;
		
		else if ( !strncmp(arg,"--help",6)
			|| !strncmp(arg,"-h",2) ) opt_help=1;
		
		else if ( !strncmp(arg,"-o",2)) {
			if (out_filename) {
				fprintf(stderr,"compiler: error: multiple output files\n");
				opt_help = 1;
			} else {
				out_filename = argv[n++];
			}
		}
		
		else if (in_filename) {
			fprintf(stderr,"compiler: error: multiple input files\n");
			opt_help = 1;
		}

		else {
			in_filename = arg;
		}

	}


	if (opt_version) {

		printf("lc2c ( version " COMPILER_VERSION_STRING " )\n" );
		printf("Copyright (c) 2017 Brown Deer Technology, LLC.\n");
//		printf( GPL3_NOTICE );

		exit(0);

	} else if (opt_help) {

		printf("USAGE: lc2c [options] <input .lol file>\n");

		printf("-v\t\t\tVerbose\n");
		printf("-q\t\t\tQuiet\n");
		printf("--debug\t\t\tOutput lc2c debug information\n");
		printf("-V,--version\t\tOutput compiler version information\n");
		printf("-h,--help\t\tOutput this help information\n");
		printf("-o <output file>\tRedirect output to file\n");
		printf("\n");

		exit(0);

	}


	symbuf = (char*)malloc(STRBUFSIZE);
	symbuf[symbuf_sz++] = '\0';
	locbuf = (char*)malloc(262144);
	locbufsz = 0;

	node_t* root = node_create();
	cur_nptr = root;



	/*
	 * open input file, call yyparse
	 * if successful, yyparse will assign a tree representing 
	 * the parsed assembly code to root
	 */

	if (in_filename) {
		if (!strcmp(in_filename,"-")) __compiler_in = stdin;
		else __compiler_in = fopen(in_filename,"r");
	}

	if (!__compiler_in) fprintf(stderr,"compiler: no input file\n");

	__compiler_parse();

	root = cur_nptr;

//	if (opt_debug) {
//		node_fprintf(stdout,root,0);
//	}

	fclose(__compiler_in);

	

	/* 
	 *	open output file, use stdout if none specified
	 */

	FILE* fp = (out_filename)? fopen(out_filename,"w") : stdout;

	if (!fp) {
		fprintf(stderr,
			"compiler: error: open '%s' for output failed\n",out_filename);
		exit(-1);
	}


	node_t* nptr1;
	node_t* nptr2;
	node_t* nptr3;

	cgen_program(fp,root);

	fclose(fp);

}



//int type_ival2index(int ival) 
//{
//	int i;
//	for(i=0;i<ntypes;i++) if (type_table[i].ival == ival) return(i);
//	return(-1);
//}


void fprintf_sym( FILE* fp, char* sym, int ll_style, int c_style)
{
	char sbuf[1024];
	char* s = sym;

	if (!ll_style && (*s=='@' || *s=='%')) ++s;

	strncpy(sbuf,s,1024);

	if (c_style) {
		if (!strncmp(sbuf,"__OpenCL_",9)) strncpy(sbuf,sbuf+9,1024);
		size_t len = strnlen(sbuf,1024);
		if (!strncmp(sbuf+len-7,"_kernel",7)) sbuf[len-7]='\0';
	}

	fprintf(fp,"%s",sbuf);
}

/*
#define __error(msg) do { \
	fprintf(stderr,"error: " msg "\n");  \
	exit(-1); \
	} while(0)

void cgen_program( FILE* fp, node_t* root )
{
	fprintf(stderr,"%d\n",root->ntyp);

	if (root->ntyp != N_PROGRAM) __error("missing program node");

	fprintf(fp,"#include <lol.h>\n");
	fprintf(fp,"int main()\n");

//	cgen_block(root->n_program.block);

}
*/




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

#define COPYRIGHT_NOTICE "Copyright (c) 2017 Brown Deer Technology, LLC.\n"

#define GPL3_NOTICE "This program is free software; you may redistribute it" \
	" under the terms of the GNU General Public License version 3 (GPLv3)." \
	" This program has absolutely no warranty.\n"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "compiler_node.h"
#include "compiler_gram.h"

char* symbuf = 0;
int symbuf_sz = 0;
node_t* cur_nptr;

//char* typbuf[16384];
//int ntypbuf = 0;

//char* locbuf;
//size_t locbufsz;


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

void cgen_program( FILE* fp, node_t* root );

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
		printf( COPYRIGHT_NOTICE );
		printf( GPL3_NOTICE );

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
//	locbuf = (char*)malloc(262144);
//	locbufsz = 0;

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


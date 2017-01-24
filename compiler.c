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

	if (opt_debug) {
		node_fprintf(stdout,root,0);
	}

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


#if(0)
	/* 
	 * print count of symbols
	 */

	if (opt_n) {

		int count = 0;

		for(nptr1=root;nptr1;nptr1=nptr1->next) switch(nptr1->ntyp) {

			case NTYP_FUNC:

				if (opt_func_def && (nptr1->n_func.flags)&F_FUNC_DEF) ++count;
				if (opt_func_dec && (nptr1->n_func.flags)&F_FUNC_DEC) ++count;
				break;

			default: break;

		}

		fprintf(fp,"%d\n",count);
		exit(0);

	}


	/* 
	 * print nm-style table 
	 */

	if (opt_nm) {

		for(nptr1=root;nptr1!=0;nptr1=nptr1->next) switch(nptr1->ntyp) {

			case NTYP_EMPTY: break;

			case NTYP_FUNC:

				if ((opt_func_def && (nptr1->n_func.flags)&F_FUNC_DEF) 
					|| (opt_func_dec && (nptr1->n_func.flags)&F_FUNC_DEC)) {

					nptr2 = nptr1->n_func.args;

					n = node_count(nptr2);
				
					fprintf(fp,"%7d ",calc_args_sz(nptr2));

					if (nptr1->n_func.flags&F_FUNC_DEC) fprintf(fp,"f ");
					else if (nptr1->n_func.flags&F_FUNC_DEF) fprintf(fp,"F ");
					else fprintf(fp,"?");

					fprintf_sym(fp,
						symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

					fprintf(fp,"\t%d",n);

					for(i=0;i<n;i++,nptr2=nptr2->next) 
						fprintf(fp," %d",calc_arg_sz(nptr2));

					fprintf(fp,"\n");

				}

				break;

			default: break;

		}

		exit(0);

	}


	/* 
	 * generate call wrappers
	 */

	char* s;

	if (opt_kcall) {

		fprintf(fp,"#include <stdio.h>\n");
		fprintf(fp,"#include \"kcall.h\"\n");
//		fprintf(fp,"#define __xcl_kcall__\n");
//		fprintf(fp,"#include \"vcore2.h\"\n");

		for(nptr1=root;nptr1;nptr1=nptr1->next) switch(nptr1->ntyp) {

			case NTYP_EMPTY: break;

			case NTYP_FUNC:

				if (nptr1->n_func.flags & F_FUNC_DEF) {

					n = node_count(nptr1->n_func.args);

			
					nptr2=nptr1->n_func.args;

					fprintf(fp,"\n/* F ");

					fprintf_sym(fp,
						symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

					fprintf(fp," %d",calc_args_sz(nptr2));

					fprintf(fp,"\t%d",n);

					for(i=0;i<n;i++,nptr2=nptr2->next) 
						fprintf(fp," %d",calc_arg_sz(nptr2));

					fprintf(fp," */\n");


					/* print typedef */

					fprintf(fp,"typedef %s",type_table[type_ival2index(
						nptr1->n_func.rettype->n_type.datatype)].c_type);

					fprintf(fp,"(*__XCL_func_");

					fprintf_sym(fp,
						symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

					fprintf(fp,"_t)(");

					for(i=0,nptr2=nptr1->n_func.args;i<n;i++,nptr2=nptr2->next) {
						if (i>0) fprintf(fp,", ");	
						fprintf_cl_type(fp,nptr2->n_arg.argtype);
					}

					fprintf(fp,");\n");


					/* print call wrapper */

					fprintf(fp,"%s ",type_table[type_ival2index(
						nptr1->n_func.rettype->n_type.datatype)].c_type);

					fprintf(fp," __attribute__((noreturn))\n");

					fprintf(fp,"__XCL_call_");

					fprintf_sym(fp,
						symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

					fprintf(fp,"(void* p)");

					fprintf(fp,"{\n");

					if (opt_kcall_debug) {
						fprintf(fp,"\tprintf(\"__XCL_call_");
						fprintf_sym(fp,
							symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
						fprintf(fp,":\\n\");\n");
					}

					fprintf(fp,"\tstruct engine_data* edata"
						" = (struct engine_data*)p;\n");

					fprintf(fp,"\tstruct thr_data* data = __get_thr_data();\n");


//					fprintf(fp,"\tint vcid = data->vcid;\n");

					if (opt_kcall_debug) {
						fprintf(fp,"\tprintf(\"__XCL_call_");
						fprintf_sym(fp,
							symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
						fprintf(fp,": vcore[] running\\n\");\n");
					}

					fprintf(fp,"\t++(edata->thr_runc);\n");

					fprintf(fp,
//						"\tif (!(setjmp(*(data->this_jbufp))))"
						"\tif (!(setjmp(*(data->this_jbufp))))"
//         			"longjmp(*(data->vcengine_jbufp),vcid+1);\n");
//         			"longjmp(*(data->vcengine_jbufp),0);\n");
         			"longjmp(*(data->engine_jbufp),0);\n");

					fprintf(fp,"\tvoid* arg_buf = edata->pr_arg_buf;\n");

					if (opt_kcall_debug) {
						fprintf(fp,"\tprintf(\"__XCL_call_");
						fprintf_sym(fp,
							symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
						fprintf(fp,": edata = %%p & %%p\\n\",edata,&edata);\n");

						fprintf(fp,"\tprintf(\"__XCL_call_");
						fprintf_sym(fp,
							symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
						fprintf(fp,": vcore[] arg_buf = %%p\\n\",arg_buf);\n");

					}

					fprintf(fp,"\t((__XCL_func_");

					fprintf_sym(fp,
						symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

					fprintf(fp,"_t)(edata->funcp))(\n");

					nptr2 = nptr1->n_func.args;

					for(i=0,nptr2=nptr1->n_func.args;i<n;i++,nptr2=nptr2->next) {

						fprintf(fp,"\t\t");
						if (i>0) fprintf(fp,",");	
						fprintf(fp,"*(");
						fprintf_cl_type(fp,nptr2->n_arg.argtype);
						fprintf(fp,"*)(arg_buf + edata->pr_arg_off[%d])\n",i);

					}

					fprintf(fp,"\t);\n");

					if (opt_kcall_debug) {
						fprintf(fp,"\tprintf(\"__XCL_call_");
						fprintf_sym(fp,
							symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
						fprintf(fp,": vcore[] edata = %%p\\n\",edata);\n");
					}

					fprintf(fp,"\t--(edata->thr_runc);\n");

//					fprintf(fp,"\tlongjmp(*(data->vcengine_jbufp),vcid+1);\n");
//					fprintf(fp,"\tlongjmp(*(data->vcengine_jbufp),0);\n");
					fprintf(fp,"\tlongjmp(*(data->engine_jbufp),0);\n");

					if (opt_kcall_debug) {
						fprintf(fp,"\tprintf(\"__XCL_call_");
						fprintf_sym(fp,
							symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
						fprintf(fp,": vcore[] halt\\n\");\n");
					}

					fprintf(fp,"}\n");

				}

				break;

			default: break;

		}

		exit(0);

	} else if (opt_kcall2) {

//		fprintf(fp,"#include <stdio.h>\n");
//		fprintf(fp,"#define __xcl_kcall2__\n");
//		fprintf(fp,"#include \"vcore.h\"\n");

		for(nptr1=root;nptr1;nptr1=nptr1->next) switch(nptr1->ntyp) {

			case NTYP_EMPTY: break;

			case NTYP_FUNC:

				if (nptr1->n_func.flags & F_FUNC_DEF) {

					n = node_count(nptr1->n_func.args);

			
					nptr2=nptr1->n_func.args;

					fprintf(fp,"\n/* F ");

					fprintf_sym(fp,
						symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

					fprintf(fp," %d",calc_args_sz(nptr2));

					fprintf(fp,"\t%d",n);

					for(i=0;i<n;i++,nptr2=nptr2->next) 
						fprintf(fp," %d",calc_arg_sz(nptr2));

					fprintf(fp," */\n");


					/* print typedef */

					fprintf(fp,"typedef %s",type_table[type_ival2index(
						nptr1->n_func.rettype->n_type.datatype)].c_type);

					fprintf(fp,"(*__XCL_func_");

					fprintf_sym(fp,
						symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

					fprintf(fp,"_t)(");

					for(i=0,nptr2=nptr1->n_func.args;i<n;i++,nptr2=nptr2->next) {
						if (i>0) fprintf(fp,", ");	
						fprintf_cl_type(fp,nptr2->n_arg.argtype);
					}

					fprintf(fp,");\n");


					/* print call wrapper */

					fprintf(fp,"%s ",type_table[type_ival2index(
						nptr1->n_func.rettype->n_type.datatype)].c_type);

//					fprintf(fp," __attribute__((noreturn))\n");

					fprintf(fp,"__XCL_call2_");

					fprintf_sym(fp,
						symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

					fprintf(fp,"(void* p)");

					fprintf(fp,"{\n");

					if (opt_kcall_debug) {
						fprintf(fp,"\tprintf(\"__XCL_call_");
						fprintf_sym(fp,
							symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
						fprintf(fp,":\\n\");\n");
					}

					fprintf(fp,"\tstruct engine_data* edata"
						" = (struct engine_data*)p;\n");

//					fprintf(fp,"\tstruct vc_data* data = __getvcdata();\n");


//					fprintf(fp,"\tint vcid = data->vcid;\n");

					if (opt_kcall_debug) {
						fprintf(fp,"\tprintf(\"__XCL_call_");
						fprintf_sym(fp,
							symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
						fprintf(fp,": vcore[%%d] running\\n\",vcid);\n");
					}

//					fprintf(fp,"\t++(edata->vc_runc);\n");

//					fprintf(fp,
//						"\tif (!(__vc_setjmp(*(data->this_jbufp))))"
//         			"__vc_longjmp(*(data->vcengine_jbufp),0);\n");

					fprintf(fp,"\tvoid* arg_buf = edata->pr_arg_buf;\n");

					fprintf(fp,"\t((__XCL_func_");

					fprintf_sym(fp,
						symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

					fprintf(fp,"_t)(edata->funcp))(\n");

					nptr2 = nptr1->n_func.args;

					for(i=0,nptr2=nptr1->n_func.args;i<n;i++,nptr2=nptr2->next) {

						fprintf(fp,"\t\t");
						if (i>0) fprintf(fp,",");	
						fprintf(fp,"*(");
						fprintf_cl_type(fp,nptr2->n_arg.argtype);
						fprintf(fp,"*)(arg_buf + edata->pr_arg_off[%d])\n",i);

					}

					fprintf(fp,"\t);\n");

//					fprintf(fp,"\t--(edata->vc_runc);\n");

//					fprintf(fp,"\tlongjmp(*(data->vcengine_jbufp),vcid+1);\n");
//					fprintf(fp,"\tlongjmp(*(data->vcengine_jbufp),0);\n");
//					fprintf(fp,"\t__vc_longjmp(*(data->vcengine_jbufp),0);\n");

					if (opt_kcall_debug) {
						fprintf(fp,"\tprintf(\"__XCL_call_");
						fprintf_sym(fp,
							symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
						fprintf(fp,": vcore[%%d] halt\\n\",vcid);\n");
					}

					fprintf(fp,"}\n");

				}

				break;

			default: break;

		}

		exit(0);

       } else if (opt_kcall3) {

               fprintf(fp,"#include <stdio.h>\n");

               for(nptr1=root;nptr1;nptr1=nptr1->next) switch(nptr1->ntyp) {

                       case NTYP_EMPTY: break;

                       case NTYP_FUNC:

                               if (nptr1->n_func.flags & F_FUNC_DEF) {

                                       n = node_count(nptr1->n_func.args);

                                       nptr2=nptr1->n_func.args;

                                       fprintf(fp,"\n/* F ");

                                       fprintf_sym(fp,
                                               symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

                                       fprintf(fp," %d",calc_args_sz(nptr2));

                                       fprintf(fp,"\t%d",n);

                                       for(i=0;i<n;i++,nptr2=nptr2->next) 
                                               fprintf(fp," %d",calc_arg_sz(nptr2));

                                       fprintf(fp," */\n");


                                       /* print typedef */

                                       fprintf(fp,"typedef %s",type_table[type_ival2index(
                                               nptr1->n_func.rettype->n_type.datatype)].c_type);

                                       fprintf(fp,"(*__XCL_func_");

                                       fprintf_sym(fp,
                                               symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

                                       fprintf(fp,"_t)(");

                                       for(i=0,nptr2=nptr1->n_func.args;i<n;i++,nptr2=nptr2->next) {
                                               if (i>0) fprintf(fp,", ");      
                                               fprintf_cl_type(fp,nptr2->n_arg.argtype);
                                       }

                                       fprintf(fp,");\n");


                                       /* print decls macro */

                                       fprintf(fp,"__KCALL_DECLS(");
                                       fprintf_sym(fp,
                                               symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
                                       fprintf(fp,")\n");


                                       /* print call wrapper */

                                       fprintf(fp,"%s ",type_table[type_ival2index(
                                               nptr1->n_func.rettype->n_type.datatype)].c_type);

//                                     fprintf(fp," __attribute__((noreturn))\n");
                                       fprintf(fp," __KCALL_ATTRIBUTES\n");

                                       fprintf(fp,"__XCL_call_");

                                       fprintf_sym(fp,
                                               symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

                                       fprintf(fp,"(void* p)");

                                       fprintf(fp,"{\n");

                                       if (opt_kcall_debug) {
                                               fprintf(fp,"\tprintf(\"__XCL_call_");
                                               fprintf_sym(fp,
                                                       symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
                                               fprintf(fp,":\\n\");\n");
                                       }

//                                     fprintf(fp,"\tstruct engine_data* edata"
//                                             " = (struct engine_data*)p;\n");

//                                     fprintf(fp,"\tstruct thr_data* data = __get_thr_data();\n");


                                       if (opt_kcall_debug) {
                                               fprintf(fp,"\tprintf(\"__XCL_call_");
                                               fprintf_sym(fp,
                                                       symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
                                               fprintf(fp,": vcore[] running\\n\");\n");
                                       }

//                                     fprintf(fp,"\t++(edata->thr_runc);\n");

//                                     fprintf(fp,
//                                             "\tif (!(setjmp(*(data->this_jbufp))))"
//                             "longjmp(*(data->engine_jbufp),0);\n");

//                                     fprintf(fp,"\tvoid* arg_buf = edata->pr_arg_buf;\n");

                                       if (opt_kcall_debug) {
                                               fprintf(fp,"\tprintf(\"__XCL_call_");
                                               fprintf_sym(fp,
                                                       symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
                                               fprintf(fp,": edata = %%p & %%p\\n\",edata,&edata);\n");

                                               fprintf(fp,"\tprintf(\"__XCL_call_");
                                               fprintf_sym(fp,
                                                       symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
                                               fprintf(fp,": vcore[] arg_buf = %%p\\n\",arg_buf);\n");

                                       }

                                       fprintf(fp,"\t__KCALL_PRE\n");

                                       fprintf(fp,"\t((__XCL_func_");

                                       fprintf_sym(fp,
                                               symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

//                                     fprintf(fp,"_t)(edata->funcp))(\n");
//                                     fprintf(fp,"_t)(__kcall_thr_ptr()))(\n");
                                       fprintf(fp,"_t)(__kcall_thr_ptr(");
                                       fprintf_sym(fp,
                                               symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
                                       fprintf(fp,")))(\n");

                                       nptr2 = nptr1->n_func.args;

                                       for(i=0,nptr2=nptr1->n_func.args;i<n;i++,nptr2=nptr2->next) {

                                               fprintf(fp,"\t\t");
                                               if (i>0) fprintf(fp,",");       

//                                             fprintf(fp,"*(");
//                                             fprintf_cl_type(fp,nptr2->n_arg.argtype);
//                                             fprintf(fp,"*)(arg_buf + edata->pr_arg_off[%d])\n",i);

                                               fprintf(fp,"__kcall_arg(");
                                               fprintf_cl_type(fp,nptr2->n_arg.argtype);
                                               fprintf(fp,",%d)\n",i);

                                       }

                                       fprintf(fp,"\t);\n");

                                       if (opt_kcall_debug) {
                                               fprintf(fp,"\tprintf(\"__XCL_call_");
                                               fprintf_sym(fp,
                                                       symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
                                               fprintf(fp,": vcore[] edata = %%p\\n\",edata);\n");
                                       }

//                                     fprintf(fp,"\t--(edata->thr_runc);\n");

//                                     fprintf(fp,"\tlongjmp(*(data->engine_jbufp),0);\n");

                                       fprintf(fp,"\t__KCALL_POST\n");

                                       if (opt_kcall_debug) {
                                               fprintf(fp,"\tprintf(\"__XCL_call_");
                                               fprintf_sym(fp,
                                                       symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
                                               fprintf(fp,": vcore[] halt\\n\");\n");
                                       }

                                       fprintf(fp,"}\n");

                               }

                               break;

                       default: break;

               }

               exit(0);




	}


	/* 
	 * print clsymtab table 
	 */

	if (opt_clsymtab) {

		j = 0;
		k = 0;

		for(nptr1=root;nptr1!=0;nptr1=nptr1->next) switch(nptr1->ntyp) {

			case NTYP_EMPTY: break;

			case NTYP_FUNC:

				if ((opt_func_def && (nptr1->n_func.flags)&F_FUNC_DEF) 
					|| (opt_func_dec && (nptr1->n_func.flags)&F_FUNC_DEC)) {

					nptr2 = nptr1->n_func.args;
					n = node_count(nptr2);
				
					fprintf(fp,"%4d ",j);

					if (nptr1->n_func.flags&F_FUNC_DEC) fprintf(fp,"f ");
					else if (nptr1->n_func.flags&F_FUNC_DEF) fprintf(fp,"F ");
					else fprintf(fp,"?");

					fprintf_sym(fp,
						symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);

					nptr3 = nptr1->n_func.rettype;
					fprintf(fp," %d %d %d %d %d %d %d",
						nptr3->n_type.datatype,
						nptr3->n_type.vecn,
						nptr3->n_type.arrn,
						nptr3->n_type.addrspace,
						nptr3->n_type.ptrc,
						n,k+1					/* the +1 allows a null in clargtab DAR */
					);

					fprintf(fp,"\n");

					++j;
					k += n;

				}

				break;

			default: break;

		}

		exit(0);

	}


	/* 
	 * print clargtab table 
	 */

	if (opt_clargtab) {

		j = 0;
		k = 0;

		fprintf(fp,"   0 (null) 0 0 0 0 0 0 ()\n");
		++j;

		for(nptr1=root;nptr1!=0;nptr1=nptr1->next) switch(nptr1->ntyp) {

			case NTYP_EMPTY: break;

			case NTYP_FUNC:

				if ((opt_func_def && (nptr1->n_func.flags)&F_FUNC_DEF) 
					|| (opt_func_dec && (nptr1->n_func.flags)&F_FUNC_DEC)) {

					nptr2 = nptr1->n_func.args;
					n = node_count(nptr2);
					k = 1;
	
					for(;nptr2!=0;nptr2=nptr2->next) {

						fprintf(fp,"%4d ",j);

//					if (nptr1->n_func.flags&F_FUNC_DEC) fprintf(fp,"f ");
//					else if (nptr1->n_func.flags&F_FUNC_DEF) fprintf(fp,"F ");
//					else fprintf(fp,"?");

					fprintf_sym(fp,
						symbuf+nptr2->n_arg.sym,opt_ll_style,opt_c_style);

					nptr3 = nptr2->n_arg.argtype;
					fprintf(fp," %d %d %d %d %d %d",
						nptr3->n_type.datatype,
						nptr3->n_type.vecn,
						nptr3->n_type.arrn,
						nptr3->n_type.addrspace,
						nptr3->n_type.ptrc,
						((k<n)?j+1:0)
					);

					fprintf(fp," (");
					fprintf_sym(fp,
						symbuf+nptr1->n_func.sym,opt_ll_style,opt_c_style);
					fprintf(fp,")");

					fprintf(fp,"\n");

					++j;
					++k;

					}

				}

				break;

			default: break;

		}

		exit(0);

	}
#endif


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


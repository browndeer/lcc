/* compiler_cgen.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "compiler_node.h"
#include "compiler_gram.h"
#include "compiler_cgen.h"

#define __error(msg) do { \
	fprintf(stderr,"error: " msg "\n");  \
	exit(-1); \
	} while(0)


void cgen_program( FILE* fp, node_t* root )
{

	if (root->ntyp != N_PROGRAM) __error("missing program node");

	fprintf(fp,"#include <lol.h>\n");
	fprintf(fp,"long long _it =0;\n");
	fprintf(fp,"long long _remote_pe = -1;\n");
	fprintf(fp,"int _remote_hold = 0;\n");
	fprintf(fp,"int main()\n");

	cgen_block(fp,root->n_program.block);

}


void cgen_block( FILE* fp, node_t* nptr )
{

	if (nptr->ntyp != N_BLOCK) __error("missing block node");

	node_t* stmt = nptr->n_block.stmts;;

	fprintf(fp,"{\n");

	if (nptr->n_block.main) {
		fprintf(fp,"shmem_init();\n");
	}

	for( ; stmt; stmt=stmt->next) cgen_stmt(fp,stmt);

	if (nptr->n_block.main) {
		fprintf(fp,"shmem_finalize();\n");
	}

	fprintf(fp,"}\n");
}


void cgen_stmt( FILE* fp, node_t* nptr )
{

	switch (nptr->ntyp) {
		
		case N_DECLARATION_STATEMENT:
			cgen_decl_stmt(fp,nptr);
			break;

		case N_EXPRESSION_STATEMENT:
			cgen_expr_stmt(fp,nptr);
			break;

		case N_ASSIGNMENT_STATEMENT:
			cgen_assign_stmt(fp,nptr);
			break;

		case N_PRINT_STATEMENT:
			cgen_print_stmt(fp,nptr);
			break;

		case N_IF_STATEMENT:
			cgen_if_stmt(fp,nptr);
			break;

		case N_SWITCH_STATEMENT:
			cgen_switch_stmt(fp,nptr);
			break;

		case N_CASE_STATEMENT:
			cgen_case_stmt(fp,nptr);
			break;

		case N_FOR_STATEMENT:
			cgen_for_stmt(fp,nptr);
			break;

		case N_WHILE_STATEMENT:
			cgen_while_stmt(fp,nptr);
			break;

		case N_BARRIER_STATEMENT:
			cgen_barrier_stmt(fp,nptr);
			break;

		case N_REMOTE_STATEMENT:
			cgen_remote_stmt(fp,nptr);
			break;

		case N_LOCK_STATEMENT:
			cgen_lock_stmt(fp,nptr);
			break;

		default:
			__error("missing statement node");

	}

}


void cgen_decl_stmt( FILE* fp, node_t* nptr )
{

	char* name = symbuf+nptr->n_decl_stmt.sym;

	if (nptr->n_decl_stmt.typ < T_VOID_ARRAY) {

		switch(nptr->n_decl_stmt.typ) {
			case T_VOID:
				fprintf(fp,"void %s",name);
				break;
			case T_BOOLEAN:
				fprintf(fp,"long long %s",name);
				break;
			case T_INTEGER:
				fprintf(fp,"long long %s",name);
				break;
			case T_FLOAT:
				fprintf(fp,"float %s",name);
				break;
			case T_STRING:
				fprintf(fp,"char* %s",name);
				break;
			case T_STRUCT:
				fprintf(fp,"struct %s",name);
				break;
			default:
				__error("bad type");
		}

		if (nptr->n_decl_stmt.init) {

			fprintf(fp," = ");
			cgen_expr(fp,nptr->n_decl_stmt.init);

		} else {

			fprintf(fp," = 0");

		}

		fprintf(fp,";\n");

	} else {

		switch(nptr->n_decl_stmt.typ) {
			case T_VOID_ARRAY:
				fprintf(fp,"void* %s",name);
				break;
			case T_BOOLEAN_ARRAY:
				fprintf(fp,"long long* %s",name);
				break;
			case T_INTEGER_ARRAY:
				fprintf(fp,"long long* %s",name);
				break;
			case T_FLOAT_ARRAY:
				fprintf(fp,"float* %s",name);
				break;
			case T_STRING_ARRAY:
				fprintf(fp,"char** %s",name);
				break;
			case T_STRUCT_ARRAY:
				fprintf(fp,"struct* %s",name);
				break;
			default:
				__error("bad type");
		}

		if (nptr->n_decl_stmt.init) {

			if (nptr->n_decl_stmt.symmetric) 
				fprintf(fp," = _shmem_malloc(");
			else
				fprintf(fp," = _malloc(");

			switch(nptr->n_decl_stmt.typ) {
				case T_VOID_ARRAY:
					fprintf(fp," sizeof(void) * ");
					break;
				case T_BOOLEAN_ARRAY:
					fprintf(fp," sizeof(long long) * ");
					break;
				case T_INTEGER_ARRAY:
					fprintf(fp," sizeof(long long) * ");
					break;
				case T_FLOAT_ARRAY:
					fprintf(fp," sizeof(float) * ");
					break;
				case T_STRING_ARRAY:
					fprintf(fp," sizeof(char*) * ");
					break;
				case T_STRUCT_ARRAY:
					fprintf(fp," sizeof(struct) * ");
					break;
				default:
					__error("bad type");
			}

			cgen_expr(fp,nptr->n_decl_stmt.init);

			fprintf(fp,");\n");

		} else {

			fprintf(fp,";\n");

		}

	}

	if (nptr->n_decl_stmt.shared) 
		fprintf(fp,"static long _lock_%s = 0;\n",name);

}


void cgen_expr_stmt( FILE* fp, node_t* nptr )
{

	fprintf(fp,"_it = ");

	cgen_expr(fp,nptr->n_expr_stmt.expr);

	fprintf(fp,";\n");

}


const char* op_name[] = {
   "<no-op>",
   "_op_add",
   "_op_sub",
   "_op_mul",
   "_op_div",
   "_op_mod",
   "_op_max",
   "_op_min",
   "_op_inv",
   "_op_pow2",
   "_op_sqrt",
   "_op_and",
   "_op_or",
   "_op_xor",
   "_op_not",
   "_op_eq",
   "_op_neq",
	"_op_index",
	"_op_rand",
	"_op_randf",
	"_op_shmem_npes",
	"_op_shmem_pe"
};


void cgen_expr( FILE* fp, node_t* nptr )
{

	switch(nptr->ntyp) {

		case N_IDENTIFIER:
			if (nptr->n_ident.expr) {
				fprintf(fp,"%s[",symbuf+nptr->n_ident.sym);
				cgen_expr(fp,nptr->n_ident.expr);
				fprintf(fp,"]");
			} else {
				fprintf(fp,"%s",symbuf+nptr->n_ident.sym);
			}
			break;

		case N_INTEGER_CONSTANT:
			fprintf(fp,"%lld",nptr->n_integer.val);
			break;

		case N_FLOAT_CONSTANT:
			fprintf(fp,"%f",nptr->n_float.val);
			break;

		case N_STRING_CONSTANT:
			fprintf(fp,"%s",symbuf+nptr->n_string.str);
			break;

		case N_EXPRESSION:

			fprintf(fp,"%s(",op_name[nptr->n_expr.op]);

			switch (nptr->n_expr.op) {

					case OP_RAND:
					case OP_RANDF:
					case OP_SHMEM_NPES:
					case OP_SHMEM_PE:
						break;

					case OP_INV:
					case OP_POW2:
					case OP_SQRT:
					case OP_NOT:
						cgen_expr(fp,nptr->n_expr.args);
						break;

				   case OP_ADD:
					case OP_SUB:
					case OP_MUL:
					case OP_DIV:
					case OP_MOD:
					case OP_MAX:
					case OP_MIN:
					case OP_AND:
					case OP_OR:
					case OP_XOR:
					case OP_EQ:
					case OP_NEQ:
						cgen_expr(fp,nptr->n_expr.args);
						printf(",");
						cgen_expr(fp,nptr->n_expr.args->next);
						break;

					default:
						__error("bad op");
			}

			fprintf(fp,")");

			break;

		default:
			__error("bad expression");
	}

}


void cgen_assign_stmt( FILE* fp, node_t* nptr )
{
	cgen_expr(fp,nptr->n_assign_stmt.target);

	fprintf(fp," = ");

	cgen_expr(fp,nptr->n_assign_stmt.expr);

	fprintf(fp,";\n");
}


void cgen_print_stmt( FILE* fp, node_t* nptr )
{
	node_t* arg = nptr->n_print_stmt.args; 

	for( ; arg; arg=arg->next) {
		fprintf(fp,"_print_val(");
		cgen_expr(fp,arg);
		fprintf(fp,");\n");
	}

}


void cgen_if_stmt( FILE* fp, node_t* nptr )
{
	fprintf(fp,"if (_it)\n");

	if (nptr->n_if_stmt.if_block) 
		cgen_block(fp,nptr->n_if_stmt.if_block);
	else
		fprintf(fp,"{}\n");

	node_t* elsif = nptr->n_if_stmt.elsif_clauses;

	for( ; elsif; elsif=elsif->next) {
		fprintf(fp,"else if (");
		cgen_expr(fp,elsif->n_elsif_clause.expr);
		fprintf(fp,")\n");
		cgen_block(fp,elsif->n_elsif_clause.block);
	}

	if (nptr->n_if_stmt.else_block) {
		fprintf(fp,"else\n");
		cgen_block(fp,nptr->n_if_stmt.else_block);
	}

}


void cgen_switch_stmt( FILE* fp, node_t* nptr )
{
	fprintf(fp,"switch (_it)\n");

	cgen_block(fp,nptr->n_switch_stmt.block);
}


void cgen_case_stmt( FILE* fp, node_t* nptr )
{
	if (nptr->n_case_stmt.expr) {
		fprintf(fp,"case ");
		cgen_expr(fp,nptr->n_case_stmt.expr);
		fprintf(fp,":\n");
	} else {
		fprintf(fp,"default:\n");
	}
}


void cgen_break_stmt( FILE* fp, node_t* nptr )
{
	fprintf(fp,"break;\n");
}


void cgen_for_stmt( FILE* fp, node_t* nptr )
{

	char* loop_sym_name = symbuf+nptr->n_for_stmt.loop_sym;

	fprintf(fp,"for (");

	fprintf(fp,"int %s = 0;",loop_sym_name);

	fprintf(fp,"_op_not(");
	cgen_expr(fp,nptr->n_for_stmt.expr);
	fprintf(fp,")");

	if (nptr->n_for_stmt.loop_iter == +1)
		fprintf(fp,"; %s++ )\n",loop_sym_name);
	else if (nptr->n_for_stmt.loop_iter == -1)
		fprintf(fp,"; %s-- )\n",loop_sym_name);
	else
		__error("bad loop iterator");

	cgen_block(fp,nptr->n_for_stmt.block);

}


void cgen_while_stmt( FILE* fp, node_t* nptr )
{

	char* loop_sym_name = symbuf+nptr->n_for_stmt.loop_sym;

	fprintf(fp,"for (");

	fprintf(fp,"int %s = 0;",loop_sym_name);

	cgen_expr(fp,nptr->n_for_stmt.expr);

	if (nptr->n_for_stmt.loop_iter == +1)
		fprintf(fp,"; %s++ )\n",loop_sym_name);
	else if (nptr->n_for_stmt.loop_iter == -1)
		fprintf(fp,"; %s-- )\n",loop_sym_name);
	else
		__error("bad loop iterator");

	cgen_block(fp,nptr->n_for_stmt.block);

}


void cgen_barrier_stmt( FILE* fp, node_t* nptr )
{
	fprintf(fp,"_barrier();\n");
}


void cgen_remote_stmt( FILE* fp, node_t* nptr )
{

	char* loop_sym_name = symbuf+nptr->n_for_stmt.loop_sym;

	if (nptr->n_remote_stmt.expr) {
		fprintf(fp,"_remote(");
		cgen_expr(fp,nptr->n_remote_stmt.expr);
		fprintf(fp,",%d);\n",nptr->n_remote_stmt.hold);
	} else {
		fprintf(fp,"_remote(0,0);\n");
	}

}


void cgen_lock_stmt( FILE* fp, node_t* nptr )
{
	node_t* target = nptr->n_lock_stmt.target;
	char* name = symbuf+target->n_ident.sym;
	int remote = target->n_ident.remote;

	switch (nptr->n_lock_stmt.lock_op) {
		case L_TRYLOCK:
			fprintf(fp,"_trylock(");
			break;
		case L_LOCK:
			fprintf(fp,"_lock(");
			break;
		case L_UNLOCK:
			fprintf(fp,"_unlock(");
			break;
		default:
			__error("bad lock op");
	}

	fprintf(fp,"_lock_%s,",name);

	if (remote)
		fprintf(fp,"_remote_pe");
	else
		fprintf(fp,"_op_shmem_pe()");

	fprintf(fp,");\n");

}



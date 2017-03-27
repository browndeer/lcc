/* compiler_node.h
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


/*
 * Defines the nodes used to for a tree representation of the assembly
 * code.  The style/design follows that used in PCC (Portable C Compiler),
 * but is much simpler.
 */

/* DAR */

#ifndef _COMPILER_NODE_H
#define _COMPILER_NODE_H

#include <stdio.h>

typedef enum {
	N_IDENTIFIER = 1,
	N_INTEGER_CONSTANT,
	N_FLOAT_CONSTANT,
	N_STRING_CONSTANT,
	N_EXPRESSION,
	N_EXPRESSION_STATEMENT,
	N_DECLARATION_STATEMENT,
	N_ASSIGNMENT_STATEMENT,
	N_PRINT_STATEMENT,
	N_INPUT_STATEMENT,
	N_IF_STATEMENT,
	N_ELSIF_CLAUSE,
	N_SWITCH_STATEMENT,
	N_CASE_STATEMENT,
	N_BREAK_STATEMENT,
	N_FOR_ITER,
	N_FOR_STATEMENT,
	N_WHILE_STATEMENT,
	N_BARRIER_STATEMENT,
	N_REMOTE_STATEMENT,
	N_LOCK_STATEMENT,
	N_BLOCK,
	N_PROGRAM,
	N_FUNC_PROTOTYPE,
	N_FUNC_DEFINITION,
	N_FUNC_EXPRESSION,
	N_RETURN_STATEMENT
} node_type_t;

typedef enum {
	OP_ADD = 1,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_MAX,
	OP_MIN,
	OP_POWR,
	OP_ROOT,
	OP_INV,
	OP_POW2,
	OP_SQRT,
	OP_AND,
	OP_OR,
	OP_XOR,
	OP_NOT,
	OP_EQ,
	OP_NEQ,
	OP_INDEX,
	OP_RAND,
	OP_RANDF,
	OP_SHMEM_NPES,
	OP_SHMEM_PE
} op_type_t;

typedef enum {
	T_VOID,
	T_BOOLEAN,
	T_INTEGER,
	T_FLOAT,
	T_STRING,
	T_STRUCT,
	T_VOID_ARRAY,
	T_BOOLEAN_ARRAY,
	T_INTEGER_ARRAY,
	T_FLOAT_ARRAY,
	T_STRING_ARRAY,
	T_STRUCT_ARRAY,
	T_FUNC
} type_t;

typedef enum {
	L_TRYLOCK=1,
	L_LOCK,
	L_UNLOCK
} lock_op_t;

typedef struct node_struct {

	struct node_struct* prev;
	struct node_struct* next;
	node_type_t ntyp;

	union {

		/***
		 *** primitives
		 ***/

		struct {
			int sym;
			type_t typ;
			struct node_struct* expr;
			int remote;
		} _n_identifier;

		struct {
			long long val;
		} _n_integer_constant;

		struct {
			float val;
		} _n_float_constant;

		struct {
			int str;
		} _n_string_constant;

	
		/***
		 *** expressions
		 ***/

		struct {
			op_type_t op;
			struct node_struct* args;
		} _n_expression;

		/***
		 *** statememnts
		 ***/

		struct {
			int sym;
			type_t typ;
			struct node_struct* init;
			int symmetric;
			int shared;
		} _n_declaration_statement;

		struct {
			struct node_struct* expr;
		} _n_expression_statement;

		struct {
			struct node_struct* target;
			struct node_struct* expr;
		} _n_assignment_statement;

		struct {
			struct node_struct* args;
			int err;
			int ext;
		} _n_print_statement;

		struct {
			struct node_struct* args;
		} _n_input_statement;

		struct {
			struct node_struct* if_block;
			struct node_struct* elsif_clauses;
			struct node_struct* else_block;
		} _n_if_statement;

		struct {
			struct node_struct* block;
		} _n_switch_statement;

		struct {
			struct node_struct* expr;
		} _n_case_statement;

		struct {
			int loop_sym;
			int loop_initval;
			int loop_iter;
		} _n_for_iter;

		struct {
			int loop_label;
			int loop_sym;
			int loop_initval;
			int loop_iter;
			int loop_cond;
			struct node_struct* expr;
			struct node_struct* block;
		} _n_for_statement;

		struct {
			int loop_label;
//			int loop_iter;
//			int loop_sym;
			int loop_cond;
			struct node_struct* expr;
			struct node_struct* block;
		} _n_while_statement;

		struct {
			struct node_struct* expr;
			int hold;
		} _n_remote_statement;

		struct {
			struct node_struct* target;
			lock_op_t lock_op;
		} _n_lock_statement;

		struct {
			int sym;
			struct node_struct* args;
		} _n_func_prototype;

		struct {
			struct node_struct* proto;
			struct node_struct* block;
		} _n_func_definition;

		struct {
			int sym;
			struct node_struct* args;
		} _n_func_expression;

		struct {
			struct node_struct* expr;
		} _n_return_statement;

		/**
		 *** clauses
		 ***/

		struct {
			struct node_struct* expr;
			struct node_struct* block;
		} _n_elsif_clause;


		/***
		 *** block
		 ***/

		struct {
			struct node_struct* stmts;
			int main;
		} _n_block;


		/***
		 *** program
		 ***/

		struct {
			struct node_struct* block;
		} _n_program;

	} _n_info;

} node_t;

#define n_ident 			_n_info._n_identifier
#define n_integer 		_n_info._n_integer_constant
#define n_float 			_n_info._n_float_constant
#define n_string 			_n_info._n_string_constant
#define n_expr				_n_info._n_expression
#define n_decl_stmt		_n_info._n_declaration_statement
#define n_expr_stmt		_n_info._n_expression_statement
#define n_assign_stmt	_n_info._n_assignment_statement
#define n_print_stmt		_n_info._n_print_statement
#define n_input_stmt		_n_info._n_input_statement
#define n_if_stmt			_n_info._n_if_statement
#define n_elsif_clause	_n_info._n_elsif_clause
#define n_switch_stmt	_n_info._n_switch_statement
#define n_case_stmt		_n_info._n_case_statement
#define n_for_iter		_n_info._n_for_iter
#define n_for_stmt		_n_info._n_for_statement
#define n_while_stmt		_n_info._n_while_statement
#define n_remote_stmt	_n_info._n_remote_statement
#define n_lock_stmt		_n_info._n_lock_statement
#define n_block			_n_info._n_block
#define n_program			_n_info._n_program
#define n_func_proto		_n_info._n_func_prototype
#define n_func_def		_n_info._n_func_definition
#define n_func_expr		_n_info._n_func_expression
#define n_return_stmt	_n_info._n_return_statement


node_t* node_create_identifier( int sym, int typ, node_t* expr, int remote );
node_t* node_update_identifier(node_t* nptr, int typ, node_t* expr,int remote);

node_t* node_create_integer( long long val );
node_t* node_create_float( float val );
node_t* node_create_string( int str );

node_t* node_create_expr0( op_type_t );
node_t* node_create_expr1( op_type_t, node_t* arg1 );
node_t* node_create_expr2( op_type_t, node_t* arg1, node_t* arg2 );
node_t* node_create_exprn( op_type_t, node_t* args );

node_t* node_create_decl_stmt(int sym,type_t typ,node_t* init, 
	int symmetric, int shared );

node_t* node_update_decl_stmt(node_t* nptr,type_t typ,node_t* init,
	int symmetric, int shared);

node_t* node_create_expr_stmt( node_t* expr );

node_t* node_create_program( node_t* block);

node_t* node_create_block( node_t* block);
node_t* node_block_add_stmt( node_t* block, node_t* stmt );

node_t* node_create_assign_stmt( node_t* target, node_t* expr );

node_t* node_create_print_stmt( node_t* arg, int err, int ext );
node_t* node_update_print_stmt( node_t* nptr, node_t* arg );

node_t* node_create_input_stmt( node_t* arg );
node_t* node_update_input_stmt( node_t* nptr, node_t* arg );

node_t* node_create_if_stmt(node_t* if_block, node_t* elsif_clauses,
	node_t* else_block );

node_t* node_create_elsif_clause( node_t* expr, node_t* block );

node_t* node_create_switch_stmt( node_t* block );
node_t* node_create_case_stmt( node_t* expr );

node_t* node_create_break_stmt( void );

//node_t* node_create_for_stmt(int loop_label, int loop_sym, int initval, 
//	int iter, node_t* expr, node_t* block );
node_t* node_create_for_iter(int loop_sym, int loop_initval, int loop_iter);
node_t* node_create_for_stmt(int loop_label, int loop_sym, int loop_initval, 
	int loop_iter, int loop_cond, node_t* expr, node_t* block );

//node_t* node_create_while_stmt(int loop_label, int iter, int loop_sym, 
//	node_t* expr, node_t* block );
node_t* node_create_while_stmt(int loop_label, int loop_cond, node_t* expr,
	node_t* block );

node_t* node_create_barrier_stmt( void );

node_t* node_create_remote_stmt( node_t* expr, int hold );

node_t* node_create_lock_stmt( node_t* target, lock_op_t lock_op );

node_t* node_create_func_proto(int sym);
node_t* node_update_func_proto(node_t* nptr, int sym );

node_t* node_create_func_def(node_t* proto, node_t* block );

node_t* node_create_func_expr(int sym );
node_t* node_update_func_expr(node_t* nptr, node_t* arg);

node_t* node_create_return_stmt( node_t* expr );

node_t* node_alloc();
void node_free(node_t* nptr);
node_t* node_get_head(node_t* nptr);
node_t* node_get_tail(node_t* nptr);
int node_count(node_t* nptr);
node_t* node_insert(node_t* nptr0, node_t* nptr);
node_t* node_insert_head(node_t* nptr0, node_t* nptr);
node_t* node_insert_tail(node_t* nptr0, node_t* nptr);
node_t* node_remove(node_t* nptr);
node_t* node_extract(node_t* nptr0, node_t* nptr1);
node_t* node_delete(node_t* nptr);
void node_destroy(node_t* nptr);
node_t* node_create();
node_t* node_copy( node_t* nptr );

/*
node_t* node_create_func_dec( node_t* rettype, int sym, node_t* args );
node_t* node_create_func_def( node_t* rettype, int sym, node_t* args );
node_t* node_create_arg( node_t* type, int sym );
node_t* node_create_type(
	int arrn, int vecn,int datatype,int addrspace, int ptrc);

node_t* node_set_addrspace( node_t* type, int addrspace );
node_t* node_set_ptrc( node_t* type, int ptrc );
node_t* node_set_vecn( node_t* type, int vecn );
node_t* node_set_arrn( node_t* type, int arrn );
*/

void node_fprintf(FILE* fp, node_t* nptr, int level );


int get_symtyp(int sym);

#endif


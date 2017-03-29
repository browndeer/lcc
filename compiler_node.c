/* compiler_node.c
 *
 * Copyright (c) 2017 Brown Deer Technology, LLC.  All Rights Reserved.
 * Copyright (c) 2008-2010 Brown Deer Technology, LLC.  All Rights Reserved.
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
 * code.  The style/design follows that used in PCC (Portable C Compiler).
 */

/* DAR */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compiler_node.h"
#include "compiler_gram.h"

extern char* symbuf;

extern int __use_shmem;

struct symtyp_entry {
	int sym;
	int typ;
	int shared;
	struct symtyp_entry* nxt;
};

struct symtyp_entry* symtyp_table = 0;

void add_symtyp( int sym, int typ, int shared )
{

	struct symtyp_entry* tmp 
		= (struct symtyp_entry*)malloc(sizeof(struct symtyp_entry));

	tmp->sym = sym;
	tmp->typ=typ;
	tmp->shared=shared;
	tmp->nxt=0;

	if (!symtyp_table) {
		symtyp_table = tmp;
//		printf("XXX add first\n");
	} else {
		struct symtyp_entry* p = symtyp_table;
		for( ; p->nxt != 0; p=p->nxt ); // printf("XXX skipping\n");
		p->nxt = tmp;
//		printf("XXX add not first\n");
	}	

//printf("XXX add symtyp %d %d\n",sym,typ);

if (shared)
printf("XXX added symbol is shared\n");
}


void mod_symtyp( int sym, int typ, int shared )
{
	struct symtyp_entry* p = symtyp_table;
	for( ; p != 0; p=p->nxt ) {
		if (p->sym == sym) {
			if (typ) p->typ = typ;
			if (shared) p->shared = shared;
		}
	}
}


int get_symtyp( int sym )
{
	struct symtyp_entry* p = symtyp_table;
	for( ; p != 0; p=p->nxt ) {
//		printf("XXX loop p p->nxt %p %p\n",p,p->nxt);
//		printf("XXX get_symtyp: compare sym %d %d\n",p->sym,sym);
		if (p->sym == sym) return p->typ;
//		printf("XXX get_symtyp: nxt is %p\n",p->nxt);
	}
	return -1;
}

int is_sym_shared( int sym )
{
	struct symtyp_entry* p = symtyp_table;
	for( ; p != 0; p=p->nxt ) {
		if (p->sym == sym) return p->shared;
	}
	return 0;
}

struct symtyp_entry* copy_symtyp_table( struct symtyp_entry* old_table )
{
	if (!old_table) return 0;

	size_t sz = sizeof(struct symtyp_entry);

	struct symtyp_entry* new_table = (struct symtyp_entry*)malloc(sz);
	memcpy(new_table,old_table,sz);

	struct symtyp_entry* q = new_table;

	struct symtyp_entry* p = old_table->nxt;

	for( ; p != 0; p=p->nxt ) {
		q = q->nxt = (struct symtyp_entry*)malloc(sz);
	}

	q->nxt = 0;

	return new_table;
}


void delete_symtyp_table( struct symtyp_entry* table )
{
	struct symtyp_entry* p = table;
	while(p) {
		struct symtyp_entry* tmp = p;
		p=p->nxt;
		free(tmp);
	}
}


node_t* 
node_alloc()
{ return((node_t*)malloc(sizeof(node_t))); }


void
node_free(node_t* nptr)
{ if (nptr) free(nptr); }


node_t* 
node_get_head(node_t* nptr)
{
	if (!nptr) return(0);
	node_t* tmp = nptr;
	while (tmp->prev) tmp = tmp->prev;
	return(tmp);
}


node_t* 
node_get_tail(node_t* nptr)
{
	if (!nptr) return(0);
	node_t* tmp = nptr;
	while (tmp->next) tmp = tmp->next;
	return(tmp);
}


int
node_count(node_t* nptr)
{
	if (!nptr) return(0);
	int count = 1;
	node_t* tmp = nptr;
	while (tmp->next) {tmp = tmp->next; ++count; }
	return(count);
}


node_t*
node_insert(node_t* nptr0, node_t* nptr)
{
	if (!nptr0 || !nptr) return(0);

	if (nptr0->next) nptr0->next->prev = nptr;
	nptr->next = nptr0->next;
	nptr0->next = nptr;
	nptr->prev = nptr0;

	return(nptr);
}


node_t*
node_insert_head(node_t* nptr0, node_t* nptr)
{
	if (!nptr0 || !nptr) return(0);

	node_t* tmp = nptr0;
	while (tmp->prev) tmp = tmp->prev;

	nptr->next = tmp;
	tmp->prev = nptr;
	nptr->prev = 0;

	return(nptr);
}


node_t*
node_insert_tail(node_t* nptr0, node_t* nptr)
{
	if (!nptr0 || !nptr) return(0);

	node_t* tmp = nptr0;
	while (tmp->next) tmp = tmp->next;

	nptr->next = 0;
	tmp->next = nptr;
	nptr->prev = tmp;

	return(nptr0);
}


node_t*
node_remove(node_t* nptr)
{
   if (!nptr) return(0);
	
	node_t* prev = nptr->prev;
	node_t* next = nptr->next;

	if (prev) prev->next = next;
	if (next) next->prev = prev;

	nptr->prev = 0;
	nptr->next = 0;

	return(next);
}


node_t*
node_extract(node_t* nptr0, node_t* nptr1)
{
   if (!nptr0 || ! nptr1) return(0);
	
	node_t* prev = nptr0->prev;
	node_t* next = nptr1->next;

	if (prev) prev->next = next;
	if (next) next->prev = prev;

	nptr0->prev = 0;
	nptr1->next = 0;

	return(nptr0);
}

node_t*
node_delete(node_t* nptr)
{
	node_t* next = node_remove(nptr);
	node_destroy(nptr);
	return(next);
}


void
node_destroy(node_t* nptr)
{
	if (!nptr) return;

	node_t* tmp;

	node_free(nptr);
}


node_t*
node_create()
{
	node_t* tmp = node_alloc();
	tmp->prev = tmp->next = 0;
	tmp->ntyp = 0;
	return(tmp);
}


node_t*
node_copy( node_t* nptr )
{
	node_t* tmp = node_alloc();
	memcpy(tmp,nptr,sizeof(node_t));
	tmp->prev = tmp->next = 0;
	tmp->ntyp = 0;
	return(tmp);
}



node_t*
node_create_identifier( int sym, int typ, node_t* expr, int remote )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_IDENTIFIER;

	tmp->n_ident.sym = sym;
	tmp->n_ident.typ = typ;
	tmp->n_ident.expr = expr;
	tmp->n_ident.remote = remote;

	if(remote) __use_shmem = 1;

	return(tmp);
}


node_t*
node_update_identifier( node_t* nptr, int typ, node_t* expr, int remote )
{
	if (typ) nptr->n_ident.typ = typ;
	if (expr) nptr->n_ident.expr = expr;
	if(remote) nptr->n_ident.remote = remote;

	if(remote) __use_shmem = 1;

	return(nptr);
}


node_t*
node_create_integer( long long val )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_INTEGER_CONSTANT;
	tmp->n_integer.val = val;
	return(tmp);
}


node_t*
node_create_float( float val )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_FLOAT_CONSTANT;
	tmp->n_float.val = val;
	return(tmp);
}


node_t*
node_create_string( int str )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_STRING_CONSTANT;
	tmp->n_string.str = str;
	return(tmp);
}


node_t*
node_create_expr0( op_type_t op )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_EXPRESSION;
	tmp->n_expr.op = op;
	tmp->n_expr.args = 0;
	return(tmp);
}


node_t*
node_create_expr1( op_type_t op, node_t* arg1 )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_EXPRESSION;
	tmp->n_expr.op = op;
	tmp->n_expr.args = arg1;
	return(tmp);
}


node_t*
node_create_expr2( op_type_t op, node_t* arg1, node_t* arg2 )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_EXPRESSION;
	tmp->n_expr.op = op;
	tmp->n_expr.args = arg1;
	node_insert_tail(tmp->n_expr.args,arg2);
	return(tmp);
}


node_t*
node_create_exprn( op_type_t op, node_t* args )
{
	return 0;
}


node_t*
node_create_decl_stmt( int sym, type_t typ, node_t* init, 
	int symmetric, int shared )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_DECLARATION_STATEMENT;
	tmp->n_decl_stmt.sym = sym;
	tmp->n_decl_stmt.typ = typ;
	tmp->n_decl_stmt.init = init;
	tmp->n_decl_stmt.symmetric = symmetric;
	tmp->n_decl_stmt.shared = shared;

	if (symmetric || shared) __use_shmem = 1;

	add_symtyp(sym,typ,shared);

	return(tmp);
}


node_t*
node_update_decl_stmt( node_t* nptr, type_t typ, node_t* init, 
	int symmetric, int shared )
{
	if (typ) nptr->n_decl_stmt.typ = typ;
	if (init) nptr->n_decl_stmt.init = init;
	if (symmetric) nptr->n_decl_stmt.symmetric = symmetric;
	if (shared) nptr->n_decl_stmt.shared = shared;

	if (symmetric || shared) __use_shmem = 1;

	mod_symtyp(nptr->n_decl_stmt.sym,typ,shared);

	return(nptr);
}


node_t*
node_create_expr_stmt( node_t* expr )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_EXPRESSION_STATEMENT;
	tmp->n_expr_stmt.expr = expr;
	return(tmp);
}


node_t*
node_create_program( node_t* block )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_PROGRAM;
	tmp->n_program.block = block;
	block->n_block.main = 1;
	return(tmp);
}


node_t*
node_create_block( node_t* stmt )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_BLOCK;
	tmp->n_block.stmts = stmt;
	tmp->n_block.main = 0;
	return(tmp);
}


node_t*
node_block_add_stmt( node_t* block, node_t* stmt )
{
	node_insert_tail(block->n_block.stmts,stmt);
	return(block);
}


node_t*
node_create_assign_stmt( node_t* target, node_t* expr )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_ASSIGNMENT_STATEMENT;
	tmp->n_assign_stmt.target = target;
	tmp->n_assign_stmt.expr = expr;
	return(tmp);
}


node_t*
node_create_print_stmt( node_t* arg, int err, int ext )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_PRINT_STATEMENT;
	tmp->n_print_stmt.args = arg;
	tmp->n_print_stmt.err = err;
	tmp->n_print_stmt.ext = ext;
	return(tmp);
}


node_t*
node_update_print_stmt( node_t* nptr, node_t* arg )
{
	node_insert_tail( nptr->n_print_stmt.args, arg );
	return(nptr);
}


node_t*
node_create_input_stmt( node_t* arg )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_INPUT_STATEMENT;
	tmp->n_input_stmt.args = arg;
	return(tmp);
}


node_t*
node_update_input_stmt( node_t* nptr, node_t* arg )
{
	node_insert_tail( nptr->n_input_stmt.args, arg );
	return(nptr);
}


node_t*
node_create_if_stmt(node_t* if_block,node_t* elsif_clauses, node_t* else_block)
{
	node_t* tmp = node_create();
	tmp->ntyp = N_IF_STATEMENT;
	tmp->n_if_stmt.if_block = if_block;
	tmp->n_if_stmt.elsif_clauses = elsif_clauses;
	tmp->n_if_stmt.else_block = else_block;
	return(tmp);
}


node_t*
node_create_elsif_clause( node_t* expr, node_t* block )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_ELSIF_CLAUSE;
	tmp->n_elsif_clause.expr = expr;
	tmp->n_elsif_clause.block = block;
	return(tmp);
}


node_t*
node_create_switch_stmt( node_t* block )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_SWITCH_STATEMENT;
	tmp->n_switch_stmt.block = block;
	return(tmp);
}

node_t*
node_create_case_stmt( node_t* expr )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_CASE_STATEMENT;
	tmp->n_case_stmt.expr = expr;
	return(tmp);
}


node_t*
node_create_break_stmt( void )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_BREAK_STATEMENT;
	return(tmp);
}


node_t*
node_create_for_iter( int loop_sym, int loop_initval, int loop_iter)
{
	node_t* tmp = node_create();
	tmp->ntyp = N_FOR_ITER;
	tmp->n_for_iter.loop_sym = loop_sym;
	tmp->n_for_iter.loop_initval = loop_initval;
	tmp->n_for_iter.loop_iter = loop_iter;
	return(tmp);
}


node_t*
node_create_for_stmt( 
	int loop_label, int loop_sym, int loop_initval, int loop_iter,
   int loop_cond, node_t* expr, node_t* block 
)
{
	node_t* tmp = node_create();
	tmp->ntyp = N_FOR_STATEMENT;
	tmp->n_for_stmt.loop_label = loop_label;
	tmp->n_for_stmt.loop_sym = loop_sym;
	tmp->n_for_stmt.loop_initval = loop_initval;
	tmp->n_for_stmt.loop_iter = loop_iter;
	tmp->n_for_stmt.loop_cond = loop_cond;
	tmp->n_for_stmt.expr = expr;
	tmp->n_for_stmt.block = block;
	return(tmp);
}


node_t*
node_create_while_stmt( 
	int loop_label, int loop_cond, node_t* expr, node_t* block 
)
{
	node_t* tmp = node_create();
	tmp->ntyp = N_WHILE_STATEMENT;
	tmp->n_while_stmt.loop_label = loop_label;
	tmp->n_while_stmt.loop_cond = loop_cond;
	tmp->n_while_stmt.expr = expr;
	tmp->n_while_stmt.block = block;
	return(tmp);
}


node_t*
node_create_barrier_stmt( void )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_BARRIER_STATEMENT;

	__use_shmem = 1;

	return(tmp);
}


node_t*
node_create_remote_stmt( node_t* expr, int hold )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_REMOTE_STATEMENT;
	tmp->n_remote_stmt.expr = expr;
	tmp->n_remote_stmt.hold = hold;

	__use_shmem = 1;

	return(tmp);
}


node_t*
node_create_lock_stmt( node_t* target, lock_op_t lock_op )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_LOCK_STATEMENT;
	tmp->n_lock_stmt.target = target;
	tmp->n_lock_stmt.lock_op = lock_op;

	__use_shmem = 1;

	return(tmp);
}



node_t*
node_create_func_proto( int sym )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_FUNC_PROTOTYPE;
	tmp->n_func_proto.sym = sym;
	tmp->n_func_proto.args = 0;

	return(tmp);
}


node_t*
node_update_func_proto( node_t* nptr, int sym )
{
	if (nptr->n_func_proto.args)
		node_insert_tail( nptr->n_func_proto.args, 
			node_create_identifier(sym,0,0,0) );
	else
		nptr->n_func_proto.args = node_create_identifier(sym,0,0,0);

	return(nptr);
}


node_t*
node_create_func_def( node_t* proto, node_t* block )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_FUNC_DEFINITION;
	tmp->n_func_def.proto = proto;
	tmp->n_func_def.block = block;

	add_symtyp(proto->n_func_proto.sym,T_FUNC,0);

	return(tmp);
}


node_t*
node_create_func_expr( int sym )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_FUNC_EXPRESSION;
	tmp->n_func_expr.sym = sym;

	return(tmp);
}


node_t*
node_update_func_expr( node_t* nptr, node_t* arg )
{
	if (nptr->n_func_expr.args) 
		node_insert_tail( nptr->n_func_expr.args, arg );
	else
		nptr->n_func_expr.args = arg;

//	printf("XXX add arg %p %p\n",nptr->n_func_expr.args,arg);

	return(nptr);
}


node_t*
node_create_return_stmt( node_t* expr )
{
	node_t* tmp = node_create();
	tmp->ntyp = N_RETURN_STATEMENT;
	tmp->n_return_stmt.expr = expr;

	return(tmp);
}


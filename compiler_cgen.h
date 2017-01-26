/* compiler_cgen.h
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

#ifndef _COMPILER_CGEN_H
#define _COMPILER_CGEN_H

void cgen_program( FILE* fp, node_t* nptr );
void cgen_block( FILE* fp, node_t* nptr );
void cgen_stmt( FILE* fp, node_t* nptr );
void cgen_decl_stmt( FILE* fp, node_t* nptr );
void cgen_expr_stmt( FILE* fp, node_t* nptr );
void cgen_expr( FILE* fp, node_t* nptr );
void cgen_assign_stmt( FILE* fp, node_t* nptr );
void cgen_print_stmt( FILE* fp, node_t* nptr );
void cgen_if_stmt( FILE* fp, node_t* nptr );
void cgen_switch_stmt( FILE* fp, node_t* nptr );
void cgen_case_stmt( FILE* fp, node_t* nptr );
void cgen_break_stmt( FILE* fp, node_t* nptr );
void cgen_for_stmt( FILE* fp, node_t* nptr );
void cgen_while_stmt( FILE* fp, node_t* nptr );
void cgen_barrier_stmt( FILE* fp, node_t* nptr );

#endif



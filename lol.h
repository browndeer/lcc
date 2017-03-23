
#ifndef _lol_h
#define _lol_h

#ifdef __E32__
#include <host_stdio.h>
#include <host_stdlib.h>
#define printf(...) host_printf(__VA_ARGS__)
#else
#include <stdio.h>
#include <stdlib.h>
#endif

#ifdef USE_SHMEM
#include <shmem.h>
#endif

#if(0)
typedef enum {
   VT_INTEGER, /**< An integer value. */
   VT_FLOAT,   /**< A decimal value. */
   VT_BOOLEAN, /**< A boolean value. */
   VT_STRING,  /**< A string value. */
   VT_NIL,     /**< Represents no value. */
   VT_FUNC,    /**< A function. */
   VT_ARRAY    /**< An array. */
} _value_type_t;

typedef union {
  	long long i;           /**< Integer data. */
  	float f;               /**< Decimal data. */
  	char *s;               /**< String data. */
//  	FuncDefStmtNode *fn;   /**< Function data. */
  	struct scopeobject *a; /**< Array data. */
} _value_data_t;

typedef struct {
	_value_data_t data;
	_value_type_t type;
} val_t;
#define _init_val(d,t) (val_t) { d, t }
#else
typedef long long val_t;
#define _init_val(d,t) (val_t) d
#endif


void _cast_val_type( long long, long long );

void _cast_val_type( long long x, long long t) {}

#define _print_val(x) ({ \
		__builtin_choose_expr(\
			__builtin_types_compatible_p( typeof(x), int),\
			printf("%d\n",x),\
			__builtin_choose_expr(\
				__builtin_types_compatible_p( typeof(x), long long),\
				printf("%lld\n",x),\
				__builtin_choose_expr(\
					__builtin_types_compatible_p( typeof(x), float),\
					printf("%f\n",x),\
					__builtin_choose_expr(\
						__builtin_types_compatible_p( typeof(x), const char[]),\
						printf("%s\n",x),\
						printf("<unknown-type> ")\
					) \
				)\
			)\
	); })

#define _input_val(x) ({ \
		__builtin_choose_expr(\
			__builtin_types_compatible_p( typeof(x), int),\
			scanf("%d",&x),\
			__builtin_choose_expr(\
				__builtin_types_compatible_p( typeof(x), long long),\
				scanf("%lld",&x),\
				__builtin_choose_expr(\
					__builtin_types_compatible_p( typeof(x), float),\
					scanf("%f",&x),\
					__builtin_choose_expr(\
						__builtin_types_compatible_p( typeof(x), char*),\
						scanf("%s",x),\
						printf("<unknown-type> ")\
					) \
				)\
			)\
	); })

val_t _op_not( long long x) { return(!x); }

#define _op_eq(x,y) (((x)==(y))? 1 : 0 )
#define _op_neq(x,y) (((x)==(y))? 0 : 1 )
#define _op_add(x,y) ((x)+(y))
#define _op_sub(x,y) ((x)-(y))
#define _op_mul(x,y) ((x)*(y))
#define _op_div(x,y) ((x)/(y))



long long _op_rand() { return (long long)rand(); }

float _op_randf() { return (float)rand()/(float)(RAND_MAX); }

#define _op_pow2(x) ((x)*(x))

#define _op_sqrt(x) ({ \
	typeof(x) tmp = __builtin_choose_expr(\
		__builtin_types_compatible_p( typeof(x), long long),\
		sqrt(x), \
		__builtin_choose_expr(\
			__builtin_types_compatible_p( typeof(x), float),\
			sqrtf(x),\
			(void)0\
		)\
	); tmp; })

#define _op_inv(x) (1/(x))	

#define _op_assign(dst,src) ({ \
	dst = src; \
	})



#ifdef USE_SHMEM

long long _op_shmem_npes() { return((long long)shmem_n_pes()); }
long long _op_shmem_pe() { return((long long)shmem_my_pe()); }

void _barrier() { shmem_barrier_all(); }

typedef long mtx_t;

#define _malloc(x) shmem_malloc(x)

#define _shmem_malloc(x) shmem_malloc(x)

#define _remote(pe,hold) { _remote_pe=pe; _remote_hold=hold; }

void _op_remote_seti( long long* x, long long y, int pe )
{ shmem_longlong_p(x,y,pe); }
	
void _op_remote_setf( float* x, float y, int pe )
{ shmem_float_p(x,y,pe); }

long long _op_remote_geti( long long* x, int pe)
{ return shmem_longlong_g(x,pe); }
	
float _op_remote_getf( float* x, int pe)
{ return shmem_float_g(x,pe); }

#define _op_remote_set( x, y, pe ) ({ \
	__builtin_choose_expr(\
		__builtin_types_compatible_p( typeof(x), long long*),\
		_op_remote_seti(x,y,pe),\
		__builtin_choose_expr(\
			__builtin_types_compatible_p( typeof(x), float*),\
			_op_remote_setf(x,y,pe),\
			(void)0\
		)\
	); })

#define _op_remote_get(x,pe) ({ \
	typeof(*x) tmp = __builtin_choose_expr(\
		__builtin_types_compatible_p( typeof(x), long long*),\
		_op_remote_geti(x,pe),\
		__builtin_choose_expr(\
			__builtin_types_compatible_p( typeof(x), float*),\
			_op_remote_getf(x,pe),\
			(void)0\
		)\
	); tmp; })

#define _trylock(x,pe) ({ \
	void* p = shmem_ptr(&x,pe); \
	shmem_test_lock(p); \
	})

#define _lock(x,pe) ({ \
	void* p = shmem_ptr(&x,pe); \
	shmem_set_lock(p); \
	})

#define _unlock(x,pe) ({ \
	void* p = shmem_ptr(&x,pe); \
	shmem_clear_lock(p); \
	})
#endif


#endif


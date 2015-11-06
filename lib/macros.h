
#pragma once

#include "string.h"
#include "cpp_magic.h"

// If assigning rval to lval modifies the content of lval
#define if_assignment_modifies(lval, rval) \
typeof(lval) _iam_##lval = lval; \
if( (_iam_##lval = lval), ((lval = rval) != _iam_##lval) )

// Declare anonymous function
/* Example:
    int (*square)(int) = lambda(int, (int i){
        return i*i;
    });
*/
#define lambda(returnType, body) \
({ \
    returnType _lambda_fn body \
    &_lambda_fn; \
})


// Aliasing-safe reinterpret-cast
#define union_cast(type, x) \
    (((union { \
        typeof(x) a; \
        type b; \
    })x).b)



// For usage with typeof, sizeof, etc
#define member(structType, memberName) \
    (((structType*)0)->memberName)


// Concatenate two tokens
#define concat(x, y) concat_impl(x, y)
#define concat_impl(x, y) x##y

// Stringify a token
#define str(s) #s
#define xstr(s) str(s)

// Run an expression when the current scope is exited
/* Example:
    int* arr = malloc(sizeof(int) * 10);
    scope_exit {
        free(arr);
    }
*/
#define scope_exit scope_exit_impl(__COUNTER__)
#define scope_exit_impl(counter) \
    auto void concat(se_fn_, counter)(__attribute__((unused)) uint8_t* concat(se_var_, counter)); \
    __attribute__((cleanup(concat(se_fn_, counter)))) uint8_t concat(se_var_, counter); \
    void concat(se_fn_, counter)(__attribute__((unused)) uint8_t* concat(se_var_, counter)) 

    
// Generate a optimized-away-at-compile-time hash of a string
#define hash(s)     ((uint8_t)(H64(s,0,0)))
#define H1(s,i,x)   (x*65599u+(uint8_t)s[(i)<strlen(s)?strlen(s)-1-(i):strlen(s)])
#define H4(s,i,x)   H1(s,i,H1(s,i+1,H1(s,i+2,H1(s,i+3,x))))
#define H16(s,i,x)  H4(s,i,H4(s,i+4,H4(s,i+8,H4(s,i+12,x))))
#define H64(s,i,x)  H16(s,i,H16(s,i+16,H16(s,i+32,H16(s,i+48,x))))

// Generate a (probably) unique uint8_t number for a type
#define typeid(t) hash(str(t))

// For use with IS_xxx macros
#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0,)

// Checks whether the argument passed is literally 'default'
// IS_DEFAULT(default) returns 1
// IS_DEFAULT(xxx) returns 0
#define IS_DEFAULT(d) CHECK(IS_DEFAULT_PROBE_##d)
#define IS_DEFAULT_PROBE_default PROBE()
#define IS_DEFAULT_PROBE_ 

// For usage with functional macros, like MAP
#define IDENTITY(x) x

// See MAP_PAIRS in cpp_magic.h
// This is the same, just for triplets
#define MAP_TRIPLETS(op,sep,...) \
  IF(HAS_ARGS(__VA_ARGS__))(EVAL(MAP_TRIPLETS_INNER(op,sep,__VA_ARGS__)))
#define MAP_TRIPLETS_INNER(op,sep,cur_val_1, cur_val_2, cur_val_3, ...) \
  op(cur_val_1,cur_val_2,cur_val_3) \
  IF(HAS_ARGS(__VA_ARGS__))( \
    sep() DEFER2(_MAP_TRIPLETS_INNER)()(op, sep, __VA_ARGS__) \
  )
#define _MAP_TRIPLETS_INNER() MAP_TRIPLETS_INNER


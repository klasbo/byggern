
#pragma once

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


#define concat(x, y) concat_impl(x, y)
#define concat_impl(x, y) x##y

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

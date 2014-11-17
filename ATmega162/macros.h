
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

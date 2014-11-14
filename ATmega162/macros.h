
#define if_assignment_modifies(lval, rval) \
typeof(lval) _##lval = lval; \
if( (_##lval = lval), ((lval = rval) != _##lval) )


#define lambda(returnType, body) \
({ \
    returnType __fn body \
    &__fn; \
})


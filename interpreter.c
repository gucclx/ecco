#include "expr.h"
#include "interpreter.h"


// this could be simplified to handling 3 cases:
// constant, function1, function2.
// in this case, we handle the special case negate,
// because even if the unary '-' is a function1
// its type is negate and not function1.
// the default case handles '+', '*' and so on
// even if these are function2, their types are ADD, MULT, ...
// respectively. 
// again, this could be simplified in the code.
// interpret is never supposed to get an invalid type.
// if something like 1/0 happens, it returns NAN.

// the argument of function1 is assumed to always be
// root->left.
double interpret(struct expr* root)
{
	switch (root->type)
	{
		case NEGATE: 	return (*root->fun1)(interpret(root->left));
		case CONSTANT:  return root->value;
		case FUNCTION1: return (*root->fun1)(interpret(root->left));
		// this default handels the types ADD, MULT, SUB, etc.
		// these have a function of arity 2.
		default:
		case FUNCTION2: return (*root->fun2)(interpret(root->left), interpret(root->right));
	}
}
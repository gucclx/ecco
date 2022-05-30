#include "expr.h"
#include "interpreter.h"
#include <math.h>


// NAN is returned if an invalid operation happens 
// or if the root type is somehow invalid
// +-inf is returned if a value is too large.
// the argument of function1 is assumed to always be
// root->left.
double interpret(struct expr* root)
{
	switch (root->type)
	{
		case CONSTANT:  return root->value;
		case FUNCTION1: return (*root->fun1)(interpret(root->left));
		case FUNCTION2: return (*root->fun2)(interpret(root->left), interpret(root->right));
	}
	return NAN;
}
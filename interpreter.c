#include "expr.h"
#include "interpreter.h"


// interpret is never supposed to get an invalid type.
// if something like 1/0 happens, it returns NAN.

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
}
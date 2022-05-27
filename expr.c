#include <stdlib.h>
#include "expr.h"


struct expr* init_expr(int t, struct expr* l, struct expr* r)
{
	struct expr* ret = malloc(sizeof(struct expr));
	if (!ret) return NULL;

	ret->type  = t;
	ret->left  = l;
	ret->right = r;
	ret->fun1  = NULL;
	ret->fun2  = NULL;
	return ret;
}

void free_expr(struct expr* e)
{
	if (!e) return;
	free_expr(e->left);
	free_expr(e->right);
	free(e);
}
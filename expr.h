#ifndef _ECC0_EXPR_H_
#define _ECC0_EXPR_H_


enum TYPES
{
	CONSTANT,
	FUNCTION1,
	FUNCTION2,
	LPAREN,
	RPAREN,
	COMMA,
	INVALID,
	END
};

struct expr
{
	int type;
	union 
	{
		double value; 
		double (*fun1)(double x);
		double (*fun2)(double x, double y);
	};
	struct expr* left;
	struct expr* right;
};

struct expr* init_expr(int t, struct expr* l, struct expr* r);
void free_expr(struct expr* e);

#endif
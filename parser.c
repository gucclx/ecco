#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "expr.h"
#include "parser.h"


struct state
{
	char* start;
	char* next;
	int type;
	union 
	{
		double value; 
		double (*fun1)(double x);
		double (*fun2)(double x, double y);
	};
};

struct expr* expression(struct state* s);

double negate(double x) 	   {return -x;};
double add(double x, double y) {return x+y;};
double sub(double x, double y) {return x-y;};
double mult(double x, double y){return x*y;};
double inc(double x, double y) {return x + y;};

double divide(double x, double y)
{
	if (y == 0.0) return NAN;
	return x/y;	
}

double glog(double b, double x)
{
	if (b <= 1.0) return NAN;
	if (x <= 0.0) return NAN;
	return log(x)/log(b);
}

struct identifier
{
	char* name;
	int type;
	union {double value; void* fun;};
};

struct identifier constants[] =
{ 
	{.name = "e",  .type = CONSTANT, .value = M_E}, 
	{.name = "pi", .type = CONSTANT, .value = M_PI} 
};

int set_constant(struct state* s, char* start, int len)
{
	if (len == 0) return 0;

	int constants_len = sizeof(constants) / sizeof(*constants);

	for (int i = 0; i < constants_len; i++)
	{
		if (strlen(constants[i].name) != len) continue;
		if (memcmp(start, constants[i].name, len) != 0) continue;
		s->value = constants[i].value;
		s->type  = constants[i].type;
		return 1;
	}
	return 0;
}

struct identifier functions[] = 
{
	{.name = "ln",   .type = FUNCTION1, .fun = log},
	{.name = "log",  .type = FUNCTION2, .fun = glog},
	{.name = "sin",  .type = FUNCTION1, .fun = sin}, 
	{.name = "cos",  .type = FUNCTION1, .fun = cos},
	{.name = "sqrt", .type = FUNCTION1, .fun = sqrt}
};

int set_function(struct state* s, char* start, int len)
{
	if (len == 0) return 0;

	int functions_len = sizeof(functions) / sizeof(*functions);

	for (int i = 0; i < functions_len; i++)
	{
		if (strlen(functions[i].name) != len) continue;
		if (memcmp(start, functions[i].name, len) != 0) continue;

		if (functions[i].type == FUNCTION1)
			s->fun1 = functions[i].fun;
		else
			s->fun2 = functions[i].fun;

		s->type = functions[i].type;
		return 1;
	}
	return 0;
}

// finds next token in the input string
void next_token(struct state* s)
{
	s->type = INVALID;

	while (1)
	{
		s->start = s->next;

		// end of input string
		if (!s->next[0])
		{
			s->type = END;
			return;
		}

		// try to find a number
		if (s->next[0] >= '0' && s->next[0] <= '9')
		{
			s->value = strtod(s->start, &s->next);
			s->type  = CONSTANT;
			return;
		}

		// try to find an operator or special symbol.
		// ignore spaces
		switch (s->next++[0])
		{
			case '+': s->type = FUNCTION2; s->fun2 = add;    return;
			case '-': s->type = FUNCTION2; s->fun2 = sub;    return;
			case '*': s->type = FUNCTION2; s->fun2 = mult;   return;
			case '/': s->type = FUNCTION2; s->fun2 = divide; return;
			case '%': s->type = FUNCTION2; s->fun2 = fmod;   return;
			case '^': s->type = FUNCTION2; s->fun2 = pow;    return;
			case '(': s->type = LPAREN; return;
			case ')': s->type = RPAREN; return;
			case ',': s->type = COMMA;  return;
		 	case ' ': case '\t': case '\n': case '\r': continue;
		}

		// try to find identifier
		// such as a constant's name
		// or a function's name
		while (isalpha(s->next[0])) s->next++;
		while (isdigit(s->next[0]) || s->next[0] == '_') s->next++;


		if (set_constant(s, s->start, s->next-s->start)) return;
		if (set_function(s, s->start, s->next-s->start)) return;
		return;
	}
}

struct expr* base(struct state* s)
{
	struct expr* ret;

	if (s->type == CONSTANT)
	{
		ret = init_expr(CONSTANT, NULL, NULL);
		ret->value = s->value;
		next_token(s);
		return ret;
	}
	if (s->type == LPAREN)
	{
		next_token(s);
		ret = expression(s);
		if (!ret) return NULL;
		if (s->type != RPAREN) return NULL;
		
		next_token(s);
		return ret;
	}
	if (s->type == FUNCTION1)
	{
		void* fun1 = s->fun1;

		next_token(s);
		if (s->type != LPAREN) return NULL;
		next_token(s);

		ret = expression(s);
		if (!ret) return NULL;
		if (s->type != RPAREN) return NULL;
		next_token(s);

		ret = init_expr(FUNCTION1, ret, NULL);
		ret->fun1 = fun1;
		return ret;
	}
	if (s->type == FUNCTION2)
	{
		void* fun2 = s->fun2;

		next_token(s);
		if (s->type != LPAREN) return NULL;
		next_token(s);

		ret = expression(s);
		if (!ret) return NULL;
		if (s->type != COMMA) return NULL;
		next_token(s);

		ret = init_expr(FUNCTION2, ret, expression(s));

		// no expression after ','
		if (!ret->right) return NULL;

		if (s->type != RPAREN) return NULL;

		ret->fun2 = fun2;
		next_token(s);
		return ret;
	}
	return NULL;
}

struct expr* unary(struct state* s)
{
	if (s->type == FUNCTION2 && s->fun2 == add)
	{
		next_token(s);
		return unary(s);
	}
	if (s->type == FUNCTION2 && s->fun2 == sub)
	{
		next_token(s);
		struct expr* ret;
		ret = unary(s);
		if (!ret) return NULL;
		ret = init_expr(FUNCTION1, ret, NULL);
		ret->fun1 = negate;
		return ret;
	}
	return base(s);
}

struct expr* power(struct state* s)
{
	struct expr* ret = unary(s);
	if (!ret) return NULL;

	while (s->type == FUNCTION2 && s->fun2 == pow)
	{
		next_token(s);
		ret = init_expr(FUNCTION2, ret, unary(s));
		if (!ret->right) return NULL;
		ret->fun2 = pow;
	}
	return ret;
}

struct expr* term(struct state* s)
{
	struct expr* ret = power(s);
	if (!ret) return NULL;

	void* fun2;
	int t;

	while (s->type == FUNCTION2 && (s->fun2 == mult   || 
									s->fun2 == divide || 
									s->fun2 == fmod))
	{
		fun2 = s->fun2;
		t    = s->type;
		next_token(s);
		ret = init_expr(t, ret, power(s));
		if (!ret->right) return NULL;
		ret->fun2 = fun2;
	}
	return ret;
}

struct expr* expression(struct state* s)
{
	struct expr* ret = term(s);
	if (!ret) return NULL;

	void* fun2;
	int t;

	while (s->type == FUNCTION2 && (s->fun2 == add || s->fun2 == sub))
	{
		fun2 = s->fun2;
		t    = s->type;

		next_token(s);
		ret = init_expr(t, ret, term(s));
		if (!ret->right) return NULL;
		ret->fun2 = fun2;
	}
	return ret;
}

struct expr* parse(char* input)
{
	struct state s;
	s.start = input;
	s.next 	= s.start;

	next_token(&s);
	struct expr* ret = expression(&s);

	if (!ret) return NULL;
	if (s.type != END) return NULL;
	return ret;
}
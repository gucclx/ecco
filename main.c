#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "expr.h"
#include "parser.h"
#include "interpreter.h"


int main()
{
	char* input = NULL;
	size_t input_size = 0;
	struct expr* expr_tree = NULL;

	printf("ECC0\n");

	while (1)
	{
		printf("> ");

		if (getline(&input, &input_size, stdin) <= 1) break;

		expr_tree = parse(input);

		if (!expr_tree)
		{
			printf("SYNTAX ERROR\n");
			free_expr(expr_tree);
			continue;
		}

		double res = interpret(expr_tree);

		if (isnan(res) || isinf(res))
		{
			printf("MATH ERROR\n");
			free_expr(expr_tree);
			continue;
		}

		printf("= %.7f\n", res);
		free_expr(expr_tree);
	}
	free(input);
}
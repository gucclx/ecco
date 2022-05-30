# ecco

Program that interprets math expressions such as 2 * 2 + 1 / sin(pi / 2) * e

### Operators: + - * / % ^

### Functions: sin(x), cos(x), ln(x), log(base, x)

### Constants: e, pi

### Grammar

expression -> term (('+' | '-') term)*

term       -> power (('*' | '/' | '%') power)*

power      -> unary ('^' unary)*

unary      -> ('+' | '-') unary | base

base 	   -> CONSTANT | FUNCTION '(' expression ')' | FUNCTION '(' expression ',' expression ')' | '(' expression ')'

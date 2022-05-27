CC = gcc

calc: main.c
	${CC} -o main main.c parser.c expr.c interpreter.c -lm -Wall -Wextra
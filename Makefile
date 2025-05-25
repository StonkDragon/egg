build:
	clang -O3 -Wall -Wextra -pedantic -std=c99 -O3 $(CFLAGS) -Iinclude -o egg src/main.c

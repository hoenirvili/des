CC = gcc
CFLAGS = -Wall -fpie -fasynchronous-unwind-tables -O2 -pipe -Werror=implicit-function-declaration -Werror=format-security -std=gnu11
SRC = $(wildcard *.c)

des: $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

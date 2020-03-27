CC = gcc
CFLAGS = -Wall -g -fpie -fasynchronous-unwind-tables -fstack-clash-protection -pipe -Werror=implicit-function-declaration -Werror=format-security -std=gnu11
SRC = $(wildcard *.c)

main: $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

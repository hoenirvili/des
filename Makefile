CC = gcc
CFLAGS = -Wall -fpie -fasynchronous-unwind-tables -g -fstack-clash-protection -pipe -Werror=implicit-function-declaration -Werror=format-security -std=gnu11
SRC = $(wildcard *.c)

main: $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

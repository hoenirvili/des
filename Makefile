CC = gcc
CFLAGS = -Wall -g -std=gnu11
SRC = $(wildcard *.c)

main: $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

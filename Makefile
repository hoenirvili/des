CC = gcc
CFLAGS = -Wall -g -Wformat-security -std=gnu11
SRC = $(wildcard *.c)

main: $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

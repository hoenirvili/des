CC = gcc
CFLAGS = -Wall -g -Wformat-security
SRC = $(wildcard *.c)

main: $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

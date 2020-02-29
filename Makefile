CC = gcc
CFLAGS = -Wall -Wformat-security -O2
SRC = $(wildcard *.c)

main: $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

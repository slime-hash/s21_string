CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Werror

all: run

re: clean all

run: s21_sprintf.o main.o
	$(CC) $(CFLAGS) s21_sprintf.o main.o -o run

s21_sprintf.o: s21_sprintf.c s21_string.h
	$(CC) $(CFLAGS) -c s21_sprintf.c

main.o: main.c s21_string.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o run

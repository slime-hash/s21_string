CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror
CHECK_CFLAGS = $(shell pkg-config --cflags check 2>/dev/null)
CHECK_LIBS = $(shell pkg-config --libs check 2>/dev/null)
ifeq ($(strip $(CHECK_LIBS)),)
CHECK_LIBS = -lcheck -lpthread -lrt -lm -lsubunit
endif

.PHONY: all re test clean

all: s21_sprintf.o

re:
	$(MAKE) clean
	$(MAKE) all

test: s21_sprintf.o s21_sprintf_tests.o
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) s21_sprintf.o s21_sprintf_tests.o -o test_sprintf $(CHECK_LIBS)
	./test_sprintf

s21_sprintf.o: s21_sprintf.c s21_string.h
	$(CC) $(CFLAGS) -c s21_sprintf.c

s21_sprintf_tests.o: s21_sprintf_tests.c s21_string.h
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) -c s21_sprintf_tests.c

clean:
	rm -f *.o test_sprintf

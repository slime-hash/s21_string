#ifndef S21_STRING_H_
#define S21_STRING_H_

#include <stdarg.h>

typedef struct {
  int minus;
  int plus;
  int space;
  int zero;
  int width;
  int precision;
  char length;
  char spec;
} s21_fmt_t;

int s21_sprintf(char *str, const char *format, ...);

#endif  // S21_STRING_H_

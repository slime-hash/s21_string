#ifndef S21_STRING_H_
#define S21_STRING_H_

#include <stdarg.h>

typedef struct {
  int minus;
  int width;
  int precision;  // СКОЛЬКО символов максимум взять (или -1 = не ограничивать)
  char spec;      // например 's'
} s21_fmt_t;

int s21_sprintf(char *str, const char *format, ...);

void s21_parse_fmt(const char *format, int *i, s21_fmt_t *fmt);

#endif  // S21_STRING_H_

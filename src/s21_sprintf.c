#include <stdarg.h>
#include "s21_string.h"

static int s21_is_digit(char c) { return c >= '0' && c <= '9'; }

static int s21_strlen_local(const char *s) {
  int n = 0;
  while (s[n] != '\0') n++;
  return n;
}

void s21_parse_fmt(const char *format, int *i, s21_fmt_t *fmt) {
  // init
  fmt->minus = 0;
  fmt->width = 0;
  fmt->precision = -1;
  fmt->spec = '\0';

  // flag '-'
  if (format[*i] == '-') {
    fmt->minus = 1;
    (*i)++;
  }

  // width: digits
  while (s21_is_digit(format[*i])) {
    fmt->width = fmt->width * 10 + (format[*i] - '0');
    (*i)++;
  }

  // precision: '.' digits ('.' without digits => 0)
  if (format[*i] == '.') {
    fmt->precision = 0;
    (*i)++;
    while (s21_is_digit(format[*i])) {
      fmt->precision = fmt->precision * 10 + (format[*i] - '0');
      (*i)++;
    }
  }

  // spec
  fmt->spec = format[*i];
  if (format[*i] != '\0') (*i)++;
}

static void s21_format_s(char *str, int *j, s21_fmt_t fmt, const char *arg) {
  int len = s21_strlen_local(arg);

  if (fmt.precision != -1 && fmt.precision < len) {
    len = fmt.precision;
  }

  int pad = 0;
  if (fmt.width > len) pad = fmt.width - len;

  if (!fmt.minus) {
    for (int p = 0; p < pad; p++) str[(*j)++] = ' ';
  }

  for (int k = 0; k < len; k++) str[(*j)++] = arg[k];

  if (fmt.minus) {
    for (int p = 0; p < pad; p++) str[(*j)++] = ' ';
  }
}

int s21_sprintf(char *str, const char *format, ...) {
  int i = 0;  // читаем format
  int j = 0;  // пишем в str

  va_list ap;
  va_start(ap, format);

  while (format[i] != '\0') {
    if (format[i] == '%' && format[i + 1] == '%') {
      str[j++] = '%';
      i += 2;
      continue;
    }

    if (format[i] == '%') {
      s21_fmt_t fmt;
      i++;  // пропускаем '%'
      s21_parse_fmt(format, &i, &fmt);  // i указывает на символ ПОСЛЕ spec

      if (fmt.spec == 's') {
        char *arg = va_arg(ap, char *);
        s21_format_s(str, &j, fmt, arg);
      } else {
        // пока не поддерживаем — можно вывести как текст "%<spec>"
        // (это временно, пока не реализованы остальные спецификаторы)
        str[j++] = '%';
        if (fmt.spec != '\0') str[j++] = fmt.spec;
      }
      continue;
    }

    str[j++] = format[i++];
  }

  va_end(ap);
  str[j] = '\0';
  return j;
}

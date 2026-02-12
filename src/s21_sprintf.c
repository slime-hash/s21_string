#include <stdarg.h>
#include <math.h>
#include "s21_string.h"

static void parse_format(const char *fmt, int *i, s21_fmt_t *f) {
  f->minus = f->plus = f->space = f->zero = 0;
  f->width = 0;
  f->precision = -1;
  f->length = 0;

  // flags
  while (fmt[*i] == '-' || fmt[*i] == '+' || fmt[*i] == ' ' || fmt[*i] == '0') {
    if (fmt[*i] == '-') f->minus = 1;
    if (fmt[*i] == '+') f->plus = 1;
    if (fmt[*i] == ' ') f->space = 1;
    if (fmt[*i] == '0') f->zero = 1;
    (*i)++;
  }

  // width
  while (fmt[*i] >= '0' && fmt[*i] <= '9') {
    f->width = f->width * 10 + (fmt[*i] - '0');
    (*i)++;
  }

  // precision
  if (fmt[*i] == '.') {
    f->precision = 0;
    (*i)++;
    while (fmt[*i] >= '0' && fmt[*i] <= '9') {
      f->precision = f->precision * 10 + (fmt[*i] - '0');
      (*i)++;
    }
  }

  // length
  if (fmt[*i] == 'h' || fmt[*i] == 'l') {
    f->length = fmt[*i];
    (*i)++;
  }

  // spec
  f->spec = fmt[*i];
  (*i)++;
}

static void print_spaces(char *str, int *j, int count) {
  for (int i = 0; i < count; i++)
    str[(*j)++] = ' ';
}

static void print_zeros(char *str, int *j, int count) {
  for (int i = 0; i < count; i++)
    str[(*j)++] = '0';
}

static int to_digits_ull(char *buf, unsigned long long x) {
  int i = 0;
  do {
    buf[i++] = (char)('0' + (x % 10));
    x /= 10;
  } while (x);
  return i;
}

static void print_char(char *str, int *j, s21_fmt_t f, char c) {
  int pad = f.width > 1 ? f.width - 1 : 0;

  if (!f.minus) print_spaces(str, j, pad);
  str[(*j)++] = c;
  if (f.minus) print_spaces(str, j, pad);
}

static void print_int(char *str, int *j, s21_fmt_t f, long long x) {
  char buf[32];
  int neg = x < 0;
  unsigned long long abs_x = neg ? (unsigned long long)(-(x + 1)) + 1ULL : (unsigned long long)x;

  int digits_len = to_digits_ull(buf, abs_x);  // без знака

  if (digits_len == 1 && buf[0] == '0' && f.precision == 0)
    digits_len = 0;

  int zeros = 0;
  if (f.precision > digits_len)
    zeros = f.precision - digits_len;
  
  char sign = 0;
  if (neg) sign = '-';
  else if (f.plus) sign = '+';
  else if (f.space) sign = ' ';

  int total_len = digits_len + zeros + (sign ? 1 : 0);
  int pad = f.width > total_len ? f.width - total_len : 0;
  int zero_pad = f.zero && !f.minus && f.precision == -1;
  
  if (!f.minus) {
    if (zero_pad) {
      if (sign) {
        str[(*j)++] = sign;
        sign = 0;
      }
      print_zeros(str, j, pad);
    } else {
      print_spaces(str, j, pad);
    }
  }

  if (sign) str[(*j)++] = sign;

  print_zeros(str, j, zeros);

  while (digits_len--) str[(*j)++] = buf[digits_len];

  if (f.minus) print_spaces(str, j, pad);
}

static void print_float(char *str, int *j, s21_fmt_t f, double x) {
  char int_buf[32];
  char frac_buf[32];

  if (f.precision == -1) f.precision = 6;
  if (f.precision > 18) f.precision = 18;

  char sign = 0;
  if (x < 0) {
    sign = '-';
    x = -x;
  } else if (f.plus) {
    sign = '+';
  } else if (f.space) {
    sign = ' ';
  }

  unsigned long long scale = 1;
  for (int i = 0; i < f.precision; i++) scale *= 10ULL;
  unsigned long long rounded = (unsigned long long)llrintl((long double)x * (long double)scale);
  unsigned long long int_part = rounded / scale;
  unsigned long long frac_int = rounded % scale;

  int int_len = to_digits_ull(int_buf, int_part);
  int frac_len = f.precision;

  for (int k = frac_len - 1; k >= 0; k--) {
    frac_buf[k] = (char)('0' + (frac_int % 10));
    frac_int /= 10ULL;
  }

  int number_len = int_len + (frac_len > 0 ? 1 + frac_len : 0);
  int total_len = number_len + (sign ? 1 : 0);
  int pad = f.width > total_len ? f.width - total_len : 0;
  int zero_pad = f.zero && !f.minus;

  if (!f.minus) {
    if (zero_pad) {
      if (sign) {
        str[(*j)++] = sign;
        sign = 0;
      }
      print_zeros(str, j, pad);
    } else {
      print_spaces(str, j, pad);
    }
  }

  if (sign) str[(*j)++] = sign;

  while (int_len--) str[(*j)++] = int_buf[int_len];

  if (frac_len > 0) {
    str[(*j)++] = '.';
    for (int k = 0; k < frac_len; k++) str[(*j)++] = frac_buf[k];
  }

  if (f.minus) print_spaces(str, j, pad);
}

static void print_string(char *str, int *j, s21_fmt_t f, char *s) {
  int len = 0;
  while (s[len] && (f.precision == -1 || len < f.precision))
    len++;

  int pad = f.width > len ? f.width - len : 0;

  if (!f.minus) print_spaces(str, j, pad);
  for (int i = 0; i < len; i++)
    str[(*j)++] = s[i];
  if (f.minus) print_spaces(str, j, pad);
}

static void print_uint(char *str, int *j, s21_fmt_t f, unsigned long long x) {
  char buf[32];
  int digits_len = to_digits_ull(buf, x);

  if (digits_len == 1 && buf[0] == '0' && f.precision == 0)
    digits_len = 0;

  int zeros = 0;
  if (f.precision > digits_len)
    zeros = f.precision - digits_len;

  int total_len = digits_len + zeros;
  int pad = f.width > total_len ? f.width - total_len : 0;
  int zero_pad = f.zero && !f.minus && f.precision == -1;

  if (!f.minus) {
    if (zero_pad) print_zeros(str, j, pad);
    else print_spaces(str, j, pad);
  }

  print_zeros(str, j, zeros);

  while (digits_len--) str[(*j)++] = buf[digits_len];

  if (f.minus) print_spaces(str, j, pad);
}

int s21_sprintf(char *str, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int i = 0, j = 0;

    while (fmt[i]) {
        if (fmt[i] != '%') {
            str[j++] = fmt[i++];
            continue;
        }
        i++;
        s21_fmt_t f;
        parse_format(fmt, &i, &f);

        if (f.spec == 'c') {
            print_char(str, &j, f, va_arg(ap, int));
        }
        else if (f.spec == 's') {
            char *s = va_arg(ap, char *);
            if (!s) s = "(null)";
            print_string(str, &j, f, s);
        }
        else if (f.spec == 'd') {
            long long val;
            if (f.length == 'h') val = (short)va_arg(ap, int);
            else if (f.length == 'l') val = va_arg(ap, long);
            else val = va_arg(ap, int);
            print_int(str, &j, f, val);
        }
        else if (f.spec == 'u') {
            unsigned long long val;
            if (f.length == 'h') val = (unsigned short)va_arg(ap, unsigned int);
            else if (f.length == 'l') val = va_arg(ap, unsigned long);
            else val = va_arg(ap, unsigned int);
            print_uint(str, &j, f, val);
        }
        else if (f.spec == 'f') {
            double val = va_arg(ap, double);
            print_float(str, &j, f, val);
        }
        else if (f.spec == '%') {
            str[j++] = '%';
        }
    }

    str[j] = '\0';
    va_end(ap);
    return j;
}

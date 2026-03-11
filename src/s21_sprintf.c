#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>

#include "s21_sprintf.h"
#include "s21_string.h"

static void parse_format(const char *fmt, int *i, s21_fmt_t *f) {
  f->minus = f->plus = f->space = f->zero = 0;
  f->width = 0;
  f->precision = -1;
  f->length = 0;

  while (fmt[*i] == '-' || fmt[*i] == '+' || fmt[*i] == ' ' || fmt[*i] == '0') {
    if (fmt[*i] == '-') f->minus = 1;
    if (fmt[*i] == '+') f->plus = 1;
    if (fmt[*i] == ' ') f->space = 1;
    if (fmt[*i] == '0') f->zero = 1;
    (*i)++;
  }

  while (fmt[*i] >= '0' && fmt[*i] <= '9') {
    f->width = f->width * 10 + (fmt[*i] - '0');
    (*i)++;
  }

  if (fmt[*i] == '.') {
    f->precision = 0;
    (*i)++;
    while (fmt[*i] >= '0' && fmt[*i] <= '9') {
      f->precision = f->precision * 10 + (fmt[*i] - '0');
      (*i)++;
    }
  }

  if (fmt[*i] == 'h' || fmt[*i] == 'l') {
    f->length = fmt[*i];
    (*i)++;
  }

  f->spec = fmt[*i];
  (*i)++;
}

static void print_spaces(char *str, int *j, int count) {
  for (int i = 0; i < count; i++) str[(*j)++] = ' ';
}

static void print_zeros(char *str, int *j, int count) {
  for (int i = 0; i < count; i++) str[(*j)++] = '0';
}

static void emit_padded(char *str, int *j, s21_fmt_t f, char sign,
                        const char *body, int body_len) {
  int total_len = body_len + (sign ? 1 : 0);
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
  for (int k = 0; k < body_len; k++) str[(*j)++] = body[k];
  if (f.minus) print_spaces(str, j, pad);
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

static int wchar_to_mb(char *out, size_t cap, wchar_t wc) {
  mbstate_t st;
  memset(&st, 0, sizeof(st));
  size_t n = wcrtomb(out, wc, &st);
  int res = (n == (size_t)-1 || n >= cap) ? -1 : (int)n;
  return res;
}

static void print_wchar(char *str, int *j, s21_fmt_t f, wint_t wc) {
  char mb[MB_LEN_MAX];
  int len = wchar_to_mb(mb, sizeof(mb), (wchar_t)wc);
  int pad = (len > 0 && f.width > len) ? f.width - len : 0;

  if (len > 0) {
    if (!f.minus) print_spaces(str, j, pad);
    for (int k = 0; k < len; k++) str[(*j)++] = mb[k];
    if (f.minus) print_spaces(str, j, pad);
  }
}

static void print_int(char *str, int *j, s21_fmt_t f, long long x) {
  char buf[32];
  int neg = x < 0;
  unsigned long long abs_x =
      neg ? (unsigned long long)(-(x + 1)) + 1ULL : (unsigned long long)x;

  int digits_len = to_digits_ull(buf, abs_x);

  if (digits_len == 1 && buf[0] == '0' && f.precision == 0) digits_len = 0;

  int zeros = 0;
  if (f.precision > digits_len) zeros = f.precision - digits_len;

  char sign = 0;
  if (neg)
    sign = '-';
  else if (f.plus)
    sign = '+';
  else if (f.space)
    sign = ' ';

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

static void format_float_parts(double x, int precision, s21_fmt_t f,
                               char *int_buf, int *int_len, char *frac_buf,
                               int *frac_len, char *sign) {
  if (precision == -1) precision = 6;
  if (precision > 18) precision = 18;

  *sign = 0;
  if (signbit(x)) {
    *sign = '-';
    x = -x;
  } else if (f.plus) {
    *sign = '+';
  } else if (f.space) {
    *sign = ' ';
  }

  unsigned long long scale = 1;
  for (int i = 0; i < precision; i++) scale *= 10ULL;
  unsigned long long rounded =
      (unsigned long long)llrintl((long double)x * (long double)scale);
  unsigned long long int_part = rounded / scale;
  unsigned long long frac_int = rounded % scale;

  *int_len = to_digits_ull(int_buf, int_part);
  *frac_len = precision;

  for (int k = precision - 1; k >= 0; k--) {
    frac_buf[k] = (char)('0' + (frac_int % 10));
    frac_int /= 10ULL;
  }
}

static void print_float(char *str, int *j, s21_fmt_t f, double x) {
  if (isnan(x) || isinf(x)) {
    char sign = signbit(x) ? '-' : (f.plus ? '+' : (f.space ? ' ' : 0));
    const char *word = isnan(x) ? "nan" : "inf";
    emit_padded(str, j, f, sign, word, 3);
  } else {
    char int_buf[32], frac_buf[32], out[64], sign;
    int int_len, frac_len, pos = 0;
    format_float_parts(x, f.precision, f, int_buf, &int_len, frac_buf, &frac_len,
                       &sign);
    while (int_len--) out[pos++] = int_buf[int_len];
    if (frac_len > 0) {
      out[pos++] = '.';
      memcpy(out + pos, frac_buf, (size_t)frac_len);
      pos += frac_len;
    }
    emit_padded(str, j, f, sign, out, pos);
  }
}

static void print_string(char *str, int *j, s21_fmt_t f, char *s) {
  int len = 0;
  while (s[len] && (f.precision == -1 || len < f.precision)) len++;

  int pad = f.width > len ? f.width - len : 0;

  if (!f.minus) print_spaces(str, j, pad);
  for (int i = 0; i < len; i++) str[(*j)++] = s[i];
  if (f.minus) print_spaces(str, j, pad);
}

static void print_wstring(char *str, int *j, s21_fmt_t f, const wchar_t *ws) {
  if (!ws) ws = L"(null)";

  char tmp[4096];
  int pos = 0;
  int stop = 0;
  int emit = 1;

  for (int i = 0; ws[i] != L'\0' && !stop; i++) {
    char mb[MB_LEN_MAX];
    int len = wchar_to_mb(mb, sizeof(mb), ws[i]);
    if (len <= 0) {
      pos = 0;
      stop = 1;
      emit = 0;
    }
    if (!stop && f.precision != -1 && pos + len > f.precision) stop = 1;
    if (!stop && pos + len >= (int)sizeof(tmp) - 1) stop = 1;
    if (!stop) {
      for (int k = 0; k < len; k++) tmp[pos++] = mb[k];
    }
  }

  tmp[pos] = '\0';

  if (emit) {
    s21_fmt_t f2 = f;
    f2.precision = -1;
    print_string(str, j, f2, tmp);
  }
}

static void print_uint(char *str, int *j, s21_fmt_t f, unsigned long long x) {
  char buf[32];
  int digits_len = to_digits_ull(buf, x);

  if (digits_len == 1 && buf[0] == '0' && f.precision == 0) digits_len = 0;

  int zeros = 0;
  if (f.precision > digits_len) zeros = f.precision - digits_len;

  int total_len = digits_len + zeros;
  int pad = f.width > total_len ? f.width - total_len : 0;
  int zero_pad = f.zero && !f.minus && f.precision == -1;

  if (!f.minus) {
    if (zero_pad)
      print_zeros(str, j, pad);
    else
      print_spaces(str, j, pad);
  }

  print_zeros(str, j, zeros);

  while (digits_len--) str[(*j)++] = buf[digits_len];

  if (f.minus) print_spaces(str, j, pad);
}

static long long get_signed_arg(va_list *ap, s21_fmt_t f) {
  long long res;
  if (f.length == 'h')
    res = (short)va_arg(*ap, int);
  else if (f.length == 'l')
    res = va_arg(*ap, long);
  else
    res = va_arg(*ap, int);
  return res;
}

static unsigned long long get_unsigned_arg(va_list *ap, s21_fmt_t f) {
  unsigned long long res;
  if (f.length == 'h')
    res = (unsigned short)va_arg(*ap, unsigned int);
  else if (f.length == 'l')
    res = va_arg(*ap, unsigned long);
  else
    res = va_arg(*ap, unsigned int);
  return res;
}

static void handle_spec(char *str, int *j, s21_fmt_t f, va_list *ap) {
  if (f.spec == 'c') {
    if (f.length == 'l')
      print_wchar(str, j, f, va_arg(*ap, wint_t));
    else
      print_char(str, j, f, va_arg(*ap, int));
  } else if (f.spec == 's') {
    if (f.length == 'l') {
      print_wstring(str, j, f, va_arg(*ap, const wchar_t *));
    } else {
      char *s = va_arg(*ap, char *);
      if (!s) s = "(null)";
      print_string(str, j, f, s);
    }
  } else if (f.spec == 'd') {
    print_int(str, j, f, get_signed_arg(ap, f));
  } else if (f.spec == 'u') {
    print_uint(str, j, f, get_unsigned_arg(ap, f));
  } else if (f.spec == 'f') {
    print_float(str, j, f, va_arg(*ap, double));
  } else if (f.spec == '%') {
    str[(*j)++] = '%';
  }
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
    handle_spec(str, &j, f, &ap);
  }
  str[j] = '\0';
  va_end(ap);
  return j;
}

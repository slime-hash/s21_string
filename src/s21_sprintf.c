#include "s21_string.h"

int s21_sprintf(char *str, const char *format, ...) {
  int i = 0;  // читаем format
  int j = 0;  // пишем в str

  while (format[i] != '\0') {
    if (format[i] == '%' && format[i + 1] == '%') {
      str[j] = '%';
      i += 2;
			j += 1;
    } else if (format[i] == '%') {
			//смотрим на следующий символ?
		} else {
			str[j] = format[i];
			i += 1;
			j += 1;
    }
  }

  str[j] = '\0';
  return j;
}

#include <stdio.h>
#include "s21_string.h"

int main(void) {
  char buf[100];

  int n1 = s21_sprintf(buf, "");
  printf("'%s' | len=%d\n", buf, n1);

  int n2 = s21_sprintf(buf, "abc");
  printf("'%s' | len=%d\n", buf, n2);

  int n3 = s21_sprintf(buf, "hello world");
  printf("'%s' | len=%d\n", buf, n3);

  int n4 = s21_sprintf(buf, "100%");
  printf("'%s' | len=%d\n", buf, n4);

  int n5 = s21_sprintf(buf, "%%");
  printf("'%s' | len=%d\n", buf, n5);

  int n6 = s21_sprintf(buf, "abc%%def");
  printf("'%s' | len=%d\n", buf, n6);

  // --- %s базовые ---
  int n7 = s21_sprintf(buf, "hello %s", "world");
  printf("'%s' | len=%d\n", buf, n7);

  int n8 = s21_sprintf(buf, "%s%s", "ab", "cd");
  printf("'%s' | len=%d\n", buf, n8);

  int n9 = s21_sprintf(buf, "%% %s %%", "ok");
  printf("'%s' | len=%d\n", buf, n9);

  // --- width для %s ---
  int n10 = s21_sprintf(buf, "%8s", "hi");
  printf("'%s' | len=%d\n", buf, n10);

  int n11 = s21_sprintf(buf, "%-8s", "hi");
  printf("'%s' | len=%d\n", buf, n11);

  // --- precision для %s ---
  int n12 = s21_sprintf(buf, "%.3s", "hello");
  printf("'%s' | len=%d\n", buf, n12);

  int n13 = s21_sprintf(buf, "%.0s", "hello");
  printf("'%s' | len=%d\n", buf, n13);

  // --- width + precision ---
  int n14 = s21_sprintf(buf, "%8.3s", "hello");
  printf("'%s' | len=%d\n", buf, n14);

  int n15 = s21_sprintf(buf, "%-8.3s", "hello");
  printf("'%s' | len=%d\n", buf, n15);

  // --- комбинации в тексте ---
  int n16 = s21_sprintf(buf, "a:%8.3s b:%-6s!", "abcdef", "xy");
  printf("'%s' | len=%d\n", buf, n16);

  return 0;
}

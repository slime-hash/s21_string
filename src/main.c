#include <stdio.h>
#include "s21_string.h"

int main() {
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
	
  return 0;
}

#include <check.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "s21_string.h"

#define BUFF 512

static int rand_between(int min, int max) {
    return min + rand() % (max - min + 1);
}

static unsigned int rand_u32(void) {
    unsigned int hi = (unsigned int)(rand() & 0xFFFF);
    unsigned int lo = (unsigned int)(rand() & 0xFFFF);
    return (hi << 16) | lo;
}

static unsigned long rand_ul(void) {
    unsigned long val = (unsigned long)rand_u32();
    val = (val << 16) ^ (unsigned long)(rand() & 0xFFFF);
    return val;
}

static void seed_iter(unsigned int base, int iter) {
    srand(base + (unsigned int)iter * 7919U);
}

static void assert_fmt_equal(const char *fmt, int iter, int n1, int n2,
                             const char *buf1, const char *buf2) {
    ck_assert_msg(n1 == n2,
                  "iter=%d fmt=\"%s\" length mismatch: std=%d s21=%d std=\"%s\" s21=\"%s\"",
                  iter, fmt, n1, n2, buf1, buf2);
    ck_assert_msg(strcmp(buf1, buf2) == 0,
                  "iter=%d fmt=\"%s\" output mismatch: std=\"%s\" s21=\"%s\"",
                  iter, fmt, buf1, buf2);
}

/* ========== ТЕСТЫ ДЛЯ SPRINTF ========== */

START_TEST(test_simple_string) {
    char buf1[BUFF], buf2[BUFF];
    int n1 = sprintf(buf1, "Hello World!");
    int n2 = s21_sprintf(buf2, "Hello World!");
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_percent_d) {
    char buf1[BUFF], buf2[BUFF];
    int val = 123;
    int n1 = sprintf(buf1, "Value: %d", val);
    int n2 = s21_sprintf(buf2, "Value: %d", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_percent_u) {
    char buf1[BUFF], buf2[BUFF];
    unsigned int val = 4000000000U;
    int n1 = sprintf(buf1, "Unsigned: %u", val);
    int n2 = s21_sprintf(buf2, "Unsigned: %u", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_percent_c) {
    char buf1[BUFF], buf2[BUFF];
    char val = 'X';
    int n1 = sprintf(buf1, "Char: %c", val);
    int n2 = s21_sprintf(buf2, "Char: %c", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_percent_s) {
    char buf1[BUFF], buf2[BUFF];
    char *val = "Test String";
    int n1 = sprintf(buf1, "String: %s", val);
    int n2 = s21_sprintf(buf2, "String: %s", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_percent_f) {
    char buf1[BUFF], buf2[BUFF];
    double val = 3.141592;
    int n1 = sprintf(buf1, "Float: %.3f", val);
    int n2 = s21_sprintf(buf2, "Float: %.3f", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_percent_percent) {
    char buf1[BUFF], buf2[BUFF];
    int n1 = sprintf(buf1, "Percent: %%");
    int n2 = s21_sprintf(buf2, "Percent: %%");
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_flags_plus_space) {
    char buf1[BUFF], buf2[BUFF];
    int val = 42;
    int n1 = sprintf(buf1, "Plus: %+d Space: % d", val, val);
    int n2 = s21_sprintf(buf2, "Plus: %+d Space: % d", val, val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_width_precision) {
    char buf1[BUFF], buf2[BUFF];
    int val = 7;
    double fval = 2.71828;
    int n1 = sprintf(buf1, "W:%5d P:%.2f", val, fval);
    int n2 = s21_sprintf(buf2, "W:%5d P:%.2f", val, fval);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_minus_flag) {
    char buf1[BUFF], buf2[BUFF];
    int val = 99;
    int n1 = sprintf(buf1, "Left: %-5dEnd", val);
    int n2 = s21_sprintf(buf2, "Left: %-5dEnd", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_null_string) {
    char buf1[BUFF], buf2[BUFF];
    char *val = NULL;
    int n1 = sprintf(buf1, "Null: %s", val);
    int n2 = s21_sprintf(buf2, "Null: %s", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_combined) {
    char buf1[BUFF], buf2[BUFF];
    int n1 = sprintf(buf1, "Int:%+5d Uns:%08u Char:%c Str:%s Float:%.4f %%", -42, 300, 'Z', "Hi!", 3.1415);
    int n2 = s21_sprintf(buf2, "Int:%+5d Uns:%08u Char:%c Str:%s Float:%.4f %%", -42, 300, 'Z', "Hi!", 3.1415);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_percent_d_negative) {
    char buf1[BUFF], buf2[BUFF];
    int val = -9876;
    int n1 = sprintf(buf1, "Neg: %d", val);
    int n2 = s21_sprintf(buf2, "Neg: %d", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_percent_d_zero) {
    char buf1[BUFF], buf2[BUFF];
    int val = 0;
    int n1 = sprintf(buf1, "Zero: %d", val);
    int n2 = s21_sprintf(buf2, "Zero: %d", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_width_right) {
    char buf1[BUFF], buf2[BUFF];
    int val = 42;
    int n1 = sprintf(buf1, "[%8d]", val);
    int n2 = s21_sprintf(buf2, "[%8d]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_width_left) {
    char buf1[BUFF], buf2[BUFF];
    int val = 42;
    int n1 = sprintf(buf1, "[%-8d]", val);
    int n2 = s21_sprintf(buf2, "[%-8d]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_precision) {
    char buf1[BUFF], buf2[BUFF];
    int val = 42;
    int n1 = sprintf(buf1, "[%.5d]", val);
    int n2 = s21_sprintf(buf2, "[%.5d]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_width_precision) {
    char buf1[BUFF], buf2[BUFF];
    int val = 42;
    int n1 = sprintf(buf1, "[%8.5d]", val);
    int n2 = s21_sprintf(buf2, "[%8.5d]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_zero_flag) {
    char buf1[BUFF], buf2[BUFF];
    int val = 42;
    int n1 = sprintf(buf1, "[%08d]", val);
    int n2 = s21_sprintf(buf2, "[%08d]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_zero_flag_negative) {
    char buf1[BUFF], buf2[BUFF];
    int val = -42;
    int n1 = sprintf(buf1, "[%08d]", val);
    int n2 = s21_sprintf(buf2, "[%08d]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_plus_zero_flag) {
    char buf1[BUFF], buf2[BUFF];
    int val = 42;
    int n1 = sprintf(buf1, "[%+08d]", val);
    int n2 = s21_sprintf(buf2, "[%+08d]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_space_flag) {
    char buf1[BUFF], buf2[BUFF];
    int val = 42;
    int n1 = sprintf(buf1, "[% 8d]", val);
    int n2 = s21_sprintf(buf2, "[% 8d]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_precision_zero_value) {
    char buf1[BUFF], buf2[BUFF];
    int val = 0;
    int n1 = sprintf(buf1, "[%.0d]", val);
    int n2 = s21_sprintf(buf2, "[%.0d]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_length_h) {
    char buf1[BUFF], buf2[BUFF];
    short val = -1234;
    int n1 = sprintf(buf1, "short:%hd", val);
    int n2 = s21_sprintf(buf2, "short:%hd", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_length_l) {
    char buf1[BUFF], buf2[BUFF];
    long val = 1234567890L;
    int n1 = sprintf(buf1, "long:%ld", val);
    int n2 = s21_sprintf(buf2, "long:%ld", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_d_int_min) {
    char buf1[BUFF], buf2[BUFF];
    int val = INT_MIN;
    int n1 = sprintf(buf1, "min:%d", val);
    int n2 = s21_sprintf(buf2, "min:%d", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_percent_u_zero) {
    char buf1[BUFF], buf2[BUFF];
    unsigned int val = 0;
    int n1 = sprintf(buf1, "Unsigned zero: %u", val);
    int n2 = s21_sprintf(buf2, "Unsigned zero: %u", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_u_width) {
    char buf1[BUFF], buf2[BUFF];
    unsigned int val = 123U;
    int n1 = sprintf(buf1, "[%8u]", val);
    int n2 = s21_sprintf(buf2, "[%8u]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_u_minus_width) {
    char buf1[BUFF], buf2[BUFF];
    unsigned int val = 123U;
    int n1 = sprintf(buf1, "[%-8u]", val);
    int n2 = s21_sprintf(buf2, "[%-8u]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_u_precision) {
    char buf1[BUFF], buf2[BUFF];
    unsigned int val = 123U;
    int n1 = sprintf(buf1, "[%.6u]", val);
    int n2 = s21_sprintf(buf2, "[%.6u]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_u_width_precision) {
    char buf1[BUFF], buf2[BUFF];
    unsigned int val = 123U;
    int n1 = sprintf(buf1, "[%10.6u]", val);
    int n2 = s21_sprintf(buf2, "[%10.6u]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_u_zero_flag) {
    char buf1[BUFF], buf2[BUFF];
    unsigned int val = 123U;
    int n1 = sprintf(buf1, "[%010u]", val);
    int n2 = s21_sprintf(buf2, "[%010u]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_u_precision_zero_value) {
    char buf1[BUFF], buf2[BUFF];
    unsigned int val = 0U;
    int n1 = sprintf(buf1, "[%.0u]", val);
    int n2 = s21_sprintf(buf2, "[%.0u]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_u_length_h) {
    char buf1[BUFF], buf2[BUFF];
    unsigned short val = 65535U;
    int n1 = sprintf(buf1, "ushort:%hu", val);
    int n2 = s21_sprintf(buf2, "ushort:%hu", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_u_length_l) {
    char buf1[BUFF], buf2[BUFF];
    unsigned long val = 4000000000UL;
    int n1 = sprintf(buf1, "ulong:%lu", val);
    int n2 = s21_sprintf(buf2, "ulong:%lu", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_s_width_right) {
    char buf1[BUFF], buf2[BUFF];
    const char *val = "abc";
    int n1 = sprintf(buf1, "[%8s]", val);
    int n2 = s21_sprintf(buf2, "[%8s]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_s_width_left) {
    char buf1[BUFF], buf2[BUFF];
    const char *val = "abc";
    int n1 = sprintf(buf1, "[%-8s]", val);
    int n2 = s21_sprintf(buf2, "[%-8s]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_s_precision) {
    char buf1[BUFF], buf2[BUFF];
    const char *val = "abcdef";
    int n1 = sprintf(buf1, "[%.3s]", val);
    int n2 = s21_sprintf(buf2, "[%.3s]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_s_width_precision) {
    char buf1[BUFF], buf2[BUFF];
    const char *val = "abcdef";
    int n1 = sprintf(buf1, "[%8.3s]", val);
    int n2 = s21_sprintf(buf2, "[%8.3s]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_s_empty) {
    char buf1[BUFF], buf2[BUFF];
    const char *val = "";
    int n1 = sprintf(buf1, "[%5s]", val);
    int n2 = s21_sprintf(buf2, "[%5s]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_c_width_right) {
    char buf1[BUFF], buf2[BUFF];
    char val = 'Q';
    int n1 = sprintf(buf1, "[%4c]", val);
    int n2 = s21_sprintf(buf2, "[%4c]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_c_width_left) {
    char buf1[BUFF], buf2[BUFF];
    char val = 'Q';
    int n1 = sprintf(buf1, "[%-4c]", val);
    int n2 = s21_sprintf(buf2, "[%-4c]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_f_default_precision) {
    char buf1[BUFF], buf2[BUFF];
    double val = 1.5;
    int n1 = sprintf(buf1, "f:%f", val);
    int n2 = s21_sprintf(buf2, "f:%f", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_f_precision_zero) {
    char buf1[BUFF], buf2[BUFF];
    double val = 3.6;
    int n1 = sprintf(buf1, "[%.0f]", val);
    int n2 = s21_sprintf(buf2, "[%.0f]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_f_width) {
    char buf1[BUFF], buf2[BUFF];
    double val = 12.34;
    int n1 = sprintf(buf1, "[%10.2f]", val);
    int n2 = s21_sprintf(buf2, "[%10.2f]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_f_minus_width) {
    char buf1[BUFF], buf2[BUFF];
    double val = 12.34;
    int n1 = sprintf(buf1, "[%-10.2f]", val);
    int n2 = s21_sprintf(buf2, "[%-10.2f]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_f_zero_flag_width) {
    char buf1[BUFF], buf2[BUFF];
    double val = 12.34;
    int n1 = sprintf(buf1, "[%010.2f]", val);
    int n2 = s21_sprintf(buf2, "[%010.2f]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_f_plus_and_space) {
    char buf1[BUFF], buf2[BUFF];
    double val = 12.34;
    int n1 = sprintf(buf1, "plus:[%+8.2f] space:[% 8.2f]", val, val);
    int n2 = s21_sprintf(buf2, "plus:[%+8.2f] space:[% 8.2f]", val, val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_f_round_carry) {
    char buf1[BUFF], buf2[BUFF];
    double val = 9.999;
    int n1 = sprintf(buf1, "[%.2f]", val);
    int n2 = s21_sprintf(buf2, "[%.2f]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_f_negative_zero_flag) {
    char buf1[BUFF], buf2[BUFF];
    double val = -3.5;
    int n1 = sprintf(buf1, "[%010.2f]", val);
    int n2 = s21_sprintf(buf2, "[%010.2f]", val);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_multi_length_mix) {
    char buf1[BUFF], buf2[BUFF];
    short hs = -12;
    long ld = 1234567L;
    unsigned short hu = 65000U;
    unsigned long lu = 4000000000UL;
    int n1 = sprintf(buf1, "hs:%hd ld:%ld hu:%hu lu:%lu", hs, ld, hu, lu);
    int n2 = s21_sprintf(buf2, "hs:%hd ld:%ld hu:%hu lu:%lu", hs, ld, hu, lu);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_many_specifiers_sequence) {
    char buf1[BUFF], buf2[BUFF];
    int n1 = sprintf(buf1, "%d|%u|%c|%s|%.2f|%%|%+05d|%-6u", -1, 42U, 'A', "ok", 2.5, 7, 9U);
    int n2 = s21_sprintf(buf2, "%d|%u|%c|%s|%.2f|%%|%+05d|%-6u", -1, 42U, 'A', "ok", 2.5, 7, 9U);
    ck_assert_int_eq(n1, n2);
    ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_stress_random_d_formats) {
    char buf1[BUFF], buf2[BUFF];
    const char *formats[] = {
        "[%d]", "[%5d]", "[%-5d]", "[%+8d]", "[% 8d]", "[%08d]",
        "[%.3d]", "[%8.3d]", "[%+08d]", "[%8.5d]", "[%hd]", "[%ld]"
    };
    int count = (int)(sizeof(formats) / sizeof(formats[0]));
    seed_iter(20260212U, _i);

    const char *fmt = formats[rand() % count];
    int n1 = 0, n2 = 0;

    if (strstr(fmt, "hd")) {
        short val = (short)rand_between(-32768, 32767);
        n1 = sprintf(buf1, fmt, val);
        n2 = s21_sprintf(buf2, fmt, val);
    } else if (strstr(fmt, "ld")) {
        long val = (long)(rand_between(-1000000000, 1000000000));
        n1 = sprintf(buf1, fmt, val);
        n2 = s21_sprintf(buf2, fmt, val);
    } else {
        int val = rand_between(-1000000000, 1000000000);
        n1 = sprintf(buf1, fmt, val);
        n2 = s21_sprintf(buf2, fmt, val);
    }

    assert_fmt_equal(fmt, _i, n1, n2, buf1, buf2);
}
END_TEST

START_TEST(test_stress_random_u_formats) {
    char buf1[BUFF], buf2[BUFF];
    const char *formats[] = {
        "[%u]", "[%8u]", "[%-8u]", "[%010u]", "[%.6u]", "[%10.6u]",
        "[%hu]", "[%lu]", "[%.0u]"
    };
    int count = (int)(sizeof(formats) / sizeof(formats[0]));
    seed_iter(20260213U, _i);

    const char *fmt = formats[rand() % count];
    int n1 = 0, n2 = 0;

    if (strstr(fmt, "hu")) {
        unsigned short val = (unsigned short)rand_u32();
        n1 = sprintf(buf1, fmt, val);
        n2 = s21_sprintf(buf2, fmt, val);
    } else if (strstr(fmt, "lu")) {
        unsigned long val = rand_ul();
        n1 = sprintf(buf1, fmt, val);
        n2 = s21_sprintf(buf2, fmt, val);
    } else {
        unsigned int val = rand_u32();
        n1 = sprintf(buf1, fmt, val);
        n2 = s21_sprintf(buf2, fmt, val);
    }

    assert_fmt_equal(fmt, _i, n1, n2, buf1, buf2);
}
END_TEST

START_TEST(test_stress_random_f_formats) {
    char buf1[BUFF], buf2[BUFF];
    const char *formats[] = {
        "[%f]", "[%.0f]", "[%.2f]", "[%8.2f]", "[%-8.2f]", "[%010.3f]",
        "[%+9.2f]", "[% 9.2f]"
    };
    int count = (int)(sizeof(formats) / sizeof(formats[0]));
    seed_iter(20260214U, _i);

    const char *fmt = formats[rand() % count];
    int num = rand_between(-50000, 50000);
    double val = (double)num / 16.0;
    int n1 = sprintf(buf1, fmt, val);
    int n2 = s21_sprintf(buf2, fmt, val);
    assert_fmt_equal(fmt, _i, n1, n2, buf1, buf2);
}
END_TEST

START_TEST(test_stress_random_s_c_formats) {
    char buf1[BUFF], buf2[BUFF];
    const char *str_formats[] = {"[%s]", "[%8s]", "[%-8s]", "[%.3s]", "[%8.3s]"};
    const char *samples[] = {"", "a", "abc", "hello", "with space", "s21_string", "long_text_123456"};
    int str_count = (int)(sizeof(str_formats) / sizeof(str_formats[0]));
    int sample_count = (int)(sizeof(samples) / sizeof(samples[0]));
    seed_iter(20260215U, _i);

    if (_i < 250) {
        const char *fmt = str_formats[rand() % str_count];
        const char *val = samples[rand() % sample_count];
        int n1 = sprintf(buf1, fmt, val);
        int n2 = s21_sprintf(buf2, fmt, val);
        assert_fmt_equal(fmt, _i, n1, n2, buf1, buf2);
    } else {
        const char *char_formats[] = {"[%c]", "[%4c]", "[%-4c]"};
        int char_count = (int)(sizeof(char_formats) / sizeof(char_formats[0]));
        const char *fmt = char_formats[rand() % char_count];
        char val = (char)rand_between(32, 126);
        int n1 = sprintf(buf1, fmt, val);
        int n2 = s21_sprintf(buf2, fmt, val);
        assert_fmt_equal(fmt, _i, n1, n2, buf1, buf2);
    }
}
END_TEST

START_TEST(test_stress_random_mixed) {
    char buf1[BUFF], buf2[BUFF];
    const char *fmt = "[%+08d|%010u|%-4c|%8.3s|%010.2f|%%]";
    const char *samples[] = {"abc", "hello", "codex", "42", "s21"};
    int sample_count = (int)(sizeof(samples) / sizeof(samples[0]));
    seed_iter(20260216U, _i);

    int d = rand_between(-999999, 999999);
    unsigned int u = rand_u32();
    char c = (char)rand_between(33, 126);
    const char *s = samples[rand() % sample_count];
    double f = (double)rand_between(-50000, 50000) / 16.0;
    int n1 = sprintf(buf1, fmt, d, u, c, s, f);
    int n2 = s21_sprintf(buf2, fmt, d, u, c, s, f);
    assert_fmt_equal(fmt, _i, n1, n2, buf1, buf2);
}
END_TEST

/* ========== НАБОР ТЕСТОВ ========== */

Suite *sprintf_test_suite(void) {
    Suite *s = suite_create("s21_sprintf_test");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, test_simple_string);
    tcase_add_test(tc, test_percent_d);
    tcase_add_test(tc, test_percent_u);
    tcase_add_test(tc, test_percent_c);
    tcase_add_test(tc, test_percent_s);
    tcase_add_test(tc, test_percent_f);
    tcase_add_test(tc, test_percent_percent);
    tcase_add_test(tc, test_flags_plus_space);
    tcase_add_test(tc, test_width_precision);
    tcase_add_test(tc, test_minus_flag);
    tcase_add_test(tc, test_null_string);
    tcase_add_test(tc, test_combined);
    tcase_add_test(tc, test_percent_d_negative);
    tcase_add_test(tc, test_percent_d_zero);
    tcase_add_test(tc, test_d_width_right);
    tcase_add_test(tc, test_d_width_left);
    tcase_add_test(tc, test_d_precision);
    tcase_add_test(tc, test_d_width_precision);
    tcase_add_test(tc, test_d_zero_flag);
    tcase_add_test(tc, test_d_zero_flag_negative);
    tcase_add_test(tc, test_d_plus_zero_flag);
    tcase_add_test(tc, test_d_space_flag);
    tcase_add_test(tc, test_d_precision_zero_value);
    tcase_add_test(tc, test_d_length_h);
    tcase_add_test(tc, test_d_length_l);
    tcase_add_test(tc, test_d_int_min);
    tcase_add_test(tc, test_percent_u_zero);
    tcase_add_test(tc, test_u_width);
    tcase_add_test(tc, test_u_minus_width);
    tcase_add_test(tc, test_u_precision);
    tcase_add_test(tc, test_u_width_precision);
    tcase_add_test(tc, test_u_zero_flag);
    tcase_add_test(tc, test_u_precision_zero_value);
    tcase_add_test(tc, test_u_length_h);
    tcase_add_test(tc, test_u_length_l);
    tcase_add_test(tc, test_s_width_right);
    tcase_add_test(tc, test_s_width_left);
    tcase_add_test(tc, test_s_precision);
    tcase_add_test(tc, test_s_width_precision);
    tcase_add_test(tc, test_s_empty);
    tcase_add_test(tc, test_c_width_right);
    tcase_add_test(tc, test_c_width_left);
    tcase_add_test(tc, test_f_default_precision);
    tcase_add_test(tc, test_f_precision_zero);
    tcase_add_test(tc, test_f_width);
    tcase_add_test(tc, test_f_minus_width);
    tcase_add_test(tc, test_f_zero_flag_width);
    tcase_add_test(tc, test_f_plus_and_space);
    tcase_add_test(tc, test_f_round_carry);
    tcase_add_test(tc, test_f_negative_zero_flag);
    tcase_add_test(tc, test_multi_length_mix);
    tcase_add_test(tc, test_many_specifiers_sequence);
    tcase_add_loop_test(tc, test_stress_random_d_formats, 0, 450);
    tcase_add_loop_test(tc, test_stress_random_u_formats, 0, 450);
    tcase_add_loop_test(tc, test_stress_random_f_formats, 0, 350);
    tcase_add_loop_test(tc, test_stress_random_s_c_formats, 0, 500);
    tcase_add_loop_test(tc, test_stress_random_mixed, 0, 300);

    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    int total;
    int failed;
    int passed;
    Suite *s = sprintf_test_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_VERBOSE);
    total = srunner_ntests_run(sr);
    failed = srunner_ntests_failed(sr);
    passed = total - failed;
    printf("\nTest summary: passed=%d failed=%d total=%d\n", passed, failed, total);
    srunner_free(sr);
    return failed;
}

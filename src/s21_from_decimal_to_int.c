#include "s21_decimal.h"

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  if (!s21_is_decimal(src) || !dst) return 1;
  int sign;
  s21_truncate(src, &src);
  sign = (src.bits[3] & 0x80000000) ? -1 : 1;
  *dst = src.bits[0] * sign;
  return 0;
}

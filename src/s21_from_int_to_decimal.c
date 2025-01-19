#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  if (!dst) return 1;
  s21_init_decimal(dst);
  if (src < 0) {
    dst->bits[0] = -src;
    dst->bits[3] ^= 0x80000000;
  } else
    dst->bits[0] = src;
  return 0;
}

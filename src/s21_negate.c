#include "s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  if (!s21_is_decimal(value)) return 1;
  int status = 1;
  if (result) {
    *result = value;
    result->bits[3] = value.bits[3] ^ 0x80000000;
    status = 0;
  }
  return status;
}

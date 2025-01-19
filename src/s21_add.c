#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = 0;
  s21_decimal_ext ext_value_1, ext_value_2, ext_result;
  s21_init_decimal_ext_by_decimal(&ext_value_1, value_1);
  s21_init_decimal_ext_by_decimal(&ext_value_2, value_2);
  if (ext_value_1.sign ^ ext_value_2.sign) {
    s21_sub_ext(ext_value_1, ext_value_2, &ext_result);
  } else {
    status = s21_add_ext(ext_value_1, ext_value_2, &ext_result);
  }
  if (status)
    status += ext_result.sign;
  else {
    status = s21_decimal_ext2decimal(ext_result, result);
  }
  return status;
}

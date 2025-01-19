#include "s21_decimal.h"

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  bool is_equal = 1;
  if (value_1.bits[0] || value_1.bits[1] || value_1.bits[2] ||
      value_2.bits[0] || value_2.bits[1] || value_2.bits[2]) {
    s21_decimal_ext ext_value_1, ext_value_2;
    s21_init_decimal_ext_by_decimal(&ext_value_1, value_1);
    s21_init_decimal_ext_by_decimal(&ext_value_2, value_2);
    if (s21_scaling_ext_to_sc(&ext_value_2, ext_value_1.sc))
      s21_scaling_ext_to_sc(&ext_value_1, ext_value_2.sc);
    for (uint_fast16_t i = 0;
         is_equal && i < sizeof(ext_value_2.bits) / sizeof(ext_value_2.bits[0]);
         ++i) {
      is_equal = !(ext_value_2.bits[i] ^ ext_value_1.bits[i]);
    }
    is_equal = is_equal && (ext_value_2.sign == ext_value_1.sign);
  }
  return is_equal;
}

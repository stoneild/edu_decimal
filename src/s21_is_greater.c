#include "s21_decimal.h"

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  bool is_greater = 0;
  if (value_1.bits[0] || value_1.bits[1] || value_1.bits[2] ||
      value_2.bits[0] || value_2.bits[1] || value_2.bits[2]) {
    s21_decimal_ext ext_value_1, ext_value_2;
    s21_init_decimal_ext_by_decimal(&ext_value_1, value_1);
    s21_init_decimal_ext_by_decimal(&ext_value_2, value_2);
    is_greater = !ext_value_1.sign && ext_value_2.sign;
    if (!is_greater && s21_scaling_ext_to_sc(&ext_value_2, ext_value_1.sc))
      s21_scaling_ext_to_sc(&ext_value_1, ext_value_2.sc);
    if (!is_greater) {
      bool is_less = 0;
      for (int_fast16_t i =
               sizeof(ext_value_2.bits) / sizeof(ext_value_2.bits[0]) - 1;
           !is_greater && !is_less && i >= 0; --i) {
        is_less = (ext_value_1.bits[i] < ext_value_2.bits[i]);
        is_greater = (ext_value_1.bits[i] > ext_value_2.bits[i]);
      }
      is_greater = (!ext_value_1.sign && is_greater) ||
                   (ext_value_1.sign && ext_value_2.sign && is_less);
    }
  }
  return is_greater;
}

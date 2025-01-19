#include "s21_decimal.h"

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = 0;
  s21_decimal_ext ext_value_2, ext_result;
  s21_init_decimal_ext_by_decimal(&ext_value_2, value_2);
  s21_init_decimal_ext(&ext_result);
  bool big_value_1[sizeof(ext_result.bits) / sizeof(ext_result.bits[0]) / 2 *
                   3] = {0};
  bool *term_1;
  uint_fast16_t i = 96;
  for (uint_fast16_t j = 0; j < 3; ++j) {
    for (uint_fast16_t bit = 0; bit < 32; ++bit)
      big_value_1[i++] = value_1.bits[j] >> bit & 0b1;
  }
  for (i = 0; !status && i < 96; ++i) {
    if (ext_value_2.bits[i]) {
      term_1 = big_value_1 + 96 - i;
      status =
          s21_add_bits(term_1, ext_result.bits, ext_result.bits,
                       sizeof(ext_result.bits) / sizeof(ext_result.bits[0]), 0);
    }
  }
  ext_result.sc = ext_value_2.sc + ((value_1.bits[3] >> 16) & 0b11111111);
  ext_result.sign = ext_value_2.sign ^ (bool)((value_1.bits[3] >> 31) & 0b1);
  if (status)
    status += ext_result.sign;
  else
    status = s21_decimal_ext2decimal(ext_result, result);
  return status;
}

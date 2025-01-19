#include "s21_decimal.h"

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0)
    return 3;
  int status = 0;
  s21_decimal_ext ext_value_1, ext_value_2, ext_result;
  s21_init_decimal_ext_by_decimal(&ext_value_1, value_1);
  s21_init_decimal_ext_by_decimal(&ext_value_2, value_2);
  ext_result.sign = ext_value_1.sign ^ ext_value_2.sign;
  ext_result.sc = 0;
  if (s21_scaling_ext_to_sc(&ext_value_2, ext_value_1.sc))
    s21_scaling_ext_to_sc(&ext_value_1, ext_value_2.sc);
  bool tmp_mod[sizeof(ext_result.bits) / sizeof(ext_result.bits[0])] = {0};
  s21_div_bits_w_mod(ext_value_1.bits, ext_value_2.bits, ext_result.bits,
                     tmp_mod,
                     sizeof(ext_result.bits) / sizeof(ext_result.bits[0]));
  for (bool overflow = 0;
       !overflow &&
       !s21_is_zero_bits(
           tmp_mod, sizeof(ext_result.bits) / sizeof(ext_result.bits[0])) &&
       ext_result.sc < 29;) {
    s21_mul_bits_on_dozen(tmp_mod,
                          sizeof(ext_result.bits) / sizeof(ext_result.bits[0]));
    overflow = s21_mul_bits_on_dozen(
        ext_result.bits, sizeof(ext_result.bits) / sizeof(ext_result.bits[0]));
    if (!overflow) {
      bool tmp_result_bits[sizeof(ext_result.bits) /
                           sizeof(ext_result.bits[0])] = {0};
      ++ext_result.sc;
      s21_div_bits_w_mod(tmp_mod, ext_value_2.bits, tmp_result_bits, tmp_mod,
                         sizeof(ext_result.bits) / sizeof(ext_result.bits[0]));
      overflow =
          s21_add_bits(ext_result.bits, tmp_result_bits, ext_result.bits,
                       sizeof(ext_result.bits) / sizeof(ext_result.bits[0]), 0);
    }
  }
  status = s21_decimal_ext2decimal(ext_result, result);
  return status;
}

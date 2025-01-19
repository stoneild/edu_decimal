#include "s21_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  if (!s21_is_decimal(value)) return 1;
  int status = 1;
  if (result) {
    s21_decimal_ext value_ext;
    s21_init_decimal_ext_by_decimal(&value_ext, value);
    uint_fast16_t cnt_of_mods = 0;
    bool last_mod[sizeof(value_ext.bits) / sizeof(value_ext.bits[0]) * 2] = {0};
    s21_down_sc_to_zero(value_ext.bits, &value_ext.sc, last_mod, &cnt_of_mods,
                        sizeof(value_ext.bits) / sizeof(value_ext.bits[0]));
    s21_fill_decimal(&value_ext, result);
    status = 0;
  }
  return status;
}

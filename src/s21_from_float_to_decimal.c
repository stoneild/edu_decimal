#include "s21_decimal.h"

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  if (!dst) return 1;
  int status = 0;
  s21_decimal_ext decimal_ext;
  s21_init_decimal_ext(&decimal_ext);
  float buf = src;
  uint_fast16_t cnt_of_mods = 0;
  bool last_mod[sizeof(decimal_ext.bits) / sizeof(decimal_ext.bits[0])] = {0};
  if (buf < 0) buf = -buf, decimal_ext.sign = 1;
  for (uint_fast16_t cnt_bits = 0;
       cnt_bits < sizeof(decimal_ext.bits) / sizeof(decimal_ext.bits[0]) / 2 &&
       buf >= 1;
       ++cnt_bits, buf /= 2)
    decimal_ext.bits[cnt_bits] = fmodf(buf, 2) >= 1;
  for (buf = fmodf(fabs(src), 1) * 10; decimal_ext.sc < 29 && buf != 0;
       ++decimal_ext.sc, buf = fmodf(buf, 1) * 10) {
    bool fract[sizeof(decimal_ext.bits) / sizeof(decimal_ext.bits[0])] = {0};
    for (uint_fast16_t cnt_bits = 0, fract_fl = (uint_fast16_t)buf;
         fract_fl >= 1; ++cnt_bits, fract_fl /= 2)
      fract[cnt_bits] = fmodf(fract_fl, 2) >= 1;
    s21_mul_bits_on_dozen(decimal_ext.bits, sizeof(decimal_ext.bits) /
                                                sizeof(decimal_ext.bits[0]));
    s21_add_bits(decimal_ext.bits, fract, decimal_ext.bits,
                 sizeof(decimal_ext.bits) / sizeof(decimal_ext.bits[0]), 0);
  }
  s21_down_sc4fl(decimal_ext.bits, &decimal_ext.sc, last_mod, &cnt_of_mods,
                 sizeof(decimal_ext.bits) / sizeof(decimal_ext.bits[0]));
  if (cnt_of_mods)
    s21_round_bits(decimal_ext.bits, last_mod, cnt_of_mods,
                   sizeof(decimal_ext.bits) / sizeof(decimal_ext.bits[0]));
  cnt_of_mods = 0;
  s21_down_sc4fl(decimal_ext.bits, &decimal_ext.sc, last_mod, &cnt_of_mods,
                 sizeof(decimal_ext.bits) / sizeof(decimal_ext.bits[0]));
  if (cnt_of_mods)
    s21_round_bits(decimal_ext.bits, last_mod, cnt_of_mods,
                   sizeof(decimal_ext.bits) / sizeof(decimal_ext.bits[0]));

  s21_norm_sc(&decimal_ext);
  s21_decimal_ext2decimal(decimal_ext, dst);
  return status;
}

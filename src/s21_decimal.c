#include "s21_decimal.h"

int s21_is_decimal(s21_decimal value) {
  return !((value.bits[3] & 0x7FE0FFFF) ||
           (((value.bits[3] >> 16) & 0b11111) > 28));
}

void s21_init_decimal(s21_decimal *decimal) {
  for (short i = 0; i < 4; ++i) decimal->bits[i] = 0;
}

void s21_init_decimal_ext(s21_decimal_ext *decimal_ext) {
  decimal_ext->sign = 0;
  decimal_ext->sc = 0;
  for (uint_fast16_t i = 0;
       i < sizeof(decimal_ext->bits) / sizeof(decimal_ext->bits[0]); ++i)
    decimal_ext->bits[i] = 0;
}

void s21_init_decimal_ext_by_decimal(s21_decimal_ext *decimal_ext,
                                     s21_decimal value) {
  decimal_ext->sign = value.bits[3] & 0x80000000;
  decimal_ext->sc = (value.bits[3] >> 16) & 0b11111111;
  uint_fast16_t i = 0;
  for (uint_fast16_t j = 0; j < 3; ++j) {
    for (uint_fast16_t bit = 0; bit < 32; ++bit)
      decimal_ext->bits[i++] = value.bits[j] >> bit & 0b1;
  }
  for (; i < sizeof(decimal_ext->bits) / sizeof(decimal_ext->bits[0]); ++i)
    decimal_ext->bits[i] = 0;
}

int s21_scaling_ext_to_sc(s21_decimal_ext *value, int_fast16_t sc) {
  if (sc < value->sc) return 1;
  uint_fast16_t diff_sc = sc - value->sc;
  for (uint_fast16_t i = 0; i < diff_sc; ++i)
    s21_mul_bits_on_dozen(value->bits,
                          sizeof(value->bits) / sizeof(value->bits[0]));
  value->sc = sc;
  return 0;
}

int s21_add_bits(bool *term_1, bool *term_2, bool *result,
                 uint_fast16_t size_of_bits, bool carry) {
  bool buf_result[size_of_bits];
  for (uint_fast16_t i = 0, tmp; i < size_of_bits; ++i) {
    assert((uint8_t)term_1[i] < 2);
    tmp = (uint8_t)term_1[i] + (uint8_t)term_2[i] + (uint8_t)carry;
    buf_result[i] = tmp & 0b01;
    carry = (tmp & 0b10) == 0b10;
  }
  for (uint_fast16_t i = 0; i < size_of_bits; ++i) result[i] = buf_result[i];
  return carry;
}

int s21_add_bits_safe(bool *term_1, bool *term_2, bool *result,
                      uint_fast16_t size_of_bits, bool carry) {
  bool buf_result[size_of_bits];
  for (uint_fast16_t i = 0, tmp; i < size_of_bits; ++i) {
    assert((uint8_t)term_1[i] < 2);
    tmp = (uint8_t)term_1[i] + (uint8_t)term_2[i] + (uint8_t)carry;
    buf_result[i] = tmp & 0b01;
    carry = (tmp & 0b10) == 0b10;
  }
  if (!carry)
    for (uint_fast16_t i = 0; i < size_of_bits; ++i) result[i] = buf_result[i];
  return carry;
}

int s21_mul_bits_on_dozen(bool *bits, uint_fast16_t size_of_bits) {
  bool overflow;
  bool safe_bits[size_of_bits + 3];
  uint_fast16_t i = 0;
  for (; i < 3; ++i) safe_bits[i] = 0;
  for (; i < size_of_bits + 3; ++i) safe_bits[i] = bits[i - 3];
  overflow = safe_bits[size_of_bits + 2] || safe_bits[size_of_bits + 1] ||
             safe_bits[size_of_bits];
  bool *term_1;
  term_1 = safe_bits + 2;
  bool carry = s21_add_bits_safe(term_1, safe_bits, bits, size_of_bits, 0);
  return overflow || carry;
}

int s21_is_less_bits(const bool *bits_1, const bool *bits_2,
                     int_fast16_t size_of_bits) {
  bool is_less = 0, is_greater = 0;
  for (--size_of_bits; !is_less && !is_greater && size_of_bits >= 0;
       --size_of_bits) {
    is_less = bits_1[size_of_bits] < bits_2[size_of_bits];
    is_greater = bits_1[size_of_bits] > bits_2[size_of_bits];
  }
  return is_less;
}

int s21_add_ext(s21_decimal_ext ext_value_1, s21_decimal_ext ext_value_2,
                s21_decimal_ext *ext_result) {
  if (s21_scaling_ext_to_sc(&ext_value_2, ext_value_1.sc))
    s21_scaling_ext_to_sc(&ext_value_1, ext_value_2.sc);
  ext_result->sign = ext_value_1.sign;
  ext_result->sc = ext_value_1.sc;
  return s21_add_bits(ext_value_1.bits, ext_value_2.bits, ext_result->bits,
                      sizeof(ext_result->bits) / sizeof(ext_result->bits[0]),
                      0);
}

bool *s21_invert_bits(const bool *bits, bool *result,
                      int_fast16_t size_of_bits) {
  for (--size_of_bits; size_of_bits >= 0; --size_of_bits)
    result[size_of_bits] = !bits[size_of_bits];
  return result;
}

int s21_sub_bits(bool *bits_1, bool *bits_2, bool *result_bits,
                 int_fast16_t size_of_bits) {
  if (s21_is_less_bits(bits_1, bits_2, size_of_bits)) return 1;
  bool invert_bits[size_of_bits];
  for (int_fast16_t bit = 0; bit < size_of_bits; ++bit) invert_bits[0] = 0;
  s21_add_bits(bits_1, s21_invert_bits(bits_2, invert_bits, size_of_bits),
               result_bits, size_of_bits, 1);
  return 0;
}

void s21_sub_ext(s21_decimal_ext ext_value_1, s21_decimal_ext ext_value_2,
                 s21_decimal_ext *ext_result) {
  if (s21_scaling_ext_to_sc(&ext_value_2, ext_value_1.sc))
    s21_scaling_ext_to_sc(&ext_value_1, ext_value_2.sc);
  if (!s21_sub_bits(ext_value_1.bits, ext_value_2.bits, ext_result->bits,
                    sizeof(ext_result->bits) / sizeof(ext_result->bits[0]))) {
    ext_result->sign = ext_value_1.sign;
  } else {
    s21_sub_bits(ext_value_2.bits, ext_value_1.bits, ext_result->bits,
                 sizeof(ext_result->bits) / sizeof(ext_result->bits[0]));
    ext_result->sign = ext_value_2.sign;
  }
  ext_result->sc = ext_value_1.sc;
}

int s21_is_zero_bits(const bool *bits, int_fast16_t size_of_bits) {
  bool is_zero = 1;
  for (--size_of_bits; is_zero && size_of_bits >= 0; --size_of_bits)
    is_zero = !bits[size_of_bits];
  return is_zero;
}

int s21_div_bits_w_mod(const bool *value_1_bits, bool *value_2_bits,
                       bool *integer, bool *mod, uint_fast16_t size_of_bits) {
  bool safe_value_1[size_of_bits * 2];
  s21_copy_bits(value_1_bits, safe_value_1, size_of_bits);
  for (uint_fast16_t bit = size_of_bits; bit < size_of_bits * 2; ++bit)
    safe_value_1[bit] = 0;
  bool tmp_res[size_of_bits];
  for (uint_fast16_t bit = 0; bit < size_of_bits; ++bit) tmp_res[bit] = 0;
  for (int_fast16_t i = size_of_bits - 1; i >= 0; --i) {
    tmp_res[i] = !s21_sub_bits(safe_value_1 + i, value_2_bits, safe_value_1 + i,
                               size_of_bits);
  }
  if (integer) s21_copy_bits(tmp_res, integer, size_of_bits);
  if (mod) s21_copy_bits(safe_value_1, mod, size_of_bits);
  return 0;
}

void s21_copy_bits(const bool *src, bool *dst, uint_fast16_t size_of_bits) {
  for (uint_fast16_t i = 0; i < size_of_bits; ++i) dst[i] = src[i];
}

int s21_is_bits_equal(const bool *bits_1, const bool *bits_2,
                      int_fast16_t size_of_bits) {
  bool is_equal = 1;
  for (--size_of_bits; is_equal && size_of_bits >= 0; --size_of_bits)
    is_equal = (bits_1[size_of_bits] == bits_2[size_of_bits]);
  return is_equal;
}

void s21_down_sc(bool *value, int_fast16_t *sc, bool *last_mod,
                 uint_fast16_t *cnt_of_mods, uint_fast16_t size_of_bits) {
  for (uint_fast16_t bit = 0; bit < size_of_bits; ++bit) last_mod[bit] = 0;
  for (; (!s21_is_zero_bits(value + size_of_bits / 2, size_of_bits / 2) ||
          *sc > 28) &&
         *sc > 0;
       --*sc) {
    s21_div_bits_by_dozen(value, last_mod, size_of_bits);
    *cnt_of_mods += !s21_is_zero_bits(last_mod, 4);
  }
}

void s21_down_sc4fl(bool *value, int_fast16_t *sc, bool *last_mod,
                    uint_fast16_t *cnt_of_mods, uint_fast16_t size_of_bits) {
  bool seven_nines[size_of_bits];
  for (uint_fast16_t bit = 0; bit < size_of_bits; ++bit) last_mod[bit] = 0;
  uint_fast16_t i = 0;
  for (uint_fast32_t seven_nines_dec = 9999999; seven_nines_dec >= 1;
       ++i, seven_nines_dec /= 2)
    seven_nines[i] = fmodl(seven_nines_dec, 2);
  for (; i < size_of_bits; ++i) seven_nines[i] = 0;
  for (; s21_is_less_bits(seven_nines, value, size_of_bits); --*sc) {
    s21_div_bits_by_dozen(value, last_mod, size_of_bits);
    *cnt_of_mods += !s21_is_zero_bits(last_mod, 4);
  }
}

void s21_down_sc_to_zero(bool *value, int_fast16_t *sc, bool *last_mod,
                         uint_fast16_t *cnt_of_mods,
                         uint_fast16_t size_of_bits) {
  for (uint_fast16_t bit = 0; bit < size_of_bits; ++bit) last_mod[bit] = 0;
  for (; *sc > 0; --*sc) {
    s21_div_bits_by_dozen(value, last_mod, size_of_bits);
    *cnt_of_mods += !s21_is_zero_bits(last_mod, 4);
  }
}

int s21_floor_bits(bool *value, bool *mod, uint_fast16_t cnt_of_mods,
                   uint_fast16_t size_of_bits) {
  uint_fast16_t status = 0;
  if (cnt_of_mods > 0) {
    s21_add_bits(value, mod + 4, value, size_of_bits, 1);
    ++status;
  }
  return status;
}

int s21_round_bits(bool *value, bool *mod, uint_fast16_t cnt_of_mods,
                   uint_fast16_t size_of_bits) {
  uint_fast16_t status = 0;
  bool five_in_bits[4] = {1, 0, 1, 0}, zero[size_of_bits];
  for (uint_fast16_t bit = 0; bit < size_of_bits; ++bit) zero[bit] = 0;
  if (!s21_is_less_bits(mod, five_in_bits, 4)) {
    if (s21_is_bits_equal(mod, five_in_bits, 4)) {
      if (cnt_of_mods > 1 || (cnt_of_mods == 1 && value[0] == 1)) {
        s21_add_bits(value, zero, value, size_of_bits, 1);
        ++status;
      }
    } else {
      s21_add_bits(value, zero, value, size_of_bits, 1);
      ++status;
    }
  }
  return status;
}

void s21_fill_decimal(s21_decimal_ext *src, s21_decimal *dst) {
  uint_fast16_t j = 0;
  for (uint_fast16_t i = 0; j < 3; ++j) {
    dst->bits[j] = 0;
    for (uint_fast16_t bit = 0; bit < 32; ++bit, ++i)
      dst->bits[j] ^= src->bits[i] << bit;
  }
  dst->bits[j] = 0;
  dst->bits[j] ^= src->sc << 16;
  dst->bits[j] ^= src->sign << 31;
}

int s21_decimal_ext2decimal(s21_decimal_ext src, s21_decimal *dst) {
  int status = 0;
  uint_fast16_t cnt_of_mods = 0;
  bool last_mod[sizeof(src.bits) / sizeof(src.bits[0])] = {0};
  s21_down_sc(src.bits, &src.sc, last_mod, &cnt_of_mods,
              sizeof(src.bits) / sizeof(src.bits[0]));
  if (s21_is_zero_bits(src.bits + sizeof(src.bits) / sizeof(src.bits[0]) / 2,
                       sizeof(src.bits) / sizeof(src.bits[0]) / 2) &&
      cnt_of_mods) {
    s21_round_bits(src.bits, last_mod, cnt_of_mods,
                   sizeof(src.bits) / sizeof(src.bits[0]));
    cnt_of_mods = 0;
  }
  s21_down_sc(src.bits, &src.sc, last_mod, &cnt_of_mods,
              sizeof(src.bits) / sizeof(src.bits[0]));
  if (s21_is_zero_bits(src.bits + sizeof(src.bits) / sizeof(src.bits[0]) / 2,
                       sizeof(src.bits) / sizeof(src.bits[0]) / 2) &&
      cnt_of_mods) {
    s21_round_bits(src.bits, last_mod, cnt_of_mods,
                   sizeof(src.bits) / sizeof(src.bits[0]));
  }
  if (!s21_is_zero_bits(src.bits + sizeof(src.bits) / sizeof(src.bits[0]) / 2,
                        sizeof(src.bits) / sizeof(src.bits[0]) / 2))
    status = 1 + src.sign;
  else
    s21_fill_decimal(&src, dst);
  return status;
}

int s21_div_bits_by_dozen(bool *bits, bool *res_mod,
                          uint_fast16_t size_of_bits) {
  bool dozen[size_of_bits];
  for (uint_fast16_t bit = 0; bit < size_of_bits; ++bit) dozen[bit] = 0;
  dozen[1] = 1, dozen[3] = 1;
  s21_div_bits_w_mod(bits, dozen, bits, res_mod, size_of_bits);
  return 0;
}

int s21_norm_sc(s21_decimal_ext *value_ext) {
  s21_decimal_ext one;
  s21_init_decimal_ext(&one), one.bits[0] = 1;
  s21_scaling_ext_to_sc(&one, value_ext->sc);
  for (; value_ext->sc < 0; ++value_ext->sc)
    s21_mul_bits_on_dozen(value_ext->bits,
                          sizeof(value_ext->bits) / sizeof(value_ext->bits[0]));
  if (s21_is_less_bits(value_ext->bits, one.bits,
                       sizeof(value_ext->bits) / sizeof(value_ext->bits[0])))
    for (; value_ext->sc > 6; --value_ext->sc)
      s21_div_bits_by_dozen(
          value_ext->bits, NULL,
          sizeof(value_ext->bits) / sizeof(value_ext->bits[0]));
  return 0;
}

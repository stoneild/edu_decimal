#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef struct {
  unsigned int bits[4];
} s21_decimal;

typedef struct {
  bool sign;
  int_fast16_t sc;
  bool bits[192];
} s21_decimal_ext;

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_less(s21_decimal, s21_decimal);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

void s21_init_decimal(s21_decimal *decimal);
void s21_init_decimal_ext(s21_decimal_ext *decimal_ext);
void s21_init_decimal_ext_by_decimal(s21_decimal_ext *decimal_ext,
                                     s21_decimal value);
int s21_scaling_ext_to_sc(s21_decimal_ext *value, int_fast16_t sc);
int s21_add_bits(bool *term_1, bool *term_2, bool *result,
                 uint_fast16_t size_of_bits, bool carry);
int s21_add_bits_safe(bool *term_1, bool *term_2, bool *result,
                      uint_fast16_t size_of_bits, bool carry);
int s21_mul_bits_on_dozen(bool *bits, uint_fast16_t size_of_bits);
int s21_is_less_bits(const bool *bits_1, const bool *bits_2,
                     int_fast16_t size_of_bits);
int s21_add_ext(s21_decimal_ext ext_value_1, s21_decimal_ext ext_value_2,
                s21_decimal_ext *ext_result);
bool *s21_invert_bits(const bool *ext_value, bool *result,
                      int_fast16_t size_of_bits);
int s21_sub_bits(bool *term_1, bool *term_2, bool *result,
                 int_fast16_t size_of_bits);
void s21_sub_ext(s21_decimal_ext ext_value_1, s21_decimal_ext ext_value_2,
                 s21_decimal_ext *ext_result);
int s21_is_zero_bits(const bool *bits, int_fast16_t size_of_bits);
int s21_div_bits_w_mod(const bool *value_1_bits, bool *value_2_bits,
                       bool *result_bits, bool *mod,
                       uint_fast16_t size_of_bits);
int s21_div_bits_by_dozen(bool *bits, bool *res_mod,
                          uint_fast16_t size_of_bits);
void s21_copy_bits(const bool *src, bool *dst, uint_fast16_t size_of_bits);
int s21_is_bits_equal(const bool *bits_1, const bool *bits_2,
                      int_fast16_t size_of_bits);
int s21_round_bits(bool *value, bool *mod, uint_fast16_t cnt_of_mods,
                   uint_fast16_t size_of_bits);
void s21_down_sc(bool *value, int_fast16_t *sc, bool *last_mod,
                 uint_fast16_t *cnt_of_mods, uint_fast16_t size_of_bits);
void s21_down_sc4fl(bool *value, int_fast16_t *sc, bool *last_mod,
                    uint_fast16_t *cnt_of_mods, uint_fast16_t size_of_bits);
void s21_fill_decimal(s21_decimal_ext *src, s21_decimal *dst);
void s21_down_sc_to_zero(bool *value, int_fast16_t *sc, bool *last_mod,
                         uint_fast16_t *cnt_of_mods,
                         uint_fast16_t size_of_bits);
int s21_floor_bits(bool *value, bool *mod, uint_fast16_t cnt_of_mods,
                   uint_fast16_t size_of_bits);
int s21_decimal_ext2decimal(s21_decimal_ext src, s21_decimal *dst);
int s21_is_decimal(s21_decimal value);
int s21_norm_sc(s21_decimal_ext *value_ext);

#endif  // S21_DECIMAL_H

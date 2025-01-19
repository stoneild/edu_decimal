#include "s21_decimal.h"

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  if (!dst || !s21_is_decimal(src)) return 1;
  int status = 0;
  s21_decimal_ext tmp_ext_res;
  s21_decimal tmp_res;
  s21_init_decimal_ext_by_decimal(&tmp_ext_res, src);
  uint_fast16_t sizeofbits =
      sizeof(tmp_ext_res.bits) / sizeof(tmp_ext_res.bits[0]);
  for (uint_fast16_t i = sizeofbits - 1; i > 31 && tmp_ext_res.sc > 0; --i)
    if (tmp_ext_res.bits[i]) {
      s21_div_bits_by_dozen(tmp_ext_res.bits, NULL, sizeofbits);
      --tmp_ext_res.sc;
    }
  status = s21_decimal_ext2decimal(tmp_ext_res, &tmp_res);
  double tmp_dst = (double)tmp_res.bits[0] * pow(10, -tmp_ext_res.sc);
  if (tmp_ext_res.sign) tmp_dst = -tmp_dst;
  *dst = (float)tmp_dst;
  return status;
}

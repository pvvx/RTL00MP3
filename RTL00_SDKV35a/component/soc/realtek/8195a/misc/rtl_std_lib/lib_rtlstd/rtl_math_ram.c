/* 
 * rtl_math_ram..o
 * pvvx 2016
 */

#include "basic_types.h"

//-------------------------------------------------------------------------
// Function declarations

float rtl_fabsf(float a);
int rtl_fabs(double a);
float rtl_cos_f32(float a);
float rtl_sin_f32(float a);

// Extern Calls:

// int __rtl_fabsf_v1_00();
// int __rtl_fabs_v1_00();
// int __rtl_cos_f32_v1_00();
// int __rtl_sin_f32_v1_00();

extern _LONG_CALL_ float __rtl_fabsf_v1_00(float a);
//----- rtl_fabsf()
float rtl_fabsf(float a)
{
  return __rtl_fabsf_v1_00(a);
}

extern _LONG_CALL_ int __rtl_fabs_v1_00(double a);
//----- rtl_fabs()
int rtl_fabs(double a)
{
  return __rtl_fabs_v1_00(a);
}

extern _LONG_CALL_ float __rtl_cos_f32_v1_00(float a);
//----- rtl_cos_f32()
float rtl_cos_f32(float a)
{
  return __rtl_cos_f32_v1_00(a);
}

extern _LONG_CALL_ float __rtl_sin_f32_v1_00(float a);
//----- rtl_sin_f32()
float rtl_sin_f32(float a)
{
  return __rtl_sin_f32_v1_00(a);
}

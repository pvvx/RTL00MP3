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

// int _rtl_fabsf_v1_00();
// int _rtl_fabs_v1_00();
// int _rtl_cos_f32_v1_00();
// int _rtl_sin_f32_v1_00();


//----- rtl_fabsf()
float rtl_fabsf(float a)
{
  return _rtl_fabsf_v1_00(a);
}

//----- rtl_fabs()
int rtl_fabs(double a)
{
  return _rtl_fabs_v1_00(a);
}

//----- rtl_cos_f32()
float rtl_cos_f32(float a)
{
  return _rtl_cos_f32_v1_00(a);
}

//----- rtl_sin_f32()
float rtl_sin_f32(float a)
{
  return _rtl_sin_f32_v1_00(a);
}

/* 
 * rtl_eabi_cast_ram.o
 * pvvx 2016
 */

#include "basic_types.h"
#include "rt_lib_rom.h"

//-------------------------------------------------------------------------
// Function declarations

int rtl_dtoi(double d);
int rtl_dtoui(double d);
float rtl_i2f(int val);
int rtl_i2d(int val);
float rtl_ui2f(unsigned int val);
int rtl_ui2d(unsigned int val);
char *rtl_itoa(int value, char *string, int radix);
char *rtl_ltoa(int value, char *string, int radix);
char *rtl_utoa(unsigned int value, char *string, int radix);
char *rtl_ultoa(unsigned int value, char *string, int radix);
int rtl_ftol(float f);
int rtl_ftod(float f);
float rtl_dtof(double d);
float rtl_fadd(float a, float b);
float rtl_fsub(float a, float b);
float rtl_fmul(float a, float b);
float rtl_fdiv(float a, float b);
int rtl_dadd(double a, double b);
int rtl_dsub(double a, double b);
int rtl_dmul(double a, double b);
int rtl_ddiv(double a, double b);
int rtl_dcmpeq(double a, double b);
int rtl_dcmplt(double a, double b);
int rtl_dcmple(double a, double b);
int rtl_dcmpgt(double a, double b);
int rtl_fcmplt(float a, float b);
int rtl_fcmpgt(float a, float b);

// Extern Calls:

// int __rtl_dtoi_v1_00();
// int __rtl_dtoui_v1_00();
// int __rtl_itof_v1_00();
// int __rtl_itod_v1_00();
// int __rtl_uitof_v1_00();
// int __rtl_uitod_v1_00();
// int __rtl_ltoa_v1_00();
// int __rtl_ultoa_v1_00();
// int __rtl_ftol_v1_00();
// int __rtl_ftod_v1_00();
// int __rtl_dtof_v1_00();
// int __rtl_fadd_v1_00();
// int __rtl_fsub_v1_00();
// int __rtl_fmul_v1_00();
// int __rtl_fdiv_v1_00();
// int __rtl_dadd_v1_00();
// int __rtl_dsub_v1_00();
// int __rtl_dmul_v1_00();
// int __rtl_ddiv_v1_00();
// int __rtl_dcmpeq_v1_00();
// int __rtl_dcmplt_v1_00();
// int __rtl_dcmple_v1_00();
// int __rtl_dcmpgt_v1_00();
// int __rtl_fcmplt_v1_00();
// int __rtl_fcmpgt_v1_00();


//----- rtl_dtoi()
int rtl_dtoi(double d)
{
  return __rtl_dtoi_v1_00(d);
}

//----- rtl_dtoui()
int rtl_dtoui(double d)
{
  return __rtl_dtoui_v1_00(d);
}

//----- rtl_i2f()
float rtl_i2f(int val)
{
  return __rtl_itof_v1_00(val);
}

//----- rtl_i2d()
int rtl_i2d(int val)
{
  return __rtl_itod_v1_00(val);
}

//----- rtl_ui2f()
float rtl_ui2f(unsigned int val)
{
  return __rtl_uitof_v1_00(val);
}

//----- rtl_ui2d()
int rtl_ui2d(unsigned int val)
{
  return __rtl_uitod_v1_00(val);
}

//----- rtl_itoa()
char *rtl_itoa(int value, char *string, int radix)
{
  return (char *)__rtl_ltoa_v1_00(value, string, radix);
}

//----- rtl_ltoa()
char *rtl_ltoa(int value, char *string, int radix)
{
  return (char *)__rtl_ltoa_v1_00(value, string, radix);
}

//----- rtl_utoa()
char *rtl_utoa(unsigned int value, char *string, int radix)
{
  return (char *)__rtl_ultoa_v1_00(value, string, radix);
}

//----- rtl_ultoa()
char *rtl_ultoa(unsigned int value, char *string, int radix)
{
  return (char *)__rtl_ultoa_v1_00(value, string, radix);
}

//----- rtl_ftol()
int rtl_ftol(float f)
{
  return __rtl_ftol_v1_00(f);
}

//----- rtl_ftod()
int rtl_ftod(float f)
{
  return __rtl_ftod_v1_00(f);
}

//----- rtl_dtof()
float rtl_dtof(double d)
{
  return __rtl_dtof_v1_00(d);
}

//----- rtl_fadd()
float rtl_fadd(float a, float b)
{
  return __rtl_fadd_v1_00(a, b);
}

//----- rtl_fsub()
float rtl_fsub(float a, float b)
{
  return __rtl_fsub_v1_00(a, b);
}

//----- rtl_fmul()
float rtl_fmul(float a, float b)
{
  return __rtl_fmul_v1_00(a, b);
}

//----- rtl_fdiv()
float rtl_fdiv(float a, float b)
{
  return __rtl_fdiv_v1_00(a, b);
}

//----- rtl_dadd()
int rtl_dadd(double a, double b)
{
  return __rtl_dadd_v1_00(a, b);
}

//----- rtl_dsub()
int rtl_dsub(double a, double b)
{
  return __rtl_dsub_v1_00(a, b);
}

//----- rtl_dmul()
int rtl_dmul(double a, double b)
{
  return __rtl_dmul_v1_00(a, b);
}

//----- rtl_ddiv()
int rtl_ddiv(double a, double b)
{
  return __rtl_ddiv_v1_00(a, b);
}

//----- rtl_dcmpeq()
int rtl_dcmpeq(double a, double b)
{
  return __rtl_dcmpeq_v1_00(a, b);
}

//----- rtl_dcmplt()
int rtl_dcmplt(double a, double b)
{
  return __rtl_dcmplt_v1_00(a, b);
}

//----- rtl_dcmple()
int rtl_dcmple(double a, double b)
{
  return __rtl_dcmple_v1_00(a, b);
}

//----- rtl_dcmpgt()
int rtl_dcmpgt(double a, double b)
{
  return __rtl_dcmpgt_v1_00(a, b);
}

//----- rtl_fcmplt()
int rtl_fcmplt(float a, float b)
{
  return __rtl_fcmplt_v1_00(a, b);
}

//----- rtl_fcmpgt()
int rtl_fcmpgt(float a, float b)
{
  return __rtl_fcmpgt_v1_00(a, b);
}

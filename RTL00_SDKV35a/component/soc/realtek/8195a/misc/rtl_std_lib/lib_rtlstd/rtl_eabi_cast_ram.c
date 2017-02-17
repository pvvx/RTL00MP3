/* 
 * rtl_eabi_cast_ram.o
 * pvvx 2016
 */

#include "basic_types.h"

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

// int _rtl_dtoi_v1_00();
// int _rtl_dtoui_v1_00();
// int _rtl_itof_v1_00();
// int _rtl_itod_v1_00();
// int _rtl_uitof_v1_00();
// int _rtl_uitod_v1_00();
// int _rtl_ltoa_v1_00();
// int _rtl_ultoa_v1_00();
// int _rtl_ftol_v1_00();
// int _rtl_ftod_v1_00();
// int _rtl_dtof_v1_00();
// int _rtl_fadd_v1_00();
// int _rtl_fsub_v1_00();
// int _rtl_fmul_v1_00();
// int _rtl_fdiv_v1_00();
// int _rtl_dadd_v1_00();
// int _rtl_dsub_v1_00();
// int _rtl_dmul_v1_00();
// int _rtl_ddiv_v1_00();
// int _rtl_dcmpeq_v1_00();
// int _rtl_dcmplt_v1_00();
// int _rtl_dcmple_v1_00();
// int _rtl_dcmpgt_v1_00();
// int _rtl_fcmplt_v1_00();
// int _rtl_fcmpgt_v1_00();


//----- rtl_dtoi()
int rtl_dtoi(double d)
{
  return _rtl_dtoi_v1_00(d);
}

//----- rtl_dtoui()
int rtl_dtoui(double d)
{
  return _rtl_dtoui_v1_00(d);
}

//----- rtl_i2f()
float rtl_i2f(int val)
{
  return _rtl_itof_v1_00(val);
}

//----- rtl_i2d()
int rtl_i2d(int val)
{
  return _rtl_itod_v1_00(val);
}

//----- rtl_ui2f()
float rtl_ui2f(unsigned int val)
{
  return _rtl_uitof_v1_00(val);
}

//----- rtl_ui2d()
int rtl_ui2d(unsigned int val)
{
  return _rtl_uitod_v1_00(val);
}

//----- rtl_itoa()
char *rtl_itoa(int value, char *string, int radix)
{
  return (char *)_rtl_ltoa_v1_00(value, string, radix);
}

//----- rtl_ltoa()
char *rtl_ltoa(int value, char *string, int radix)
{
  return (char *)_rtl_ltoa_v1_00(value, string, radix);
}

//----- rtl_utoa()
char *rtl_utoa(unsigned int value, char *string, int radix)
{
  return (char *)_rtl_ultoa_v1_00(value, string, radix);
}

//----- rtl_ultoa()
char *rtl_ultoa(unsigned int value, char *string, int radix)
{
  return (char *)_rtl_ultoa_v1_00(value, string, radix);
}

//----- rtl_ftol()
int rtl_ftol(float f)
{
  return _rtl_ftol_v1_00(f);
}

//----- rtl_ftod()
int rtl_ftod(float f)
{
  return _rtl_ftod_v1_00(f);
}

//----- rtl_dtof()
float rtl_dtof(double d)
{
  return _rtl_dtof_v1_00(d);
}

//----- rtl_fadd()
float rtl_fadd(float a, float b)
{
  return _rtl_fadd_v1_00(a, b);
}

//----- rtl_fsub()
float rtl_fsub(float a, float b)
{
  return _rtl_fsub_v1_00(a, b);
}

//----- rtl_fmul()
float rtl_fmul(float a, float b)
{
  return _rtl_fmul_v1_00(a, b);
}

//----- rtl_fdiv()
float rtl_fdiv(float a, float b)
{
  return _rtl_fdiv_v1_00(a, b);
}

//----- rtl_dadd()
int rtl_dadd(double a, double b)
{
  return _rtl_dadd_v1_00(a, b);
}

//----- rtl_dsub()
int rtl_dsub(double a, double b)
{
  return _rtl_dsub_v1_00(a, b);
}

//----- rtl_dmul()
int rtl_dmul(double a, double b)
{
  return _rtl_dmul_v1_00(a, b);
}

//----- rtl_ddiv()
int rtl_ddiv(double a, double b)
{
  return _rtl_ddiv_v1_00(a, b);
}

//----- rtl_dcmpeq()
int rtl_dcmpeq(double a, double b)
{
  return _rtl_dcmpeq_v1_00(a, b);
}

//----- rtl_dcmplt()
int rtl_dcmplt(double a, double b)
{
  return _rtl_dcmplt_v1_00(a, b);
}

//----- rtl_dcmple()
int rtl_dcmple(double a, double b)
{
  return _rtl_dcmple_v1_00(a, b);
}

//----- rtl_dcmpgt()
int rtl_dcmpgt(double a, double b)
{
  return _rtl_dcmpgt_v1_00(a, b);
}

//----- rtl_fcmplt()
int rtl_fcmplt(float a, float b)
{
  return _rtl_fcmplt_v1_00(a, b);
}

//----- rtl_fcmpgt()
int rtl_fcmpgt(float a, float b)
{
  return _rtl_fcmpgt_v1_00(a, b);
}

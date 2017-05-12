/* 
 *  RTL871x1Ax: RAM libc 
 *  Created on: 22/02/2017
 *      Author: pvvx
 */

#include "rtl_bios_data.h"
#include "va_list.h"

#define CHECK_LIBC_INIT 0
//-------------------------------------------------------------------------
// Function declarations

//void libc_init();
#if 0
int snprintf(char *str, size_t size, const char *fmt, ...);
int sprintf(char *str, const char *fmt, ...);
int printf(const char *fmt, ...);
int vprintf(const char * fmt, __VALIST param);
int vsnprintf(char *str, size_t size, const char *fmt, __VALIST param);
int vfprintf(FILE *fp, const char *fmt0, va_list ap);
void * memchr(const void * src_void , int c , size_t length);
int memcmp(const void *m1, const void *m2, size_t n);
void * memcpy(void *dst0, const void *src0, size_t len0);
void * memmove(void *dst_void, const void *src_void, size_t length);
void * memset(void *m, int c, size_t n);
char * strcat(char *s1, const char *s2);
char * strchr(const char *s1, int i);
int strcmp(const char *s1, const char *s2);
char * strcpy(char *dst0, const char *src0);
size_t strlen(const char *str);
char * strncat(char *s1, const char *s2, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
char * strncpy(char *dst0, const char *src0, size_t count);
char * strstr(const char *searchee, const char *lookfor);
char * strsep(char **source_ptr, const char *delim);
char * strtok(char *s, const char *delim);
int sscanf(const char *buf, const char *fmt, ...);
char toupper(char ch);
int _stricmp (const char *s1, const char *s2);
unsigned long long __aeabi_llsr(unsigned long long val, unsigned int shift);
#endif
// Extern Calls:
// extern int init_rom_libgloss_ram_map(_DWORD)
// extern int _rom_mallocr_init_v1_00(void)
// extern int __rtl_vfprintf_r_v1_00(_DWORD, _DWORD, _DWORD)
// extern int __rtl_fflush_r_v1_00(_DWORD, _DWORD)
// extern int __rtl_memchr_v1_00(_DWORD, _DWORD, _DWORD)
// extern int __rtl_memcmp_v1_00(_DWORD, _DWORD, _DWORD)
// extern int __rtl_memcpy_v1_00(_DWORD, _DWORD, _DWORD)
// extern int __rtl_memmove_v1_00(_DWORD, _DWORD, _DWORD)
// extern int __rtl_memset_v1_00(_DWORD, _DWORD, _DWORD)
// extern int __rtl_strcat_v1_00(_DWORD, _DWORD)
// extern int __rtl_strchr_v1_00(_DWORD, _DWORD)
// extern int __rtl_strcmp_v1_00(_DWORD, _DWORD)
// extern int __rtl_strcpy_v1_00(_DWORD, _DWORD)
// extern int __rtl_strlen_v1_00(_DWORD)
// extern int __rtl_strncat_v1_00(_DWORD, _DWORD, _DWORD)
// extern int __rtl_strncmp_v1_00(_DWORD, _DWORD, _DWORD)
// extern int __rtl_strncpy_v1_00(_DWORD, _DWORD, _DWORD)
// extern int __rtl_strstr_v1_00(_DWORD, _DWORD)
// extern int __rtl_strsep_v1_00(_DWORD, _DWORD)
// extern int __rtl_strtok_v1_00(_DWORD, _DWORD)

//-------------------------------------------------------------------------
// Data declarations

extern struct _reent * _rtl_impure_ptr;

extern char libc_has_init;
extern char print_off;

//-------------------------------------------------------------------------
// Function
//----- snprintf()
int snprintf(char *str, size_t size, const char *fmt, ...) {
	va_list args;
	va_start (args, fmt);
	int result;
	int w;
	FILE f;
#if CHECK_LIBC_INIT
	if (!libc_has_init) {
		rtl_libc_init();
	}
#endif	
	if (size >= 0) {
		f._flags = 520;
		if (size)
			w = size - 1;
		else
			w = 0;
		f._w = w; /* write space left for putc() */
		f._bf._size = w;
		f._file = -1; /* fileno, if Unix descriptor, else -1 */
		f._p = str; /* current position in (some) buffer */
		f._bf._base = str;
		result = __rtl_vfprintf_r_v1_00(_rtl_impure_ptr, &f, fmt, args);
		if (result + 1 < 0)
			_rtl_impure_ptr->_errno = 139;
		if (size)
			*f._p = 0;
	} else {
		_rtl_impure_ptr->_errno = 139;
		result = -1;
	}
	return result;
}

#ifndef ENAC_FLOAT
//----- sprintf()
int sprintf(char *str, const char *fmt, ...) {
	FILE f;

#if CHECK_LIBC_INIT
	if (!libc_has_init) {
		rtl_libc_init();
	}
#endif	
	f._flags = 520;
	f._w = 0x7FFFFFFF;
	f._bf._size = 0x7FFFFFFF;
	f._file = -1;
	f._p = str;
	f._bf._base = str;
	va_list args;
	va_start (args, fmt);
	int result = __rtl_vfprintf_r_v1_00(_rtl_impure_ptr, &f, fmt, args);
	*f._p = 0;
//	va_end (args);
	return result;
}

//----- printf()
int printf(const char *fmt, ...) {
#if CHECK_LIBC_INIT
	if (!libc_has_init) {
		rtl_libc_init();
	}
#endif	
	if(!print_off) {

		va_list args;
		va_start (args, fmt);
		int result = __rtl_vfprintf_r_v1_00(_rtl_impure_ptr,
				_rtl_impure_ptr->_stdout, fmt, args);
		__rtl_fflush_r_v1_00(_rtl_impure_ptr, _rtl_impure_ptr->_stdout);
		//	va_end (args);
		return result;
	}
	else return 0;
}

//----- vprintf()
int vprintf(const char * fmt, __VALIST param) {
#if CHECK_LIBC_INIT
	if (!libc_has_init) {
		rtl_libc_init();
	}
#endif
	int result = __rtl_vfprintf_r_v1_00(_rtl_impure_ptr,
			_rtl_impure_ptr->_stdout, fmt, param);
	__rtl_fflush_r_v1_00(_rtl_impure_ptr, _rtl_impure_ptr->_stdout);
	return result;
}
#endif // ENAC_FLOAT

//----- vsnprintf()
int vsnprintf(char *str, size_t size, const char *fmt, __VALIST param) {
	int result;
	int w;
	int v11;
	FILE f;
#if CHECK_LIBC_INIT
	if (!libc_has_init) {
		rtl_libc_init();
	}
#endif	
	if (size >= 0) {
		if (size)
			w = size - 1;
		else
			w = 0;
		f._flags = 520;
		f._p = str;
		f._bf._base = str;
		f._w = w;
		f._bf._size = w;
		f._file = -1;
		result = __rtl_vfprintf_r_v1_00(_rtl_impure_ptr, &f, fmt, param);
		if (result + 1 < 0)
			_rtl_impure_ptr->_errno = 139;
		if (size)
			*f._p = 0;
	} else {
		_rtl_impure_ptr->_errno = 139;
		result = -1;
	}
	return result;
}

//----- vfprintf()
int vfprintf(FILE *fp, const char *fmt0, va_list ap) {
#if CHECK_LIBC_INIT
	if (!libc_has_init) {
		rtl_libc_init();
	}
#endif	
	return __rtl_vfprintf_r_v1_00(_rtl_impure_ptr, fp, fmt0, ap);
}

//----- memchr()
void * memchr(const void * src_void , int c , size_t length) {
	return __rtl_memchr_v1_00(src_void, c, length);
}

//----- memcmp()
int memcmp(const void *m1, const void *m2, size_t n) {
	return __rtl_memcmp_v1_00(m1, m2, n);
}

//----- memcpy()
void * memcpy(void *dst0, const void *src0, size_t len0) {
	return __rtl_memcpy_v1_00(dst0, src0, len0);
}

//----- memmove()
void * memmove(void *dst_void, const void *src_void, size_t length) {
	return __rtl_memmove_v1_00(dst_void, src_void, length);
}

//----- memset()
void * memset(void *m, int c, size_t n) {
	return __rtl_memset_v1_00(m, c, n);
}

//----- strcat()
char * strcat(char *s1, const char *s2) {
	return (char *) __rtl_strcat_v1_00(s1, s2);
}

//----- strchr()
char * strchr(const char *s1, int i) {
	return (char *) __rtl_strchr_v1_00(s1, i);
}

//----- strcmp()
int strcmp(const char *s1, const char *s2) {
	return __rtl_strcmp_v1_00(s1, s2);
}

//----- strcpy()
char * strcpy(char *dst0, const char *src0) {
	return (char *) __rtl_strcpy_v1_00(dst0, src0);
}

//----- strlen()
size_t strlen(const char *str) {
	return __rtl_strlen_v1_00(str);
}

//----- strncat()
char * strncat(char *s1, const char *s2, size_t n) {
	return (char *) __rtl_strncat_v1_00(s1, s2, n);
}

//----- strncmp()
int strncmp(const char *s1, const char *s2, size_t n) {
	return __rtl_strncmp_v1_00(s1, s2, n);
}

//----- strncpy()
char * strncpy(char *dst0, const char *src0, size_t count) {
	return (char *) __rtl_strncpy_v1_00(dst0, src0, count);
}

//----- strstr()
char * strstr(const char *searchee, const char *lookfor) {
	return (char *) __rtl_strstr_v1_00(searchee, lookfor);
}

//----- strsep()
char * strsep(char **source_ptr, const char *delim) {
	return (char *) __rtl_strsep_v1_00(source_ptr, delim);
}

//----- strtok()
char * strtok(char *s, const char *delim) {
	return (char *) __rtl_strtok_v1_00(s, delim);
}

int sscanf(const char *buf, const char *fmt, ...) {
	va_list args;
	int i;

	va_start(args, fmt);
	i = _vsscanf(buf, fmt, args);
	va_end(args);

	return i;
}

char toupper(char ch) {
 	return  ((ch >= 'a' && ch <= 'z') ? ch - 'a' + 'A' : ch);
};

int _stricmp (const char *s1, const char *s2)
{
  while (*s2 != 0 && toupper(*s1) == toupper(*s2))
    s1++, s2++;
  return (int) (toupper(*s1) - toupper(*s2));
}

unsigned long long __aeabi_llsr(unsigned long long val, unsigned int shift)
{
       u32 lo = ((u32)val >> shift) | ((u32)(val >> 32) << (32 - shift));
       u32 hi = (u32)val >> shift;

       return ((unsigned long long)hi << 32) | lo;
}

/*
#undef __VFP_FP__

#if defined(__VFP_FP__)
typedef	long __jmp_buf[10 + 8 + 1];	// d8-d15 fpu + fpscr
#else
typedef	long __jmp_buf[10];
#endif

int setjmp(__jmp_buf buf) __attribute__ ((noinline));
int setjmp(__jmp_buf buf)
{
	register void * r0 __asm__("r0") = buf;
	__asm__(
		"mov     %%ip, %%sp\n"
		"stmia %[store]!, {%%r4-%%r9, %%sl, %%fp, %%ip, %%lr}\n"
#if defined(__VFP_FP__)
		"vstmia %[store]!, {%%d8-%%d15}\n"
		"vmrs    %%r1, fpscr\n"
		"str	 %%r1, [%[store]], #4\n"
#endif
		"mov.w		%r0, #0\n"
			: : [store] "r" (r0) :);
}

void longjmp(__jmp_buf buf, long value)  __attribute__((noreturn));
void longjmp(__jmp_buf buf, long value)
{
	__asm__(
		"ldmia %[load]!, {%%r4-%%r9, %%sl, %%fp, %%ip, %%lr}\n"
#if defined(__VFP_FP__)
		"vldmia	%[load]!, {%%d8-%%d15}\n"
		"ldr     %%r0, [%[load]], #4\n"
		"vmsr    fpscr, %%r0\n"
#endif
		"mov     %%sp, %%ip\n"
		"movs    %%r0, %%r1\n"
		"it      eq\n"
		"moveq   %%r0, #1\n"
		"bx      lr\n"
			: : [load] "r" (buf), [value] "r" (value):);
	__builtin_unreachable();
}
*/

extern __attribute__ ((long_call)) unsigned int Rand(void);

unsigned int rand(void)
{
	return Rand();
}



//----- rtl_dtoi()
int __aeabi_dtoi(double d)
{
  return __rtl_dtoi_v1_00(d);
}

//----- __aeabi_dtoui()
int __aeabi_dtoui(double d)
{
  return __rtl_dtoui_v1_00(d);
}

//----- __aeabi_i2f()
float __aeabi_i2f(int val)
{
  return __rtl_itof_v1_00(val);
}

//----- __aeabi_i2d()
int __aeabi_i2d(int val)
{
  return __rtl_itod_v1_00(val);
}

//----- __aeabi_ui2f()
float __aeabi_ui2f(unsigned int val)
{
  return __rtl_uitof_v1_00(val);
}

//----- __aeabi_ui2d()
int __aeabi_ui2d(unsigned int val)
{
  return __rtl_uitod_v1_00(val);
}

//----- __aeabi_itoa()
char * __aeabi_itoa(int value, char *string, int radix)
{
  return (char *)__rtl_ltoa_v1_00(value, string, radix);
}

//----- __aeabi_ltoa()
char * __aeabi_ltoa(int value, char *string, int radix)
{
  return (char *)__rtl_ltoa_v1_00(value, string, radix);
}

//----- __aeabi_utoa()
char * __aeabi_utoa(unsigned int value, char *string, int radix)
{
  return (char *)__rtl_ultoa_v1_00(value, string, radix);
}

//----- __aeabi_ultoa()
char * __aeabi_ultoa(unsigned int value, char *string, int radix)
{
  return (char *)__rtl_ultoa_v1_00(value, string, radix);
}

//----- __aeabi_ftol()
int __aeabi_ftol(float f)
{
  return __rtl_ftol_v1_00(f);
}

//----- __aeabi_ftod()
int __aeabi_ftod(float f)
{
  return __rtl_ftod_v1_00(f);
}

//----- __aeabi_dtof()
float __aeabi_dtof(double d)
{
  return __rtl_dtof_v1_00(d);
}

//----- __aeabi_fadd()
float __aeabi_fadd(float a, float b)
{
  return __rtl_fadd_v1_00(a, b);
}

//----- __aeabi_fsub()
float __aeabi_fsub(float a, float b)
{
  return __rtl_fsub_v1_00(a, b);
}

//----- __aeabi_fmul()
float __aeabi_fmul(float a, float b)
{
  return __rtl_fmul_v1_00(a, b);
}

//----- __aeabi_fdiv()
float __aeabi_fdiv(float a, float b)
{
  return __rtl_fdiv_v1_00(a, b);
}

//----- __aeabi_dadd()
int __aeabi_dadd(double a, double b)
{
  return __rtl_dadd_v1_00(a, b);
}

//----- __aeabi_dsub()
int __aeabi_dsub(double a, double b)
{
  return __rtl_dsub_v1_00(a, b);
}

//----- __aeabi_dmul()
int __aeabi_dmul(double a, double b)
{
  return __rtl_dmul_v1_00(a, b);
}

//----- __aeabi_ddiv()
int __aeabi_ddiv(double a, double b)
{
  return __rtl_ddiv_v1_00(a, b);
}

//----- __aeabi_dcmpeq()
int __aeabi_dcmpeq(double a, double b)
{
  return __rtl_dcmpeq_v1_00(a, b);
}

//----- __aeabi_dcmplt()
int __aeabi_dcmplt(double a, double b)
{
  return __rtl_dcmplt_v1_00(a, b);
}

//----- __aeabi_dcmple()
int __aeabi_dcmple(double a, double b)
{
  return __rtl_dcmple_v1_00(a, b);
}

//----- __aeabi_dcmpgt()
int __aeabi_dcmpgt(double a, double b)
{
  return __rtl_dcmpgt_v1_00(a, b);
}

//----- __aeabi_fcmplt()
int __aeabi_fcmplt(float a, float b)
{
  return __rtl_fcmplt_v1_00(a, b);
}

//----- __aeabi_fcmpgt()
int __aeabi_fcmpgt(float a, float b)
{
  return __rtl_fcmpgt_v1_00(a, b);
}

/* 
 *  RTL871x1Ax: RAM libc 
 *  Created on: 22/02/2017
 *      Author: pvvx
 */

#include "rtl_bios_data.h"
#include "va_list.h"

#define NOT_CHECK_LIBC_INIT 1

//-------------------------------------------------------------------------
// Function declarations

//void libc_init();
//int snprintf(char *str, size_t size, const char *fmt, ...);
//int sprintf(char *str, const char *fmt, ...);
//int printf(const char *fmt, ...);
//int vprintf(const char *fmt, void *param);
//int vsnprintf(char *str, size_t size, const char *fmt, void *param);
//int vfprintf(FILE *fp, const char *fmt0, va_list ap);
//int memchr(const void *src_void, int c, size_t length);
//int memcmp(const void *m1, const void *m2, size_t n);
//int memcpy(void *dst0, const void *src0, size_t len0);
//int memmove(void *dst_void, const void *src_void, size_t length);
//int memset(void *m, int c, size_t n);
//char * strcat(char *s1, const char *s2);
//char * strchr(const char *s1, int i);
//int strcmp(const char *s1, const char *s2);
//char * strcpy(char *dst0, const char *src0);
//int strlen(const char *str);
//char * strncat(char *s1, const char *s2, size_t n);
//int strncmp(const char *s1, const char *s2, size_t n);
//char * strncpy(char *dst0, const char *src0, size_t count);
//char * strstr(const char *searchee, const char *lookfor);
//char * strsep(char **source_ptr, const char *delim);
//char * strtok(char *s, const char *delim);
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

extern int libc_has_init;
// extern impure_ptr
// extern impure_ptr

//-------------------------------------------------------------------------
// Function

//----- snprintf()
int snprintf(char *str, size_t size, const char *fmt, ...) {
	va_list args;
	va_start (args, fmt);
	int result;
	int w;
	FILE f;
#if NOT_CHECK_LIBC_INIT
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

//----- sprintf()
int sprintf(char *str, const char *fmt, ...) {
	FILE f;

#if NOT_CHECK_LIBC_INIT
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
#if NOT_CHECK_LIBC_INIT
	if (!libc_has_init) {
		rtl_libc_init();
	}
#endif	
	va_list args;
	va_start (args, fmt);
	int result = __rtl_vfprintf_r_v1_00(_rtl_impure_ptr,
			_rtl_impure_ptr->_stdout, fmt, args);
	__rtl_fflush_r_v1_00(_rtl_impure_ptr, _rtl_impure_ptr->_stdout);
	//	va_end (args);
	return result;
}

//----- vprintf()
int vprintf(const char * fmt, __VALIST param) {
//int vprintf(const char *fmt, void *param) {
#if NOT_CHECK_LIBC_INIT
	if (!libc_has_init) {
		rtl_libc_init();
	}
#endif	
	int result = __rtl_vfprintf_r_v1_00(_rtl_impure_ptr,
			_rtl_impure_ptr->_stdout, fmt, param);
	__rtl_fflush_r_v1_00(_rtl_impure_ptr, _rtl_impure_ptr->_stdout);
	return result;
}

//----- vsnprintf()
int vsnprintf(char *str, size_t size, const char *fmt, __VALIST param) {
	int result;
	int w;
	int v11;
	FILE f;
#if NOT_CHECK_LIBC_INIT
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
#if NOT_CHECK_LIBC_INIT
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

#define TOUPPER(CH) \
  (((CH) >= 'a' && (CH) <= 'z') ? ((CH) - 'a' + 'A') : (CH))

int _stricmp (const char *s1, const char *s2)
{
  while (*s2 != 0 && TOUPPER (*s1) == TOUPPER (*s2))
    s1++, s2++;
  return (int) (TOUPPER (*s1) - TOUPPER (*s2));
}

unsigned long long __aeabi_llsr(unsigned long long val, unsigned int shift)
{
       u32 lo = ((u32)val >> shift) | ((u32)(val >> 32) << (32 - shift));
       u32 hi = (u32)val >> shift;

       return ((unsigned long long)hi << 32) | lo;
}

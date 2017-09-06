/* 
 * ram_libc.o
 * pvvx 2016
 */

#include "rtl_bios_data.h"
#include "va_list.h"

#define CHECK_LIBC_INIT 0

//-------------------------------------------------------------------------
// Function declarations
#if 0
void rtl_libc_init(void);
int rtl_snprintf(char *str, size_t size, const char *fmt, ...);
int rtl_sprintf(char *str, const char *fmt, ...);
int rtl_printf(const char *fmt, ...);
int rtl_vprintf(const char *fmt, void *param);
int rtl_vsnprintf(char *str, size_t size, const char *fmt, void *param);
int rtl_vfprintf(FILE *fp, const char *fmt0, va_list ap);
void * rtl_memchr(const void * src_void , int c , size_t length);
int rtl_memcmp(const void *m1, const void *m2, size_t n);
void * rtl_memcpy(void *dst0, const void *src0, size_t len0);
void * rtl_memmove(void *dst_void, const void *src_void, size_t length);
void * rtl_memset(void *m, int c, size_t n);
char * rtl_strcat(char *s1, const char *s2);
char * rtl_strchr(const char *s1, int i);
int rtl_strcmp(const char *s1, const char *s2);
char * rtl_strcpy(char *dst0, const char *src0);
size_t rtl_strlen(const char *str);
char * rtl_strncat(char *s1, const char *s2, size_t n);
int rtl_strncmp(const char *s1, const char *s2, size_t n);
char * rtl_strncpy(char *dst0, const char *src0, size_t count);
char * rtl_strstr(const char *searchee, const char *lookfor);
char * rtl_strsep(char **source_ptr, const char *delim);
char * rtl_strtok(char *s, const char *delim);
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

char libc_has_init;
char print_off;
// extern rtl_impure_ptr
// extern impure_ptr

//-------------------------------------------------------------------------
// Function

//----- rtl_libc_init()
void rtl_libc_init(void) {
	__rom_mallocr_init_v1_00();
	init_rom_libgloss_ram_map();
	libc_has_init = 1;
}

//----- rtl_snprintf()
int rtl_snprintf(char *str, size_t size, const char *fmt, ...) {
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

//----- rtl_sprintf()
int rtl_sprintf(char *str, const char *fmt, ...) {
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

//----- rtl_printf()
int rtl_printf(const char *fmt, ...) {
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

//----- rtl_vprintf()
int rtl_vprintf(const char *fmt, va_list param) {
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

//----- rtl_vsnprintf()
int rtl_vsnprintf(char *str, size_t size, const char *fmt, va_list param) {
	int result;
	int w;
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

//----- rtl_vfprintf()
int rtl_vfprintf(FILE *fp, const char *fmt0, va_list ap) {
#if CHECK_LIBC_INIT
	if (!libc_has_init) {
		rtl_libc_init();
	}
#endif	
	return __rtl_vfprintf_r_v1_00(_rtl_impure_ptr, fp, fmt0, ap);
}

//----- rtl_memchr()
void * rtl_memchr(const void * src_void , int c , size_t length) {
	return __rtl_memchr_v1_00(src_void, c, length);
}

//----- rtl_memcmp()
int rtl_memcmp(const void *m1, const void *m2, size_t n) {
	return __rtl_memcmp_v1_00(m1, m2, n);
}

//----- rtl_memcpy()
void * rtl_memcpy(void *dst0, const void *src0, size_t len0) {
	return __rtl_memcpy_v1_00(dst0, src0, len0);
}

//----- rtl_memmove()
void * rtl_memmove(void *dst_void, const void *src_void, size_t length) {
	return __rtl_memmove_v1_00(dst_void, src_void, length);
}

//----- rtl_memset()
void * rtl_memset(void *m, int c, size_t n) {
	return __rtl_memset_v1_00(m, c, n);
}

//----- rtl_strcat()
char * rtl_strcat(char *s1, const char *s2) {
	return (char *) __rtl_strcat_v1_00(s1, s2);
}

//----- rtl_strchr()
char * rtl_strchr(const char *s1, int i) {
	return (char *) __rtl_strchr_v1_00(s1, i);
}

//----- rtl_strcmp()
int rtl_strcmp(const char *s1, const char *s2) {
	return __rtl_strcmp_v1_00(s1, s2);
}

//----- rtl_strcpy()
char * rtl_strcpy(char *dst0, const char *src0) {
	return (char *) __rtl_strcpy_v1_00(dst0, src0);
}

//----- rtl_strlen()
size_t rtl_strlen(const char *str) {
	return __rtl_strlen_v1_00(str);
}

//----- rtl_strncat()
char * rtl_strncat(char *s1, const char *s2, size_t n) {
	return (char *) __rtl_strncat_v1_00(s1, s2, n);
}

//----- rtl_strncmp()
int rtl_strncmp(const char *s1, const char *s2, size_t n) {
	return __rtl_strncmp_v1_00(s1, s2, n);
}

//----- rtl_strncpy()
char * rtl_strncpy(char *dst0, const char *src0, size_t count) {
	return (char *) __rtl_strncpy_v1_00(dst0, src0, count);
}

//----- rtl_strstr()
char * rtl_strstr(const char *searchee, const char *lookfor) {
	return (char *) __rtl_strstr_v1_00(searchee, lookfor);
}

//----- rtl_strsep()
char * rtl_strsep(char **source_ptr, const char *delim) {
	return (char *) __rtl_strsep_v1_00(source_ptr, delim);
}

//----- rtl_strtok()
char * rtl_strtok(char *s, const char *delim) {
	return (char *) __rtl_strtok_v1_00(s, delim);
}


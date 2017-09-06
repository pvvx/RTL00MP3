/* 
 * ram_libgloss_retarget.o
 * pvvx 2016
 */

#include "rtl_bios_data.h"

//-------------------------------------------------------------------------
// Function declarations

int ram_libgloss_close(int fildes);
int ram_libgloss_fstat(int fildes, struct stat *st);
int ram_libgloss_isatty(int file);
int ram_libgloss_lseek(int file, int ptr, int dir);
int ram_libgloss_read(int file, char *ptr, int len);
char *ram_libgloss_sbrk(int incr);
int ram_libgloss_write(int file, const char *ptr, int len);
int ram_libgloss_open(char *file, int flags, int mode);
void init_rom_libgloss_ram_map(void);
// Extern Calls:
//extern int HalSerialPutcRtl8195a();
//extern int rtl_strcmp();
extern char end;

//-------------------------------------------------------------------------
// Data declarations
static char *rheap_end;
// extern __rtl_errno;
// extern end;
// extern rom_libgloss_ram_map;

//----- ram_libgloss_close()
int ram_libgloss_close(int fildes) {
	__rtl_errno = 88;
	return -1;
}

//----- ram_libgloss_fstat()
int ram_libgloss_fstat(int fildes, struct stat *st) {
	int result;

	if ((unsigned int) fildes > 2) {
		__rtl_errno = 9;
		result = -1;
	} else {
		st->st_mode = 0x2000;
		result = 0;
	}
	return result;
}

//----- ram_libgloss_isatty()
int ram_libgloss_isatty(int file) {
	int result;

	if (file <= 2)
		result = 1;
	else
		result = -1;
	return result;
}

//----- ram_libgloss_lseek()
int ram_libgloss_lseek(int file, int ptr, int dir) {
	__rtl_errno = 88;
	return -1;
}

//----- ram_libgloss_read()
int ram_libgloss_read(int file, char *ptr, int len) {
	__rtl_errno = 88;
	return -1;
}

//----- ram_libgloss_sbrk()
char *ram_libgloss_sbrk(int incr) {
	char *prev_heap_end;

	if (!rheap_end)
		rheap_end = (char *) &end;
	prev_heap_end = rheap_end;
	rheap_end += incr;
#if CONFIG_DEBUG_LOG > 4
	DBG_8195A("ROM_heap = %p[%d], end = %p\n", prev_heap_end, incr, rheap_end);
#endif
	return prev_heap_end;
}

//----- ram_libgloss_write()
int ram_libgloss_write(int file, const char *ptr, int len) {
	int i;
	for (i = 0; i < len; ++i)
		HalSerialPutcRtl8195a(ptr[i]);
	return len;
}

//----- ram_libgloss_open()
int ram_libgloss_open(char *file, int flags, int mode) {
// file->_p
	int result = rtl_strcmp(file, "/stdin");

	if (result) {
		if (rtl_strcmp(file, "/stdout")) {
			if (rtl_strcmp(file, "/stderr"))
				result = -1;
			else
				result = 2;
		} else {
			result = 1;
		}
	} else
		result = 0;
	return result;
}

//----- init_rom_libgloss_ram_map()
void init_rom_libgloss_ram_map(void) {
	rom_libgloss_ram_map.libgloss_close = ram_libgloss_close;
	rom_libgloss_ram_map.libgloss_fstat = ram_libgloss_fstat;
	rom_libgloss_ram_map.libgloss_isatty = ram_libgloss_isatty;
	rom_libgloss_ram_map.libgloss_lseek = ram_libgloss_lseek;
	rom_libgloss_ram_map.libgloss_open = ram_libgloss_open;
	rom_libgloss_ram_map.libgloss_read = ram_libgloss_read;
	rom_libgloss_ram_map.libgloss_write = ram_libgloss_write;
	rom_libgloss_ram_map.libgloss_sbrk = (void*)ram_libgloss_sbrk;
}


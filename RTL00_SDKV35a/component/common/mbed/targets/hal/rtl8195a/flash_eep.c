/*
 * flash_eep.c
 *
 *  Created on: 19/01/2015
 *  Port RTL87xx: 15/10/16
 *      Author: pvvx
 */

#include <rtl8195a.h>
//#include <FreeRTOS.h>
//#include <queue.h>
#include <osdep_api.h>
#include <osdep_service.h>
#include "device_lock.h"
#include "flash_api.h"
#include "flash_eep.h"

//-----------------------------------------------------------------------------
#ifdef CONFIG_DEBUG_LOG
#define DEBUGSOO CONFIG_DEBUG_LOG
#else
#define DEBUGSOO 0
#endif

#define mMIN(a, b)  ((a < b)? a : b)
#define align(a) ((a + 3) & 0xFFFFFFFC)

typedef union // заголовок объекта сохранения feep
{
	struct {
	unsigned short size;
	unsigned short id;
	} __attribute__((packed)) n;
	unsigned int x;
} __attribute__((packed)) fobj_head;

#define fobj_head_size 4
#define fobj_x_free 0xffffffff
#define MAX_FOBJ_SIZE 512 // максимальный размер сохраняемых объeктов
#define FMEM_ERROR_MAX 5
//-----------------------------------------------------------------------------
flash_t	flash;
//QueueHandle_t flash_mutex;
//-----------------------------------------------------------------------------


/*-------------------------------------------------------------------------------------
 Копирует данные из области align(4) (flash, registers, ...) в область align(1) (ram)
--------------------------------------------------------------------------------------*/
void FLASH_EEP_ATTR copy_align4_to_align1(unsigned char * pd, void * ps, unsigned int len)
{
	union {
		unsigned char uc[4];
		unsigned int ud;
	}tmp;
	unsigned int *p = (unsigned int *)((unsigned int)ps & (~3));
	unsigned int xlen = (unsigned int)ps & 3;
	//	unsigned int size = len;

	if(xlen) {
		tmp.ud = *p++;
		while (len)  {
			len--;
			*pd++ = tmp.uc[xlen++];
			if(xlen & 4) break;
		}
	}
	xlen = len >> 2;
	while(xlen) {
		tmp.ud = *p++;
		*pd++ = tmp.uc[0];
		*pd++ = tmp.uc[1];
		*pd++ = tmp.uc[2];
		*pd++ = tmp.uc[3];
		xlen--;
	}
	if(len & 3) {
		tmp.ud = *p;
		pd[0] = tmp.uc[0];
		if(len & 2) {
			pd[1] = tmp.uc[1];
			if(len & 1) {
				pd[2] = tmp.uc[2];
			}
		}
	}
	//	return size;
}
/*------------------------------------------------------------------------------------
 Копирует данные из области align(1) (ram) в область align(4) (flash, registers)
--------------------------------------------------------------------------------------*/
void FLASH_EEP_ATTR copy_align1_to_align4(void * pd, unsigned char * ps, unsigned int len)
{
	union {
		unsigned char uc[4];
		unsigned int ud;
	}tmp;
	unsigned int *p = (unsigned int *)(((unsigned int)pd) & (~3));
	unsigned int xlen = (unsigned int)pd & 3;
//	unsigned int size = len;
	if(xlen) {
		tmp.ud = *p;
		while (len)  {
			len--;
			tmp.uc[xlen++] = *ps++;
			if(xlen & 4) break;
		}
		*p++ = tmp.ud;
	}
	xlen = len >> 2;
	while(xlen) {
		tmp.uc[0] = *ps++;
		tmp.uc[1] = *ps++;
		tmp.uc[2] = *ps++;
		tmp.uc[3] = *ps++;
		*p++ = tmp.ud;
		xlen--;
	}
	if(len & 3) {
		tmp.ud = *p;
		tmp.uc[0] = ps[0];
		if(len & 2) {
			tmp.uc[1] = ps[1];
			if(len & 1) {
				tmp.uc[2] = ps[2];
			}
		}
		*p = tmp.ud;
	}
//	return size;
}
/*------------------------------------------------------------------------------------
 Запись байта в область align(4) (flash, registers)
--------------------------------------------------------------------------------------*/
void FLASH_EEP_ATTR write_align4_chr(unsigned char *pd, unsigned char c)
{
	union {
		unsigned char uc[4];
		unsigned int ud;
	}tmp;
	unsigned int *p = (unsigned int *)((unsigned int)pd & (~3));
	unsigned int xlen = (unsigned int)pd & 3;
	tmp.ud = *p;
	tmp.uc[xlen] = c;
	*p = tmp.ud;
}
/*------------------------------------------------------------------------------------
 Чтение байта из области align(4) (flash, registers)
--------------------------------------------------------------------------------------*/
unsigned char FLASH_EEP_ATTR get_align4_chr(const unsigned char *ps)
{
	return (*((unsigned int *)((unsigned int)ps & (~3))))>>(((unsigned int)ps & 3) << 3);
}
/*------------------------------------------------------------------------------------
 Сравнение данных в области align(4) (flash, registers, ...) с областью align(1) (ram)
--------------------------------------------------------------------------------------*/
int FLASH_EEP_ATTR cmp_align1_align4(unsigned char * pd, void * ps, unsigned int len)
{
	union {
		unsigned char uc[4];
		unsigned int ud;
	}tmp;
	unsigned int *p = (unsigned int *)((unsigned int)ps & (~3));
	unsigned int xlen = (unsigned int)ps & 3;
	if(xlen) {
		tmp.ud = *p++;
		while (len)  {
			len--;
			if(*pd++ != tmp.uc[xlen++]) return 1;
			if(xlen & 4) break;
		}
	}
	xlen = len >> 2;
	while(xlen) {
		tmp.uc[0] = *pd++;
		tmp.uc[1] = *pd++;
		tmp.uc[2] = *pd++;
		tmp.uc[3] = *pd++;
		if(*p++ != tmp.ud) return 1;
		xlen--;
	}
	if(len & 3) {
		tmp.ud = *p;
		if(pd[0] != tmp.uc[0]) return 1;
		if(len & 2) {
			if(pd[1] != tmp.uc[1]) return 1;
			if(len & 1) {
				if(pd[2] != tmp.uc[2]) return 1;
			}
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
LOCAL void _fwrite_word(unsigned int addr, unsigned int dw)
{
	//Write word
	HAL_WRITE32(SPI_FLASH_BASE, addr, dw);

	// Wait spic busy done
	SpicWaitBusyDoneRtl8195A();

	// Wait flash busy done (wip=0)
    if(flashobj.SpicInitPara.flashtype == FLASH_MICRON)
        SpicWaitOperationDoneRtl8195A(flashobj.SpicInitPara);
    else
        SpicWaitWipDoneRefinedRtl8195A(flashobj.SpicInitPara);
}
//-----------------------------------------------------------------------------
// FunctionName : get_addr_bscfg
// Опции:
//  false - поиск текушего сегмента
//  true - поиск нового сегмента для записи (pack)
// Returns     : новый адрес сегмента для записи
// ret < FMEM_ERROR_MAX - ошибка
//-----------------------------------------------------------------------------
LOCAL FLASH_EEP_ATTR unsigned int get_addr_bscfg(bool flg)
{
	unsigned int x1 = (flg)? 0 : 0xFFFFFFFF, x2;
	unsigned int faddr = FMEMORY_SCFG_BASE_ADDR;
	unsigned int reta = FMEMORY_SCFG_BASE_ADDR;
	do {
		x2 = HAL_READ32(SPI_FLASH_BASE, faddr); // if(flash_read(faddr, &x2, 4)) return -(FMEM_FLASH_ERR);
		if(flg) { // поиск нового сегмента для записи (pack)
			if(x2 > x1 || x2 == 0xFFFFFFFF) {
				x1 = x2;
				reta = faddr; // новый адрес сегмента для записи
			}
		}
		else if(x2 < x1) { // поиск текушего сегмента
			x1 = x2;
			reta = faddr; // новый адрес сегмента для записи
		};
		faddr += FMEMORY_SCFG_BANK_SIZE;
	} while(faddr < (FMEMORY_SCFG_BASE_ADDR + FMEMORY_SCFG_BANKS * FMEMORY_SCFG_BANK_SIZE));

	if((!flg)&&(x1 == 0xFFFFFFFF)&&(reta == FMEMORY_SCFG_BASE_ADDR)) {
		_fwrite_word(reta, --x1); // if(flash_write(reta, &x1, 4)) return -(FMEM_FLASH_ERR);
	}
#if DEBUGSOO > 3
		DBG_FEEP_INFO("base seg: %p [%d]\n", reta, x2);
#endif
	return reta;
}
//-----------------------------------------------------------------------------
// FunctionName : get_addr_fobj
// Опции:
//  false - Поиск последней записи объекта по id и size
//  true - Поиск присуствия записи объекта по id и size
// Returns : адрес записи данных объекта
// 0 - не найден
// ret < FMEM_ERROR_MAX - ошибка
//-----------------------------------------------------------------------------
LOCAL FLASH_EEP_ATTR unsigned int get_addr_fobj(unsigned int base, fobj_head *obj, bool flg)
{
//	if(base == 0) return 0;
	fobj_head fobj;
	unsigned int faddr = base + 4;
	unsigned int fend = base + FMEMORY_SCFG_BANK_SIZE - align(fobj_head_size);
	unsigned int reta = 0;
	do {
		fobj.x = HAL_READ32(SPI_FLASH_BASE, faddr); // if(flash_read(faddr, &fobj, fobj_head_size)) return -(FMEM_FLASH_ERR);
		if(fobj.x == fobj_x_free) break;
		if(fobj.n.size <= MAX_FOBJ_SIZE) {
			if(fobj.n.id == obj->n.id) {
				if(flg) {
					return faddr;
				}
				obj->n.size = fobj.n.size;
				reta = faddr;
			}
			faddr += align(fobj.n.size + fobj_head_size);
		}
		else faddr += align(MAX_FOBJ_SIZE + fobj_head_size);
	}
	while(faddr < fend);
	return reta;
}
//-----------------------------------------------------------------------------
// FunctionName : get_addr_fend
// Поиск последнего адреса в сегменте для записи объекта
// Returns : адрес для записи объекта
// ret < FMEM_ERROR_MAX - ошибка
// ret = 0 - не влезет, на pack
//-----------------------------------------------------------------------------
LOCAL FLASH_EEP_ATTR unsigned int get_addr_fobj_save(unsigned int base, fobj_head obj)
{
	fobj_head fobj;
	unsigned int faddr = base + 4;
	unsigned int fend = base + FMEMORY_SCFG_BANK_SIZE - align(obj.n.size + fobj_head_size);
	do {
		fobj.x = HAL_READ32(SPI_FLASH_BASE, faddr); // if(flash_read(faddr, &fobj, fobj_head_size)) return -(FMEM_FLASH_ERR);
		if(fobj.x == fobj_x_free) {
			if(faddr < fend) {
				return faddr;
			}
			return 0; // не влезет, на pack
		}
		if(fobj.n.size <= MAX_FOBJ_SIZE) {
			faddr += align(fobj.n.size + fobj_head_size);
		}
		else faddr += align(MAX_FOBJ_SIZE + fobj_head_size);
	}
	while(faddr < fend);
	return 0; // не влезет, на pack
}
//=============================================================================
// FunctionName : pack_cfg_fmem
// Returns      : адрес для записи объекта
//-----------------------------------------------------------------------------
LOCAL FLASH_EEP_ATTR unsigned int pack_cfg_fmem(fobj_head obj)
{
	fobj_head fobj;
	unsigned int fnewseg = get_addr_bscfg(true); // поиск нового сегмента для записи (pack)
	if(fnewseg < FMEM_ERROR_MAX) return fnewseg; // error
	unsigned int foldseg = get_addr_bscfg(false); // поиск текушего сегмента
	if(foldseg < FMEM_ERROR_MAX) return fnewseg; // error
	unsigned int faddr = foldseg;
	unsigned int rdaddr, wraddr;
	unsigned short len;
	unsigned int * pbuf = (unsigned int *) malloc(align(MAX_FOBJ_SIZE + fobj_head_size) >> 2);
	if(pbuf == NULL) {
#if DEBUGSOO > 1
		DBG_FEEP_ERR("pack malloc error!\n");
#endif
		return -(FMEM_MEM_ERR);
	}
#if DEBUGSOO > 3
	DBG_FEEP_INFO("repack base to new seg: %p\n", fnewseg);
#endif
	SpicSectorEraseFlashRtl8195A(fnewseg); //	if(flash_erase_sector(fnewseg)) return -(FMEM_FLASH_ERR);
	faddr += 4;
	wraddr = fnewseg + 4;
	do {
		fobj.x = HAL_READ32(SPI_FLASH_BASE, faddr); //if(flash_read(faddr, &fobj, fobj_head_size)) return -(FMEM_FLASH_ERR); // последовательное чтение id из старого сегмента
		if(fobj.x == fobj_x_free) break;
		if(fobj.n.size > MAX_FOBJ_SIZE) len = align(MAX_FOBJ_SIZE + fobj_head_size);
		else len = align(fobj.n.size + fobj_head_size);
		if(fobj.n.id != obj.n.id &&  fobj.n.size <= MAX_FOBJ_SIZE) { // объект валидный
			if(get_addr_fobj(fnewseg, &fobj, true) == 0) { // найдем, сохранили ли мы его уже? нет
				rdaddr = get_addr_fobj(foldseg, &fobj, false); // найдем последнее сохранение объекта в старом сенгменте, size изменен
				if(rdaddr < FMEM_ERROR_MAX) return rdaddr; // ???
				if(wraddr + len >= fnewseg + FMEMORY_SCFG_BANK_SIZE) {
#if DEBUGSOO > 1
					DBG_FEEP_ERR("pack segment overflow!\n");
#endif
					return -(FMEM_OVR_ERR);
				};
#if 0
				copy_align4_to_align1((uint8 *)pbuf, rdaddr, len);
#else
				SpicUserReadFourByteRtl8195A(len, rdaddr, (unsigned int *)pbuf, flashobj.SpicInitPara.Mode.BitMode);
#endif
				int i = 0;
				int size4b = len >> 2;
				// перепишем данные obj в новый сектор
				while(size4b--) {
					_fwrite_word(wraddr, pbuf[i++]); // if(flash_write(wraddr, &pbuf[i++], 4)) return -(FMEM_FLASH_ERR);
				};
			};
		};
		faddr += len;
	} while(faddr  < (foldseg + FMEMORY_SCFG_BANK_SIZE - align(fobj_head_size+1)));
	free(pbuf);
	// обратный счетчик стираний/записей секторов как id
	_fwrite_word(fnewseg, HAL_READ32(SPI_FLASH_BASE, foldseg) - 1); // if(flash_write(fnewseg, &foldseg + SPI_FLASH_BASE, 4)) return -(FMEM_FLASH_ERR);
#if DEBUGSOO > 3
	DBG_FEEP_INFO("free: %d\n", FMEMORY_SCFG_BANK_SIZE - (faddr & (FMEMORY_SCFG_BANK_SIZE-1)));
#endif
	return get_addr_fobj_save(fnewseg, obj); // адрес для записи объекта;
}
//-----------------------------------------------------------------------------
LOCAL signed short FLASH_EEP_ATTR _flash_write_cfg(void *ptr, unsigned short id, unsigned short size)
{
	fobj_head fobj;
	fobj.n.id = id;
	fobj.n.size = size;
	bool retb = false;
	unsigned int faddr = get_addr_bscfg(false);

	if(faddr >= FMEM_ERROR_MAX)  {
		unsigned int xfaddr = get_addr_fobj(faddr, &fobj, false);
		if(xfaddr > FMEM_ERROR_MAX && size == fobj.n.size) {
			if(size == 0
					|| cmp_align1_align4(ptr, (void *)SPI_FLASH_BASE + xfaddr + fobj_head_size, size) == 0) {
#if DEBUGSOO > 3
					DBG_FEEP_INFO("write obj is identical, id: %04x [%d]\n", id, size);
#endif
					return size; // уже записано то-же самое
			}
#if DEBUGSOO > 100
			else {
				int i;
				uint8 * p = (uint8 *)(SPI_FLASH_BASE + xfaddr + fobj_head_size);
				uint8 * r = (uint8 *) ptr;
				for(i=0; i < size; i+=8) {
					DBG_8195A("buf[%d]\t%02X %02X %02X %02X  %02X %02X %02X %02X\n",
								i, r[i], r[i+1], r[i+2], r[i+3], r[i+4], r[i+5], r[i+6], r[i+7]);
					DBG_8195A("obj[%d]\t%02X %02X %02X %02X  %02X %02X %02X %02X\n",
								i, p[i], p[i+1], p[i+2], p[i+3], p[i+4], p[i+5], p[i+6], p[i+7]);
				}
			}
#endif
		}
	}
#if DEBUGSOO > 2
	DBG_FEEP_INFO("write obj id: %04x [%d]\n", id, size);
#endif
	fobj.n.size = size;
	faddr = get_addr_fobj_save(faddr, fobj);
	if(faddr == 0) {
		faddr = pack_cfg_fmem(fobj);
		if(faddr == 0) return FMEM_NOT_FOUND;
	}
	else if(faddr < FMEM_ERROR_MAX) return - faddr - 1; // error

#if DEBUGSOO > 3
	DBG_FEEP_INFO("write obj to faddr %p\n", faddr);
#endif
	_fwrite_word(faddr, fobj.x); // if(flash_write(faddr, &fobj.x, 4)) return FMEM_FLASH_ERR;
	faddr+=4;
	union {
		unsigned char uc[4];
		unsigned int ud;
	}tmp;
#if 0
	u32 len = (size + 3) & (~3);
	if(len) SpicUserProgramRtl8195A((u8 *)ptr, 1, faddr, len);
#else
	u32 len = (size + 3) >> 2;
	uint8 * ps = ptr;
  	while(len--) {
		tmp.uc[0] = *ps++;
		tmp.uc[1] = *ps++;
		tmp.uc[2] = *ps++;
		tmp.uc[3] = *ps++;
		_fwrite_word(faddr, tmp.ud); // if(flash_write(faddr, &tmp.ud, 4)) return FMEM_FLASH_ERR;
		faddr += 4;
	}
#endif
	return size;
}
//=============================================================================
//- Сохранить объект в flash --------------------------------------------------
//  Returns	: false/true
//-----------------------------------------------------------------------------
bool FLASH_EEP_ATTR flash_write_cfg(void *ptr, unsigned short id, unsigned short size)
{
	if(size > MAX_FOBJ_SIZE) return false;
	bool retb = false;
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	// SPIC Init
    flash_turnon();
    if(fspic_isinit == 0) flash_init(&flashobj);
	if(_flash_write_cfg(ptr, id, size) >= 0) {
#if DEBUGSOO > 3
		DBG_FEEP_INFO("saved ok\n");
#endif
		retb = true;
	}
    SpicDisableRtl8195A();
    device_mutex_unlock(RT_DEV_LOCK_FLASH);
	return retb;
}
//=============================================================================
//- Прочитать объект из flash -------------------------------------------------
//  Параметры:
//   prt - указатель, куда сохранить
//   id - идентификатор искомого объекта
//   maxsize - сколько байт сохранить максимум из найденного объекта, по ptr
//  Returns:
//  -3 - error
//  -2 - flash rd/wr/clr error
//  -1 - не найден
//   0..MAX_FOBJ_SIZE - ok, сохраненный размер объекта
//-----------------------------------------------------------------------------
signed short FLASH_EEP_ATTR flash_read_cfg(void *ptr, unsigned short id, unsigned short maxsize)
{
    signed short rets = FMEM_ERROR;
	if (maxsize <= MAX_FOBJ_SIZE) {
		device_mutex_lock(RT_DEV_LOCK_FLASH);
		fobj_head fobj;
		fobj.n.id = id;
		fobj.n.size = 0;
#if DEBUGSOO > 2
		DBG_FEEP_INFO("read obj id: %04x[%d]\n", id, maxsize);
#endif
		// SPIC Init
	    flash_turnon();
	    if(fspic_isinit == 0) flash_init(&flashobj);
		unsigned int faddr = get_addr_bscfg(false);
		if(faddr >= FMEM_ERROR_MAX) {
			faddr = get_addr_fobj(faddr, &fobj, false);
			if(faddr >= FMEM_ERROR_MAX) {
#if 0
				if(maxsize != 0 && ptr != NULL)
					copy_align4_to_align1(ptr, SPI_FLASH_BASE + faddr + fobj_head_size, mMIN(fobj.n.size, maxsize));
#else
				if(maxsize != 0 && ptr != NULL)
					SpicUserReadRtl8195A(mMIN(fobj.n.size, maxsize), faddr + fobj_head_size, ptr, flashobj.SpicInitPara.Mode.BitMode);
#endif
#if DEBUGSOO > 3
				DBG_FEEP_INFO("read ok, faddr: %p, size: %d\n", faddr,  fobj.n.size);
#endif
				rets = fobj.n.size;
			}
			else {
#if DEBUGSOO > 3
				DBG_FEEP_INFO("obj not found\n");
#endif
				rets = -faddr-1;
			}
		}
		else rets = -faddr-1;
	    SpicDisableRtl8195A();
		device_mutex_unlock(RT_DEV_LOCK_FLASH);
	}
    return rets;
}
//=============================================================================

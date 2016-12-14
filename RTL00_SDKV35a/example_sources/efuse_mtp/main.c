#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "hal_efuse.h"
#include "efuse_api.h"
#include "osdep_service.h"
#include "device_lock.h"

_LONG_CALL_ROM_ extern u32 HALEFUSEOneByteReadROM(IN	u32 CtrlSetting, IN	u16 Addr, OUT u8 *Data, IN u8 L25OutVoltage);
_LONG_CALL_ROM_ extern u32 HALEFUSEOneByteWriteROM(IN	u32 CtrlSetting, IN	u16 Addr, IN u8 Data, IN u8 L25OutVoltage);


//====================================================== Start libs
//-----
int _HalEFUSEPowerSwitch8195AROM(uint8_t bWrite, uint8_t PwrState, uint8_t L25OutVoltage) {
	  if (PwrState == 1) {
		  HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_EEPROM_CTRL0, (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EEPROM_CTRL0) & 0xFFFFFF) | 0x69000000); // EFUSE_UNLOCK
		  if (!(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_FUNC_EN) & BIT_SYS_FEN_EELDR))	// REG_SYS_FUNC_EN BIT_SYS_FEN_EELDR ?
			  HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_FUNC_EN, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_FUNC_EN) | BIT_SYS_FEN_EELDR);
		  if (!(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL0) & BIT_SYSON_CK_EELDR_EN))	// REG_SYS_CLK_CTRL0 BIT_SYSON_CK_EELDR_EN ?
			  HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL0, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL0) | BIT_SYSON_CK_EELDR_EN);
		  if (!(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & BIT_PESOC_EELDR_CK_SEL)) // REG_SYS_CLK_CTRL1 BIT_PESOC_EELDR_CK_SEL ?
			  HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) | BIT_PESOC_EELDR_CK_SEL);
		  if (bWrite == 1)
			  HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_REGU_CTRL0, (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_REGU_CTRL0) & 0xFFFFF0FF) | BIT_SYS_REGU_LDO25E_EN | BIT_SYS_REGU_LDO25E_ADJ(L25OutVoltage));
	  }
	  else
	  {
		  HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_EEPROM_CTRL0, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EEPROM_CTRL0) & 0xFFFFFF); // EFUSE_UNLOCK
		  if ( bWrite == 1 )
			  HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_REGU_CTRL0, (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_REGU_CTRL0) & (~BIT_SYS_REGU_LDO25E_EN)));
	  }
	  return bWrite;
}

//-----
int _HALEFUSEOneByteReadROM(uint32_t CtrlSetting, uint16_t Addr, uint8_t *Data, uint8_t L25OutVoltage)
{
int i = 0, ret = 0;
	if ( (Addr <= 0xFF) || ((CtrlSetting & 0xFFFF) == 0x26AE) ) {
		_HalEFUSEPowerSwitch8195AROM(1, 1, L25OutVoltage);

		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_TEST, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_TEST) & (~BIT_SYS_EF_FORCE_PGMEN));
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL,
				(CtrlSetting & (~(BIT_SYS_EF_RWFLAG | (BIT_MASK_SYS_EF_ADDR << BIT_SHIFT_SYS_EF_ADDR)	| (BIT_MASK_SYS_EF_DATA << BIT_SHIFT_SYS_EF_DATA))))
				| BIT_SYS_EF_ADDR(Addr));
		if(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL) & BIT_SYS_EF_RWFLAG) {
			*Data = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL);
			ret = 1;
		}
		else while(1) {
			HalDelayUs(1000);
			if(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL) & BIT_SYS_EF_RWFLAG) {
				*Data = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL);
				ret = 1;
				break;
			}
			if (i++ >= 100) {
				*Data = -1;
				ret = 1;
				break;
			};
		};
		_HalEFUSEPowerSwitch8195AROM(1, 0, L25OutVoltage);
	}
	else *Data = -1;
	return ret;
}

//-----
int _HALOTPOneByteReadRAM(uint32_t CtrlSetting, int Addr, uint8_t *Data, uint8_t L25OutVoltage)
{
	int result;
	  if ( (unsigned int)(Addr - 128) > 0x1F )
	    result = 1;
	  else
	    result = _HALEFUSEOneByteReadROM(CtrlSetting, Addr, Data, L25OutVoltage);
	return result;
}

//-----
int _HALEFUSEOneByteReadRAM(uint32_t CtrlSetting, int Addr, uint8_t *Data, uint8_t L25OutVoltage)
{
	int result;

	if ( (unsigned int)(Addr - 160) > 0x33 )
	{
		result = _HALEFUSEOneByteReadROM(CtrlSetting, Addr, Data, L25OutVoltage);
	}
	else
	{
		*Data = -1;
		result = 1;
	}
	return result;
}

//-----
void _ReadEOTPContant(uint8_t *pContant)
{
	int i;
	for(i = 0; i < 32; i++ )
	    _HALOTPOneByteRead(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), i+128, &pContant[i], L25EOUTVOLTAGE);
}

//-----
void _ReadEfuseContant(int UserCode, uint8_t *pContant)
{
#define	EFUSE_SECTION 11
  uint8_t *pbuf;
  int eFuse_Addr;
  int offset;
  int bcnt;
  int i, j;
  uint8_t DataTemp0;
  uint8_t DataTemp1;

  pbuf = pContant;
  eFuse_Addr = 0;
  do {
    _HALEFUSEOneByteReadRAM(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), eFuse_Addr, &DataTemp0, L25EOUTVOLTAGE);
    if ( DataTemp0 == 0x0FF )  break;
    if ( (DataTemp0 & 0x0F) == 0x0F ) {
      _HALEFUSEOneByteReadRAM(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), ++eFuse_Addr, &DataTemp1, L25EOUTVOLTAGE);
      offset = ((DataTemp1 & 0x0F0) | (DataTemp0 >> 4)) >> 1;
      bcnt = (~DataTemp1) & 0x0F;
      if (((UserCode + EFUSE_SECTION) << 2) > offset || offset >= ((UserCode + EFUSE_SECTION + 1) << 2))  {
        while (bcnt)
        {
          if (bcnt & 1) eFuse_Addr += 2;
          bcnt >>= 1;
        }
      }
      else
      {
        int base = (offset - ((EFUSE_SECTION + UserCode) << 2)) << 3;
        j = 0;
        while ( bcnt )
        {
          if ( bcnt & 1 )
          {
            _HALEFUSEOneByteReadRAM(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), ++eFuse_Addr, &pbuf[base + j], L25EOUTVOLTAGE);
            _HALEFUSEOneByteReadRAM(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), ++eFuse_Addr, &pbuf[base + j + 1], L25EOUTVOLTAGE);
          }
          bcnt >>= 1;
          j += 2;
        }
      }
    }
    else
    {
      for (i = (~DataTemp0) & 0x0F; i; i >>= 1 )
      {
        if (i & 1) eFuse_Addr += 2;
      }
    }
    eFuse_Addr++;
  }
  while (eFuse_Addr <= 0x7E);
}

//-----
void _ReadEfuseContant1(uint8_t *pContant)
{
	_ReadEfuseContant(0, pContant);
}

//-----
void _ReadEfuseContant2(uint8_t *pContant)
{
	_ReadEfuseContant(1, pContant);
}

//-----
void _ReadEfuseContant3(uint8_t *pContant)
{
	_ReadEfuseContant(2, pContant);
}


int _efuse_otp_read(u8 address, u8 len, u8 *buf)
{
	u8 content[32];	// the OTP max length is 32

	if((address + len) > 32) return -1;
	_ReadEOTPContant(content);
	_memcpy(buf, content + address, len);
	return 0;
}
//====================================================== end libs

//======================================================
// OTP : one time programming
//======================================================

uint8_t buf[128];

#define OTP_MAX_LEN 32		// The OTP max length is 32 bytes
static void efuse_otp_task(void *param)
{
	int ret;
	u8 i;
	
	DBG_8195A("\nefuse OTP block: Test Start\n");
	// read OTP content
	device_mutex_lock(RT_DEV_LOCK_EFUSE);
	ret = efuse_otp_read(0, OTP_MAX_LEN, buf);
	device_mutex_unlock(RT_DEV_LOCK_EFUSE);
	if(ret < 0){
		DBG_8195A("efuse OTP block: read address and length error\n");
		goto exit;
	}
	for(i=0; i<OTP_MAX_LEN; i+=8){
		DBG_8195A("[%d]\t%02X %02X %02X %02X  %02X %02X %02X %02X\n",
					i, buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
	}
	int x = 0;
	while(x < 1024) {
		DBG_8195A("efuse OTP block at %d:\n", x);
		device_mutex_lock(RT_DEV_LOCK_EFUSE);
		for(i = 0; i < sizeof(buf); i++ )
//			_HALEFUSEOneByteReadROM(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), i+x, &buf[i], L25EOUTVOLTAGE);
			_HALEFUSEOneByteReadROM(0x26AF, i+x, &buf[i], L25EOUTVOLTAGE);
		device_mutex_unlock(RT_DEV_LOCK_EFUSE);
		for(i = 0; i < sizeof(buf); i+=8){
			DBG_8195A("[%04x]\t%02X %02X %02X %02X  %02X %02X %02X %02X\n",
						i+x, buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
		}
		x+=sizeof(buf);
	}



/*
	// write OTP content
	_memset(buf, 0xFF, OTP_MAX_LEN);
	if(0){ // fill your data
		for(i=0; i<OTP_MAX_LEN; i++)
			buf[i] = i;
	}
	if(0){ // write
		device_mutex_lock(RT_DEV_LOCK_EFUSE);
		ret = efuse_otp_write(0, OTP_MAX_LEN, buf);
		device_mutex_unlock(RT_DEV_LOCK_EFUSE);
		if(ret < 0){
			DBG_8195A("efuse OTP block: write address and length error\n");
			goto exit;
		}
		DBG_8195A("\nWrite Done.\n");
	}
	DBG_8195A("\n");
	
	// read OTP content
	device_mutex_lock(RT_DEV_LOCK_EFUSE);
	ret = efuse_otp_read(0, OTP_MAX_LEN, buf);
	device_mutex_unlock(RT_DEV_LOCK_EFUSE);
	if(ret < 0){
		DBG_8195A("efuse OTP block: read address and length error\n");
		goto exit;
	}
	for(i=0; i<OTP_MAX_LEN; i+=8){
		DBG_8195A("[%d]\t%02X %02X %02X %02X  %02X %02X %02X %02X\n",
					i, buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
	}
*/
	DBG_8195A("efuse OTP block: Test Done\n");
	vTaskDelete(NULL);
	
exit:
	DBG_8195A("efuse OTP block: Test Fail!\n");
	vTaskDelete(NULL);
}

//======================================================
// MTP : M? time programming
//======================================================

#define MTP_MAX_LEN 32		// The MTP max length is 32 bytes
static void efuse_mtp_task(void *param)
{
	int ret;
	u8 i;
	
	DBG_8195A("\nefuse MTP block: Test Start\n");
	// read MTP content
	_memset(buf, 0xFF, MTP_MAX_LEN);
	device_mutex_lock(RT_DEV_LOCK_EFUSE);
	efuse_mtp_read(buf);
	device_mutex_unlock(RT_DEV_LOCK_EFUSE);
	for(i=0; i<MTP_MAX_LEN; i+=8){
		DBG_8195A("[%d]\t%02X %02X %02X %02X  %02X %02X %02X %02X\n",
					i, buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
	}
	DBG_8195A("\nefuse MTP block: Test Start\n");
	// read MTP content
	_memset(buf, 0xFF, MTP_MAX_LEN);
	device_mutex_lock(RT_DEV_LOCK_EFUSE);
	_ReadEfuseContant1(buf);
	device_mutex_unlock(RT_DEV_LOCK_EFUSE);
	for(i=0; i<MTP_MAX_LEN; i+=8){
		DBG_8195A("[%d]\t%02X %02X %02X %02X  %02X %02X %02X %02X\n",
					i, buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
	}
/*
	// write MTP content
	_memset(buf, 0xFF, MTP_MAX_LEN);
	if(0){ // fill your data
		for(i=0; i<MTP_MAX_LEN; i++)
			buf[i] = i;
	}
	if(0){ // write
		device_mutex_lock(RT_DEV_LOCK_EFUSE);
		ret = efuse_mtp_write(buf, MTP_MAX_LEN);
		device_mutex_unlock(RT_DEV_LOCK_EFUSE);
		if(ret < 0){
			DBG_8195A("efuse MTP block: write length error\n");
			goto exit;
		}
		DBG_8195A("\nWrite Done\n");
		DBG_8195A("Remain %d\n", efuse_get_remaining_length());
	}
	DBG_8195A("\n");
	
	// read MTP content
	_memset(buf, 0xFF, MTP_MAX_LEN);
	device_mutex_lock(RT_DEV_LOCK_EFUSE);
	efuse_mtp_read(buf);
	device_mutex_unlock(RT_DEV_LOCK_EFUSE);
	for(i=0; i<MTP_MAX_LEN; i+=8){
		DBG_8195A("[%d]\t%02X %02X %02X %02X  %02X %02X %02X %02X\n",
					i, buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
	}
	
*/
	DBG_8195A("efuse MTP block: Test Done\n");
	vTaskDelete(NULL);
exit:
	DBG_8195A("efuse MTP block: Test Fail!\n");
	vTaskDelete(NULL);
}

void main(void)
{
    ConfigDebugErr = -1; // ~_DBG_GDMA_;
    ConfigDebugInfo = -1; // ~_DBG_GDMA_;
    ConfigDebugWarn = -1; // ~_DBG_GDMA_;

    DBG_8195A("EFUSE_CTRL=%08x\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL));

	if(xTaskCreate(efuse_mtp_task, ((const char*)"efuse_mtp_task"), 512, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(efuse_mtp_task) failed", __FUNCTION__);

	if(xTaskCreate(efuse_otp_task, ((const char*)"efuse_otp_task"), 512, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(efuse_otp_task) failed", __FUNCTION__);
	
	/*Enable Schedule, Start Kernel*/
	if(rtw_get_scheduler_state() == OS_SCHEDULER_NOT_STARTED)
		vTaskStartScheduler();
	else
		vTaskDelete(NULL);
}

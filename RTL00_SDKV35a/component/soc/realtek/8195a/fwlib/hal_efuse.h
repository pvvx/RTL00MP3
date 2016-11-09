/*
 *  Routines to access hardware
 *  hal_efuse.h
 */

#ifndef _HAL_EFUSE_H_
#define _HAL_EFUSE_H_

_LONG_CALL_ROM_ extern VOID HalEFUSEPowerSwitch8195AROM(IN u8 bWrite, IN u8 PwrState, IN u8 L25OutVoltage);
_LONG_CALL_ROM_ extern u32 HALEFUSEOneByteReadROM(IN	u32 CtrlSetting, IN	u16 Addr, OUT u8 *Data, IN u8 L25OutVoltage);
_LONG_CALL_ROM_ extern u32 HALEFUSEOneByteWriteROM(IN	u32 CtrlSetting, IN	u16 Addr, IN u8 Data, IN u8 L25OutVoltage);

int HALOTPOneByteReadRAM(IN unsigned int CtrlSetting, IN unsigned short Addr, OUT unsigned char *Data, IN unsigned char L25OutVoltage);
int HALOTPOneByteWriteRAM(IN unsigned int CtrlSetting, IN unsigned short Addr, IN unsigned char Data, IN unsigned char L25OutVoltage);
int HALEFUSEOneByteReadRAM(IN unsigned int CtrlSetting, IN unsigned short Addr, IN unsigned char *Data, IN unsigned char L25OutVoltage);
int HALEFUSEOneByteWriteRAM(IN unsigned int CtrlSetting, IN unsigned short Addr, IN unsigned char Data, IN unsigned char L25OutVoltage);


void ReadEfuseContant(IN unsigned char UserCode, OUT unsigned char *pContant);
void ReadEfuseContant1(OUT unsigned char *pContant);
void ReadEfuseContant2(OUT unsigned char *pContant);
void ReadEfuseContant3(OUT unsigned char *pContant);
int GetRemainingEfuseLength(void);

int WriteEfuseContant(IN unsigned char UserCode, IN unsigned char CodeWordNum, IN unsigned char WordEnable, unsigned char *pContant);
int WriteEfuseContant1(IN unsigned char CodeWordNum, IN unsigned char WordEnable, IN unsigned char *pContant);
int WriteEfuseContant2(IN unsigned char CodeWordNum, IN unsigned char WordEnable, IN unsigned char *pContant);
int WriteEfuseContant3(IN unsigned char CodeWordNum, IN unsigned char WordEnable, IN unsigned char *pContant);

void ReadEOTPContant(IN unsigned char *pContant);
void WriteEOTPContant(OUT unsigned char *pContant);
void HALJtagOff(void);

#define EFUSERead8     HALEFUSEOneByteReadRAM
#define EFUSEWrite8    HALEFUSEOneByteWriteRAM

#define L25EOUTVOLTAGE	7
#endif


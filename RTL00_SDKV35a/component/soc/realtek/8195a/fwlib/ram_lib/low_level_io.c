/* 
	decompiled low_level_io.o
*/
#include ......
//-------------------------------------------------------------------------
// Function declarations
void mode_init();
void HalSerialPutcRtl8195a(int c, int a2, char a3);
signed int DiagPrintf(const char *fmt, ...);
void log_uart_enable_printf();
void log_uart_disable_printf();
//-------------------------------------------------------------------------
// Data declarations
uint32_t backupWarn;
uint32_t backupErr;
uint32_t backupInfo;
int disablePrintf;
// extern _UNKNOWN use_mode;
// extern _UNKNOWN ConfigDebugErr;
// extern _UNKNOWN ConfigDebugInfo;
// extern _UNKNOWN ConfigDebugWarn;

//----- 
void mode_init()
{
  use_mode = 1;
}

//----- 
void HalSerialPutcRtl8195a(int c)
{
  signed int v3; // r3@2

  if ( disablePrintf != 1 )
  {
    v3 = 6540;
    do
    {
      if ( !--v3 )
        break;
      a3 = v40003014;
    }
    while ( !(v40003014 & 0x60) );
    if ( c == 10 ) a3 = 13;
    v40003000 = c;
    if ( c == 10 ) v40003000 = a3;
  }
}

//-----
signed int DiagPrintf(const char *fmt, ...)
{
  va_list va;

  va_start(va, fmt);
  if ( disablePrintf != 1 )
    VSprintf(0, fmt, va);
  return 1;
}

//-----
void log_uart_enable_printf()
{
  disablePrintf = 0;
  ConfigDebugErr = backupErr;
  ConfigDebugInfo = backupInfo;
  ConfigDebugWarn = backupWarn;
}

//-----
void log_uart_disable_printf()
{
  disablePrintf = 1;
  backupErr = ConfigDebugErr;
  backupInfo = ConfigDebugInfo;
  backupWarn = ConfigDebugWarn;
  ConfigDebugErr = 0;
  ConfigDebugInfo = 0;
  ConfigDebugWarn = 0;
}



import ctypes

_jl = None

def loadJLinkARMdll(lib_str):
    global _jl
    _jl = ctypes.cdll.LoadLibrary(lib_str)
    if _jl:
        return _jl
    else:
        raise RuntimeError('JLink ARM dynamic library is not loaded')

def _jlink():
    if _jl:
        return _jl
    else:
        raise RuntimeError('JLink ARM dynamic library is not loaded')

def open():
    r = _jlink().JLINKARM_Open()
    if r != 0: raise RuntimeError('Adapter is not found')

def exec_command(cmd, a, b):
    r = _jlink().JLINKARM_ExecCommand(cmd, a, b)
    #if r != 0: raise RuntimeError('Unable to execute command')

def tif_select(n):
    if not isinstance(n, ctypes.c_ulong):
        n = ctypes.c_ulong(n)
    r = _jlink().JLINKARM_TIF_Select(n)
    if r != 0: raise RuntimeError('Unable to select TIF')

def set_speed(speed):
    if not isinstance(speed, ctypes.c_ulong):
        speed = ctypes.c_ulong(speed)
    r = _jlink().JLINKARM_SetSpeed(speed)
    #if r != 0: raise RuntimeError('Unable to set speed')

def reset():
    r = _jlink().JLINKARM_Reset()
    if r != 0: raise RuntimeError('fail')

def is_connected():
    if _jlink().JLINKARM_IsConnected() != 0: return True
    else: return False

def go():
    r = _jlink().JLINKARM_Go()
    #if r != 0: raise RuntimeError('fail')

def write_u32(r, val):
    if not isinstance(val, ctypes.c_ulong):
        val = ctypes.c_ulong(val)
    r = _jlink().JLINKARM_WriteU32(r, val)
    if r != 0: raise RuntimeError('fail')

def read_reg(r):
    return _jlink().JLINKARM_ReadReg(r)

def write_reg(r, val):
    if not isinstance(val, ctypes.c_ulong):
        val = ctypes.c_ulong(val)
    ret = _jlink().JLINKARM_WriteReg(r, val)
    #if ret != 0: raise RuntimeError('fail')

def get_speed():
    return _jlink().JLINKARM_GetSpeed()

def write_mem(startaddress, buf):
    _jlink().JLINKARM_WriteMem.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_char_p]
    _jlink().JLINKARM_WriteMem.restype = ctypes.c_int
    print "writemem addr %s size %s" % (hex(startaddress), hex(len(buf)))
    r = _jlink().JLINKARM_WriteMem(startaddress, len(buf), buf)
    #if r != 0: raise RuntimeError('fail')

def read_mem_U32(startaddress, count):
    buftype=ctypes.c_uint32 * int(count)
    buf=buftype()
    r=_jlink().JLINKARM_ReadMemU32(startaddress, count, buf, 0)
    #if r != 0: raise RuntimeError('fail')
    return buf

def close():
    r = _jlink().JLINKARM_Close()
    #if r != 0: raise RuntimeError('fail')

def clear_TCK():
    r = _jlink().JLINKARM_ClrTCK()
def set_TCK():
    r = _jlink().JLINKARM_SetTCK()

def clear_TMS():
    r = _jlink().JLINKARM_ClrTMS()
def set_TMS():
    r = _jlink().JLINKARM_SetTMS()

def clear_TDI():
    r = _jlink().JLINKARM_ClrTDI()
def set_TDI():
    r = _jlink().JLINKARM_SetTDI()

def clear_TRST():
    r = _jlink().JLINKARM_ClrTRST()
def set_TRST():
    r = _jlink().JLINKARM_SetTRST()

def clear_RESET():
    r = _jlink().JLINKARM_ClrRESET()
def set_RESET():
    r = _jlink().JLINKARM_SetRESET()

def halt():
    r = _jlink().JLINKARM_Halt()

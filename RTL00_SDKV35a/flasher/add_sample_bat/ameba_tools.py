import jlinkarm as jl

def loaddll(dll_path):
    jl.loadJLinkARMdll(dll_path)
    jl.open()
    if jl.is_connected == 0:
       raise RuntimeError('Jlink not connected')
    jl.exec_command('device Cortex-M3', 0, 0)
    jl.exec_command('endian little', 0, 0)
    jl.tif_select(1)
    jl.set_speed(1000)
    jl.clear_RESET()
    jl.clear_TRST()
    time.sleep(0.01)
    jl.set_RESET()
    jl.set_TRST()
    jl.reset()
    jl.halt()
    jl.set_speed(3500)

def ram_all(binfile, image1=True, image2=True):
    bin = None
    with file(binfile, 'rb') as f:
        bin = f.read()
    get_dword = lambda address: struct.unpack('<L', bin[address:address + 4])[0]

    if image1:
        i1_address = 0x0
        i1_length = get_dword(0x10)
        jl.write_mem(0x10000bc8, bin[i1_address + 0x20: i1_address + 0x20 + i1_length])

    if image2:
        i2_address = (get_dword(0x18) & 0xffff) * 1024
        i2_length = get_dword(i2_address) + 0x10
        jl.write_mem(0x10006000, bin[i2_address + 0x10: i2_address + 0x10 + i2_length])

    jl.reset()
    jl.write_u32(0x40000210, 0x20111113)
    jl.go()
    jl.close()

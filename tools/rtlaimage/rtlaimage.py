#!/usr/bin/env python
# (RTL8195AM/RTL8711AM/RTL8711AF/RTL8710AF) RtlImages Utility
# pvvx 

from __future__ import print_function, division
from operator import attrgetter

import argparse
import os
import struct
import sys

__version__ = "20.01.18"

PYTHON2 = sys.version_info[0] < 3  # True if on pre-Python 3

HM_IS_HDRR  = 1
HM_IS_HDRD  = 2
HM_IS_BOOT = 4
HM_IS_OTA = 8
HM_IS_SRAM = 16
HM_IS_SDRAM = 32

# Function to return nth byte of a bitstring
# Different behaviour on Python 2 vs 3
if PYTHON2:
	def byte(bitstr, index):
		return ord(bitstr[index])
else:
	def byte(bitstr, index):
		return bitstr[index]


class FatalError(RuntimeError):
	"""
	Wrapper class for runtime errors that aren't caused by internal bugs, but by
	xchip responses or input content.
	"""
	def __init__(self, message):
		RuntimeError.__init__(self, message)

	@staticmethod
	def WithResult(message, result):
		"""
		Return a fatal error object that appends the hex values of
		'result' as a string formatted argument.
		"""
		message += " (result was %s)" % hexify(result)
		return FatalError(message)

class ImageSegment(object):
	""" Wrapper class for a segment in an RTL image
	(very similar to a section in an ELFImage also) """
	def __init__(self, addr, data, file_offs = None):
		self.addr = addr
		# pad all ImageSegments to at least 4 bytes length
		pad_mod = len(data) % 4
		if pad_mod != 0:
			data += b"\x00" * (4 - pad_mod)
		self.data = data
		self.file_offs = file_offs

	def copy_with_new_addr(self, new_addr):
		""" Return a new ImageSegment with same data, but mapped at
		a new address. """
		return ImageSegment(new_addr, self.data, 0)

	def __repr__(self):
		r = "len 0x%05x load 0x%08x" % (len(self.data), self.addr)
		if self.file_offs is not None:
			r += " file_offs 0x%08x" % (self.file_offs)
		return r

class ELFSection(ImageSegment):
	""" Wrapper class for a section in an ELF image, has a section
	name as well as the common properties of an ImageSegment. """
	def __init__(self, name, addr, data, size):
		super(ELFSection, self).__init__(addr, data)
		self.name = name.decode("utf-8")
		self.size = size 

	def __repr__(self):
		return "%s %s" % (self.name, super(ELFSection, self).__repr__())

class ELFFile(object):
	SEC_TYPE_PROGBITS = 0x01
	SEC_TYPE_STRTAB = 0x03

	def __init__(self, name):
		# Load sections from the ELF file
		self.name = name
		try:
			with open(self.name, 'rb') as f:
				self._read_elf_file(f)
		except:
			print('Error: Not open input ELF file <%s>!' % self.name)
			sys.exit(-1)

	def get_section(self, section_name):
		for s in self.sections:
			if s.name == section_name:
				return s
		raise ValueError("No section %s in ELF file" % section_name)

	def _read_elf_file(self, f):
		# read the ELF file header
		LEN_FILE_HEADER = 0x34
		try:
			(ident,_type,machine,_version,
			 self.entrypoint,_phoff,shoff,_flags,
			 _ehsize, _phentsize,_phnum,_shentsize,
			 _shnum,shstrndx) = struct.unpack("<16sHHLLLLLHHHHHH", f.read(LEN_FILE_HEADER))
		except struct.error as e:
			raise FatalError("Failed to read a valid ELF header from %s: %s" % (self.name, e))

		if byte(ident, 0) != 0x7f or ident[1:4] != b'ELF':
			raise FatalError("%s has invalid ELF magic header" % self.name)
		if machine != 0x28:
			raise FatalError("%s does not appear to be an RTL ELF file. e_machine=%04x" % (self.name, machine))
		self._read_sections(f, shoff, shstrndx, _shnum)

	def _read_sections(self, f, section_header_offs, shstrndx, shnum):
		LEN_SEC_HEADER = 0x28
		f.seek(section_header_offs)
#		print("%d sections found in ELF file" % shnum)
		if shnum == 0 or shnum > 100:
			raise FatalError("%d sections found in ELF file!" % shnum) 
		section_header = f.read(shnum *LEN_SEC_HEADER)
		if len(section_header) == 0:
			raise FatalError("No section header found at offset %04x in ELF file." % section_header_offs)
		if len(section_header) != shnum *LEN_SEC_HEADER:
			print('WARNING: Unexpected ELF section header length %04x is not mod-%02x' % (len(section_header), LEN_SEC_HEADER))

		# walk through the section header and extract all sections
		section_header_offsets = range(0, len(section_header), LEN_SEC_HEADER)

		def read_section_header(offs):
			name_offs,sec_type,_flags,lma,sec_offs,size = struct.unpack_from("<LLLLLL", section_header[offs:])
#			print('read_section_header at %08x' % offs)
			return (name_offs, sec_type, lma, size, sec_offs)
		all_sections = [read_section_header(offs) for offs in section_header_offsets]
		prog_sections = [s for s in all_sections if s[1] == ELFFile.SEC_TYPE_PROGBITS]

		# search for the string table section
		if not shstrndx * LEN_SEC_HEADER in section_header_offsets:
			raise FatalError("ELF file has no STRTAB section at shstrndx %d" % shstrndx)
		_,sec_type,_,sec_size,sec_offs = read_section_header(shstrndx * LEN_SEC_HEADER)
		if sec_type != ELFFile.SEC_TYPE_STRTAB:
			print('WARNING: ELF file has incorrect STRTAB section type 0x%02x' % sec_type)
		f.seek(sec_offs)
		string_table = f.read(sec_size)

		# build the real list of ELFSections by reading the actual section names from the
		# string table section, and actual data for each section from the ELF file itself
		def lookup_string(offs):
			raw = string_table[offs:]
			return raw[:raw.index(b'\x00')]

		def read_data(offs, size):
			f.seek(offs)
			return f.read(size)

		prog_sections = [ELFSection(lookup_string(n_offs), lma, read_data(offs, size), size) for (n_offs, _type, lma, size, offs) in prog_sections if lma != 0]
		self.sections = prog_sections

class xFirmwareImage(object):
	def __init__(self, fname, segnames, hmode, addrl, addrh):
		self.fname = fname
		self.segnames = segnames 
		self.hm = hmode 
		self.addrl = addrl
		self.addrh = addrh
		self.addr = None 
		self.data = ''
		self.size = 0	# size code/data only (without header)
		self.imgsegs = []

	def load(self, sections):
		imgsegs = [s for s in sections if s.name in self.segnames]
		if imgsegs:
			if len(imgsegs) > 1:
				sorted(imgsegs, key = attrgetter('addr'))
			elif len(imgsegs[0].data) == 0:	# file size == 0
				self.addr = imgsegs[0].addr
#				print('Segment at 0x%08x,' % self.addr, 'size 0x00000000 copy to image', self.fname)
				sections.remove(imgsegs[0])
				return
			self.addr = imgsegs[0].addr
			s = imgsegs[len(imgsegs) - 1]
			self.size = s.addr + len(s.data) - self.addr
#			print('Segment at 0x%08x,' % self.addr, 'size 0x%08x' % self.size, 'copy to image', self.fname)
			if self.addr < self.addrl and self.addr > self.addrh:
				print('Segment Address Error!')
				return
			if self.size:
				if self.hm & HM_IS_HDRR:	
					self.data = struct.pack(b'<LLLL', self.size, self.addr, 0x35393138, 0x31313738)
				elif self.hm & HM_IS_HDRD:	
					self.data = struct.pack(b'<LLLL', self.size, self.addr, 0xffffffff, 0xffffffff)
				elif self.hm & HM_IS_BOOT: # boot 	
					self.data = struct.pack(b'<LLLLLLLL', 0x96969999, 0xFC66CC3F, 0x03CC33C0, 0x6231DCE5, self.size, self.addr, 0xffff002c, 0xffffffff)
				x = self.addr	
				for s in imgsegs:
					while x < s.addr:
						self.data += '\0'
						x += 1
					self.data += s.data
#					print('Segment at 0x%08x,' % s.addr, 'size 0x%08x' % len(s.data), 'name', s.name, 'copy to image', self.fname)
					x += s.size
					sections.remove(s)
		
	def save(self, outdir, flg_p):
		if self.size:
#			print('Segment at 0x%08x,' % self.addr, 'size 0x%08x' % self.size, 'save to', self.fname)
			try:
				if flg_p:
					with open(outdir + self.fname + '.p.bin', "wb") as f:
						f.write(self.data)
						f.close()
				else:
					with open(outdir + self.fname + '.bin', "wb") as f:
						if self.hm & (HM_IS_HDRR | HM_IS_HDRD):
							f.write(self.data[16:])
						elif self.hm & HM_IS_BOOT:
							f.write(self.data[32:])
						else:
							f.write(self.data)
						f.close()
			except:
				print('Error: Not write file <%s>!' % self.fname)
				sys.exit(-1)
				
	def save_ota(self, f, fn, chks):
		if self.size:
#			print('Segment at 0x%08x,' % self.addr, 'size 0x%08x' % self.size, 'save to %s' % fn)
			try:
				f.write(self.data)
				i = 0
				while i < len(self.data):
					chks += byte(self.data, i)
					i += 1
			except:
				print('Error: Not write file %s!' % fn)
				sys.exit(-1)
		return chks
	
	def save_sram(self, f, fn):
		if self.size:
#			print('Segment at 0x%08x,' % self.addr, 'size 0x%08x' % self.size, 'save to %s' % fn)
			try:
				if self.hm & HM_IS_BOOT:
					gap = 0x0b000
					i = len(self.data)  
					if i > gap:
						print('Error: Segment %s is big!' % self.name)
					f.write(self.data)
					while i < gap:
						f.write('\xff')
						i += 1
				else:
					f.write(self.data)
			except:
				print('Error: Not write file %s!' % fn)
				sys.exit(-1)

def elf2image(args):

	e = ELFFile(args.elffile) # ELFSection is a subclass of ImageSegment
#	print(e.sections)
	image = [xFirmwareImage('ram_1', ['.ram.start.table', '.ram_image1.text', '.ram_image1.data'], HM_IS_BOOT + HM_IS_SRAM, 0x10000bc8, 0x10070000),
			 xFirmwareImage('ram_2', ['.image2.start.table', '.ram_image2.text', '.ram_image2.rodata', '.ram.data'], HM_IS_OTA + HM_IS_HDRR + HM_IS_SRAM, 0x10006000, 0x10070000),
			 xFirmwareImage('sdram', ['.sdr_text', '.sdr_rodata', '.sdr_data'], HM_IS_OTA + HM_IS_HDRD + HM_IS_SDRAM, 0x30000000, 0x30200000)]
	
	img_sram_use = 0
	img_sdram_use = 0
	
	for s in image:
		s.load(e.sections)
		if s.hm & HM_IS_SRAM:
			img_sram_use += s.size
		elif s.hm & HM_IS_SDRAM:
			img_sdram_use += s.size

	for s in image:
		if s.size:
			print('Segment at 0x%08x,' % s.addr, 'size 0x%08x' % s.size, '(%s)' % s.fname)
			s.save(args.outdir, None);
			s.save(args.outdir, 1);

	if args.ota:
		fn = args.outdir + 'ota.bin'
		try:
			chks = 0
			with open(fn, "wb") as f:
				for s in image:
					if s.hm & HM_IS_OTA:
						chks = s.save_ota(f, fn, chks)
				f.write(struct.pack('<L', chks))
				f.close()
				
		except:
			print('Error: Not write file <%s>!' % fn)
			sys.exit(-1)

	if args.ram_all:
		if image[0].size == 0 and image[1].size == 0:
			print('Error: Ram_all = 0!')
			sys.exit(-1)
		fn = args.outdir + 'ram_all.bin'
		try:
			with open(fn, "wb") as f:
				for s in image:
					s.save_sram(f, fn)
				f.close()
				
		except:
			print('Error: Not write file <%s>!' % fn)
			sys.exit(-1)

	print('Images size: SRAM %u bytes, SDRAM %u bytes [%u]' % (img_sram_use, img_sdram_use, img_sram_use + img_sdram_use))
	

def main():
	parser = argparse.ArgumentParser(description='RtlAImages Utility version %s' % __version__, prog='rtlimage')
	parser.add_argument('--ram_all', '-r', action='store_true', help='Generate ram_all files')
	parser.add_argument('--ota', '-a', action='store_true', help='Generate OTA files')
	parser.add_argument('elffile', type=str, help='Input ELF file',)
	parser.add_argument('--outdir', '-o', type=str, default='', help='Outpyt directory')

	args = parser.parse_args()

	print('RtlAImages Utility version %s' % __version__)
	if args.elffile is None:
		parser.print_help()
		sys.exit(1)
	elf2image(args)
	sys.exit(0)

if __name__ == '__main__':
	try:
		main()
	except FatalError as e:
		print('\nA fatal error occurred: %s' % e)
		sys.exit(2)

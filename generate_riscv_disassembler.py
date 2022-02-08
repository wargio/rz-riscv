#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022 deroad <wargio@libero.it>
# SPDX-License-Identifier: LGPL-3.0-only

import json
import sys
import re
import sys
from datetime import date

## https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf

registers_std = [ 'rd', 'rs1', 'rs2', 'rs3' ]

registers_vec = [ 'vd', 'vs1', 'vs2', 'vs3' ]

registers_flt = [ 'fd', 'fs1', 'fs2', 'fs3' ]

instrtbl = []
decoders = []
stucttbl = []
stuctfmt = '\t{{ {name}, /* {pad}{mask} */ 0x{cbits:08x}, 0x{cmask:08x}, {decode} }},'

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

class Bitfield():
	def __init__(self, obj, bits):
		super(Bitfield, self).__init__()
		self._between = obj['between']
		self._choices = obj['choices']
		self._name = obj['name']
		self._type = obj['type']
		self._bits = bits

	def format_key(self):
		if self._type == 'suffix':
			return 'suffix'
		elif self._type == 'immediate':
			if 'hi' in self._name:
				return self._name.replace("hi", "")
		return self._name

	def format_value(self):
		if self._type == 'immediate':
			if self._name == 'jimm20':
				return '0x%" PFMT64x "'
			return "0x%x"
		elif self._type in ['register', 'suffix', 'choice', 'nfields']:
			return "%s"
		else:
			sys.exit("unknown bitfield type: " + self._type)

	def format_var(self):
		if self._type == 'immediate':
			if 'hi' in self._name:
				return self._name.replace("hi", "")
			elif 'lo' in self._name:
				return None
			elif self._name == 'jimm20':
				return "pc + jimm20"
		elif self._type in ['suffix', 'nfields']:
			return None
		elif self._type == 'register':
			if self._name in registers_flt:
				return "registers_flt[{}]".format(self._name)
			elif self._name in registers_vec:
				return "registers_vec[{}]".format(self._name)
			return "registers_std[{}]".format(self._name)
		return self._name

	def extract(self):
		extract = 1
		for x in range(self._between[1], self._between[0]):
			extract <<= 1
			extract |= 1
		extract <<= self._between[1]
		shift = self._between[1]
		return "\tut{bits} {name} = (data & 0x{ext:x}) >> {sh};\n".format(bits=self._bits, name=self._name, ext=extract, sh=shift)

class Description():
	def __init__(self, obj, bits):
		super(Description, self).__init__()
		self._bitfields = list(map(lambda x: Bitfield(x, bits), obj['bitfields']))
		self._format = obj['format']
		self._type = obj['type']

	def bitfields(self):
		fields = ""
		for bf in self._bitfields:
			fields += bf.extract()
		return fields

	def format(self):
		kargs = {'name':''}
		variables = []
		for bf in self._bitfields:
			key = bf.format_key()
			if ("{" + key + "}") not in self._format:
				continue
			kargs[key] = bf.format_value()
			variables.append(bf.format_var())
		variables = list(filter(None, variables))
		fmt = self._format.format(**kargs)
		return "\trz_str_buf_appendf(\"{fmt}\", {vars});\n".format(fmt=fmt, vars=', '.join(variables))
	
	def is_unique(self):
		return " " not in self._format

class Instr(object):
	def __init__(self, obj):
		super(Instr, self).__init__()
		self._bitmask = obj['bitmask']
		self._cbits = obj['cbits']
		self._cmask = obj['cmask']
		self._fields = obj['fields']
		self._is_pseudo = obj['is_pseudo']
		self._mnemonic = obj['mnemonic']
		self._standard = obj['standard']
		self._size = obj['size']
		self._description = Description(obj['description'], self._size)

	def decoder(self):
		if self._description.is_unique():
			return ""
		func = ""
		func += "static ut32 riscv_{0}_{1}(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {{\n".format(self._standard, self._bitmask)
		func += self._description.bitfields()
		func += "\trz_strbuf_set(sb, name);\n"
		func += self._description.format()
		func += "\treturn {};\n".format(int(self._size / 8))
		func += "}\n"
		return func


	def struct(self):
		name = '"' + self._mnemonic + '"'
		decode = 'riscv_unique' if self._description.is_unique() else 'riscv_{0}_{1}'.format(self._standard, self._bitmask)
		pad = " " * (18 - len(name))
		return stuctfmt.format(name=name, pad=pad, mask=self._bitmask, cbits=self._cbits, cmask=self._cmask, decode=decode)

with open('./riscv_opcodes.json', 'r') as f:
	instrtbl += json.load(f)

instrtbl = list(map(lambda x: Instr(x), instrtbl))
instrtbl.sort(key=lambda x: x._cmask)
decoders = list(set(map(lambda x: x.decoder(), instrtbl)))
decoders.sort()
stucttbl = list(map(lambda x: x.struct(), instrtbl))


print("// SPDX-FileCopyrightText: {year} deroad <wargio@libero.it>".format(year=date.today().year))
print("// SPDX-License-Identifier: LGPL-3.0-only")
print("""
#include <rz_types.h>
#include <rz_util.h>

#define RISCV_INVALID_SIZE 0

typedef ut32 (*Decode)(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb);

const char *registers_std[32] = {
	\"zero\", \"ra\", \"sp\", \"gp\", \"tp\", \"t0\", \"t1\", \"t2\", \"s0\",
	\"s1\", \"a0\", \"a1\", \"a2\", \"a3\", \"a4\", \"a5\", \"a6\", \"a7\",
	\"s2\", \"s3\", \"s4\", \"s5\", \"s6\", \"s7\", \"s8\", \"s9\", \"s10\",
	\"s11\", \"t3\", \"t4\", \"t5\", \"t6\"
	\"v0\", \"v1\", \"v2\", \"v3\", \"v4\", \"v5\", \"v6\", \"v7\", \"v8\",
};

const char *registers_vec[32] = {
	\"v9\", \"v10\", \"v11\", \"v12\", \"v13\", \"v14\", \"v15\", \"v16\",
	\"v17\", \"v18\", \"v19\", \"v20\", \"v21\", \"v22\", \"v23\", \"v24\",
	\"v25\", \"v26\", \"v27\", \"v28\", \"v29\", \"v30\", \"v31\"
};

const char *registers_flt[32] = {
	\"f0\", \"f1\", \"f2\", \"f3\", \"f4\", \"f5\", \"f6\", \"f7\", \"f8\",
	\"f9\", \"f10\", \"f11\", \"f12\", \"f13\", \"f14\", \"f15\", \"f16\",
	\"f17\", \"f18\", \"f19\", \"f20\", \"f21\", \"f22\", \"f23\", \"f24\",
	\"f25\", \"f26\", \"f27\", \"f28\", \"f29\", \"f30\", \"f31\"
};

typedef struct riscv_decoder_t {
	cchar* name; /*  instruction name */
	ut16 cbits; /*   constant bits */
	ut16 mbits; /*   mask to compare with constant bits */
	ut32 opsize; /*  instruction size */
	Decode decode;
} RISCVInstruction;

static ut32 riscv_unique(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	rz_strbuf_set(sb, name);
	return 4;
}""")

print("\n".join(decoders))

print("""static const RISCVInstruction instructions[] = {""")

print("\n".join(stucttbl))

print("""}

ut32 riscv_disassembler(const ut8 *buffer, const ut32 size, ut64 pc, bool be, RzStrBuf *sb) {
	rz_return_val_if_fail(buffer && size && sb, false);
	if (size < 2) {
		return RISCV_INVALID_SIZE;
	}

	ut32 masked;
	ut32 data = rz_read_ble16(buffer, be);

	for (ut32 i = 0; i < RZ_ARRAY_SIZE(instructions); ++i) {
		masked = data & instructions[i].mbits;
		if (masked == instructions[i].cbits) {
			return instructions[i].decode(instructions[i].name, data, pc, sb);
		}
	}
	return RISCV_INVALID_SIZE;
}
""")


#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022 deroad <wargio@libero.it>
# SPDX-License-Identifier: LGPL-3.0-only

import math
import sys
import json
import os
from collections import OrderedDict

namelist = []
match = OrderedDict()
mask = OrderedDict()
pseudos = {}
arguments = {}
standard = {}

# operands begin/end in bits (needs to be reversed)
operand_pos = {
  'rd': (11,7),
  'rt': (19,15), # source+dest register address. Overlaps rs1.
  'rs1': (19,15),
  'rs2': (24,20),
  'rs3': (31,27),
  'fd': (11,7),
  'ft': (19,15), # source+dest register address. Overlaps fs1.
  'fs1': (19,15),
  'fs2': (24,20),
  'fs3': (31,27),
  'aqrl': (26,25),
  'fm': (31,28),
  'pred': (27,24),
  'succ': (23,20),
  'rm': (14,12),
  'funct3': (14,12),
  'imm20': (31,12),
  'jimm20': (31,12),
  'imm12': (31,20),
  'imm12hi': (31,25),
  'bimm12hi': (31,25),
  'imm12lo': (11,7),
  'bimm12lo': (11,7),
  'zimm': (19,15),
  'shamt': (25,20),
  'shamtw': (24,20),
  'bs': (31,30), # byte select for RV32K AES
  'rnum': (23,20), # round constant for RV64 AES
  'rc': (29,25),
  'imm2': (21,20),
  'imm3': (22,20),
  'imm4': (23,20),
  'imm5': (24,20),
  'imm6': (25,20),

  # for vectors
  'vd': (11,7),
  'vs3': (11,7),
  'vs1': (19,15),
  'vs2': (24,20),
  'vm': (25,25),
  'wd': (26,26),
  'amoop': (31,27),
  'nf': (31,29),
  'simm5': (19,15),
  'zimm10': (29,20),
  'zimm11': (30,20),

  # for compressed (16 bits)
  'crd': (11,7),
  'crd2': (6,2),
  'cimmhi': (12,12),
  'cimmlo': (6,2),
  'cssimm': (12,7),
  'clrs1': (9,7),
  'clrs2': (4,2),
  'climmhi': (12,10),
  'climmlo': (6,5),
  'cbimmhi': (12,10),
  'cbimmlo': (6,2),
  'cjimm': (12,2),
}

# operands mask
operand_mask = {
  'rd': "a" * (1 + 11 - 7),
  'rt': "b" * (1 + 19 - 15), # source+dest register address. Overlaps rs1.
  'rs1': "c" * (1 + 19 - 15),
  'rs2': "d" * (1 + 24 - 20),
  'rs3': "e" * (1 + 31 - 27),
  'fd': "a" * (1 + 11 - 7), # same pattern as fd 
  'ft': "b" * (1 + 19 - 15), # same pattern as ft
  'fs1': "c" * (1 + 19 - 15), # same pattern as fs1 
  'fs2': "d" * (1 + 24 - 20), # same pattern as fs2 
  'fs3': "e" * (1 + 31 - 27), # same pattern as fs3 
  'aqrl': "f" * (1 + 26 - 25),
  'fm': "g" * (1 + 31 - 28),
  'pred': "h" * (1 + 27 - 24),
  'succ': "i" * (1 + 23 - 20),
  'rm': "j" * (1 + 14 - 12),
  'funct3': "k" * (1 + 14 - 12),
  'imm20': "l" * (1 + 31 - 12),
  'jimm20': "m" * (1 + 31 - 12),
  'imm12': "n" * (1 + 31 - 20),
  'imm12hi': "o" * (1 + 31 - 25),
  'bimm12hi': "p" * (1 + 31 - 25),
  'imm12lo': "q" * (1 + 11 - 7),
  'bimm12lo': "r" * (1 + 11 - 7),
  'zimm': "s" * (1 + 19 - 15),
  'shamt': "t" * (1 + 25 - 20),
  'shamtw': "u" * (1 + 24 - 20),
  'bs': "v" * (1 + 31 - 30), # byte select for RV32K AES
  'rnum': "w" * (1 + 23 - 20), # round constant for RV64 AES
  'rc': "x" * (1 + 29 - 25),
  'imm2': "y" * (1 + 21 - 20),
  'imm3': "z" * (1 + 22 - 20),
  'imm4': "A" * (1 + 23 - 20),
  'imm5': "B" * (1 + 24 - 20),
  'imm6': "C" * (1 + 25 - 20),

  # for vectors
  'vd': "D" * (1 + 11 - 7),
  'vs3': "E" * (1 + 11 - 7),
  'vs1': "F" * (1 + 19 - 15),
  'vs2': "G" * (1 + 24 - 20),
  'vm': "H" * (1 + 25 - 25),
  'wd': "I" * (1 + 26 - 26),
  'amoop': "J" * (1 + 31 - 27),
  'nf': "K" * (1 + 31 - 29),
  'simm5': "L" * (1 + 19 - 15),
  'zimm10': "M" * (1 + 29 - 20),
  'zimm11': "N" * (1 + 30 - 20),

  # for compressed (16 bits)
  'crd': "O" * (1 + 11 - 7),
  'crd2': "P" * (1 + 6 - 2),
  'cimmhi': "Q" * (1 + 12 - 2),
  'cimmlo': "R" * (1 + 6 - 2),
  'cssimm': "S" * (1 + 12 - 7),
  'clrs1': "T" * (1 + 9 - 7),
  'clrs2': "U" * (1 + 4 - 2),
  'climmhi': "V" * (1 + 12 - 0),
  'climmlo': "W" * (1 + 6 - 5),
  'cbimmhi': "X" * (1 + 12 - 0),
  'cbimmlo': "Y" * (1 + 6 - 2),
  'cjimm': "Z" * (1 + 12 - 2),
}

# these are sorted on purpose
r_type_operands = ['wd', 'vd', 'vs1', 'vs2', 'vs3', 'rd', 'rs1', 'rs2', 'rs3', 'fd', 'fs1', 'fs2', 'fs3', 'rm']

registers_flt = {
  'rd': 'fd',
  'rs1': 'fs1',
  'rs2': 'fs2',
  'rs3': 'fs3',
}

def mask_arg(s, n):
  beg, end = operand_pos[n]
  beg = 31 - beg
  end = 32 - end
  if '?' * len(operand_mask[n]) != s[beg:end]:
    sys.exit(f"{name} has wrong bits {s[0:beg] + '|' + s[beg:end] + '|' + s[end:]}")
  tmp = s[0:beg] + operand_mask[n]
  return tmp + s[len(tmp):]

def yank(num,start,len):
  return (num >> start) & ((1 << len) - 1)

def create_bitmask(name):
  s = ""
  for i in range(31, -1, -1):
    if yank(mask[name], i, 1):
      s = '%s%d' % (s, yank(match[name], i, 1))
    else:
      s = s + '?'
  for arg in arguments[name]:
    s = mask_arg(s, arg)
  if '?' in s and name not in pseudos:
    sys.exit(f"{name} has mask {s}")
  elif name in pseudos:
    s = s.replace('?', '0')
  return s

def operand_type(name):
  if name in [ 'nf' ]:
    return 'nfields'

  if name in ['fm', 'aqrl']:
    return 'suffix'
  
  if name in [
      'bimm12hi', 'bimm12lo', 'funct3', 'imm12', 'imm12hi', 'imm12lo',
      'imm2', 'imm20', 'imm3', 'imm4', 'imm5', 'imm6', 'jimm20', 'bs',
      'simm5', 'shamt', 'shamtw', 'zimm', 'zimm10', 'zimm11', 'vm',
      'rnum' ]:
    return 'immediate'

  if name in ['pred', 'succ', 'rm']:
    return 'choice'
  
  if name in r_type_operands:
    return 'register'

def operand_choice(name):
  if name == 'nf': # nfields
    return [
      ['1', 0b000], ['2', 0b001], ['3', 0b010], ['4', 0b011], 
      ['5', 0b100], ['6', 0b101], ['7', 0b110], ['8', 0b111], 
    ]

  if name == 'fm': # fence
    return [
      ['', 0b000], ['.tso', 0b1000]
    ]

  if name == 'aqrl':
    return [
      ['', 0b00 ],
      ['.lr', 0b01 ],
      ['.aq', 0b10 ],
      ['.aqlr', 0b11 ],
    ]

  if name in ['pred', 'succ']:
    return [
      ['', 0b0000 ],
      ['w', 0b0001 ],
      ['r', 0b0010 ],
      ['rw', 0b0011 ],
      ['o', 0b0100 ],
      ['ow', 0b0101 ],
      ['or', 0b0110 ],
      ['orw', 0b0111 ],
      ['i', 0b1000 ],
      ['iw', 0b1001 ],
      ['ir', 0b1010 ],
      ['irw', 0b1011 ],
      ['io', 0b1100 ],
      ['iow', 0b1101 ],
      ['ior', 0b1110 ],
      ['iorw', 0b1111 ],
    ]

  if name == 'rm': # Floating poing rounding
    return [
      ['rne', 0b000], # Round to Nearest, ties to Even
      ['rtz', 0b001], # Round towards Zero
      ['rdn', 0b010], # Round Down (towards −inf)
      ['rup', 0b011], # Round Up (towards +inf)
      ['rmm', 0b100], # Round to Nearest, ties to Max Magnitude
      ['', 0b111], # dyn: Dynamic rounding mode (default if not specified)
    ]

  return []

def create_operand(name):
  return {
    'name': name,
    'between': list(operand_pos[name]),
    'type': operand_type(name),
    'choices': operand_choice(name),
  }

def create_exec_env_type(name, cbits, operands):
  return {
    'format': "{name}",
    'type': 'exec',
    'bitfields': list(map(create_operand, operands)),
  }

def create_fence_type(name, cbits, operands):
  fmt = " {pred}, {succ}"
  if name in pseudos:
    fmt = ""
  return {
    'format': "{name}" + fmt,
    'type': 'fence',
    'bitfields': list(map(create_operand, operands)),
  }

def create_fence_i_type(name, cbits, operands):
  return {
    'format': "{name}",
    'type': 'fence_i',
    'bitfields': list(map(create_operand, operands)),
  }

def create_amo_type(name, cbits, operands):
  fmt = "{suffix} {rd}, {rs1}, ({rs2})" if 'rs2' in operands else "{suffix} {rd}, ({rs1})"
  return {
    'format': "{name}" + fmt,
    'type': 'amo',
    'bitfields': list(map(create_operand, operands)),
  }

def create_ish_type(name, cbits, operands):
  if 'rd' in operands:
    fmt = " {rd}, {rs1}, {shamt}" if 'rs1' in operands else " {rd}, {shamt}"
  elif 'fd' in operands:
    fmt = " {fd}, {fs1}, {shamt}" if 'fs1' in operands else " {fd}, {shamt}"
  return {
    'format': "{name}" + fmt,
    'type': 'ish',
    'bitfields': list(map(create_operand, operands)),
  }

def create_ishw_type(name, cbits, operands):
  fmt = "{name} {fd}, {shamtw}" if "fd" in operands else "{name} {rd}, {shamtw}"
  return {
    'format': fmt,
    'type': 'ishw',
    'bitfields': list(map(create_operand, operands)),
  }

def create_u_type(name, cbits, operands):
  return {
    'format': "{name} {rd}, {imm20}",
    'type': 'u',
    'bitfields': list(map(create_operand, operands)),
  }

def create_uj_type(name, cbits, operands):
  return {
    'format': "{name} {rd}, {jimm20}",
    'type': 'uj',
    'bitfields': list(map(create_operand, operands)),
  }

def create_csr_type(name, cbits, operands):
  return {
    'format': "{name} {rd}, {rs1}, {imm12}",
    'type': 'csr',
    'bitfields': list(map(create_operand, operands)),
  }

def create_i_type(name, cbits, operands):
  fmt = "{name} {fd}, {fs1}, {imm12}" if "fd" in operands else "{name} {rd}, {rs1}, {imm12}"
  return {
    'format': fmt,
    'type': 'i',
    'bitfields': list(map(create_operand, operands)),
  }

def create_s3_type(name, cbits, operands):
  fmt = "{name} {fs1}, {imm12}({fs2})" if "fs1" in operands else "{name} {rs1}, {imm12}({rs2})"
  return {
    'format': fmt,
    'type': 's3',
    'bitfields': list(map(create_operand, operands)),
  }

def create_s2_type(name, cbits, operands):
  return {
    'format': "{name} {rs1}, {imm12}",
    'type': 's2',
    'bitfields': list(map(create_operand, operands)),
  }

def create_sb_type(name, cbits, operands):
  return {
    'format': "{name} {rs1}, {rs2}, {bimm12}",
    'type': 'sb',
    'bitfields': list(map(create_operand, operands)),
  }

def create_r4_type(name, cbits, operands):
  return {
    'format': "{name} {rd}, {rs1}, {rs2}, {rs3}",
    'type': 'r4',
    'bitfields': list(map(create_operand, operands)),
  }

def create_r_type(name, cbits, operands):
  def only_regs(reg):
    return reg in r_type_operands
  operands = list(filter(only_regs, operands))
  fmt = ""
  if len(operands) > 0:
    operands.sort(key=lambda x: r_type_operands.index(x))
    fmt = " {" + "}, {".join(operands) + "}"
  return {
    'format': "{name}" + fmt,
    'type': 'r{}'.format(len(operands)),
    'bitfields': list(map(create_operand, operands))
  }

def create_description(n):
  if False and n in ['ecall', 'ebreak']:
    return create_exec_env_type(n, match[n], arguments[n])
  elif n == 'fence.i':
    return create_fence_i_type(n, match[n], arguments[n])
  elif n in ['fence', 'fence.tso', 'pause']:
    return create_fence_type(n, match[n], arguments[n])
  elif 'aqrl' in arguments[n]:
    return create_amo_type(n, match[n], arguments[n])
  elif 'shamt' in arguments[n]:
    return create_ish_type(n, match[n], arguments[n])
  elif 'shamtw' in arguments[n]:
    return create_ishw_type(n, match[n], arguments[n])
  elif 'imm20' in arguments[n]:
    return create_u_type(n, match[n], arguments[n])
  elif 'jimm20' in arguments[n]:
    return create_uj_type(n, match[n], arguments[n])
  elif n[:3] == 'csr':
    return create_csr_type(n, match[n], arguments[n])
  elif 'imm12' in arguments[n]:
    return create_i_type(n, match[n], arguments[n])
  elif 'imm12hi' in arguments[n] and 'imm12lo' in arguments[n]:
    return create_s3_type(n, match[n], arguments[n])
  elif 'imm12hi' in arguments[n]:
    return create_s2_type(n, match[n], arguments[n])
  elif 'bimm12hi' in arguments[n]:
    return create_sb_type(n, match[n], arguments[n])
  elif 'rs3' in arguments[n]:
    return create_r4_type(n, match[n], arguments[n])
  return create_r_type(n, match[n], arguments[n])

def make_json():
  obj = []
  for name in namelist:
    if name.startswith('c.'):
     # skipping all compressed format
     continue
    elif name.startswith('custom'):
     # skipping all custom pseudo code
     continue

    bitmask = create_bitmask(name)
    description = create_description(name)

    obj.append({
      'mnemonic': name,
      'is_pseudo': name in pseudos,
      'bitmask': bitmask,
      'description': description,
      'size': len(bitmask),
      'cmask': mask[name],
      'cbits': match[name],
      'fields': arguments[name],
      'standard': standard[name],
    })
  print(json.dumps(obj, sort_keys=True, indent=4))

def parse_inputs(args):
  for fn in args:
    f = open(fn)
    for line in f:
      line = line.partition('#')
      tokens = line[0].split()

      if len(tokens) == 0:
        continue
      assert len(tokens) >= 2

      name = tokens[0]
      pseudo = name[0] == '@'
      if pseudo:
        name = name[1:]
      mymatch = 0
      mymask = 0
      cover = 0

      if not name in list(arguments.keys()):
        arguments[name] = []

      for token in tokens[1:]:
        if len(token.split('=')) == 2:
          tokens = token.split('=')
          if len(tokens[0].split('..')) == 2:
            tmp = tokens[0].split('..')
            hi = int(tmp[0])
            lo = int(tmp[1])
            if hi <= lo:
              sys.exit("%s: bad range %d..%d" % (name,hi,lo))
          else:
            hi = lo = int(tokens[0])

          if tokens[1] != 'ignore':
            val = int(tokens[1], 0)
            if val >= (1 << (hi-lo+1)):
              sys.exit("%s: bad value %d for range %d..%d" % (name,val,hi,lo))
            mymatch = mymatch | (val << lo)
            mymask = mymask | ((1<<(hi+1))-(1<<lo))

          if cover & ((1<<(hi+1))-(1<<lo)):
            sys.exit("%s: overspecified" % name)
          cover = cover | ((1<<(hi+1))-(1<<lo))

        elif token in operand_pos:
          if cover & ((1<<(operand_pos[token][0]+1))-(1<<operand_pos[token][1])):
            sys.exit("%s: overspecified" % name)
          cover = cover | ((1<<(operand_pos[token][0]+1))-(1<<operand_pos[token][1]))
          arguments[name].append(token)

        else:
          sys.exit("%s: unknown token %s" % (name,token))

      if not (cover == 0xFFFFFFFF or cover == 0xFFFF):
        sys.exit("%s: not all bits are covered" % name)

      if pseudo:
        pseudos[name] = 1
      else:
        for name2,match2 in match.items():
          if name2 not in pseudos and (match2 & mymask) == mymatch:
              sys.exit("%s and %s overlap" % (name,name2))

      mask[name] = mymask
      match[name] = mymatch
      standard[name] = os.path.basename(fn).replace('opcodes-', '').replace('-pseudo', '')


      # convert reg arguments to floating point reg arguments
      if name[0] == 'f':
        if name in ['fcvt.w.s', 'fcvt.wu.s', 'fcvt.l.s', 'fcvt.lu.s', 'fcvt.w.q', 'fcvt.wu.q', 'fcvt.l.q', 'fcvt.lu.q']:
          arguments[name] = [ registers_flt[a] if a == 'rs1' else a for a in arguments[name]]
        elif name in ['fcvt.s.w', 'fcvt.s.wu', 'fcvt.s.l', 'fcvt.s.lu', 'fcvt.q.w', 'fcvt.q.wu', 'fcvt.q.l', 'fcvt.q.lu']:
          arguments[name] = [ registers_flt[a] if a == 'rd' else a for a in arguments[name]]
        else:
          arguments[name] = [ registers_flt[a] if a in registers_flt else a for a in arguments[name]]

      namelist.append(name)

    f.close()
  return (namelist, pseudos, mask, match, arguments)

if __name__ == "__main__":
  if len(sys.argv) < 2:
    sys.exit("{} riscv-opcodes/opcodes-*".format(sys.argv[0]))
  parse_inputs(sys.argv[1:])
  make_json()

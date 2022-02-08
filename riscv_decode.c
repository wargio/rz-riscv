// SPDX-FileCopyrightText: 2022 deroad <wargio@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only

#include <rz_types.h>
#include <rz_util.h>

#define RISCV_INVALID_SIZE 0

typedef ut32 (*Decode)(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb);

const char *registers_std[32] = {
	"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0",
	"s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
	"s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10",
	"s11", "t3", "t4", "t5", "t6"
	"v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8",
};

const char *registers_vec[32] = {
	"v9", "v10", "v11", "v12", "v13", "v14", "v15", "v16",
	"v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24",
	"v25", "v26", "v27", "v28", "v29", "v30", "v31"
};

const char *registers_flt[32] = {
	"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8",
	"f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16",
	"f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24",
	"f25", "f26", "f27", "f28", "f29", "f30", "f31"
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
}

static ut32 riscv_pseudo_00000000000100000010aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_flt[fd]);
	return 4;
}

static ut32 riscv_pseudo_000000000001ccccc001aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_pseudo_000000000001sssss101aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_flt[fd]);
	return 4;
}

static ut32 riscv_pseudo_00000000001000000010aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_flt[fd]);
	return 4;
}

static ut32 riscv_pseudo_000000000010ccccc001aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_pseudo_000000000010sssss101aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_flt[fd]);
	return 4;
}

static ut32 riscv_pseudo_00000000001100000010aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_flt[fd]);
	return 4;
}

static ut32 riscv_pseudo_000000000011ccccc001aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_pseudo_0000000uuuuuccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_pseudo_0000000uuuuuccccc101aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_pseudo_0100000uuuuuccccc101aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_pseudo_11000000000000000010aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_std[rd]);
	return 4;
}

static ut32 riscv_pseudo_11000000000100000010aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_std[rd]);
	return 4;
}

static ut32 riscv_pseudo_11000000001000000010aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_std[rd]);
	return 4;
}

static ut32 riscv_pseudo_11001000000000000010aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_std[rd]);
	return 4;
}

static ut32 riscv_pseudo_11001000000100000010aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_std[rd]);
	return 4;
}

static ut32 riscv_pseudo_11001000001000000010aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_std[rd]);
	return 4;
}

static ut32 riscv_pseudo_111000000000ccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_pseudo_111100000000ccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_rv32a_00000ffdddddccccc010aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32a_00001ffdddddccccc010aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32a_00010ff00000ccccc010aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, (%s)", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32a_00011ffdddddccccc010aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32a_00100ffdddddccccc010aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32a_01000ffdddddccccc010aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32a_01100ffdddddccccc010aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32a_10000ffdddddccccc010aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32a_10100ffdddddccccc010aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32a_11000ffdddddccccc010aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32a_11100ffdddddccccc010aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000100dddddccccc001aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000100dddddccccc100aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000100dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000100dddddccccc110aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000100dddddccccc111aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000100uuuuuccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_rv32b_0000100uuuuuccccc101aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_rv32b_0000101dddddccccc001aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000101dddddccccc010aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000101dddddccccc011aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000101dddddccccc100aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000101dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000101dddddccccc110aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0000101dddddccccc111aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0010000dddddccccc001aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0010000dddddccccc010aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0010000dddddccccc100aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0010000dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0010000dddddccccc110aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_001000ttttttccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv32b_001000ttttttccccc101aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv32b_0010100dddddccccc001aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0010100dddddccccc010aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0010100dddddccccc100aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0010100dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0010100dddddccccc110aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_001010ttttttccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv32b_001010ttttttccccc101aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv32b_0100000dddddccccc100aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0100000dddddccccc110aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0100000dddddccccc111aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0100100dddddccccc001aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0100100dddddccccc100aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0100100dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0100100dddddccccc110aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0100100dddddccccc111aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_010010ttttttccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv32b_010010ttttttccccc101aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv32b_011000000000ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32b_011000000001ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32b_011000000010ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32b_011000000100ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32b_011000000101ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32b_011000010000ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32b_011000010001ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32b_011000010010ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32b_011000011000ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32b_011000011001ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32b_011000011010ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32b_0110000dddddccccc001aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0110000dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_011000ttttttccccc101aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv32b_0110100dddddccccc001aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_0110100dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32b_011010ttttttccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv32b_011010ttttttccccc101aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv32b_eeeee10dddddccccc001aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3]);
	return 4;
}

static ut32 riscv_rv32b_eeeee10dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3]);
	return 4;
}

static ut32 riscv_rv32b_eeeee11dddddccccc001aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 rs3 = (data & 0xf8000000) >> 27;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2], registers_std[rs3]);
	return 4;
}

static ut32 riscv_rv32b_eeeee11dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 rs3 = (data & 0xf8000000) >> 27;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2], registers_std[rs3]);
	return 4;
}

static ut32 riscv_rv32b_eeeee1ttttttccccc101aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_flt[fd], registers_flt[fs1], shamt);
	return 4;
}

static ut32 riscv_rv32d-zfh_010000100010cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32d-zfh_010001000001cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32d_0000001dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32d_0000101dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32d_0001001dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32d_0001101dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32d_0010001dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32d_0010001dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32d_0010001dddddccccc010aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32d_0010101dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32d_0010101dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32d_010000000001cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32d_010000100000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32d_010110100000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32d_1010001dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32d_1010001dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32d_1010001dddddccccc010aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32d_110000100000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32d_110000100001cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32d_110100100000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32d_110100100001cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32d_111000100000ccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_rv32d_eeeee01dddddcccccjjjaaaaa1000011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32d_eeeee01dddddcccccjjjaaaaa1000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32d_eeeee01dddddcccccjjjaaaaa1001011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32d_eeeee01dddddcccccjjjaaaaa1001111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32d_nnnnnnnnnnnnccccc011aaaaa0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_flt[fd], registers_flt[fs1], imm12);
	return 4;
}

static ut32 riscv_rv32d_ooooooodddddccccc011qqqqq0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 imm12hi = (data & 0xfe000000) >> 25;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 imm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x(%s)", imm12, registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32f_0000000dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32f_0000100dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32f_0001000dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32f_0001100dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32f_0010000dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32f_0010000dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32f_0010000dddddccccc010aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32f_0010100dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32f_0010100dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32f_010110000000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32f_1010000dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32f_1010000dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32f_1010000dddddccccc010aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32f_110000000000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32f_110000000001cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32f_110100000000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 fd = (data & 0xf80) >> 7;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rs1], registers_flt[fd], rm);
	return 4;
}

static ut32 riscv_rv32f_110100000001cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 fd = (data & 0xf80) >> 7;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rs1], registers_flt[fd], rm);
	return 4;
}

static ut32 riscv_rv32f_111000000000ccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_rv32f_111000000000ccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_rv32f_111100000000ccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_rv32f_eeeee00dddddcccccjjjaaaaa1000011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32f_eeeee00dddddcccccjjjaaaaa1000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32f_eeeee00dddddcccccjjjaaaaa1001011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32f_eeeee00dddddcccccjjjaaaaa1001111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32f_nnnnnnnnnnnnccccc010aaaaa0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_flt[fd], registers_flt[fs1], imm12);
	return 4;
}

static ut32 riscv_rv32f_ooooooodddddccccc010qqqqq0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 imm12hi = (data & 0xfe000000) >> 25;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 imm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x(%s)", imm12, registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32h_0010001dddddccccc000000001110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32h_011000000000ccccc100aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32h_011000000001ccccc100aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32h_0110001dddddccccc000000001110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32h_0110001dddddccccc100000001110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32h_011001000000ccccc100aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32h_011001000001ccccc100aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32h_011001000011ccccc100aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32h_0110011dddddccccc100000001110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32h_011010000000ccccc100aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32h_011010000011ccccc100aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv32h_0110101dddddccccc100000001110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_0000000dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_0000000dddddccccc001aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_0000000dddddccccc010aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_0000000dddddccccc011aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_0000000dddddccccc100aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_0000000dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_0000000dddddccccc110aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_0000000dddddccccc111aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_0100000dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_0100000dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_gggghhhhiiiiccccc000aaaaa0001111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fm = (data & 0xf0000000) >> 28;
	ut32 pred = (data & 0xf000000) >> 24;
	ut32 succ = (data & 0xf00000) >> 20;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", pred, succ);
	return 4;
}

static ut32 riscv_rv32i_llllllllllllllllllllaaaaa0010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 imm20 = (data & 0xfffff000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], imm20);
	return 4;
}

static ut32 riscv_rv32i_llllllllllllllllllllaaaaa0110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 imm20 = (data & 0xfffff000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], imm20);
	return 4;
}

static ut32 riscv_rv32i_mmmmmmmmmmmmmmmmmmmmaaaaa1101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 jimm20 = (data & 0xfffff000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%" PFMT64x "", registers_std[rd], pc + jimm20);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc000aaaaa0000011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc000aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc000aaaaa1100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc001aaaaa0000011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc010aaaaa0000011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc010aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc011aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc100aaaaa0000011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc100aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc101aaaaa0000011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc110aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_nnnnnnnnnnnnccccc111aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv32i_ooooooodddddccccc000qqqqq0100011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 imm12hi = (data & 0xfe000000) >> 25;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 imm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x(%s)", imm12, registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_ooooooodddddccccc001qqqqq0100011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 imm12hi = (data & 0xfe000000) >> 25;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 imm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x(%s)", imm12, registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_ooooooodddddccccc010qqqqq0100011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 imm12hi = (data & 0xfe000000) >> 25;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 imm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x(%s)", imm12, registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_pppppppdddddccccc000rrrrr1100011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 bimm12hi = (data & 0xfe000000) >> 25;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 bimm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", bimm12, registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_pppppppdddddccccc001rrrrr1100011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 bimm12hi = (data & 0xfe000000) >> 25;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 bimm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", bimm12, registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_pppppppdddddccccc100rrrrr1100011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 bimm12hi = (data & 0xfe000000) >> 25;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 bimm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", bimm12, registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_pppppppdddddccccc101rrrrr1100011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 bimm12hi = (data & 0xfe000000) >> 25;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 bimm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", bimm12, registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_pppppppdddddccccc110rrrrr1100011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 bimm12hi = (data & 0xfe000000) >> 25;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 bimm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", bimm12, registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32i_pppppppdddddccccc111rrrrr1100011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 bimm12hi = (data & 0xfe000000) >> 25;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 bimm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", bimm12, registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32k_0101000dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32k_0101001dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32k_0101010dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32k_0101011dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32k_0101110dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32k_0101111dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32k_vv10001dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32k_vv10011dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32k_vv10101dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32k_vv10111dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32m_0000001dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32m_0000001dddddccccc001aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32m_0000001dddddccccc010aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32m_0000001dddddccccc011aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32m_0000001dddddccccc100aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32m_0000001dddddccccc101aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32m_0000001dddddccccc110aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32m_0000001dddddccccc111aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv32q-zfh_010001000011cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32q-zfh_010001100010cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32q_0000011dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32q_0000111dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32q_0001011dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32q_0001111dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32q_0010011dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32q_0010011dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32q_0010011dddddccccc010aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32q_0010111dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32q_0010111dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32q_010000000011cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32q_010000100011cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32q_010001100000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32q_010001100001cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32q_010111100000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32q_1010011dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32q_1010011dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32q_1010011dddddccccc010aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32q_110001100000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32q_110001100001cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32q_110101100000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 fd = (data & 0xf80) >> 7;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rs1], registers_flt[fd], rm);
	return 4;
}

static ut32 riscv_rv32q_110101100001cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 fd = (data & 0xf80) >> 7;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rs1], registers_flt[fd], rm);
	return 4;
}

static ut32 riscv_rv32q_111001100000ccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_rv32q_eeeee11dddddcccccjjjaaaaa1000011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32q_eeeee11dddddcccccjjjaaaaa1000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32q_eeeee11dddddcccccjjjaaaaa1001011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32q_eeeee11dddddcccccjjjaaaaa1001111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32q_nnnnnnnnnnnnccccc100aaaaa0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_flt[fd], registers_flt[fs1], imm12);
	return 4;
}

static ut32 riscv_rv32q_ooooooodddddccccc100qqqqq0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 imm12hi = (data & 0xfe000000) >> 25;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 imm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x(%s)", imm12, registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32zfh_0000010dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32zfh_0000110dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32zfh_0001010dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32zfh_0001110dddddcccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], rm);
	return 4;
}

static ut32 riscv_rv32zfh_0010010dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32zfh_0010010dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32zfh_0010010dddddccccc010aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32zfh_0010110dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32zfh_0010110dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32zfh_010000000010cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32zfh_010001000000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32zfh_010111000000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32zfh_1010010dddddccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32zfh_1010010dddddccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32zfh_1010010dddddccccc010aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv32zfh_110001000000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32zfh_110001000001cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32zfh_110101000000cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32zfh_110101000001cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv32zfh_111001000000ccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_rv32zfh_111001000000ccccc001aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_rv32zfh_111101000000ccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_rv32zfh_eeeee10dddddcccccjjjaaaaa1000011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32zfh_eeeee10dddddcccccjjjaaaaa1000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32zfh_eeeee10dddddcccccjjjaaaaa1001011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32zfh_eeeee10dddddcccccjjjaaaaa1001111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3], rm);
	return 4;
}

static ut32 riscv_rv32zfh_nnnnnnnnnnnnccccc001aaaaa0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_flt[fd], registers_flt[fs1], imm12);
	return 4;
}

static ut32 riscv_rv32zfh_ooooooodddddccccc001qqqqq0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 imm12hi = (data & 0xfe000000) >> 25;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 imm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x(%s)", imm12, registers_flt[fs1], registers_flt[fs2]);
	return 4;
}

static ut32 riscv_rv64a_00000ffdddddccccc011aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64a_00001ffdddddccccc011aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64a_00010ff00000ccccc011aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, (%s)", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64a_00011ffdddddccccc011aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64a_00100ffdddddccccc011aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64a_01000ffdddddccccc011aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64a_01100ffdddddccccc011aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64a_10000ffdddddccccc011aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64a_10100ffdddddccccc011aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64a_11000ffdddddccccc011aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64a_11100ffdddddccccc011aaaaa0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 aqrl = (data & 0x6000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf("%s %s, %s, (%s)", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0000100dddddccccc000aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0000100dddddccccc001aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0000100dddddccccc011aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0000100dddddccccc100aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0000100dddddccccc101aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0000100dddddccccc110aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_000010ttttttccccc001aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv64b_0010000dddddccccc001aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0010000dddddccccc010aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0010000dddddccccc100aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0010000dddddccccc101aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0010000dddddccccc110aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0010000uuuuuccccc001aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_rv64b_0010000uuuuuccccc101aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_rv64b_0010100dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0010100dddddccccc101aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0010100uuuuuccccc101aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_rv64b_0100100dddddccccc011aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0100100dddddccccc100aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0100100dddddccccc110aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0100100dddddccccc111aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_011000000000ccccc001aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64b_011000000001ccccc001aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64b_011000000010ccccc001aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64b_011000000011ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64b_011000010011ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64b_011000011011ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64b_0110000dddddccccc001aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0110000dddddccccc101aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0110000uuuuuccccc101aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_rv64b_0110100dddddccccc101aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64b_0110100uuuuuccccc101aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_rv64b_eeeee10dddddccccc001aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3]);
	return 4;
}

static ut32 riscv_rv64b_eeeee10dddddccccc101aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 fs2 = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_flt[fd], registers_flt[fs1], registers_flt[fs2], registers_flt[fs3]);
	return 4;
}

static ut32 riscv_rv64b_eeeee10uuuuuccccc101aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	ut32 fs3 = (data & 0xf8000000) >> 27;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_flt[fd], shamtw);
	return 4;
}

static ut32 riscv_rv64d_110000100010cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv64d_110000100011cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv64d_110100100010cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv64d_110100100011cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv64d_111000100000ccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_rv64d_111100100000ccccc000aaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_flt[fd], registers_flt[fs1]);
	return 4;
}

static ut32 riscv_rv64f_110000000010cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv64f_110000000011cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv64f_110100000010cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 fd = (data & 0xf80) >> 7;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rs1], registers_flt[fd], rm);
	return 4;
}

static ut32 riscv_rv64f_110100000011cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 fd = (data & 0xf80) >> 7;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rs1], registers_flt[fd], rm);
	return 4;
}

static ut32 riscv_rv64h_011010000001ccccc100aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64h_011011000000ccccc100aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64h_0110111dddddccccc100000001110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64i_0000000dddddccccc000aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64i_0000000dddddccccc001aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64i_0000000dddddccccc101aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64i_0000000uuuuuccccc001aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_rv64i_0000000uuuuuccccc101aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_rv64i_000000ttttttccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv64i_000000ttttttccccc101aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv64i_0100000dddddccccc000aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64i_0100000dddddccccc101aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64i_0100000uuuuuccccc101aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamtw = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rd], shamtw);
	return 4;
}

static ut32 riscv_rv64i_010000ttttttccccc101aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 shamt = (data & 0x3f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], shamt);
	return 4;
}

static ut32 riscv_rv64i_nnnnnnnnnnnnccccc000aaaaa0011011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv64i_nnnnnnnnnnnnccccc011aaaaa0000011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv64i_nnnnnnnnnnnnccccc110aaaaa0000011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_rv64i_ooooooodddddccccc011qqqqq0100011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 imm12hi = (data & 0xfe000000) >> 25;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 imm12lo = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x(%s)", imm12, registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64k_000100000100ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64k_000100000101ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64k_000100000110ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64k_000100000111ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64k_001100000000ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64k_00110001wwwwccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rv64k_0011001dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64k_0011011dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64k_0011101dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64k_0011111dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64k_0111111dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64m_0000001dddddccccc000aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64m_0000001dddddccccc100aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64m_0000001dddddccccc101aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64m_0000001dddddccccc110aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64m_0000001dddddccccc111aaaaa0111011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rv64q_110001100010cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv64q_110001100011cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv64q_110101100010cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 fd = (data & 0xf80) >> 7;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rs1], registers_flt[fd], rm);
	return 4;
}

static ut32 riscv_rv64q_110101100011cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 fd = (data & 0xf80) >> 7;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rs1], registers_flt[fd], rm);
	return 4;
}

static ut32 riscv_rv64zfh_110001000010cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv64zfh_110001000011cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv64zfh_110101000010cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rv64zfh_110101000011cccccjjjaaaaa1010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 fd = (data & 0xf80) >> 7;
	ut32 fs1 = (data & 0xf8000) >> 15;
	ut32 rm = (data & 0x7000) >> 12;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_flt[fd], registers_flt[fs1], rm);
	return 4;
}

static ut32 riscv_rvk_000100000000ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvk_000100000001ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvk_000100000010ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvk_000100000011ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvk_000100001000ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvk_000100001001ccccc001aaaaa0010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvk_vv11000dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvk_vv11010dddddccccc000aaaaa0110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000010dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000011dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000110dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000111dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0000111dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001010dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001011dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001100dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001110dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001111dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0001111dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010010dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010011dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010100dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010110dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010111dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0010111dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011010BBBBBccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_0011010dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011011BBBBBccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_0011011dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011100dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011101dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011110dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011111dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0011111dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100010dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100011dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100101dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100110dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100110dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100111dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0100111dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101010dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101011dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101100dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101101dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101110dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101110dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101111dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0101111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110010dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110011dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110100dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110101dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110110dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110110dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110111dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0110111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_01110000AAAAccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_01110001AAAAccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_0111000BBBBBccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_0111000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_01110010AAAAccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_01110011AAAAccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_0111001BBBBBccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_0111001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_01110100AAAAccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_01110101AAAAccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_0111010BBBBBccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_0111010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0111011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_011110000zzzccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_011110001zzzccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_0111100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0111100dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_011110100zzzccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_011110101zzzccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_0111101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0111101dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_011111000zzzccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_011111001zzzccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_0111110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0111110dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_0111111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000000BBBBBccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1000000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000001BBBBBccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1000001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_10000100AAAAccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_10000101AAAAccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1000010BBBBBccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1000010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000011dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_100011000zzzccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_100011010zzzccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1000110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000111dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1000111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001011dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001111dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1001111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1010101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_101011000zzzccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011001000ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011001001ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011001010ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011001011ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011001100ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011001101ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011001110ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011001111ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011010000ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011010001ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011010010ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011010011ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011010100ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011010111ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011011000ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1010110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_101011100000ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011100001ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011100011ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011101000ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011101001ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011101011ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011111000ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011111001ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_101011111011ccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1010111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1011111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100010dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100011dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100100dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100101dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100110dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100111dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1100111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1101000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1101001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1101001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1101010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1101011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_110101CCCCCCccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1101100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1101101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1101110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1101111BBBBBccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1101111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1110000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1110000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1110000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1110001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1110001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1110010BBBBBccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1110010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1110011dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1110011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1110100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1110101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_111010CCCCCCccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1110110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1110111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111000dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111000dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111000dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111001dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111001dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111001dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111010BBBBBccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvp_1111010dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111011dddddccccc010aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111100dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111101dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111110dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111110dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111111dddddccccc000aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_1111111dddddccccc001aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvp_eeeee00dddddccccc011aaaaa1110111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs3 = (data & 0xf8000000) >> 27;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[rs3], registers_std[rs2], registers_std[rs1], registers_std[rd]);
	return 4;
}

static ut32 riscv_rvv_000000101000ccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000000101000ccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000000101000ccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000000101000ccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000000101000ccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000000101011ccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000000101011ccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000000HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000000HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000000HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000000HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000000HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000000HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000001101000ccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000001HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000001HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_00000IHGGGGGccccc000DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_00000IHGGGGGccccc101DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_00000IHGGGGGccccc110DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_00000IHGGGGGccccc111DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000010HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000010HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000010HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000010HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000010HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000011101000ccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000011HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000011HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000011HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000011HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_00001IHGGGGGccccc000DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_00001IHGGGGGccccc101DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_00001IHGGGGGccccc110DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_00001IHGGGGGccccc111DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000100HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000100HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000100HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000100HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000100HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000101HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000101HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000101HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000101HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000110HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000110HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000110HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000110HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000110HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000111101000ccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_000111HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000111HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000111HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_000111HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001000101000ccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001000101000ccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001000101000ccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001000101000ccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001000101000ccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001000HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001000HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001000HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001000HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001001HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001001HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001001HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001001HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001001HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001001HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001001HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_00100IHGGGGGccccc000DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_00100IHGGGGGccccc101DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_00100IHGGGGGccccc110DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_00100IHGGGGGccccc111DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001010HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001010HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001010HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001010HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001010HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001010HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001010HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001011HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001011HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001011HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001011HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001011HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001100HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001100HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001100HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001110HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001110HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001110HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001110HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001110HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001111HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_001111HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001111HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_001111HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_0100000GGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0100000GGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0100000GGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_010000100000ccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_010000100000ccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_0100001GGGGG00000001aaaaa1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs2], registers_std[rd]);
	return 4;
}

static ut32 riscv_rvv_0100001GGGGG00000010aaaaa1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs2], registers_std[rd]);
	return 4;
}

static ut32 riscv_rvv_010000HGGGGG10000010aaaaa1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs2], registers_std[rd]);
	return 4;
}

static ut32 riscv_rvv_010000HGGGGG10001010aaaaa1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs2], registers_std[rd]);
	return 4;
}

static ut32 riscv_rvv_0100010GGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0100010GGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0100010GGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_0100011GGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0100011GGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0100011GGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_01000IHGGGGGccccc000DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_01000IHGGGGGccccc101DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_01000IHGGGGGccccc110DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_01000IHGGGGGccccc111DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_0100100GGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0100100GGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00000001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00001001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00010001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00010010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00011001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00011010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00100010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00101010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00110001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00110010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00111001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG00111010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG01000001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG01001001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG01010001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG01011001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG01100001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG01110001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG01111001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG10000001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG10001001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG10010001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG10011001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG10100001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG10101001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG10110001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010010HGGGGG10111001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0100110GGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0100110GGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_0100111GGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0100111GGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_010011HGGGGG00000001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010011HGGGGG00100001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010011HGGGGG00101001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010011HGGGGG10000001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010100H0000010001010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_vec[vd]);
	return 4;
}

static ut32 riscv_rvv_010100HGGGGG00001010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010100HGGGGG00010010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010100HGGGGG00011010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_010100HGGGGG10000010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0101110GGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0101110GGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_0101110GGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_0101110GGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_010111100000FFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs1]);
	return 4;
}

static ut32 riscv_rvv_010111100000LLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_vec[vd]);
	return 4;
}

static ut32 riscv_rvv_010111100000ccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_010111100000ccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_0101111GGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011000101000ccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011000101000ccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011000101000ccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011000101000ccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011000101000ccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011000HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011000HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011000HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011000HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011000HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011000HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011001HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011001HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011001HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011001HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011001HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011001HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_01100IHGGGGGccccc000DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_01100IHGGGGGccccc101DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_01100IHGGGGGccccc110DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_01100IHGGGGGccccc111DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011010HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011010HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011010HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011011HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011011HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011011HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011011HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011011HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011100HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011100HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011100HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011100HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011100HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011100HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011101HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011101HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011101HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011101HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011101HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011110HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011110HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011110HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011111HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011111HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_011111HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_011111HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_0NNNNNNNNNNNccccc111aaaaa1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_1000000dddddccccc111aaaaa1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_std[rd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_100000HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100000HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100000HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100000HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100000HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100000HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100000HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100001HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100001HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100001HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100001HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100001HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100001HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_10000IHGGGGGccccc000DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_10000IHGGGGGccccc101DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_10000IHGGGGGccccc110DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_10000IHGGGGGccccc111DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100010HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100010HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100010HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100010HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100011HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100011HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100011HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100011HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100100HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100100HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100100HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100100HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100101HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100101HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100101HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100101HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100101HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100110HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100110HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_1001111GGGGG00000011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_1001111GGGGG00001011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_1001111GGGGG00011011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_1001111GGGGG00111011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_1001111GGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100111HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100111HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_100111HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100111HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_100111HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101000HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101000HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101000HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101000HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101000HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101001HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101001HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101001HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101001HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101001HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101001HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101001HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_10100IHGGGGGccccc000DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_10100IHGGGGGccccc101DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_10100IHGGGGGccccc110DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_10100IHGGGGGccccc111DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101010HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101010HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101010HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101010HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101010HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101011HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101011HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101011HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101011HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101011HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101011HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101011HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101100HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101100HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101100HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101100HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101100HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101101HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101101HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101101HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101101HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101101HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101101HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101101HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101110HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101110HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101110HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101110HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101110HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101111HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101111HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101111HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101111HGGGGGLLLLL011DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_101111HGGGGGccccc100DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101111HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_101111HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110000HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110000HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110000HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110000HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110000HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110001HGGGGGFFFFF000DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110001HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110001HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110001HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_11000IHGGGGGccccc000DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_11000IHGGGGGccccc101DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_11000IHGGGGGccccc110DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_11000IHGGGGGccccc111DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110010HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110010HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110010HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110010HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110011HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110011HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110011HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110100HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110100HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110100HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110100HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110101HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110101HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110110HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110110HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110110HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110110HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_110111HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_110111HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111000101000ccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111000101000ccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111000101000ccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111000101000ccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111000101000ccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111000HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_111000HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_111000HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111000HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_11100IHGGGGGccccc000DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_11100IHGGGGGccccc101DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_11100IHGGGGGccccc110DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_11100IHGGGGGccccc111DDDDD0101111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 wd = (data & 0x4000000) >> 26;
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s, %s", registers_std[wd], registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111010HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_111010HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111011HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_111011HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111100HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_111100HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_111100HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111100HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111101HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_111101HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_111101HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111101HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111110HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_111110HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111110HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111111HGGGGGFFFFF001DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_111111HGGGGGFFFFF010DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs1 = (data & 0xf8000) >> 15;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs1], registers_vec[vs2]);
	return 4;
}

static ut32 riscv_rvv_111111HGGGGGccccc101DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_111111HGGGGGccccc110DDDDD1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_11MMMMMMMMMMsssss111aaaaa1010111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_std[rd]);
	return 4;
}

static ut32 riscv_rvv_KKK000H00000ccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK000H00000ccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK000H00000ccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK000H00000ccccc101EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK000H00000ccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK000H00000ccccc110EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK000H00000ccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK000H00000ccccc111EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK000H10000ccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK000H10000ccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK000H10000ccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK000H10000ccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK001HGGGGGccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK001HGGGGGccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK001HGGGGGccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK001HGGGGGccccc101EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK001HGGGGGccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK001HGGGGGccccc110EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK001HGGGGGccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK001HGGGGGccccc111EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK010Hdddddccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK010Hdddddccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs3], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK010Hdddddccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK010Hdddddccccc101EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs3], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK010Hdddddccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK010Hdddddccccc110EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs3], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK010Hdddddccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK010Hdddddccccc111EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs3], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK011HGGGGGccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK011HGGGGGccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK011HGGGGGccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK011HGGGGGccccc101EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK011HGGGGGccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK011HGGGGGccccc110EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK011HGGGGGccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK011HGGGGGccccc111EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H00000ccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H00000ccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H00000ccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H00000ccccc101EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H00000ccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H00000ccccc110EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H00000ccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H00000ccccc111EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H10000ccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H10000ccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H10000ccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK100H10000ccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_vec[vd], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK101HGGGGGccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK101HGGGGGccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK101HGGGGGccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK101HGGGGGccccc101EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK101HGGGGGccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK101HGGGGGccccc110EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK101HGGGGGccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK101HGGGGGccccc111EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK110Hdddddccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK110Hdddddccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs3], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK110Hdddddccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK110Hdddddccccc101EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs3], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK110Hdddddccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK110Hdddddccccc110EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs3], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK110Hdddddccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK110Hdddddccccc111EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs3], registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_rvv_KKK111HGGGGGccccc000DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK111HGGGGGccccc000EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK111HGGGGGccccc101DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK111HGGGGGccccc101EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK111HGGGGGccccc110DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK111HGGGGGccccc110EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK111HGGGGGccccc111DDDDD0000111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vd = (data & 0xf80) >> 7;
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vd], registers_vec[vs2], registers_std[rs1]);
	return 4;
}

static ut32 riscv_rvv_KKK111HGGGGGccccc111EEEEE0100111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 vs2 = (data & 0x1f00000) >> 20;
	ut32 vs3 = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, %s", registers_vec[vs2], registers_vec[vs3], registers_std[rs1]);
	return 4;
}

static ut32 riscv_svinval_0001011dddddccccc000000001110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_svinval_0010011dddddccccc000000001110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_svinval_0110011dddddccccc000000001110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_system_0001001dddddccccc000000001110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 rs2 = (data & 0x1f00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s", registers_std[rs1], registers_std[rs2]);
	return 4;
}

static ut32 riscv_system_nnnnnnnnnnnnccccc001aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_system_nnnnnnnnnnnnccccc010aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_system_nnnnnnnnnnnnccccc011aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_system_nnnnnnnnnnnnccccc101aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_system_nnnnnnnnnnnnccccc110aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_system_nnnnnnnnnnnnccccc111aaaaa1110011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rd = (data & 0xf80) >> 7;
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12 = (data & 0xfff00000) >> 20;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, %s, 0x%x", registers_std[rd], registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_zicbo_000000000000ccccc010000000001111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_std[rs1]);
	return 4;
}

static ut32 riscv_zicbo_000000000001ccccc010000000001111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_std[rs1]);
	return 4;
}

static ut32 riscv_zicbo_000000000010ccccc010000000001111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_std[rs1]);
	return 4;
}

static ut32 riscv_zicbo_000000000100ccccc010000000001111(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s", registers_std[rs1]);
	return 4;
}

static ut32 riscv_zicbo_ooooooo00000ccccc110000000010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12hi = (data & 0xfe000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_zicbo_ooooooo00001ccccc110000000010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12hi = (data & 0xfe000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rs1], imm12);
	return 4;
}

static ut32 riscv_zicbo_ooooooo00011ccccc110000000010011(cchar* name, ut32 data, ut64 pc, RzStrBuf *sb) {
	ut32 rs1 = (data & 0xf8000) >> 15;
	ut32 imm12hi = (data & 0xfe000000) >> 25;
	rz_strbuf_set(sb, name);
	rz_str_buf_appendf(" %s, 0x%x", registers_std[rs1], imm12);
	return 4;
}

static const RISCVInstruction instructions[] = {
	{ "jal", /*              mmmmmmmmmmmmmmmmmmmmaaaaa1101111 */ 0x0000006f, 0x0000007f, riscv_rv32i_mmmmmmmmmmmmmmmmmmmmaaaaa1101111 },
	{ "lui", /*              llllllllllllllllllllaaaaa0110111 */ 0x00000037, 0x0000007f, riscv_rv32i_llllllllllllllllllllaaaaa0110111 },
	{ "auipc", /*            llllllllllllllllllllaaaaa0010111 */ 0x00000017, 0x0000007f, riscv_rv32i_llllllllllllllllllllaaaaa0010111 },
	{ "fld", /*              nnnnnnnnnnnnccccc011aaaaa0000111 */ 0x00003007, 0x0000707f, riscv_rv32d_nnnnnnnnnnnnccccc011aaaaa0000111 },
	{ "fsd", /*              ooooooodddddccccc011qqqqq0100111 */ 0x00003027, 0x0000707f, riscv_rv32d_ooooooodddddccccc011qqqqq0100111 },
	{ "flw", /*              nnnnnnnnnnnnccccc010aaaaa0000111 */ 0x00002007, 0x0000707f, riscv_rv32f_nnnnnnnnnnnnccccc010aaaaa0000111 },
	{ "fsw", /*              ooooooodddddccccc010qqqqq0100111 */ 0x00002027, 0x0000707f, riscv_rv32f_ooooooodddddccccc010qqqqq0100111 },
	{ "beq", /*              pppppppdddddccccc000rrrrr1100011 */ 0x00000063, 0x0000707f, riscv_rv32i_pppppppdddddccccc000rrrrr1100011 },
	{ "bne", /*              pppppppdddddccccc001rrrrr1100011 */ 0x00001063, 0x0000707f, riscv_rv32i_pppppppdddddccccc001rrrrr1100011 },
	{ "blt", /*              pppppppdddddccccc100rrrrr1100011 */ 0x00004063, 0x0000707f, riscv_rv32i_pppppppdddddccccc100rrrrr1100011 },
	{ "bge", /*              pppppppdddddccccc101rrrrr1100011 */ 0x00005063, 0x0000707f, riscv_rv32i_pppppppdddddccccc101rrrrr1100011 },
	{ "bltu", /*             pppppppdddddccccc110rrrrr1100011 */ 0x00006063, 0x0000707f, riscv_rv32i_pppppppdddddccccc110rrrrr1100011 },
	{ "bgeu", /*             pppppppdddddccccc111rrrrr1100011 */ 0x00007063, 0x0000707f, riscv_rv32i_pppppppdddddccccc111rrrrr1100011 },
	{ "jalr", /*             nnnnnnnnnnnnccccc000aaaaa1100111 */ 0x00000067, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc000aaaaa1100111 },
	{ "addi", /*             nnnnnnnnnnnnccccc000aaaaa0010011 */ 0x00000013, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc000aaaaa0010011 },
	{ "slti", /*             nnnnnnnnnnnnccccc010aaaaa0010011 */ 0x00002013, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc010aaaaa0010011 },
	{ "sltiu", /*            nnnnnnnnnnnnccccc011aaaaa0010011 */ 0x00003013, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc011aaaaa0010011 },
	{ "xori", /*             nnnnnnnnnnnnccccc100aaaaa0010011 */ 0x00004013, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc100aaaaa0010011 },
	{ "ori", /*              nnnnnnnnnnnnccccc110aaaaa0010011 */ 0x00006013, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc110aaaaa0010011 },
	{ "andi", /*             nnnnnnnnnnnnccccc111aaaaa0010011 */ 0x00007013, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc111aaaaa0010011 },
	{ "lb", /*               nnnnnnnnnnnnccccc000aaaaa0000011 */ 0x00000003, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc000aaaaa0000011 },
	{ "lh", /*               nnnnnnnnnnnnccccc001aaaaa0000011 */ 0x00001003, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc001aaaaa0000011 },
	{ "lw", /*               nnnnnnnnnnnnccccc010aaaaa0000011 */ 0x00002003, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc010aaaaa0000011 },
	{ "lbu", /*              nnnnnnnnnnnnccccc100aaaaa0000011 */ 0x00004003, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc100aaaaa0000011 },
	{ "lhu", /*              nnnnnnnnnnnnccccc101aaaaa0000011 */ 0x00005003, 0x0000707f, riscv_rv32i_nnnnnnnnnnnnccccc101aaaaa0000011 },
	{ "sb", /*               ooooooodddddccccc000qqqqq0100011 */ 0x00000023, 0x0000707f, riscv_rv32i_ooooooodddddccccc000qqqqq0100011 },
	{ "sh", /*               ooooooodddddccccc001qqqqq0100011 */ 0x00001023, 0x0000707f, riscv_rv32i_ooooooodddddccccc001qqqqq0100011 },
	{ "sw", /*               ooooooodddddccccc010qqqqq0100011 */ 0x00002023, 0x0000707f, riscv_rv32i_ooooooodddddccccc010qqqqq0100011 },
	{ "fence", /*            gggghhhhiiiiccccc000aaaaa0001111 */ 0x0000000f, 0x0000707f, riscv_rv32i_gggghhhhiiiiccccc000aaaaa0001111 },
	{ "fence.i", /*          nnnnnnnnnnnnccccc001aaaaa0001111 */ 0x0000100f, 0x0000707f, riscv_unique },
	{ "flq", /*              nnnnnnnnnnnnccccc100aaaaa0000111 */ 0x00004007, 0x0000707f, riscv_rv32q_nnnnnnnnnnnnccccc100aaaaa0000111 },
	{ "fsq", /*              ooooooodddddccccc100qqqqq0100111 */ 0x00004027, 0x0000707f, riscv_rv32q_ooooooodddddccccc100qqqqq0100111 },
	{ "flh", /*              nnnnnnnnnnnnccccc001aaaaa0000111 */ 0x00001007, 0x0000707f, riscv_rv32zfh_nnnnnnnnnnnnccccc001aaaaa0000111 },
	{ "fsh", /*              ooooooodddddccccc001qqqqq0100111 */ 0x00001027, 0x0000707f, riscv_rv32zfh_ooooooodddddccccc001qqqqq0100111 },
	{ "addiw", /*            nnnnnnnnnnnnccccc000aaaaa0011011 */ 0x0000001b, 0x0000707f, riscv_rv64i_nnnnnnnnnnnnccccc000aaaaa0011011 },
	{ "ld", /*               nnnnnnnnnnnnccccc011aaaaa0000011 */ 0x00003003, 0x0000707f, riscv_rv64i_nnnnnnnnnnnnccccc011aaaaa0000011 },
	{ "lwu", /*              nnnnnnnnnnnnccccc110aaaaa0000011 */ 0x00006003, 0x0000707f, riscv_rv64i_nnnnnnnnnnnnccccc110aaaaa0000011 },
	{ "sd", /*               ooooooodddddccccc011qqqqq0100011 */ 0x00003023, 0x0000707f, riscv_rv64i_ooooooodddddccccc011qqqqq0100011 },
	{ "csrrw", /*            nnnnnnnnnnnnccccc001aaaaa1110011 */ 0x00001073, 0x0000707f, riscv_system_nnnnnnnnnnnnccccc001aaaaa1110011 },
	{ "csrrs", /*            nnnnnnnnnnnnccccc010aaaaa1110011 */ 0x00002073, 0x0000707f, riscv_system_nnnnnnnnnnnnccccc010aaaaa1110011 },
	{ "csrrc", /*            nnnnnnnnnnnnccccc011aaaaa1110011 */ 0x00003073, 0x0000707f, riscv_system_nnnnnnnnnnnnccccc011aaaaa1110011 },
	{ "csrrwi", /*           nnnnnnnnnnnnccccc101aaaaa1110011 */ 0x00005073, 0x0000707f, riscv_system_nnnnnnnnnnnnccccc101aaaaa1110011 },
	{ "csrrsi", /*           nnnnnnnnnnnnccccc110aaaaa1110011 */ 0x00006073, 0x0000707f, riscv_system_nnnnnnnnnnnnccccc110aaaaa1110011 },
	{ "csrrci", /*           nnnnnnnnnnnnccccc111aaaaa1110011 */ 0x00007073, 0x0000707f, riscv_system_nnnnnnnnnnnnccccc111aaaaa1110011 },
	{ "prefetch.i", /*       ooooooo00000ccccc110000000010011 */ 0x00006013, 0x01f07fff, riscv_zicbo_ooooooo00000ccccc110000000010011 },
	{ "prefetch.r", /*       ooooooo00001ccccc110000000010011 */ 0x00106013, 0x01f07fff, riscv_zicbo_ooooooo00001ccccc110000000010011 },
	{ "prefetch.w", /*       ooooooo00011ccccc110000000010011 */ 0x00306013, 0x01f07fff, riscv_zicbo_ooooooo00011ccccc110000000010011 },
	{ "fsri", /*             eeeee1ttttttccccc101aaaaa0010011 */ 0x04005013, 0x0400707f, riscv_rv32b_eeeee1ttttttccccc101aaaaa0010011 },
	{ "fmadd.d", /*          eeeee01dddddcccccjjjaaaaa1000011 */ 0x02000043, 0x0600007f, riscv_rv32d_eeeee01dddddcccccjjjaaaaa1000011 },
	{ "fmsub.d", /*          eeeee01dddddcccccjjjaaaaa1000111 */ 0x02000047, 0x0600007f, riscv_rv32d_eeeee01dddddcccccjjjaaaaa1000111 },
	{ "fnmsub.d", /*         eeeee01dddddcccccjjjaaaaa1001011 */ 0x0200004b, 0x0600007f, riscv_rv32d_eeeee01dddddcccccjjjaaaaa1001011 },
	{ "fnmadd.d", /*         eeeee01dddddcccccjjjaaaaa1001111 */ 0x0200004f, 0x0600007f, riscv_rv32d_eeeee01dddddcccccjjjaaaaa1001111 },
	{ "fmadd.s", /*          eeeee00dddddcccccjjjaaaaa1000011 */ 0x00000043, 0x0600007f, riscv_rv32f_eeeee00dddddcccccjjjaaaaa1000011 },
	{ "fmsub.s", /*          eeeee00dddddcccccjjjaaaaa1000111 */ 0x00000047, 0x0600007f, riscv_rv32f_eeeee00dddddcccccjjjaaaaa1000111 },
	{ "fnmsub.s", /*         eeeee00dddddcccccjjjaaaaa1001011 */ 0x0000004b, 0x0600007f, riscv_rv32f_eeeee00dddddcccccjjjaaaaa1001011 },
	{ "fnmadd.s", /*         eeeee00dddddcccccjjjaaaaa1001111 */ 0x0000004f, 0x0600007f, riscv_rv32f_eeeee00dddddcccccjjjaaaaa1001111 },
	{ "fmadd.q", /*          eeeee11dddddcccccjjjaaaaa1000011 */ 0x06000043, 0x0600007f, riscv_rv32q_eeeee11dddddcccccjjjaaaaa1000011 },
	{ "fmsub.q", /*          eeeee11dddddcccccjjjaaaaa1000111 */ 0x06000047, 0x0600007f, riscv_rv32q_eeeee11dddddcccccjjjaaaaa1000111 },
	{ "fnmsub.q", /*         eeeee11dddddcccccjjjaaaaa1001011 */ 0x0600004b, 0x0600007f, riscv_rv32q_eeeee11dddddcccccjjjaaaaa1001011 },
	{ "fnmadd.q", /*         eeeee11dddddcccccjjjaaaaa1001111 */ 0x0600004f, 0x0600007f, riscv_rv32q_eeeee11dddddcccccjjjaaaaa1001111 },
	{ "fmadd.h", /*          eeeee10dddddcccccjjjaaaaa1000011 */ 0x04000043, 0x0600007f, riscv_rv32zfh_eeeee10dddddcccccjjjaaaaa1000011 },
	{ "fmsub.h", /*          eeeee10dddddcccccjjjaaaaa1000111 */ 0x04000047, 0x0600007f, riscv_rv32zfh_eeeee10dddddcccccjjjaaaaa1000111 },
	{ "fnmsub.h", /*         eeeee10dddddcccccjjjaaaaa1001011 */ 0x0400004b, 0x0600007f, riscv_rv32zfh_eeeee10dddddcccccjjjaaaaa1001011 },
	{ "fnmadd.h", /*         eeeee10dddddcccccjjjaaaaa1001111 */ 0x0400004f, 0x0600007f, riscv_rv32zfh_eeeee10dddddcccccjjjaaaaa1001111 },
	{ "cmix", /*             eeeee11dddddccccc001aaaaa0110011 */ 0x06001033, 0x0600707f, riscv_rv32b_eeeee11dddddccccc001aaaaa0110011 },
	{ "cmov", /*             eeeee11dddddccccc101aaaaa0110011 */ 0x06005033, 0x0600707f, riscv_rv32b_eeeee11dddddccccc101aaaaa0110011 },
	{ "fsl", /*              eeeee10dddddccccc001aaaaa0110011 */ 0x04001033, 0x0600707f, riscv_rv32b_eeeee10dddddccccc001aaaaa0110011 },
	{ "fsr", /*              eeeee10dddddccccc101aaaaa0110011 */ 0x04005033, 0x0600707f, riscv_rv32b_eeeee10dddddccccc101aaaaa0110011 },
	{ "fslw", /*             eeeee10dddddccccc001aaaaa0111011 */ 0x0400103b, 0x0600707f, riscv_rv64b_eeeee10dddddccccc001aaaaa0111011 },
	{ "fsrw", /*             eeeee10dddddccccc101aaaaa0111011 */ 0x0400503b, 0x0600707f, riscv_rv64b_eeeee10dddddccccc101aaaaa0111011 },
	{ "fsriw", /*            eeeee10uuuuuccccc101aaaaa0011011 */ 0x0400501b, 0x0600707f, riscv_rv64b_eeeee10uuuuuccccc101aaaaa0011011 },
	{ "bpick", /*            eeeee00dddddccccc011aaaaa1110111 */ 0x00003077, 0x0600707f, riscv_rvp_eeeee00dddddccccc011aaaaa1110111 },
	{ "vluxei8.v", /*        KKK001HGGGGGccccc000DDDDD0000111 */ 0x04000007, 0x1c00707f, riscv_rvv_KKK001HGGGGGccccc000DDDDD0000111 },
	{ "vluxei16.v", /*       KKK001HGGGGGccccc101DDDDD0000111 */ 0x04005007, 0x1c00707f, riscv_rvv_KKK001HGGGGGccccc101DDDDD0000111 },
	{ "vluxei32.v", /*       KKK001HGGGGGccccc110DDDDD0000111 */ 0x04006007, 0x1c00707f, riscv_rvv_KKK001HGGGGGccccc110DDDDD0000111 },
	{ "vluxei64.v", /*       KKK001HGGGGGccccc111DDDDD0000111 */ 0x04007007, 0x1c00707f, riscv_rvv_KKK001HGGGGGccccc111DDDDD0000111 },
	{ "vluxei128.v", /*      KKK101HGGGGGccccc000DDDDD0000111 */ 0x14000007, 0x1c00707f, riscv_rvv_KKK101HGGGGGccccc000DDDDD0000111 },
	{ "vluxei256.v", /*      KKK101HGGGGGccccc101DDDDD0000111 */ 0x14005007, 0x1c00707f, riscv_rvv_KKK101HGGGGGccccc101DDDDD0000111 },
	{ "vluxei512.v", /*      KKK101HGGGGGccccc110DDDDD0000111 */ 0x14006007, 0x1c00707f, riscv_rvv_KKK101HGGGGGccccc110DDDDD0000111 },
	{ "vluxei1024.v", /*     KKK101HGGGGGccccc111DDDDD0000111 */ 0x14007007, 0x1c00707f, riscv_rvv_KKK101HGGGGGccccc111DDDDD0000111 },
	{ "vsuxei8.v", /*        KKK001HGGGGGccccc000EEEEE0100111 */ 0x04000027, 0x1c00707f, riscv_rvv_KKK001HGGGGGccccc000EEEEE0100111 },
	{ "vsuxei16.v", /*       KKK001HGGGGGccccc101EEEEE0100111 */ 0x04005027, 0x1c00707f, riscv_rvv_KKK001HGGGGGccccc101EEEEE0100111 },
	{ "vsuxei32.v", /*       KKK001HGGGGGccccc110EEEEE0100111 */ 0x04006027, 0x1c00707f, riscv_rvv_KKK001HGGGGGccccc110EEEEE0100111 },
	{ "vsuxei64.v", /*       KKK001HGGGGGccccc111EEEEE0100111 */ 0x04007027, 0x1c00707f, riscv_rvv_KKK001HGGGGGccccc111EEEEE0100111 },
	{ "vsuxei128.v", /*      KKK101HGGGGGccccc000EEEEE0100111 */ 0x14000027, 0x1c00707f, riscv_rvv_KKK101HGGGGGccccc000EEEEE0100111 },
	{ "vsuxei256.v", /*      KKK101HGGGGGccccc101EEEEE0100111 */ 0x14005027, 0x1c00707f, riscv_rvv_KKK101HGGGGGccccc101EEEEE0100111 },
	{ "vsuxei512.v", /*      KKK101HGGGGGccccc110EEEEE0100111 */ 0x14006027, 0x1c00707f, riscv_rvv_KKK101HGGGGGccccc110EEEEE0100111 },
	{ "vsuxei1024.v", /*     KKK101HGGGGGccccc111EEEEE0100111 */ 0x14007027, 0x1c00707f, riscv_rvv_KKK101HGGGGGccccc111EEEEE0100111 },
	{ "vlse8.v", /*          KKK010Hdddddccccc000DDDDD0000111 */ 0x08000007, 0x1c00707f, riscv_rvv_KKK010Hdddddccccc000DDDDD0000111 },
	{ "vlse16.v", /*         KKK010Hdddddccccc101DDDDD0000111 */ 0x08005007, 0x1c00707f, riscv_rvv_KKK010Hdddddccccc101DDDDD0000111 },
	{ "vlse32.v", /*         KKK010Hdddddccccc110DDDDD0000111 */ 0x08006007, 0x1c00707f, riscv_rvv_KKK010Hdddddccccc110DDDDD0000111 },
	{ "vlse64.v", /*         KKK010Hdddddccccc111DDDDD0000111 */ 0x08007007, 0x1c00707f, riscv_rvv_KKK010Hdddddccccc111DDDDD0000111 },
	{ "vlse128.v", /*        KKK110Hdddddccccc000DDDDD0000111 */ 0x18000007, 0x1c00707f, riscv_rvv_KKK110Hdddddccccc000DDDDD0000111 },
	{ "vlse256.v", /*        KKK110Hdddddccccc101DDDDD0000111 */ 0x18005007, 0x1c00707f, riscv_rvv_KKK110Hdddddccccc101DDDDD0000111 },
	{ "vlse512.v", /*        KKK110Hdddddccccc110DDDDD0000111 */ 0x18006007, 0x1c00707f, riscv_rvv_KKK110Hdddddccccc110DDDDD0000111 },
	{ "vlse1024.v", /*       KKK110Hdddddccccc111DDDDD0000111 */ 0x18007007, 0x1c00707f, riscv_rvv_KKK110Hdddddccccc111DDDDD0000111 },
	{ "vsse8.v", /*          KKK010Hdddddccccc000EEEEE0100111 */ 0x08000027, 0x1c00707f, riscv_rvv_KKK010Hdddddccccc000EEEEE0100111 },
	{ "vsse16.v", /*         KKK010Hdddddccccc101EEEEE0100111 */ 0x08005027, 0x1c00707f, riscv_rvv_KKK010Hdddddccccc101EEEEE0100111 },
	{ "vsse32.v", /*         KKK010Hdddddccccc110EEEEE0100111 */ 0x08006027, 0x1c00707f, riscv_rvv_KKK010Hdddddccccc110EEEEE0100111 },
	{ "vsse64.v", /*         KKK010Hdddddccccc111EEEEE0100111 */ 0x08007027, 0x1c00707f, riscv_rvv_KKK010Hdddddccccc111EEEEE0100111 },
	{ "vsse128.v", /*        KKK110Hdddddccccc000EEEEE0100111 */ 0x18000027, 0x1c00707f, riscv_rvv_KKK110Hdddddccccc000EEEEE0100111 },
	{ "vsse256.v", /*        KKK110Hdddddccccc101EEEEE0100111 */ 0x18005027, 0x1c00707f, riscv_rvv_KKK110Hdddddccccc101EEEEE0100111 },
	{ "vsse512.v", /*        KKK110Hdddddccccc110EEEEE0100111 */ 0x18006027, 0x1c00707f, riscv_rvv_KKK110Hdddddccccc110EEEEE0100111 },
	{ "vsse1024.v", /*       KKK110Hdddddccccc111EEEEE0100111 */ 0x18007027, 0x1c00707f, riscv_rvv_KKK110Hdddddccccc111EEEEE0100111 },
	{ "vloxei8.v", /*        KKK011HGGGGGccccc000DDDDD0000111 */ 0x0c000007, 0x1c00707f, riscv_rvv_KKK011HGGGGGccccc000DDDDD0000111 },
	{ "vloxei16.v", /*       KKK011HGGGGGccccc101DDDDD0000111 */ 0x0c005007, 0x1c00707f, riscv_rvv_KKK011HGGGGGccccc101DDDDD0000111 },
	{ "vloxei32.v", /*       KKK011HGGGGGccccc110DDDDD0000111 */ 0x0c006007, 0x1c00707f, riscv_rvv_KKK011HGGGGGccccc110DDDDD0000111 },
	{ "vloxei64.v", /*       KKK011HGGGGGccccc111DDDDD0000111 */ 0x0c007007, 0x1c00707f, riscv_rvv_KKK011HGGGGGccccc111DDDDD0000111 },
	{ "vloxei128.v", /*      KKK111HGGGGGccccc000DDDDD0000111 */ 0x1c000007, 0x1c00707f, riscv_rvv_KKK111HGGGGGccccc000DDDDD0000111 },
	{ "vloxei256.v", /*      KKK111HGGGGGccccc101DDDDD0000111 */ 0x1c005007, 0x1c00707f, riscv_rvv_KKK111HGGGGGccccc101DDDDD0000111 },
	{ "vloxei512.v", /*      KKK111HGGGGGccccc110DDDDD0000111 */ 0x1c006007, 0x1c00707f, riscv_rvv_KKK111HGGGGGccccc110DDDDD0000111 },
	{ "vloxei1024.v", /*     KKK111HGGGGGccccc111DDDDD0000111 */ 0x1c007007, 0x1c00707f, riscv_rvv_KKK111HGGGGGccccc111DDDDD0000111 },
	{ "vsoxei8.v", /*        KKK011HGGGGGccccc000EEEEE0100111 */ 0x0c000027, 0x1c00707f, riscv_rvv_KKK011HGGGGGccccc000EEEEE0100111 },
	{ "vsoxei16.v", /*       KKK011HGGGGGccccc101EEEEE0100111 */ 0x0c005027, 0x1c00707f, riscv_rvv_KKK011HGGGGGccccc101EEEEE0100111 },
	{ "vsoxei32.v", /*       KKK011HGGGGGccccc110EEEEE0100111 */ 0x0c006027, 0x1c00707f, riscv_rvv_KKK011HGGGGGccccc110EEEEE0100111 },
	{ "vsoxei64.v", /*       KKK011HGGGGGccccc111EEEEE0100111 */ 0x0c007027, 0x1c00707f, riscv_rvv_KKK011HGGGGGccccc111EEEEE0100111 },
	{ "vsoxei128.v", /*      KKK111HGGGGGccccc000EEEEE0100111 */ 0x1c000027, 0x1c00707f, riscv_rvv_KKK111HGGGGGccccc000EEEEE0100111 },
	{ "vsoxei256.v", /*      KKK111HGGGGGccccc101EEEEE0100111 */ 0x1c005027, 0x1c00707f, riscv_rvv_KKK111HGGGGGccccc101EEEEE0100111 },
	{ "vsoxei512.v", /*      KKK111HGGGGGccccc110EEEEE0100111 */ 0x1c006027, 0x1c00707f, riscv_rvv_KKK111HGGGGGccccc110EEEEE0100111 },
	{ "vsoxei1024.v", /*     KKK111HGGGGGccccc111EEEEE0100111 */ 0x1c007027, 0x1c00707f, riscv_rvv_KKK111HGGGGGccccc111EEEEE0100111 },
	{ "vle8.v", /*           KKK000H00000ccccc000DDDDD0000111 */ 0x00000007, 0x1df0707f, riscv_rvv_KKK000H00000ccccc000DDDDD0000111 },
	{ "vle16.v", /*          KKK000H00000ccccc101DDDDD0000111 */ 0x00005007, 0x1df0707f, riscv_rvv_KKK000H00000ccccc101DDDDD0000111 },
	{ "vle32.v", /*          KKK000H00000ccccc110DDDDD0000111 */ 0x00006007, 0x1df0707f, riscv_rvv_KKK000H00000ccccc110DDDDD0000111 },
	{ "vle64.v", /*          KKK000H00000ccccc111DDDDD0000111 */ 0x00007007, 0x1df0707f, riscv_rvv_KKK000H00000ccccc111DDDDD0000111 },
	{ "vle128.v", /*         KKK100H00000ccccc000DDDDD0000111 */ 0x10000007, 0x1df0707f, riscv_rvv_KKK100H00000ccccc000DDDDD0000111 },
	{ "vle256.v", /*         KKK100H00000ccccc101DDDDD0000111 */ 0x10005007, 0x1df0707f, riscv_rvv_KKK100H00000ccccc101DDDDD0000111 },
	{ "vle512.v", /*         KKK100H00000ccccc110DDDDD0000111 */ 0x10006007, 0x1df0707f, riscv_rvv_KKK100H00000ccccc110DDDDD0000111 },
	{ "vle1024.v", /*        KKK100H00000ccccc111DDDDD0000111 */ 0x10007007, 0x1df0707f, riscv_rvv_KKK100H00000ccccc111DDDDD0000111 },
	{ "vse8.v", /*           KKK000H00000ccccc000EEEEE0100111 */ 0x00000027, 0x1df0707f, riscv_rvv_KKK000H00000ccccc000EEEEE0100111 },
	{ "vse16.v", /*          KKK000H00000ccccc101EEEEE0100111 */ 0x00005027, 0x1df0707f, riscv_rvv_KKK000H00000ccccc101EEEEE0100111 },
	{ "vse32.v", /*          KKK000H00000ccccc110EEEEE0100111 */ 0x00006027, 0x1df0707f, riscv_rvv_KKK000H00000ccccc110EEEEE0100111 },
	{ "vse64.v", /*          KKK000H00000ccccc111EEEEE0100111 */ 0x00007027, 0x1df0707f, riscv_rvv_KKK000H00000ccccc111EEEEE0100111 },
	{ "vse128.v", /*         KKK100H00000ccccc000EEEEE0100111 */ 0x10000027, 0x1df0707f, riscv_rvv_KKK100H00000ccccc000EEEEE0100111 },
	{ "vse256.v", /*         KKK100H00000ccccc101EEEEE0100111 */ 0x10005027, 0x1df0707f, riscv_rvv_KKK100H00000ccccc101EEEEE0100111 },
	{ "vse512.v", /*         KKK100H00000ccccc110EEEEE0100111 */ 0x10006027, 0x1df0707f, riscv_rvv_KKK100H00000ccccc110EEEEE0100111 },
	{ "vse1024.v", /*        KKK100H00000ccccc111EEEEE0100111 */ 0x10007027, 0x1df0707f, riscv_rvv_KKK100H00000ccccc111EEEEE0100111 },
	{ "vle8ff.v", /*         KKK000H10000ccccc000DDDDD0000111 */ 0x01000007, 0x1df0707f, riscv_rvv_KKK000H10000ccccc000DDDDD0000111 },
	{ "vle16ff.v", /*        KKK000H10000ccccc101DDDDD0000111 */ 0x01005007, 0x1df0707f, riscv_rvv_KKK000H10000ccccc101DDDDD0000111 },
	{ "vle32ff.v", /*        KKK000H10000ccccc110DDDDD0000111 */ 0x01006007, 0x1df0707f, riscv_rvv_KKK000H10000ccccc110DDDDD0000111 },
	{ "vle64ff.v", /*        KKK000H10000ccccc111DDDDD0000111 */ 0x01007007, 0x1df0707f, riscv_rvv_KKK000H10000ccccc111DDDDD0000111 },
	{ "vle128ff.v", /*       KKK100H10000ccccc000DDDDD0000111 */ 0x11000007, 0x1df0707f, riscv_rvv_KKK100H10000ccccc000DDDDD0000111 },
	{ "vle256ff.v", /*       KKK100H10000ccccc101DDDDD0000111 */ 0x11005007, 0x1df0707f, riscv_rvv_KKK100H10000ccccc101DDDDD0000111 },
	{ "vle512ff.v", /*       KKK100H10000ccccc110DDDDD0000111 */ 0x11006007, 0x1df0707f, riscv_rvv_KKK100H10000ccccc110DDDDD0000111 },
	{ "vle1024ff.v", /*      KKK100H10000ccccc111DDDDD0000111 */ 0x11007007, 0x1df0707f, riscv_rvv_KKK100H10000ccccc111DDDDD0000111 },
	{ "aes32esmi", /*        vv10011dddddccccc000aaaaa0110011 */ 0x26000033, 0x3e00707f, riscv_rv32k_vv10011dddddccccc000aaaaa0110011 },
	{ "aes32esi", /*         vv10001dddddccccc000aaaaa0110011 */ 0x22000033, 0x3e00707f, riscv_rv32k_vv10001dddddccccc000aaaaa0110011 },
	{ "aes32dsmi", /*        vv10111dddddccccc000aaaaa0110011 */ 0x2e000033, 0x3e00707f, riscv_rv32k_vv10111dddddccccc000aaaaa0110011 },
	{ "aes32dsi", /*         vv10101dddddccccc000aaaaa0110011 */ 0x2a000033, 0x3e00707f, riscv_rv32k_vv10101dddddccccc000aaaaa0110011 },
	{ "sm4ed", /*            vv11000dddddccccc000aaaaa0110011 */ 0x30000033, 0x3e00707f, riscv_rvk_vv11000dddddccccc000aaaaa0110011 },
	{ "sm4ks", /*            vv11010dddddccccc000aaaaa0110011 */ 0x34000033, 0x3e00707f, riscv_rvk_vv11010dddddccccc000aaaaa0110011 },
	{ "vsetvli", /*          0NNNNNNNNNNNccccc111aaaaa1010111 */ 0x00007057, 0x8000707f, riscv_rvv_0NNNNNNNNNNNccccc111aaaaa1010111 },
	{ "vsetivli", /*         11MMMMMMMMMMsssss111aaaaa1010111 */ 0xc0007057, 0xc000707f, riscv_rvv_11MMMMMMMMMMsssss111aaaaa1010111 },
	{ "amoadd.w", /*         00000ffdddddccccc010aaaaa0101111 */ 0x0000202f, 0xf800707f, riscv_rv32a_00000ffdddddccccc010aaaaa0101111 },
	{ "amoxor.w", /*         00100ffdddddccccc010aaaaa0101111 */ 0x2000202f, 0xf800707f, riscv_rv32a_00100ffdddddccccc010aaaaa0101111 },
	{ "amoor.w", /*          01000ffdddddccccc010aaaaa0101111 */ 0x4000202f, 0xf800707f, riscv_rv32a_01000ffdddddccccc010aaaaa0101111 },
	{ "amoand.w", /*         01100ffdddddccccc010aaaaa0101111 */ 0x6000202f, 0xf800707f, riscv_rv32a_01100ffdddddccccc010aaaaa0101111 },
	{ "amomin.w", /*         10000ffdddddccccc010aaaaa0101111 */ 0x8000202f, 0xf800707f, riscv_rv32a_10000ffdddddccccc010aaaaa0101111 },
	{ "amomax.w", /*         10100ffdddddccccc010aaaaa0101111 */ 0xa000202f, 0xf800707f, riscv_rv32a_10100ffdddddccccc010aaaaa0101111 },
	{ "amominu.w", /*        11000ffdddddccccc010aaaaa0101111 */ 0xc000202f, 0xf800707f, riscv_rv32a_11000ffdddddccccc010aaaaa0101111 },
	{ "amomaxu.w", /*        11100ffdddddccccc010aaaaa0101111 */ 0xe000202f, 0xf800707f, riscv_rv32a_11100ffdddddccccc010aaaaa0101111 },
	{ "amoswap.w", /*        00001ffdddddccccc010aaaaa0101111 */ 0x0800202f, 0xf800707f, riscv_rv32a_00001ffdddddccccc010aaaaa0101111 },
	{ "sc.w", /*             00011ffdddddccccc010aaaaa0101111 */ 0x1800202f, 0xf800707f, riscv_rv32a_00011ffdddddccccc010aaaaa0101111 },
	{ "amoadd.d", /*         00000ffdddddccccc011aaaaa0101111 */ 0x0000302f, 0xf800707f, riscv_rv64a_00000ffdddddccccc011aaaaa0101111 },
	{ "amoxor.d", /*         00100ffdddddccccc011aaaaa0101111 */ 0x2000302f, 0xf800707f, riscv_rv64a_00100ffdddddccccc011aaaaa0101111 },
	{ "amoor.d", /*          01000ffdddddccccc011aaaaa0101111 */ 0x4000302f, 0xf800707f, riscv_rv64a_01000ffdddddccccc011aaaaa0101111 },
	{ "amoand.d", /*         01100ffdddddccccc011aaaaa0101111 */ 0x6000302f, 0xf800707f, riscv_rv64a_01100ffdddddccccc011aaaaa0101111 },
	{ "amomin.d", /*         10000ffdddddccccc011aaaaa0101111 */ 0x8000302f, 0xf800707f, riscv_rv64a_10000ffdddddccccc011aaaaa0101111 },
	{ "amomax.d", /*         10100ffdddddccccc011aaaaa0101111 */ 0xa000302f, 0xf800707f, riscv_rv64a_10100ffdddddccccc011aaaaa0101111 },
	{ "amominu.d", /*        11000ffdddddccccc011aaaaa0101111 */ 0xc000302f, 0xf800707f, riscv_rv64a_11000ffdddddccccc011aaaaa0101111 },
	{ "amomaxu.d", /*        11100ffdddddccccc011aaaaa0101111 */ 0xe000302f, 0xf800707f, riscv_rv64a_11100ffdddddccccc011aaaaa0101111 },
	{ "amoswap.d", /*        00001ffdddddccccc011aaaaa0101111 */ 0x0800302f, 0xf800707f, riscv_rv64a_00001ffdddddccccc011aaaaa0101111 },
	{ "sc.d", /*             00011ffdddddccccc011aaaaa0101111 */ 0x1800302f, 0xf800707f, riscv_rv64a_00011ffdddddccccc011aaaaa0101111 },
	{ "vamoswapei8.v", /*    00001IHGGGGGccccc000DDDDD0101111 */ 0x0800002f, 0xf800707f, riscv_rvv_00001IHGGGGGccccc000DDDDD0101111 },
	{ "vamoaddei8.v", /*     00000IHGGGGGccccc000DDDDD0101111 */ 0x0000002f, 0xf800707f, riscv_rvv_00000IHGGGGGccccc000DDDDD0101111 },
	{ "vamoxorei8.v", /*     00100IHGGGGGccccc000DDDDD0101111 */ 0x2000002f, 0xf800707f, riscv_rvv_00100IHGGGGGccccc000DDDDD0101111 },
	{ "vamoandei8.v", /*     01100IHGGGGGccccc000DDDDD0101111 */ 0x6000002f, 0xf800707f, riscv_rvv_01100IHGGGGGccccc000DDDDD0101111 },
	{ "vamoorei8.v", /*      01000IHGGGGGccccc000DDDDD0101111 */ 0x4000002f, 0xf800707f, riscv_rvv_01000IHGGGGGccccc000DDDDD0101111 },
	{ "vamominei8.v", /*     10000IHGGGGGccccc000DDDDD0101111 */ 0x8000002f, 0xf800707f, riscv_rvv_10000IHGGGGGccccc000DDDDD0101111 },
	{ "vamomaxei8.v", /*     10100IHGGGGGccccc000DDDDD0101111 */ 0xa000002f, 0xf800707f, riscv_rvv_10100IHGGGGGccccc000DDDDD0101111 },
	{ "vamominuei8.v", /*    11000IHGGGGGccccc000DDDDD0101111 */ 0xc000002f, 0xf800707f, riscv_rvv_11000IHGGGGGccccc000DDDDD0101111 },
	{ "vamomaxuei8.v", /*    11100IHGGGGGccccc000DDDDD0101111 */ 0xe000002f, 0xf800707f, riscv_rvv_11100IHGGGGGccccc000DDDDD0101111 },
	{ "vamoswapei16.v", /*   00001IHGGGGGccccc101DDDDD0101111 */ 0x0800502f, 0xf800707f, riscv_rvv_00001IHGGGGGccccc101DDDDD0101111 },
	{ "vamoaddei16.v", /*    00000IHGGGGGccccc101DDDDD0101111 */ 0x0000502f, 0xf800707f, riscv_rvv_00000IHGGGGGccccc101DDDDD0101111 },
	{ "vamoxorei16.v", /*    00100IHGGGGGccccc101DDDDD0101111 */ 0x2000502f, 0xf800707f, riscv_rvv_00100IHGGGGGccccc101DDDDD0101111 },
	{ "vamoandei16.v", /*    01100IHGGGGGccccc101DDDDD0101111 */ 0x6000502f, 0xf800707f, riscv_rvv_01100IHGGGGGccccc101DDDDD0101111 },
	{ "vamoorei16.v", /*     01000IHGGGGGccccc101DDDDD0101111 */ 0x4000502f, 0xf800707f, riscv_rvv_01000IHGGGGGccccc101DDDDD0101111 },
	{ "vamominei16.v", /*    10000IHGGGGGccccc101DDDDD0101111 */ 0x8000502f, 0xf800707f, riscv_rvv_10000IHGGGGGccccc101DDDDD0101111 },
	{ "vamomaxei16.v", /*    10100IHGGGGGccccc101DDDDD0101111 */ 0xa000502f, 0xf800707f, riscv_rvv_10100IHGGGGGccccc101DDDDD0101111 },
	{ "vamominuei16.v", /*   11000IHGGGGGccccc101DDDDD0101111 */ 0xc000502f, 0xf800707f, riscv_rvv_11000IHGGGGGccccc101DDDDD0101111 },
	{ "vamomaxuei16.v", /*   11100IHGGGGGccccc101DDDDD0101111 */ 0xe000502f, 0xf800707f, riscv_rvv_11100IHGGGGGccccc101DDDDD0101111 },
	{ "vamoswapei32.v", /*   00001IHGGGGGccccc110DDDDD0101111 */ 0x0800602f, 0xf800707f, riscv_rvv_00001IHGGGGGccccc110DDDDD0101111 },
	{ "vamoaddei32.v", /*    00000IHGGGGGccccc110DDDDD0101111 */ 0x0000602f, 0xf800707f, riscv_rvv_00000IHGGGGGccccc110DDDDD0101111 },
	{ "vamoxorei32.v", /*    00100IHGGGGGccccc110DDDDD0101111 */ 0x2000602f, 0xf800707f, riscv_rvv_00100IHGGGGGccccc110DDDDD0101111 },
	{ "vamoandei32.v", /*    01100IHGGGGGccccc110DDDDD0101111 */ 0x6000602f, 0xf800707f, riscv_rvv_01100IHGGGGGccccc110DDDDD0101111 },
	{ "vamoorei32.v", /*     01000IHGGGGGccccc110DDDDD0101111 */ 0x4000602f, 0xf800707f, riscv_rvv_01000IHGGGGGccccc110DDDDD0101111 },
	{ "vamominei32.v", /*    10000IHGGGGGccccc110DDDDD0101111 */ 0x8000602f, 0xf800707f, riscv_rvv_10000IHGGGGGccccc110DDDDD0101111 },
	{ "vamomaxei32.v", /*    10100IHGGGGGccccc110DDDDD0101111 */ 0xa000602f, 0xf800707f, riscv_rvv_10100IHGGGGGccccc110DDDDD0101111 },
	{ "vamominuei32.v", /*   11000IHGGGGGccccc110DDDDD0101111 */ 0xc000602f, 0xf800707f, riscv_rvv_11000IHGGGGGccccc110DDDDD0101111 },
	{ "vamomaxuei32.v", /*   11100IHGGGGGccccc110DDDDD0101111 */ 0xe000602f, 0xf800707f, riscv_rvv_11100IHGGGGGccccc110DDDDD0101111 },
	{ "vamoswapei64.v", /*   00001IHGGGGGccccc111DDDDD0101111 */ 0x0800702f, 0xf800707f, riscv_rvv_00001IHGGGGGccccc111DDDDD0101111 },
	{ "vamoaddei64.v", /*    00000IHGGGGGccccc111DDDDD0101111 */ 0x0000702f, 0xf800707f, riscv_rvv_00000IHGGGGGccccc111DDDDD0101111 },
	{ "vamoxorei64.v", /*    00100IHGGGGGccccc111DDDDD0101111 */ 0x2000702f, 0xf800707f, riscv_rvv_00100IHGGGGGccccc111DDDDD0101111 },
	{ "vamoandei64.v", /*    01100IHGGGGGccccc111DDDDD0101111 */ 0x6000702f, 0xf800707f, riscv_rvv_01100IHGGGGGccccc111DDDDD0101111 },
	{ "vamoorei64.v", /*     01000IHGGGGGccccc111DDDDD0101111 */ 0x4000702f, 0xf800707f, riscv_rvv_01000IHGGGGGccccc111DDDDD0101111 },
	{ "vamominei64.v", /*    10000IHGGGGGccccc111DDDDD0101111 */ 0x8000702f, 0xf800707f, riscv_rvv_10000IHGGGGGccccc111DDDDD0101111 },
	{ "vamomaxei64.v", /*    10100IHGGGGGccccc111DDDDD0101111 */ 0xa000702f, 0xf800707f, riscv_rvv_10100IHGGGGGccccc111DDDDD0101111 },
	{ "vamominuei64.v", /*   11000IHGGGGGccccc111DDDDD0101111 */ 0xc000702f, 0xf800707f, riscv_rvv_11000IHGGGGGccccc111DDDDD0101111 },
	{ "vamomaxuei64.v", /*   11100IHGGGGGccccc111DDDDD0101111 */ 0xe000702f, 0xf800707f, riscv_rvv_11100IHGGGGGccccc111DDDDD0101111 },
	{ "lr.w", /*             00010ff00000ccccc010aaaaa0101111 */ 0x1000202f, 0xf9f0707f, riscv_rv32a_00010ff00000ccccc010aaaaa0101111 },
	{ "lr.d", /*             00010ff00000ccccc011aaaaa0101111 */ 0x1000302f, 0xf9f0707f, riscv_rv64a_00010ff00000ccccc011aaaaa0101111 },
	{ "sloi", /*             001000ttttttccccc001aaaaa0010011 */ 0x20001013, 0xfc00707f, riscv_rv32b_001000ttttttccccc001aaaaa0010011 },
	{ "sroi", /*             001000ttttttccccc101aaaaa0010011 */ 0x20005013, 0xfc00707f, riscv_rv32b_001000ttttttccccc101aaaaa0010011 },
	{ "rori", /*             011000ttttttccccc101aaaaa0010011 */ 0x60005013, 0xfc00707f, riscv_rv32b_011000ttttttccccc101aaaaa0010011 },
	{ "bclri", /*            010010ttttttccccc001aaaaa0010011 */ 0x48001013, 0xfc00707f, riscv_rv32b_010010ttttttccccc001aaaaa0010011 },
	{ "bseti", /*            001010ttttttccccc001aaaaa0010011 */ 0x28001013, 0xfc00707f, riscv_rv32b_001010ttttttccccc001aaaaa0010011 },
	{ "binvi", /*            011010ttttttccccc001aaaaa0010011 */ 0x68001013, 0xfc00707f, riscv_rv32b_011010ttttttccccc001aaaaa0010011 },
	{ "bexti", /*            010010ttttttccccc101aaaaa0010011 */ 0x48005013, 0xfc00707f, riscv_rv32b_010010ttttttccccc101aaaaa0010011 },
	{ "gorci", /*            001010ttttttccccc101aaaaa0010011 */ 0x28005013, 0xfc00707f, riscv_rv32b_001010ttttttccccc101aaaaa0010011 },
	{ "grevi", /*            011010ttttttccccc101aaaaa0010011 */ 0x68005013, 0xfc00707f, riscv_rv32b_011010ttttttccccc101aaaaa0010011 },
	{ "slli.uw", /*          000010ttttttccccc001aaaaa0011011 */ 0x0800101b, 0xfc00707f, riscv_rv64b_000010ttttttccccc001aaaaa0011011 },
	{ "slli", /*             000000ttttttccccc001aaaaa0010011 */ 0x00001013, 0xfc00707f, riscv_rv64i_000000ttttttccccc001aaaaa0010011 },
	{ "srli", /*             000000ttttttccccc101aaaaa0010011 */ 0x00005013, 0xfc00707f, riscv_rv64i_000000ttttttccccc101aaaaa0010011 },
	{ "srai", /*             010000ttttttccccc101aaaaa0010011 */ 0x40005013, 0xfc00707f, riscv_rv64i_010000ttttttccccc101aaaaa0010011 },
	{ "bitrevi", /*          111010CCCCCCccccc000aaaaa1110111 */ 0xe8000077, 0xfc00707f, riscv_rvp_111010CCCCCCccccc000aaaaa1110111 },
	{ "srai.u", /*           110101CCCCCCccccc001aaaaa1110111 */ 0xd4001077, 0xfc00707f, riscv_rvp_110101CCCCCCccccc001aaaaa1110111 },
	{ "vfadd.vf", /*         000000HGGGGGccccc101DDDDD1010111 */ 0x00005057, 0xfc00707f, riscv_rvv_000000HGGGGGccccc101DDDDD1010111 },
	{ "vfsub.vf", /*         000010HGGGGGccccc101DDDDD1010111 */ 0x08005057, 0xfc00707f, riscv_rvv_000010HGGGGGccccc101DDDDD1010111 },
	{ "vfmin.vf", /*         000100HGGGGGccccc101DDDDD1010111 */ 0x10005057, 0xfc00707f, riscv_rvv_000100HGGGGGccccc101DDDDD1010111 },
	{ "vfmax.vf", /*         000110HGGGGGccccc101DDDDD1010111 */ 0x18005057, 0xfc00707f, riscv_rvv_000110HGGGGGccccc101DDDDD1010111 },
	{ "vfsgnj.vf", /*        001000HGGGGGccccc101DDDDD1010111 */ 0x20005057, 0xfc00707f, riscv_rvv_001000HGGGGGccccc101DDDDD1010111 },
	{ "vfsgnjn.vf", /*       001001HGGGGGccccc101DDDDD1010111 */ 0x24005057, 0xfc00707f, riscv_rvv_001001HGGGGGccccc101DDDDD1010111 },
	{ "vfsgnjx.vf", /*       001010HGGGGGccccc101DDDDD1010111 */ 0x28005057, 0xfc00707f, riscv_rvv_001010HGGGGGccccc101DDDDD1010111 },
	{ "vfslide1up.vf", /*    001110HGGGGGccccc101DDDDD1010111 */ 0x38005057, 0xfc00707f, riscv_rvv_001110HGGGGGccccc101DDDDD1010111 },
	{ "vfslide1down.vf", /*  001111HGGGGGccccc101DDDDD1010111 */ 0x3c005057, 0xfc00707f, riscv_rvv_001111HGGGGGccccc101DDDDD1010111 },
	{ "vmfeq.vf", /*         011000HGGGGGccccc101DDDDD1010111 */ 0x60005057, 0xfc00707f, riscv_rvv_011000HGGGGGccccc101DDDDD1010111 },
	{ "vmfle.vf", /*         011001HGGGGGccccc101DDDDD1010111 */ 0x64005057, 0xfc00707f, riscv_rvv_011001HGGGGGccccc101DDDDD1010111 },
	{ "vmflt.vf", /*         011011HGGGGGccccc101DDDDD1010111 */ 0x6c005057, 0xfc00707f, riscv_rvv_011011HGGGGGccccc101DDDDD1010111 },
	{ "vmfne.vf", /*         011100HGGGGGccccc101DDDDD1010111 */ 0x70005057, 0xfc00707f, riscv_rvv_011100HGGGGGccccc101DDDDD1010111 },
	{ "vmfgt.vf", /*         011101HGGGGGccccc101DDDDD1010111 */ 0x74005057, 0xfc00707f, riscv_rvv_011101HGGGGGccccc101DDDDD1010111 },
	{ "vmfge.vf", /*         011111HGGGGGccccc101DDDDD1010111 */ 0x7c005057, 0xfc00707f, riscv_rvv_011111HGGGGGccccc101DDDDD1010111 },
	{ "vfdiv.vf", /*         100000HGGGGGccccc101DDDDD1010111 */ 0x80005057, 0xfc00707f, riscv_rvv_100000HGGGGGccccc101DDDDD1010111 },
	{ "vfrdiv.vf", /*        100001HGGGGGccccc101DDDDD1010111 */ 0x84005057, 0xfc00707f, riscv_rvv_100001HGGGGGccccc101DDDDD1010111 },
	{ "vfmul.vf", /*         100100HGGGGGccccc101DDDDD1010111 */ 0x90005057, 0xfc00707f, riscv_rvv_100100HGGGGGccccc101DDDDD1010111 },
	{ "vfrsub.vf", /*        100111HGGGGGccccc101DDDDD1010111 */ 0x9c005057, 0xfc00707f, riscv_rvv_100111HGGGGGccccc101DDDDD1010111 },
	{ "vfmadd.vf", /*        101000HGGGGGccccc101DDDDD1010111 */ 0xa0005057, 0xfc00707f, riscv_rvv_101000HGGGGGccccc101DDDDD1010111 },
	{ "vfnmadd.vf", /*       101001HGGGGGccccc101DDDDD1010111 */ 0xa4005057, 0xfc00707f, riscv_rvv_101001HGGGGGccccc101DDDDD1010111 },
	{ "vfmsub.vf", /*        101010HGGGGGccccc101DDDDD1010111 */ 0xa8005057, 0xfc00707f, riscv_rvv_101010HGGGGGccccc101DDDDD1010111 },
	{ "vfnmsub.vf", /*       101011HGGGGGccccc101DDDDD1010111 */ 0xac005057, 0xfc00707f, riscv_rvv_101011HGGGGGccccc101DDDDD1010111 },
	{ "vfmacc.vf", /*        101100HGGGGGccccc101DDDDD1010111 */ 0xb0005057, 0xfc00707f, riscv_rvv_101100HGGGGGccccc101DDDDD1010111 },
	{ "vfnmacc.vf", /*       101101HGGGGGccccc101DDDDD1010111 */ 0xb4005057, 0xfc00707f, riscv_rvv_101101HGGGGGccccc101DDDDD1010111 },
	{ "vfmsac.vf", /*        101110HGGGGGccccc101DDDDD1010111 */ 0xb8005057, 0xfc00707f, riscv_rvv_101110HGGGGGccccc101DDDDD1010111 },
	{ "vfnmsac.vf", /*       101111HGGGGGccccc101DDDDD1010111 */ 0xbc005057, 0xfc00707f, riscv_rvv_101111HGGGGGccccc101DDDDD1010111 },
	{ "vfwadd.vf", /*        110000HGGGGGccccc101DDDDD1010111 */ 0xc0005057, 0xfc00707f, riscv_rvv_110000HGGGGGccccc101DDDDD1010111 },
	{ "vfwsub.vf", /*        110010HGGGGGccccc101DDDDD1010111 */ 0xc8005057, 0xfc00707f, riscv_rvv_110010HGGGGGccccc101DDDDD1010111 },
	{ "vfwadd.wf", /*        110100HGGGGGccccc101DDDDD1010111 */ 0xd0005057, 0xfc00707f, riscv_rvv_110100HGGGGGccccc101DDDDD1010111 },
	{ "vfwsub.wf", /*        110110HGGGGGccccc101DDDDD1010111 */ 0xd8005057, 0xfc00707f, riscv_rvv_110110HGGGGGccccc101DDDDD1010111 },
	{ "vfwmul.vf", /*        111000HGGGGGccccc101DDDDD1010111 */ 0xe0005057, 0xfc00707f, riscv_rvv_111000HGGGGGccccc101DDDDD1010111 },
	{ "vfwmacc.vf", /*       111100HGGGGGccccc101DDDDD1010111 */ 0xf0005057, 0xfc00707f, riscv_rvv_111100HGGGGGccccc101DDDDD1010111 },
	{ "vfwnmacc.vf", /*      111101HGGGGGccccc101DDDDD1010111 */ 0xf4005057, 0xfc00707f, riscv_rvv_111101HGGGGGccccc101DDDDD1010111 },
	{ "vfwmsac.vf", /*       111110HGGGGGccccc101DDDDD1010111 */ 0xf8005057, 0xfc00707f, riscv_rvv_111110HGGGGGccccc101DDDDD1010111 },
	{ "vfwnmsac.vf", /*      111111HGGGGGccccc101DDDDD1010111 */ 0xfc005057, 0xfc00707f, riscv_rvv_111111HGGGGGccccc101DDDDD1010111 },
	{ "vfadd.vv", /*         000000HGGGGGFFFFF001DDDDD1010111 */ 0x00001057, 0xfc00707f, riscv_rvv_000000HGGGGGFFFFF001DDDDD1010111 },
	{ "vfredusum.vs", /*     000001HGGGGGFFFFF001DDDDD1010111 */ 0x04001057, 0xfc00707f, riscv_rvv_000001HGGGGGFFFFF001DDDDD1010111 },
	{ "vfsub.vv", /*         000010HGGGGGFFFFF001DDDDD1010111 */ 0x08001057, 0xfc00707f, riscv_rvv_000010HGGGGGFFFFF001DDDDD1010111 },
	{ "vfredosum.vs", /*     000011HGGGGGFFFFF001DDDDD1010111 */ 0x0c001057, 0xfc00707f, riscv_rvv_000011HGGGGGFFFFF001DDDDD1010111 },
	{ "vfmin.vv", /*         000100HGGGGGFFFFF001DDDDD1010111 */ 0x10001057, 0xfc00707f, riscv_rvv_000100HGGGGGFFFFF001DDDDD1010111 },
	{ "vfredmin.vs", /*      000101HGGGGGFFFFF001DDDDD1010111 */ 0x14001057, 0xfc00707f, riscv_rvv_000101HGGGGGFFFFF001DDDDD1010111 },
	{ "vfmax.vv", /*         000110HGGGGGFFFFF001DDDDD1010111 */ 0x18001057, 0xfc00707f, riscv_rvv_000110HGGGGGFFFFF001DDDDD1010111 },
	{ "vfredmax.vs", /*      000111HGGGGGFFFFF001DDDDD1010111 */ 0x1c001057, 0xfc00707f, riscv_rvv_000111HGGGGGFFFFF001DDDDD1010111 },
	{ "vfsgnj.vv", /*        001000HGGGGGFFFFF001DDDDD1010111 */ 0x20001057, 0xfc00707f, riscv_rvv_001000HGGGGGFFFFF001DDDDD1010111 },
	{ "vfsgnjn.vv", /*       001001HGGGGGFFFFF001DDDDD1010111 */ 0x24001057, 0xfc00707f, riscv_rvv_001001HGGGGGFFFFF001DDDDD1010111 },
	{ "vfsgnjx.vv", /*       001010HGGGGGFFFFF001DDDDD1010111 */ 0x28001057, 0xfc00707f, riscv_rvv_001010HGGGGGFFFFF001DDDDD1010111 },
	{ "vmfeq.vv", /*         011000HGGGGGFFFFF001DDDDD1010111 */ 0x60001057, 0xfc00707f, riscv_rvv_011000HGGGGGFFFFF001DDDDD1010111 },
	{ "vmfle.vv", /*         011001HGGGGGFFFFF001DDDDD1010111 */ 0x64001057, 0xfc00707f, riscv_rvv_011001HGGGGGFFFFF001DDDDD1010111 },
	{ "vmflt.vv", /*         011011HGGGGGFFFFF001DDDDD1010111 */ 0x6c001057, 0xfc00707f, riscv_rvv_011011HGGGGGFFFFF001DDDDD1010111 },
	{ "vmfne.vv", /*         011100HGGGGGFFFFF001DDDDD1010111 */ 0x70001057, 0xfc00707f, riscv_rvv_011100HGGGGGFFFFF001DDDDD1010111 },
	{ "vfdiv.vv", /*         100000HGGGGGFFFFF001DDDDD1010111 */ 0x80001057, 0xfc00707f, riscv_rvv_100000HGGGGGFFFFF001DDDDD1010111 },
	{ "vfmul.vv", /*         100100HGGGGGFFFFF001DDDDD1010111 */ 0x90001057, 0xfc00707f, riscv_rvv_100100HGGGGGFFFFF001DDDDD1010111 },
	{ "vfmadd.vv", /*        101000HGGGGGFFFFF001DDDDD1010111 */ 0xa0001057, 0xfc00707f, riscv_rvv_101000HGGGGGFFFFF001DDDDD1010111 },
	{ "vfnmadd.vv", /*       101001HGGGGGFFFFF001DDDDD1010111 */ 0xa4001057, 0xfc00707f, riscv_rvv_101001HGGGGGFFFFF001DDDDD1010111 },
	{ "vfmsub.vv", /*        101010HGGGGGFFFFF001DDDDD1010111 */ 0xa8001057, 0xfc00707f, riscv_rvv_101010HGGGGGFFFFF001DDDDD1010111 },
	{ "vfnmsub.vv", /*       101011HGGGGGFFFFF001DDDDD1010111 */ 0xac001057, 0xfc00707f, riscv_rvv_101011HGGGGGFFFFF001DDDDD1010111 },
	{ "vfmacc.vv", /*        101100HGGGGGFFFFF001DDDDD1010111 */ 0xb0001057, 0xfc00707f, riscv_rvv_101100HGGGGGFFFFF001DDDDD1010111 },
	{ "vfnmacc.vv", /*       101101HGGGGGFFFFF001DDDDD1010111 */ 0xb4001057, 0xfc00707f, riscv_rvv_101101HGGGGGFFFFF001DDDDD1010111 },
	{ "vfmsac.vv", /*        101110HGGGGGFFFFF001DDDDD1010111 */ 0xb8001057, 0xfc00707f, riscv_rvv_101110HGGGGGFFFFF001DDDDD1010111 },
	{ "vfnmsac.vv", /*       101111HGGGGGFFFFF001DDDDD1010111 */ 0xbc001057, 0xfc00707f, riscv_rvv_101111HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwadd.vv", /*        110000HGGGGGFFFFF001DDDDD1010111 */ 0xc0001057, 0xfc00707f, riscv_rvv_110000HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwredusum.vs", /*    110001HGGGGGFFFFF001DDDDD1010111 */ 0xc4001057, 0xfc00707f, riscv_rvv_110001HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwsub.vv", /*        110010HGGGGGFFFFF001DDDDD1010111 */ 0xc8001057, 0xfc00707f, riscv_rvv_110010HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwredosum.vs", /*    110011HGGGGGFFFFF001DDDDD1010111 */ 0xcc001057, 0xfc00707f, riscv_rvv_110011HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwadd.wv", /*        110100HGGGGGFFFFF001DDDDD1010111 */ 0xd0001057, 0xfc00707f, riscv_rvv_110100HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwsub.wv", /*        110110HGGGGGFFFFF001DDDDD1010111 */ 0xd8001057, 0xfc00707f, riscv_rvv_110110HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwmul.vv", /*        111000HGGGGGFFFFF001DDDDD1010111 */ 0xe0001057, 0xfc00707f, riscv_rvv_111000HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwmacc.vv", /*       111100HGGGGGFFFFF001DDDDD1010111 */ 0xf0001057, 0xfc00707f, riscv_rvv_111100HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwnmacc.vv", /*      111101HGGGGGFFFFF001DDDDD1010111 */ 0xf4001057, 0xfc00707f, riscv_rvv_111101HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwmsac.vv", /*       111110HGGGGGFFFFF001DDDDD1010111 */ 0xf8001057, 0xfc00707f, riscv_rvv_111110HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwnmsac.vv", /*      111111HGGGGGFFFFF001DDDDD1010111 */ 0xfc001057, 0xfc00707f, riscv_rvv_111111HGGGGGFFFFF001DDDDD1010111 },
	{ "vadd.vx", /*          000000HGGGGGccccc100DDDDD1010111 */ 0x00004057, 0xfc00707f, riscv_rvv_000000HGGGGGccccc100DDDDD1010111 },
	{ "vsub.vx", /*          000010HGGGGGccccc100DDDDD1010111 */ 0x08004057, 0xfc00707f, riscv_rvv_000010HGGGGGccccc100DDDDD1010111 },
	{ "vrsub.vx", /*         000011HGGGGGccccc100DDDDD1010111 */ 0x0c004057, 0xfc00707f, riscv_rvv_000011HGGGGGccccc100DDDDD1010111 },
	{ "vminu.vx", /*         000100HGGGGGccccc100DDDDD1010111 */ 0x10004057, 0xfc00707f, riscv_rvv_000100HGGGGGccccc100DDDDD1010111 },
	{ "vmin.vx", /*          000101HGGGGGccccc100DDDDD1010111 */ 0x14004057, 0xfc00707f, riscv_rvv_000101HGGGGGccccc100DDDDD1010111 },
	{ "vmaxu.vx", /*         000110HGGGGGccccc100DDDDD1010111 */ 0x18004057, 0xfc00707f, riscv_rvv_000110HGGGGGccccc100DDDDD1010111 },
	{ "vmax.vx", /*          000111HGGGGGccccc100DDDDD1010111 */ 0x1c004057, 0xfc00707f, riscv_rvv_000111HGGGGGccccc100DDDDD1010111 },
	{ "vand.vx", /*          001001HGGGGGccccc100DDDDD1010111 */ 0x24004057, 0xfc00707f, riscv_rvv_001001HGGGGGccccc100DDDDD1010111 },
	{ "vor.vx", /*           001010HGGGGGccccc100DDDDD1010111 */ 0x28004057, 0xfc00707f, riscv_rvv_001010HGGGGGccccc100DDDDD1010111 },
	{ "vxor.vx", /*          001011HGGGGGccccc100DDDDD1010111 */ 0x2c004057, 0xfc00707f, riscv_rvv_001011HGGGGGccccc100DDDDD1010111 },
	{ "vrgather.vx", /*      001100HGGGGGccccc100DDDDD1010111 */ 0x30004057, 0xfc00707f, riscv_rvv_001100HGGGGGccccc100DDDDD1010111 },
	{ "vslideup.vx", /*      001110HGGGGGccccc100DDDDD1010111 */ 0x38004057, 0xfc00707f, riscv_rvv_001110HGGGGGccccc100DDDDD1010111 },
	{ "vslidedown.vx", /*    001111HGGGGGccccc100DDDDD1010111 */ 0x3c004057, 0xfc00707f, riscv_rvv_001111HGGGGGccccc100DDDDD1010111 },
	{ "vmseq.vx", /*         011000HGGGGGccccc100DDDDD1010111 */ 0x60004057, 0xfc00707f, riscv_rvv_011000HGGGGGccccc100DDDDD1010111 },
	{ "vmsne.vx", /*         011001HGGGGGccccc100DDDDD1010111 */ 0x64004057, 0xfc00707f, riscv_rvv_011001HGGGGGccccc100DDDDD1010111 },
	{ "vmsltu.vx", /*        011010HGGGGGccccc100DDDDD1010111 */ 0x68004057, 0xfc00707f, riscv_rvv_011010HGGGGGccccc100DDDDD1010111 },
	{ "vmslt.vx", /*         011011HGGGGGccccc100DDDDD1010111 */ 0x6c004057, 0xfc00707f, riscv_rvv_011011HGGGGGccccc100DDDDD1010111 },
	{ "vmsleu.vx", /*        011100HGGGGGccccc100DDDDD1010111 */ 0x70004057, 0xfc00707f, riscv_rvv_011100HGGGGGccccc100DDDDD1010111 },
	{ "vmsle.vx", /*         011101HGGGGGccccc100DDDDD1010111 */ 0x74004057, 0xfc00707f, riscv_rvv_011101HGGGGGccccc100DDDDD1010111 },
	{ "vmsgtu.vx", /*        011110HGGGGGccccc100DDDDD1010111 */ 0x78004057, 0xfc00707f, riscv_rvv_011110HGGGGGccccc100DDDDD1010111 },
	{ "vmsgt.vx", /*         011111HGGGGGccccc100DDDDD1010111 */ 0x7c004057, 0xfc00707f, riscv_rvv_011111HGGGGGccccc100DDDDD1010111 },
	{ "vsaddu.vx", /*        100000HGGGGGccccc100DDDDD1010111 */ 0x80004057, 0xfc00707f, riscv_rvv_100000HGGGGGccccc100DDDDD1010111 },
	{ "vsadd.vx", /*         100001HGGGGGccccc100DDDDD1010111 */ 0x84004057, 0xfc00707f, riscv_rvv_100001HGGGGGccccc100DDDDD1010111 },
	{ "vssubu.vx", /*        100010HGGGGGccccc100DDDDD1010111 */ 0x88004057, 0xfc00707f, riscv_rvv_100010HGGGGGccccc100DDDDD1010111 },
	{ "vssub.vx", /*         100011HGGGGGccccc100DDDDD1010111 */ 0x8c004057, 0xfc00707f, riscv_rvv_100011HGGGGGccccc100DDDDD1010111 },
	{ "vsll.vx", /*          100101HGGGGGccccc100DDDDD1010111 */ 0x94004057, 0xfc00707f, riscv_rvv_100101HGGGGGccccc100DDDDD1010111 },
	{ "vsmul.vx", /*         100111HGGGGGccccc100DDDDD1010111 */ 0x9c004057, 0xfc00707f, riscv_rvv_100111HGGGGGccccc100DDDDD1010111 },
	{ "vsrl.vx", /*          101000HGGGGGccccc100DDDDD1010111 */ 0xa0004057, 0xfc00707f, riscv_rvv_101000HGGGGGccccc100DDDDD1010111 },
	{ "vsra.vx", /*          101001HGGGGGccccc100DDDDD1010111 */ 0xa4004057, 0xfc00707f, riscv_rvv_101001HGGGGGccccc100DDDDD1010111 },
	{ "vssrl.vx", /*         101010HGGGGGccccc100DDDDD1010111 */ 0xa8004057, 0xfc00707f, riscv_rvv_101010HGGGGGccccc100DDDDD1010111 },
	{ "vssra.vx", /*         101011HGGGGGccccc100DDDDD1010111 */ 0xac004057, 0xfc00707f, riscv_rvv_101011HGGGGGccccc100DDDDD1010111 },
	{ "vnsrl.wx", /*         101100HGGGGGccccc100DDDDD1010111 */ 0xb0004057, 0xfc00707f, riscv_rvv_101100HGGGGGccccc100DDDDD1010111 },
	{ "vnsra.wx", /*         101101HGGGGGccccc100DDDDD1010111 */ 0xb4004057, 0xfc00707f, riscv_rvv_101101HGGGGGccccc100DDDDD1010111 },
	{ "vnclipu.wx", /*       101110HGGGGGccccc100DDDDD1010111 */ 0xb8004057, 0xfc00707f, riscv_rvv_101110HGGGGGccccc100DDDDD1010111 },
	{ "vnclip.wx", /*        101111HGGGGGccccc100DDDDD1010111 */ 0xbc004057, 0xfc00707f, riscv_rvv_101111HGGGGGccccc100DDDDD1010111 },
	{ "vadd.vv", /*          000000HGGGGGFFFFF000DDDDD1010111 */ 0x00000057, 0xfc00707f, riscv_rvv_000000HGGGGGFFFFF000DDDDD1010111 },
	{ "vsub.vv", /*          000010HGGGGGFFFFF000DDDDD1010111 */ 0x08000057, 0xfc00707f, riscv_rvv_000010HGGGGGFFFFF000DDDDD1010111 },
	{ "vminu.vv", /*         000100HGGGGGFFFFF000DDDDD1010111 */ 0x10000057, 0xfc00707f, riscv_rvv_000100HGGGGGFFFFF000DDDDD1010111 },
	{ "vmin.vv", /*          000101HGGGGGFFFFF000DDDDD1010111 */ 0x14000057, 0xfc00707f, riscv_rvv_000101HGGGGGFFFFF000DDDDD1010111 },
	{ "vmaxu.vv", /*         000110HGGGGGFFFFF000DDDDD1010111 */ 0x18000057, 0xfc00707f, riscv_rvv_000110HGGGGGFFFFF000DDDDD1010111 },
	{ "vmax.vv", /*          000111HGGGGGFFFFF000DDDDD1010111 */ 0x1c000057, 0xfc00707f, riscv_rvv_000111HGGGGGFFFFF000DDDDD1010111 },
	{ "vand.vv", /*          001001HGGGGGFFFFF000DDDDD1010111 */ 0x24000057, 0xfc00707f, riscv_rvv_001001HGGGGGFFFFF000DDDDD1010111 },
	{ "vor.vv", /*           001010HGGGGGFFFFF000DDDDD1010111 */ 0x28000057, 0xfc00707f, riscv_rvv_001010HGGGGGFFFFF000DDDDD1010111 },
	{ "vxor.vv", /*          001011HGGGGGFFFFF000DDDDD1010111 */ 0x2c000057, 0xfc00707f, riscv_rvv_001011HGGGGGFFFFF000DDDDD1010111 },
	{ "vrgather.vv", /*      001100HGGGGGFFFFF000DDDDD1010111 */ 0x30000057, 0xfc00707f, riscv_rvv_001100HGGGGGFFFFF000DDDDD1010111 },
	{ "vrgatherei16.vv", /*  001110HGGGGGFFFFF000DDDDD1010111 */ 0x38000057, 0xfc00707f, riscv_rvv_001110HGGGGGFFFFF000DDDDD1010111 },
	{ "vmseq.vv", /*         011000HGGGGGFFFFF000DDDDD1010111 */ 0x60000057, 0xfc00707f, riscv_rvv_011000HGGGGGFFFFF000DDDDD1010111 },
	{ "vmsne.vv", /*         011001HGGGGGFFFFF000DDDDD1010111 */ 0x64000057, 0xfc00707f, riscv_rvv_011001HGGGGGFFFFF000DDDDD1010111 },
	{ "vmsltu.vv", /*        011010HGGGGGFFFFF000DDDDD1010111 */ 0x68000057, 0xfc00707f, riscv_rvv_011010HGGGGGFFFFF000DDDDD1010111 },
	{ "vmslt.vv", /*         011011HGGGGGFFFFF000DDDDD1010111 */ 0x6c000057, 0xfc00707f, riscv_rvv_011011HGGGGGFFFFF000DDDDD1010111 },
	{ "vmsleu.vv", /*        011100HGGGGGFFFFF000DDDDD1010111 */ 0x70000057, 0xfc00707f, riscv_rvv_011100HGGGGGFFFFF000DDDDD1010111 },
	{ "vmsle.vv", /*         011101HGGGGGFFFFF000DDDDD1010111 */ 0x74000057, 0xfc00707f, riscv_rvv_011101HGGGGGFFFFF000DDDDD1010111 },
	{ "vsaddu.vv", /*        100000HGGGGGFFFFF000DDDDD1010111 */ 0x80000057, 0xfc00707f, riscv_rvv_100000HGGGGGFFFFF000DDDDD1010111 },
	{ "vsadd.vv", /*         100001HGGGGGFFFFF000DDDDD1010111 */ 0x84000057, 0xfc00707f, riscv_rvv_100001HGGGGGFFFFF000DDDDD1010111 },
	{ "vssubu.vv", /*        100010HGGGGGFFFFF000DDDDD1010111 */ 0x88000057, 0xfc00707f, riscv_rvv_100010HGGGGGFFFFF000DDDDD1010111 },
	{ "vssub.vv", /*         100011HGGGGGFFFFF000DDDDD1010111 */ 0x8c000057, 0xfc00707f, riscv_rvv_100011HGGGGGFFFFF000DDDDD1010111 },
	{ "vsll.vv", /*          100101HGGGGGFFFFF000DDDDD1010111 */ 0x94000057, 0xfc00707f, riscv_rvv_100101HGGGGGFFFFF000DDDDD1010111 },
	{ "vsmul.vv", /*         100111HGGGGGFFFFF000DDDDD1010111 */ 0x9c000057, 0xfc00707f, riscv_rvv_100111HGGGGGFFFFF000DDDDD1010111 },
	{ "vsrl.vv", /*          101000HGGGGGFFFFF000DDDDD1010111 */ 0xa0000057, 0xfc00707f, riscv_rvv_101000HGGGGGFFFFF000DDDDD1010111 },
	{ "vsra.vv", /*          101001HGGGGGFFFFF000DDDDD1010111 */ 0xa4000057, 0xfc00707f, riscv_rvv_101001HGGGGGFFFFF000DDDDD1010111 },
	{ "vssrl.vv", /*         101010HGGGGGFFFFF000DDDDD1010111 */ 0xa8000057, 0xfc00707f, riscv_rvv_101010HGGGGGFFFFF000DDDDD1010111 },
	{ "vssra.vv", /*         101011HGGGGGFFFFF000DDDDD1010111 */ 0xac000057, 0xfc00707f, riscv_rvv_101011HGGGGGFFFFF000DDDDD1010111 },
	{ "vnsrl.wv", /*         101100HGGGGGFFFFF000DDDDD1010111 */ 0xb0000057, 0xfc00707f, riscv_rvv_101100HGGGGGFFFFF000DDDDD1010111 },
	{ "vnsra.wv", /*         101101HGGGGGFFFFF000DDDDD1010111 */ 0xb4000057, 0xfc00707f, riscv_rvv_101101HGGGGGFFFFF000DDDDD1010111 },
	{ "vnclipu.wv", /*       101110HGGGGGFFFFF000DDDDD1010111 */ 0xb8000057, 0xfc00707f, riscv_rvv_101110HGGGGGFFFFF000DDDDD1010111 },
	{ "vnclip.wv", /*        101111HGGGGGFFFFF000DDDDD1010111 */ 0xbc000057, 0xfc00707f, riscv_rvv_101111HGGGGGFFFFF000DDDDD1010111 },
	{ "vwredsumu.vs", /*     110000HGGGGGFFFFF000DDDDD1010111 */ 0xc0000057, 0xfc00707f, riscv_rvv_110000HGGGGGFFFFF000DDDDD1010111 },
	{ "vwredsum.vs", /*      110001HGGGGGFFFFF000DDDDD1010111 */ 0xc4000057, 0xfc00707f, riscv_rvv_110001HGGGGGFFFFF000DDDDD1010111 },
	{ "vadd.vi", /*          000000HGGGGGLLLLL011DDDDD1010111 */ 0x00003057, 0xfc00707f, riscv_rvv_000000HGGGGGLLLLL011DDDDD1010111 },
	{ "vrsub.vi", /*         000011HGGGGGLLLLL011DDDDD1010111 */ 0x0c003057, 0xfc00707f, riscv_rvv_000011HGGGGGLLLLL011DDDDD1010111 },
	{ "vand.vi", /*          001001HGGGGGLLLLL011DDDDD1010111 */ 0x24003057, 0xfc00707f, riscv_rvv_001001HGGGGGLLLLL011DDDDD1010111 },
	{ "vor.vi", /*           001010HGGGGGLLLLL011DDDDD1010111 */ 0x28003057, 0xfc00707f, riscv_rvv_001010HGGGGGLLLLL011DDDDD1010111 },
	{ "vxor.vi", /*          001011HGGGGGLLLLL011DDDDD1010111 */ 0x2c003057, 0xfc00707f, riscv_rvv_001011HGGGGGLLLLL011DDDDD1010111 },
	{ "vrgather.vi", /*      001100HGGGGGLLLLL011DDDDD1010111 */ 0x30003057, 0xfc00707f, riscv_rvv_001100HGGGGGLLLLL011DDDDD1010111 },
	{ "vslideup.vi", /*      001110HGGGGGLLLLL011DDDDD1010111 */ 0x38003057, 0xfc00707f, riscv_rvv_001110HGGGGGLLLLL011DDDDD1010111 },
	{ "vslidedown.vi", /*    001111HGGGGGLLLLL011DDDDD1010111 */ 0x3c003057, 0xfc00707f, riscv_rvv_001111HGGGGGLLLLL011DDDDD1010111 },
	{ "vmseq.vi", /*         011000HGGGGGLLLLL011DDDDD1010111 */ 0x60003057, 0xfc00707f, riscv_rvv_011000HGGGGGLLLLL011DDDDD1010111 },
	{ "vmsne.vi", /*         011001HGGGGGLLLLL011DDDDD1010111 */ 0x64003057, 0xfc00707f, riscv_rvv_011001HGGGGGLLLLL011DDDDD1010111 },
	{ "vmsleu.vi", /*        011100HGGGGGLLLLL011DDDDD1010111 */ 0x70003057, 0xfc00707f, riscv_rvv_011100HGGGGGLLLLL011DDDDD1010111 },
	{ "vmsle.vi", /*         011101HGGGGGLLLLL011DDDDD1010111 */ 0x74003057, 0xfc00707f, riscv_rvv_011101HGGGGGLLLLL011DDDDD1010111 },
	{ "vmsgtu.vi", /*        011110HGGGGGLLLLL011DDDDD1010111 */ 0x78003057, 0xfc00707f, riscv_rvv_011110HGGGGGLLLLL011DDDDD1010111 },
	{ "vmsgt.vi", /*         011111HGGGGGLLLLL011DDDDD1010111 */ 0x7c003057, 0xfc00707f, riscv_rvv_011111HGGGGGLLLLL011DDDDD1010111 },
	{ "vsaddu.vi", /*        100000HGGGGGLLLLL011DDDDD1010111 */ 0x80003057, 0xfc00707f, riscv_rvv_100000HGGGGGLLLLL011DDDDD1010111 },
	{ "vsadd.vi", /*         100001HGGGGGLLLLL011DDDDD1010111 */ 0x84003057, 0xfc00707f, riscv_rvv_100001HGGGGGLLLLL011DDDDD1010111 },
	{ "vsll.vi", /*          100101HGGGGGLLLLL011DDDDD1010111 */ 0x94003057, 0xfc00707f, riscv_rvv_100101HGGGGGLLLLL011DDDDD1010111 },
	{ "vsrl.vi", /*          101000HGGGGGLLLLL011DDDDD1010111 */ 0xa0003057, 0xfc00707f, riscv_rvv_101000HGGGGGLLLLL011DDDDD1010111 },
	{ "vsra.vi", /*          101001HGGGGGLLLLL011DDDDD1010111 */ 0xa4003057, 0xfc00707f, riscv_rvv_101001HGGGGGLLLLL011DDDDD1010111 },
	{ "vssrl.vi", /*         101010HGGGGGLLLLL011DDDDD1010111 */ 0xa8003057, 0xfc00707f, riscv_rvv_101010HGGGGGLLLLL011DDDDD1010111 },
	{ "vssra.vi", /*         101011HGGGGGLLLLL011DDDDD1010111 */ 0xac003057, 0xfc00707f, riscv_rvv_101011HGGGGGLLLLL011DDDDD1010111 },
	{ "vnsrl.wi", /*         101100HGGGGGLLLLL011DDDDD1010111 */ 0xb0003057, 0xfc00707f, riscv_rvv_101100HGGGGGLLLLL011DDDDD1010111 },
	{ "vnsra.wi", /*         101101HGGGGGLLLLL011DDDDD1010111 */ 0xb4003057, 0xfc00707f, riscv_rvv_101101HGGGGGLLLLL011DDDDD1010111 },
	{ "vnclipu.wi", /*       101110HGGGGGLLLLL011DDDDD1010111 */ 0xb8003057, 0xfc00707f, riscv_rvv_101110HGGGGGLLLLL011DDDDD1010111 },
	{ "vnclip.wi", /*        101111HGGGGGLLLLL011DDDDD1010111 */ 0xbc003057, 0xfc00707f, riscv_rvv_101111HGGGGGLLLLL011DDDDD1010111 },
	{ "vredsum.vs", /*       000000HGGGGGFFFFF010DDDDD1010111 */ 0x00002057, 0xfc00707f, riscv_rvv_000000HGGGGGFFFFF010DDDDD1010111 },
	{ "vredand.vs", /*       000001HGGGGGFFFFF010DDDDD1010111 */ 0x04002057, 0xfc00707f, riscv_rvv_000001HGGGGGFFFFF010DDDDD1010111 },
	{ "vredor.vs", /*        000010HGGGGGFFFFF010DDDDD1010111 */ 0x08002057, 0xfc00707f, riscv_rvv_000010HGGGGGFFFFF010DDDDD1010111 },
	{ "vredxor.vs", /*       000011HGGGGGFFFFF010DDDDD1010111 */ 0x0c002057, 0xfc00707f, riscv_rvv_000011HGGGGGFFFFF010DDDDD1010111 },
	{ "vredminu.vs", /*      000100HGGGGGFFFFF010DDDDD1010111 */ 0x10002057, 0xfc00707f, riscv_rvv_000100HGGGGGFFFFF010DDDDD1010111 },
	{ "vredmin.vs", /*       000101HGGGGGFFFFF010DDDDD1010111 */ 0x14002057, 0xfc00707f, riscv_rvv_000101HGGGGGFFFFF010DDDDD1010111 },
	{ "vredmaxu.vs", /*      000110HGGGGGFFFFF010DDDDD1010111 */ 0x18002057, 0xfc00707f, riscv_rvv_000110HGGGGGFFFFF010DDDDD1010111 },
	{ "vredmax.vs", /*       000111HGGGGGFFFFF010DDDDD1010111 */ 0x1c002057, 0xfc00707f, riscv_rvv_000111HGGGGGFFFFF010DDDDD1010111 },
	{ "vaaddu.vv", /*        001000HGGGGGFFFFF010DDDDD1010111 */ 0x20002057, 0xfc00707f, riscv_rvv_001000HGGGGGFFFFF010DDDDD1010111 },
	{ "vaadd.vv", /*         001001HGGGGGFFFFF010DDDDD1010111 */ 0x24002057, 0xfc00707f, riscv_rvv_001001HGGGGGFFFFF010DDDDD1010111 },
	{ "vasubu.vv", /*        001010HGGGGGFFFFF010DDDDD1010111 */ 0x28002057, 0xfc00707f, riscv_rvv_001010HGGGGGFFFFF010DDDDD1010111 },
	{ "vasub.vv", /*         001011HGGGGGFFFFF010DDDDD1010111 */ 0x2c002057, 0xfc00707f, riscv_rvv_001011HGGGGGFFFFF010DDDDD1010111 },
	{ "vmandn.mm", /*        011000HGGGGGFFFFF010DDDDD1010111 */ 0x60002057, 0xfc00707f, riscv_rvv_011000HGGGGGFFFFF010DDDDD1010111 },
	{ "vmand.mm", /*         011001HGGGGGFFFFF010DDDDD1010111 */ 0x64002057, 0xfc00707f, riscv_rvv_011001HGGGGGFFFFF010DDDDD1010111 },
	{ "vmor.mm", /*          011010HGGGGGFFFFF010DDDDD1010111 */ 0x68002057, 0xfc00707f, riscv_rvv_011010HGGGGGFFFFF010DDDDD1010111 },
	{ "vmxor.mm", /*         011011HGGGGGFFFFF010DDDDD1010111 */ 0x6c002057, 0xfc00707f, riscv_rvv_011011HGGGGGFFFFF010DDDDD1010111 },
	{ "vmorn.mm", /*         011100HGGGGGFFFFF010DDDDD1010111 */ 0x70002057, 0xfc00707f, riscv_rvv_011100HGGGGGFFFFF010DDDDD1010111 },
	{ "vmnand.mm", /*        011101HGGGGGFFFFF010DDDDD1010111 */ 0x74002057, 0xfc00707f, riscv_rvv_011101HGGGGGFFFFF010DDDDD1010111 },
	{ "vmnor.mm", /*         011110HGGGGGFFFFF010DDDDD1010111 */ 0x78002057, 0xfc00707f, riscv_rvv_011110HGGGGGFFFFF010DDDDD1010111 },
	{ "vmxnor.mm", /*        011111HGGGGGFFFFF010DDDDD1010111 */ 0x7c002057, 0xfc00707f, riscv_rvv_011111HGGGGGFFFFF010DDDDD1010111 },
	{ "vdivu.vv", /*         100000HGGGGGFFFFF010DDDDD1010111 */ 0x80002057, 0xfc00707f, riscv_rvv_100000HGGGGGFFFFF010DDDDD1010111 },
	{ "vdiv.vv", /*          100001HGGGGGFFFFF010DDDDD1010111 */ 0x84002057, 0xfc00707f, riscv_rvv_100001HGGGGGFFFFF010DDDDD1010111 },
	{ "vremu.vv", /*         100010HGGGGGFFFFF010DDDDD1010111 */ 0x88002057, 0xfc00707f, riscv_rvv_100010HGGGGGFFFFF010DDDDD1010111 },
	{ "vrem.vv", /*          100011HGGGGGFFFFF010DDDDD1010111 */ 0x8c002057, 0xfc00707f, riscv_rvv_100011HGGGGGFFFFF010DDDDD1010111 },
	{ "vmulhu.vv", /*        100100HGGGGGFFFFF010DDDDD1010111 */ 0x90002057, 0xfc00707f, riscv_rvv_100100HGGGGGFFFFF010DDDDD1010111 },
	{ "vmul.vv", /*          100101HGGGGGFFFFF010DDDDD1010111 */ 0x94002057, 0xfc00707f, riscv_rvv_100101HGGGGGFFFFF010DDDDD1010111 },
	{ "vmulhsu.vv", /*       100110HGGGGGFFFFF010DDDDD1010111 */ 0x98002057, 0xfc00707f, riscv_rvv_100110HGGGGGFFFFF010DDDDD1010111 },
	{ "vmulh.vv", /*         100111HGGGGGFFFFF010DDDDD1010111 */ 0x9c002057, 0xfc00707f, riscv_rvv_100111HGGGGGFFFFF010DDDDD1010111 },
	{ "vmadd.vv", /*         101001HGGGGGFFFFF010DDDDD1010111 */ 0xa4002057, 0xfc00707f, riscv_rvv_101001HGGGGGFFFFF010DDDDD1010111 },
	{ "vnmsub.vv", /*        101011HGGGGGFFFFF010DDDDD1010111 */ 0xac002057, 0xfc00707f, riscv_rvv_101011HGGGGGFFFFF010DDDDD1010111 },
	{ "vmacc.vv", /*         101101HGGGGGFFFFF010DDDDD1010111 */ 0xb4002057, 0xfc00707f, riscv_rvv_101101HGGGGGFFFFF010DDDDD1010111 },
	{ "vnmsac.vv", /*        101111HGGGGGFFFFF010DDDDD1010111 */ 0xbc002057, 0xfc00707f, riscv_rvv_101111HGGGGGFFFFF010DDDDD1010111 },
	{ "vwaddu.vv", /*        110000HGGGGGFFFFF010DDDDD1010111 */ 0xc0002057, 0xfc00707f, riscv_rvv_110000HGGGGGFFFFF010DDDDD1010111 },
	{ "vwadd.vv", /*         110001HGGGGGFFFFF010DDDDD1010111 */ 0xc4002057, 0xfc00707f, riscv_rvv_110001HGGGGGFFFFF010DDDDD1010111 },
	{ "vwsubu.vv", /*        110010HGGGGGFFFFF010DDDDD1010111 */ 0xc8002057, 0xfc00707f, riscv_rvv_110010HGGGGGFFFFF010DDDDD1010111 },
	{ "vwsub.vv", /*         110011HGGGGGFFFFF010DDDDD1010111 */ 0xcc002057, 0xfc00707f, riscv_rvv_110011HGGGGGFFFFF010DDDDD1010111 },
	{ "vwaddu.wv", /*        110100HGGGGGFFFFF010DDDDD1010111 */ 0xd0002057, 0xfc00707f, riscv_rvv_110100HGGGGGFFFFF010DDDDD1010111 },
	{ "vwadd.wv", /*         110101HGGGGGFFFFF010DDDDD1010111 */ 0xd4002057, 0xfc00707f, riscv_rvv_110101HGGGGGFFFFF010DDDDD1010111 },
	{ "vwsubu.wv", /*        110110HGGGGGFFFFF010DDDDD1010111 */ 0xd8002057, 0xfc00707f, riscv_rvv_110110HGGGGGFFFFF010DDDDD1010111 },
	{ "vwsub.wv", /*         110111HGGGGGFFFFF010DDDDD1010111 */ 0xdc002057, 0xfc00707f, riscv_rvv_110111HGGGGGFFFFF010DDDDD1010111 },
	{ "vwmulu.vv", /*        111000HGGGGGFFFFF010DDDDD1010111 */ 0xe0002057, 0xfc00707f, riscv_rvv_111000HGGGGGFFFFF010DDDDD1010111 },
	{ "vwmulsu.vv", /*       111010HGGGGGFFFFF010DDDDD1010111 */ 0xe8002057, 0xfc00707f, riscv_rvv_111010HGGGGGFFFFF010DDDDD1010111 },
	{ "vwmul.vv", /*         111011HGGGGGFFFFF010DDDDD1010111 */ 0xec002057, 0xfc00707f, riscv_rvv_111011HGGGGGFFFFF010DDDDD1010111 },
	{ "vwmaccu.vv", /*       111100HGGGGGFFFFF010DDDDD1010111 */ 0xf0002057, 0xfc00707f, riscv_rvv_111100HGGGGGFFFFF010DDDDD1010111 },
	{ "vwmacc.vv", /*        111101HGGGGGFFFFF010DDDDD1010111 */ 0xf4002057, 0xfc00707f, riscv_rvv_111101HGGGGGFFFFF010DDDDD1010111 },
	{ "vwmaccsu.vv", /*      111111HGGGGGFFFFF010DDDDD1010111 */ 0xfc002057, 0xfc00707f, riscv_rvv_111111HGGGGGFFFFF010DDDDD1010111 },
	{ "vaaddu.vx", /*        001000HGGGGGccccc110DDDDD1010111 */ 0x20006057, 0xfc00707f, riscv_rvv_001000HGGGGGccccc110DDDDD1010111 },
	{ "vaadd.vx", /*         001001HGGGGGccccc110DDDDD1010111 */ 0x24006057, 0xfc00707f, riscv_rvv_001001HGGGGGccccc110DDDDD1010111 },
	{ "vasubu.vx", /*        001010HGGGGGccccc110DDDDD1010111 */ 0x28006057, 0xfc00707f, riscv_rvv_001010HGGGGGccccc110DDDDD1010111 },
	{ "vasub.vx", /*         001011HGGGGGccccc110DDDDD1010111 */ 0x2c006057, 0xfc00707f, riscv_rvv_001011HGGGGGccccc110DDDDD1010111 },
	{ "vslide1up.vx", /*     001110HGGGGGccccc110DDDDD1010111 */ 0x38006057, 0xfc00707f, riscv_rvv_001110HGGGGGccccc110DDDDD1010111 },
	{ "vslide1down.vx", /*   001111HGGGGGccccc110DDDDD1010111 */ 0x3c006057, 0xfc00707f, riscv_rvv_001111HGGGGGccccc110DDDDD1010111 },
	{ "vdivu.vx", /*         100000HGGGGGccccc110DDDDD1010111 */ 0x80006057, 0xfc00707f, riscv_rvv_100000HGGGGGccccc110DDDDD1010111 },
	{ "vdiv.vx", /*          100001HGGGGGccccc110DDDDD1010111 */ 0x84006057, 0xfc00707f, riscv_rvv_100001HGGGGGccccc110DDDDD1010111 },
	{ "vremu.vx", /*         100010HGGGGGccccc110DDDDD1010111 */ 0x88006057, 0xfc00707f, riscv_rvv_100010HGGGGGccccc110DDDDD1010111 },
	{ "vrem.vx", /*          100011HGGGGGccccc110DDDDD1010111 */ 0x8c006057, 0xfc00707f, riscv_rvv_100011HGGGGGccccc110DDDDD1010111 },
	{ "vmulhu.vx", /*        100100HGGGGGccccc110DDDDD1010111 */ 0x90006057, 0xfc00707f, riscv_rvv_100100HGGGGGccccc110DDDDD1010111 },
	{ "vmul.vx", /*          100101HGGGGGccccc110DDDDD1010111 */ 0x94006057, 0xfc00707f, riscv_rvv_100101HGGGGGccccc110DDDDD1010111 },
	{ "vmulhsu.vx", /*       100110HGGGGGccccc110DDDDD1010111 */ 0x98006057, 0xfc00707f, riscv_rvv_100110HGGGGGccccc110DDDDD1010111 },
	{ "vmulh.vx", /*         100111HGGGGGccccc110DDDDD1010111 */ 0x9c006057, 0xfc00707f, riscv_rvv_100111HGGGGGccccc110DDDDD1010111 },
	{ "vmadd.vx", /*         101001HGGGGGccccc110DDDDD1010111 */ 0xa4006057, 0xfc00707f, riscv_rvv_101001HGGGGGccccc110DDDDD1010111 },
	{ "vnmsub.vx", /*        101011HGGGGGccccc110DDDDD1010111 */ 0xac006057, 0xfc00707f, riscv_rvv_101011HGGGGGccccc110DDDDD1010111 },
	{ "vmacc.vx", /*         101101HGGGGGccccc110DDDDD1010111 */ 0xb4006057, 0xfc00707f, riscv_rvv_101101HGGGGGccccc110DDDDD1010111 },
	{ "vnmsac.vx", /*        101111HGGGGGccccc110DDDDD1010111 */ 0xbc006057, 0xfc00707f, riscv_rvv_101111HGGGGGccccc110DDDDD1010111 },
	{ "vwaddu.vx", /*        110000HGGGGGccccc110DDDDD1010111 */ 0xc0006057, 0xfc00707f, riscv_rvv_110000HGGGGGccccc110DDDDD1010111 },
	{ "vwadd.vx", /*         110001HGGGGGccccc110DDDDD1010111 */ 0xc4006057, 0xfc00707f, riscv_rvv_110001HGGGGGccccc110DDDDD1010111 },
	{ "vwsubu.vx", /*        110010HGGGGGccccc110DDDDD1010111 */ 0xc8006057, 0xfc00707f, riscv_rvv_110010HGGGGGccccc110DDDDD1010111 },
	{ "vwsub.vx", /*         110011HGGGGGccccc110DDDDD1010111 */ 0xcc006057, 0xfc00707f, riscv_rvv_110011HGGGGGccccc110DDDDD1010111 },
	{ "vwaddu.wx", /*        110100HGGGGGccccc110DDDDD1010111 */ 0xd0006057, 0xfc00707f, riscv_rvv_110100HGGGGGccccc110DDDDD1010111 },
	{ "vwadd.wx", /*         110101HGGGGGccccc110DDDDD1010111 */ 0xd4006057, 0xfc00707f, riscv_rvv_110101HGGGGGccccc110DDDDD1010111 },
	{ "vwsubu.wx", /*        110110HGGGGGccccc110DDDDD1010111 */ 0xd8006057, 0xfc00707f, riscv_rvv_110110HGGGGGccccc110DDDDD1010111 },
	{ "vwsub.wx", /*         110111HGGGGGccccc110DDDDD1010111 */ 0xdc006057, 0xfc00707f, riscv_rvv_110111HGGGGGccccc110DDDDD1010111 },
	{ "vwmulu.vx", /*        111000HGGGGGccccc110DDDDD1010111 */ 0xe0006057, 0xfc00707f, riscv_rvv_111000HGGGGGccccc110DDDDD1010111 },
	{ "vwmulsu.vx", /*       111010HGGGGGccccc110DDDDD1010111 */ 0xe8006057, 0xfc00707f, riscv_rvv_111010HGGGGGccccc110DDDDD1010111 },
	{ "vwmul.vx", /*         111011HGGGGGccccc110DDDDD1010111 */ 0xec006057, 0xfc00707f, riscv_rvv_111011HGGGGGccccc110DDDDD1010111 },
	{ "vwmaccu.vx", /*       111100HGGGGGccccc110DDDDD1010111 */ 0xf0006057, 0xfc00707f, riscv_rvv_111100HGGGGGccccc110DDDDD1010111 },
	{ "vwmacc.vx", /*        111101HGGGGGccccc110DDDDD1010111 */ 0xf4006057, 0xfc00707f, riscv_rvv_111101HGGGGGccccc110DDDDD1010111 },
	{ "vwmaccus.vx", /*      111110HGGGGGccccc110DDDDD1010111 */ 0xf8006057, 0xfc00707f, riscv_rvv_111110HGGGGGccccc110DDDDD1010111 },
	{ "vwmaccsu.vx", /*      111111HGGGGGccccc110DDDDD1010111 */ 0xfc006057, 0xfc00707f, riscv_rvv_111111HGGGGGccccc110DDDDD1010111 },
	{ "vfredsum.vs", /*      000001HGGGGGFFFFF001DDDDD1010111 */ 0x04001057, 0xfc00707f, riscv_rvv_000001HGGGGGFFFFF001DDDDD1010111 },
	{ "vfwredsum.vs", /*     110001HGGGGGFFFFF001DDDDD1010111 */ 0xc4001057, 0xfc00707f, riscv_rvv_110001HGGGGGFFFFF001DDDDD1010111 },
	{ "vmornot.mm", /*       011100HGGGGGFFFFF010DDDDD1010111 */ 0x70002057, 0xfc00707f, riscv_rvv_011100HGGGGGFFFFF010DDDDD1010111 },
	{ "vmandnot.mm", /*      011000HGGGGGFFFFF010DDDDD1010111 */ 0x60002057, 0xfc00707f, riscv_rvv_011000HGGGGGFFFFF010DDDDD1010111 },
	{ "vfcvt.xu.f.v", /*     010010HGGGGG00000001DDDDD1010111 */ 0x48001057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00000001DDDDD1010111 },
	{ "vfcvt.x.f.v", /*      010010HGGGGG00001001DDDDD1010111 */ 0x48009057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00001001DDDDD1010111 },
	{ "vfcvt.f.xu.v", /*     010010HGGGGG00010001DDDDD1010111 */ 0x48011057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00010001DDDDD1010111 },
	{ "vfcvt.f.x.v", /*      010010HGGGGG00011001DDDDD1010111 */ 0x48019057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00011001DDDDD1010111 },
	{ "vfcvt.rtz.xu.f.v", /* 010010HGGGGG00110001DDDDD1010111 */ 0x48031057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00110001DDDDD1010111 },
	{ "vfcvt.rtz.x.f.v", /*  010010HGGGGG00111001DDDDD1010111 */ 0x48039057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00111001DDDDD1010111 },
	{ "vfwcvt.xu.f.v", /*    010010HGGGGG01000001DDDDD1010111 */ 0x48041057, 0xfc0ff07f, riscv_rvv_010010HGGGGG01000001DDDDD1010111 },
	{ "vfwcvt.x.f.v", /*     010010HGGGGG01001001DDDDD1010111 */ 0x48049057, 0xfc0ff07f, riscv_rvv_010010HGGGGG01001001DDDDD1010111 },
	{ "vfwcvt.f.xu.v", /*    010010HGGGGG01010001DDDDD1010111 */ 0x48051057, 0xfc0ff07f, riscv_rvv_010010HGGGGG01010001DDDDD1010111 },
	{ "vfwcvt.f.x.v", /*     010010HGGGGG01011001DDDDD1010111 */ 0x48059057, 0xfc0ff07f, riscv_rvv_010010HGGGGG01011001DDDDD1010111 },
	{ "vfwcvt.f.f.v", /*     010010HGGGGG01100001DDDDD1010111 */ 0x48061057, 0xfc0ff07f, riscv_rvv_010010HGGGGG01100001DDDDD1010111 },
	{ "vfwcvt.rtz.xu.f.v", /* 010010HGGGGG01110001DDDDD1010111 */ 0x48071057, 0xfc0ff07f, riscv_rvv_010010HGGGGG01110001DDDDD1010111 },
	{ "vfwcvt.rtz.x.f.v", /* 010010HGGGGG01111001DDDDD1010111 */ 0x48079057, 0xfc0ff07f, riscv_rvv_010010HGGGGG01111001DDDDD1010111 },
	{ "vfncvt.xu.f.w", /*    010010HGGGGG10000001DDDDD1010111 */ 0x48081057, 0xfc0ff07f, riscv_rvv_010010HGGGGG10000001DDDDD1010111 },
	{ "vfncvt.x.f.w", /*     010010HGGGGG10001001DDDDD1010111 */ 0x48089057, 0xfc0ff07f, riscv_rvv_010010HGGGGG10001001DDDDD1010111 },
	{ "vfncvt.f.xu.w", /*    010010HGGGGG10010001DDDDD1010111 */ 0x48091057, 0xfc0ff07f, riscv_rvv_010010HGGGGG10010001DDDDD1010111 },
	{ "vfncvt.f.x.w", /*     010010HGGGGG10011001DDDDD1010111 */ 0x48099057, 0xfc0ff07f, riscv_rvv_010010HGGGGG10011001DDDDD1010111 },
	{ "vfncvt.f.f.w", /*     010010HGGGGG10100001DDDDD1010111 */ 0x480a1057, 0xfc0ff07f, riscv_rvv_010010HGGGGG10100001DDDDD1010111 },
	{ "vfncvt.rod.f.f.w", /* 010010HGGGGG10101001DDDDD1010111 */ 0x480a9057, 0xfc0ff07f, riscv_rvv_010010HGGGGG10101001DDDDD1010111 },
	{ "vfncvt.rtz.xu.f.w", /* 010010HGGGGG10110001DDDDD1010111 */ 0x480b1057, 0xfc0ff07f, riscv_rvv_010010HGGGGG10110001DDDDD1010111 },
	{ "vfncvt.rtz.x.f.w", /* 010010HGGGGG10111001DDDDD1010111 */ 0x480b9057, 0xfc0ff07f, riscv_rvv_010010HGGGGG10111001DDDDD1010111 },
	{ "vfsqrt.v", /*         010011HGGGGG00000001DDDDD1010111 */ 0x4c001057, 0xfc0ff07f, riscv_rvv_010011HGGGGG00000001DDDDD1010111 },
	{ "vfrsqrt7.v", /*       010011HGGGGG00100001DDDDD1010111 */ 0x4c021057, 0xfc0ff07f, riscv_rvv_010011HGGGGG00100001DDDDD1010111 },
	{ "vfrec7.v", /*         010011HGGGGG00101001DDDDD1010111 */ 0x4c029057, 0xfc0ff07f, riscv_rvv_010011HGGGGG00101001DDDDD1010111 },
	{ "vfclass.v", /*        010011HGGGGG10000001DDDDD1010111 */ 0x4c081057, 0xfc0ff07f, riscv_rvv_010011HGGGGG10000001DDDDD1010111 },
	{ "vzext.vf8", /*        010010HGGGGG00010010DDDDD1010111 */ 0x48012057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00010010DDDDD1010111 },
	{ "vsext.vf8", /*        010010HGGGGG00011010DDDDD1010111 */ 0x4801a057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00011010DDDDD1010111 },
	{ "vzext.vf4", /*        010010HGGGGG00100010DDDDD1010111 */ 0x48022057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00100010DDDDD1010111 },
	{ "vsext.vf4", /*        010010HGGGGG00101010DDDDD1010111 */ 0x4802a057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00101010DDDDD1010111 },
	{ "vzext.vf2", /*        010010HGGGGG00110010DDDDD1010111 */ 0x48032057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00110010DDDDD1010111 },
	{ "vsext.vf2", /*        010010HGGGGG00111010DDDDD1010111 */ 0x4803a057, 0xfc0ff07f, riscv_rvv_010010HGGGGG00111010DDDDD1010111 },
	{ "vmsbf.m", /*          010100HGGGGG00001010DDDDD1010111 */ 0x5000a057, 0xfc0ff07f, riscv_rvv_010100HGGGGG00001010DDDDD1010111 },
	{ "vmsof.m", /*          010100HGGGGG00010010DDDDD1010111 */ 0x50012057, 0xfc0ff07f, riscv_rvv_010100HGGGGG00010010DDDDD1010111 },
	{ "vmsif.m", /*          010100HGGGGG00011010DDDDD1010111 */ 0x5001a057, 0xfc0ff07f, riscv_rvv_010100HGGGGG00011010DDDDD1010111 },
	{ "viota.m", /*          010100HGGGGG10000010DDDDD1010111 */ 0x50082057, 0xfc0ff07f, riscv_rvv_010100HGGGGG10000010DDDDD1010111 },
	{ "vcpop.m", /*          010000HGGGGG10000010aaaaa1010111 */ 0x40082057, 0xfc0ff07f, riscv_rvv_010000HGGGGG10000010aaaaa1010111 },
	{ "vfirst.m", /*         010000HGGGGG10001010aaaaa1010111 */ 0x4008a057, 0xfc0ff07f, riscv_rvv_010000HGGGGG10001010aaaaa1010111 },
	{ "vpopc.m", /*          010000HGGGGG10000010aaaaa1010111 */ 0x40082057, 0xfc0ff07f, riscv_rvv_010000HGGGGG10000010aaaaa1010111 },
	{ "vid.v", /*            010100H0000010001010DDDDD1010111 */ 0x5008a057, 0xfdfff07f, riscv_rvv_010100H0000010001010DDDDD1010111 },
	{ "fadd.d", /*           0000001dddddcccccjjjaaaaa1010011 */ 0x02000053, 0xfe00007f, riscv_rv32d_0000001dddddcccccjjjaaaaa1010011 },
	{ "fsub.d", /*           0000101dddddcccccjjjaaaaa1010011 */ 0x0a000053, 0xfe00007f, riscv_rv32d_0000101dddddcccccjjjaaaaa1010011 },
	{ "fmul.d", /*           0001001dddddcccccjjjaaaaa1010011 */ 0x12000053, 0xfe00007f, riscv_rv32d_0001001dddddcccccjjjaaaaa1010011 },
	{ "fdiv.d", /*           0001101dddddcccccjjjaaaaa1010011 */ 0x1a000053, 0xfe00007f, riscv_rv32d_0001101dddddcccccjjjaaaaa1010011 },
	{ "fadd.s", /*           0000000dddddcccccjjjaaaaa1010011 */ 0x00000053, 0xfe00007f, riscv_rv32f_0000000dddddcccccjjjaaaaa1010011 },
	{ "fsub.s", /*           0000100dddddcccccjjjaaaaa1010011 */ 0x08000053, 0xfe00007f, riscv_rv32f_0000100dddddcccccjjjaaaaa1010011 },
	{ "fmul.s", /*           0001000dddddcccccjjjaaaaa1010011 */ 0x10000053, 0xfe00007f, riscv_rv32f_0001000dddddcccccjjjaaaaa1010011 },
	{ "fdiv.s", /*           0001100dddddcccccjjjaaaaa1010011 */ 0x18000053, 0xfe00007f, riscv_rv32f_0001100dddddcccccjjjaaaaa1010011 },
	{ "fadd.q", /*           0000011dddddcccccjjjaaaaa1010011 */ 0x06000053, 0xfe00007f, riscv_rv32q_0000011dddddcccccjjjaaaaa1010011 },
	{ "fsub.q", /*           0000111dddddcccccjjjaaaaa1010011 */ 0x0e000053, 0xfe00007f, riscv_rv32q_0000111dddddcccccjjjaaaaa1010011 },
	{ "fmul.q", /*           0001011dddddcccccjjjaaaaa1010011 */ 0x16000053, 0xfe00007f, riscv_rv32q_0001011dddddcccccjjjaaaaa1010011 },
	{ "fdiv.q", /*           0001111dddddcccccjjjaaaaa1010011 */ 0x1e000053, 0xfe00007f, riscv_rv32q_0001111dddddcccccjjjaaaaa1010011 },
	{ "fadd.h", /*           0000010dddddcccccjjjaaaaa1010011 */ 0x04000053, 0xfe00007f, riscv_rv32zfh_0000010dddddcccccjjjaaaaa1010011 },
	{ "fsub.h", /*           0000110dddddcccccjjjaaaaa1010011 */ 0x0c000053, 0xfe00007f, riscv_rv32zfh_0000110dddddcccccjjjaaaaa1010011 },
	{ "fmul.h", /*           0001010dddddcccccjjjaaaaa1010011 */ 0x14000053, 0xfe00007f, riscv_rv32zfh_0001010dddddcccccjjjaaaaa1010011 },
	{ "fdiv.h", /*           0001110dddddcccccjjjaaaaa1010011 */ 0x1c000053, 0xfe00007f, riscv_rv32zfh_0001110dddddcccccjjjaaaaa1010011 },
	{ "slli.rv32", /*        0000000uuuuuccccc001aaaaa0010011 */ 0x00001013, 0xfe00707f, riscv_pseudo_0000000uuuuuccccc001aaaaa0010011 },
	{ "srli.rv32", /*        0000000uuuuuccccc101aaaaa0010011 */ 0x00005013, 0xfe00707f, riscv_pseudo_0000000uuuuuccccc101aaaaa0010011 },
	{ "srai.rv32", /*        0100000uuuuuccccc101aaaaa0010011 */ 0x40005013, 0xfe00707f, riscv_pseudo_0100000uuuuuccccc101aaaaa0010011 },
	{ "andn", /*             0100000dddddccccc111aaaaa0110011 */ 0x40007033, 0xfe00707f, riscv_rv32b_0100000dddddccccc111aaaaa0110011 },
	{ "orn", /*              0100000dddddccccc110aaaaa0110011 */ 0x40006033, 0xfe00707f, riscv_rv32b_0100000dddddccccc110aaaaa0110011 },
	{ "xnor", /*             0100000dddddccccc100aaaaa0110011 */ 0x40004033, 0xfe00707f, riscv_rv32b_0100000dddddccccc100aaaaa0110011 },
	{ "slo", /*              0010000dddddccccc001aaaaa0110011 */ 0x20001033, 0xfe00707f, riscv_rv32b_0010000dddddccccc001aaaaa0110011 },
	{ "sro", /*              0010000dddddccccc101aaaaa0110011 */ 0x20005033, 0xfe00707f, riscv_rv32b_0010000dddddccccc101aaaaa0110011 },
	{ "rol", /*              0110000dddddccccc001aaaaa0110011 */ 0x60001033, 0xfe00707f, riscv_rv32b_0110000dddddccccc001aaaaa0110011 },
	{ "ror", /*              0110000dddddccccc101aaaaa0110011 */ 0x60005033, 0xfe00707f, riscv_rv32b_0110000dddddccccc101aaaaa0110011 },
	{ "bclr", /*             0100100dddddccccc001aaaaa0110011 */ 0x48001033, 0xfe00707f, riscv_rv32b_0100100dddddccccc001aaaaa0110011 },
	{ "bset", /*             0010100dddddccccc001aaaaa0110011 */ 0x28001033, 0xfe00707f, riscv_rv32b_0010100dddddccccc001aaaaa0110011 },
	{ "binv", /*             0110100dddddccccc001aaaaa0110011 */ 0x68001033, 0xfe00707f, riscv_rv32b_0110100dddddccccc001aaaaa0110011 },
	{ "bext", /*             0100100dddddccccc101aaaaa0110011 */ 0x48005033, 0xfe00707f, riscv_rv32b_0100100dddddccccc101aaaaa0110011 },
	{ "gorc", /*             0010100dddddccccc101aaaaa0110011 */ 0x28005033, 0xfe00707f, riscv_rv32b_0010100dddddccccc101aaaaa0110011 },
	{ "grev", /*             0110100dddddccccc101aaaaa0110011 */ 0x68005033, 0xfe00707f, riscv_rv32b_0110100dddddccccc101aaaaa0110011 },
	{ "sh1add", /*           0010000dddddccccc010aaaaa0110011 */ 0x20002033, 0xfe00707f, riscv_rv32b_0010000dddddccccc010aaaaa0110011 },
	{ "sh2add", /*           0010000dddddccccc100aaaaa0110011 */ 0x20004033, 0xfe00707f, riscv_rv32b_0010000dddddccccc100aaaaa0110011 },
	{ "sh3add", /*           0010000dddddccccc110aaaaa0110011 */ 0x20006033, 0xfe00707f, riscv_rv32b_0010000dddddccccc110aaaaa0110011 },
	{ "clmul", /*            0000101dddddccccc001aaaaa0110011 */ 0x0a001033, 0xfe00707f, riscv_rv32b_0000101dddddccccc001aaaaa0110011 },
	{ "clmulr", /*           0000101dddddccccc010aaaaa0110011 */ 0x0a002033, 0xfe00707f, riscv_rv32b_0000101dddddccccc010aaaaa0110011 },
	{ "clmulh", /*           0000101dddddccccc011aaaaa0110011 */ 0x0a003033, 0xfe00707f, riscv_rv32b_0000101dddddccccc011aaaaa0110011 },
	{ "min", /*              0000101dddddccccc100aaaaa0110011 */ 0x0a004033, 0xfe00707f, riscv_rv32b_0000101dddddccccc100aaaaa0110011 },
	{ "minu", /*             0000101dddddccccc101aaaaa0110011 */ 0x0a005033, 0xfe00707f, riscv_rv32b_0000101dddddccccc101aaaaa0110011 },
	{ "max", /*              0000101dddddccccc110aaaaa0110011 */ 0x0a006033, 0xfe00707f, riscv_rv32b_0000101dddddccccc110aaaaa0110011 },
	{ "maxu", /*             0000101dddddccccc111aaaaa0110011 */ 0x0a007033, 0xfe00707f, riscv_rv32b_0000101dddddccccc111aaaaa0110011 },
	{ "shfl", /*             0000100dddddccccc001aaaaa0110011 */ 0x08001033, 0xfe00707f, riscv_rv32b_0000100dddddccccc001aaaaa0110011 },
	{ "unshfl", /*           0000100dddddccccc101aaaaa0110011 */ 0x08005033, 0xfe00707f, riscv_rv32b_0000100dddddccccc101aaaaa0110011 },
	{ "bcompress", /*        0000100dddddccccc110aaaaa0110011 */ 0x08006033, 0xfe00707f, riscv_rv32b_0000100dddddccccc110aaaaa0110011 },
	{ "bdecompress", /*      0100100dddddccccc110aaaaa0110011 */ 0x48006033, 0xfe00707f, riscv_rv32b_0100100dddddccccc110aaaaa0110011 },
	{ "pack", /*             0000100dddddccccc100aaaaa0110011 */ 0x08004033, 0xfe00707f, riscv_rv32b_0000100dddddccccc100aaaaa0110011 },
	{ "packu", /*            0100100dddddccccc100aaaaa0110011 */ 0x48004033, 0xfe00707f, riscv_rv32b_0100100dddddccccc100aaaaa0110011 },
	{ "packh", /*            0000100dddddccccc111aaaaa0110011 */ 0x08007033, 0xfe00707f, riscv_rv32b_0000100dddddccccc111aaaaa0110011 },
	{ "bfp", /*              0100100dddddccccc111aaaaa0110011 */ 0x48007033, 0xfe00707f, riscv_rv32b_0100100dddddccccc111aaaaa0110011 },
	{ "shfli", /*            0000100uuuuuccccc001aaaaa0010011 */ 0x08001013, 0xfe00707f, riscv_rv32b_0000100uuuuuccccc001aaaaa0010011 },
	{ "unshfli", /*          0000100uuuuuccccc101aaaaa0010011 */ 0x08005013, 0xfe00707f, riscv_rv32b_0000100uuuuuccccc101aaaaa0010011 },
	{ "xperm4", /*           0010100dddddccccc010aaaaa0110011 */ 0x28002033, 0xfe00707f, riscv_rv32b_0010100dddddccccc010aaaaa0110011 },
	{ "xperm8", /*           0010100dddddccccc100aaaaa0110011 */ 0x28004033, 0xfe00707f, riscv_rv32b_0010100dddddccccc100aaaaa0110011 },
	{ "xperm16", /*          0010100dddddccccc110aaaaa0110011 */ 0x28006033, 0xfe00707f, riscv_rv32b_0010100dddddccccc110aaaaa0110011 },
	{ "fsgnj.d", /*          0010001dddddccccc000aaaaa1010011 */ 0x22000053, 0xfe00707f, riscv_rv32d_0010001dddddccccc000aaaaa1010011 },
	{ "fsgnjn.d", /*         0010001dddddccccc001aaaaa1010011 */ 0x22001053, 0xfe00707f, riscv_rv32d_0010001dddddccccc001aaaaa1010011 },
	{ "fsgnjx.d", /*         0010001dddddccccc010aaaaa1010011 */ 0x22002053, 0xfe00707f, riscv_rv32d_0010001dddddccccc010aaaaa1010011 },
	{ "fmin.d", /*           0010101dddddccccc000aaaaa1010011 */ 0x2a000053, 0xfe00707f, riscv_rv32d_0010101dddddccccc000aaaaa1010011 },
	{ "fmax.d", /*           0010101dddddccccc001aaaaa1010011 */ 0x2a001053, 0xfe00707f, riscv_rv32d_0010101dddddccccc001aaaaa1010011 },
	{ "fle.d", /*            1010001dddddccccc000aaaaa1010011 */ 0xa2000053, 0xfe00707f, riscv_rv32d_1010001dddddccccc000aaaaa1010011 },
	{ "flt.d", /*            1010001dddddccccc001aaaaa1010011 */ 0xa2001053, 0xfe00707f, riscv_rv32d_1010001dddddccccc001aaaaa1010011 },
	{ "feq.d", /*            1010001dddddccccc010aaaaa1010011 */ 0xa2002053, 0xfe00707f, riscv_rv32d_1010001dddddccccc010aaaaa1010011 },
	{ "fsgnj.s", /*          0010000dddddccccc000aaaaa1010011 */ 0x20000053, 0xfe00707f, riscv_rv32f_0010000dddddccccc000aaaaa1010011 },
	{ "fsgnjn.s", /*         0010000dddddccccc001aaaaa1010011 */ 0x20001053, 0xfe00707f, riscv_rv32f_0010000dddddccccc001aaaaa1010011 },
	{ "fsgnjx.s", /*         0010000dddddccccc010aaaaa1010011 */ 0x20002053, 0xfe00707f, riscv_rv32f_0010000dddddccccc010aaaaa1010011 },
	{ "fmin.s", /*           0010100dddddccccc000aaaaa1010011 */ 0x28000053, 0xfe00707f, riscv_rv32f_0010100dddddccccc000aaaaa1010011 },
	{ "fmax.s", /*           0010100dddddccccc001aaaaa1010011 */ 0x28001053, 0xfe00707f, riscv_rv32f_0010100dddddccccc001aaaaa1010011 },
	{ "fle.s", /*            1010000dddddccccc000aaaaa1010011 */ 0xa0000053, 0xfe00707f, riscv_rv32f_1010000dddddccccc000aaaaa1010011 },
	{ "flt.s", /*            1010000dddddccccc001aaaaa1010011 */ 0xa0001053, 0xfe00707f, riscv_rv32f_1010000dddddccccc001aaaaa1010011 },
	{ "feq.s", /*            1010000dddddccccc010aaaaa1010011 */ 0xa0002053, 0xfe00707f, riscv_rv32f_1010000dddddccccc010aaaaa1010011 },
	{ "add", /*              0000000dddddccccc000aaaaa0110011 */ 0x00000033, 0xfe00707f, riscv_rv32i_0000000dddddccccc000aaaaa0110011 },
	{ "sub", /*              0100000dddddccccc000aaaaa0110011 */ 0x40000033, 0xfe00707f, riscv_rv32i_0100000dddddccccc000aaaaa0110011 },
	{ "sll", /*              0000000dddddccccc001aaaaa0110011 */ 0x00001033, 0xfe00707f, riscv_rv32i_0000000dddddccccc001aaaaa0110011 },
	{ "slt", /*              0000000dddddccccc010aaaaa0110011 */ 0x00002033, 0xfe00707f, riscv_rv32i_0000000dddddccccc010aaaaa0110011 },
	{ "sltu", /*             0000000dddddccccc011aaaaa0110011 */ 0x00003033, 0xfe00707f, riscv_rv32i_0000000dddddccccc011aaaaa0110011 },
	{ "xor", /*              0000000dddddccccc100aaaaa0110011 */ 0x00004033, 0xfe00707f, riscv_rv32i_0000000dddddccccc100aaaaa0110011 },
	{ "srl", /*              0000000dddddccccc101aaaaa0110011 */ 0x00005033, 0xfe00707f, riscv_rv32i_0000000dddddccccc101aaaaa0110011 },
	{ "sra", /*              0100000dddddccccc101aaaaa0110011 */ 0x40005033, 0xfe00707f, riscv_rv32i_0100000dddddccccc101aaaaa0110011 },
	{ "or", /*               0000000dddddccccc110aaaaa0110011 */ 0x00006033, 0xfe00707f, riscv_rv32i_0000000dddddccccc110aaaaa0110011 },
	{ "and", /*              0000000dddddccccc111aaaaa0110011 */ 0x00007033, 0xfe00707f, riscv_rv32i_0000000dddddccccc111aaaaa0110011 },
	{ "sha512sum0r", /*      0101000dddddccccc000aaaaa0110011 */ 0x50000033, 0xfe00707f, riscv_rv32k_0101000dddddccccc000aaaaa0110011 },
	{ "sha512sum1r", /*      0101001dddddccccc000aaaaa0110011 */ 0x52000033, 0xfe00707f, riscv_rv32k_0101001dddddccccc000aaaaa0110011 },
	{ "sha512sig0l", /*      0101010dddddccccc000aaaaa0110011 */ 0x54000033, 0xfe00707f, riscv_rv32k_0101010dddddccccc000aaaaa0110011 },
	{ "sha512sig0h", /*      0101110dddddccccc000aaaaa0110011 */ 0x5c000033, 0xfe00707f, riscv_rv32k_0101110dddddccccc000aaaaa0110011 },
	{ "sha512sig1l", /*      0101011dddddccccc000aaaaa0110011 */ 0x56000033, 0xfe00707f, riscv_rv32k_0101011dddddccccc000aaaaa0110011 },
	{ "sha512sig1h", /*      0101111dddddccccc000aaaaa0110011 */ 0x5e000033, 0xfe00707f, riscv_rv32k_0101111dddddccccc000aaaaa0110011 },
	{ "mul", /*              0000001dddddccccc000aaaaa0110011 */ 0x02000033, 0xfe00707f, riscv_rv32m_0000001dddddccccc000aaaaa0110011 },
	{ "mulh", /*             0000001dddddccccc001aaaaa0110011 */ 0x02001033, 0xfe00707f, riscv_rv32m_0000001dddddccccc001aaaaa0110011 },
	{ "mulhsu", /*           0000001dddddccccc010aaaaa0110011 */ 0x02002033, 0xfe00707f, riscv_rv32m_0000001dddddccccc010aaaaa0110011 },
	{ "mulhu", /*            0000001dddddccccc011aaaaa0110011 */ 0x02003033, 0xfe00707f, riscv_rv32m_0000001dddddccccc011aaaaa0110011 },
	{ "div", /*              0000001dddddccccc100aaaaa0110011 */ 0x02004033, 0xfe00707f, riscv_rv32m_0000001dddddccccc100aaaaa0110011 },
	{ "divu", /*             0000001dddddccccc101aaaaa0110011 */ 0x02005033, 0xfe00707f, riscv_rv32m_0000001dddddccccc101aaaaa0110011 },
	{ "rem", /*              0000001dddddccccc110aaaaa0110011 */ 0x02006033, 0xfe00707f, riscv_rv32m_0000001dddddccccc110aaaaa0110011 },
	{ "remu", /*             0000001dddddccccc111aaaaa0110011 */ 0x02007033, 0xfe00707f, riscv_rv32m_0000001dddddccccc111aaaaa0110011 },
	{ "fsgnj.q", /*          0010011dddddccccc000aaaaa1010011 */ 0x26000053, 0xfe00707f, riscv_rv32q_0010011dddddccccc000aaaaa1010011 },
	{ "fsgnjn.q", /*         0010011dddddccccc001aaaaa1010011 */ 0x26001053, 0xfe00707f, riscv_rv32q_0010011dddddccccc001aaaaa1010011 },
	{ "fsgnjx.q", /*         0010011dddddccccc010aaaaa1010011 */ 0x26002053, 0xfe00707f, riscv_rv32q_0010011dddddccccc010aaaaa1010011 },
	{ "fmin.q", /*           0010111dddddccccc000aaaaa1010011 */ 0x2e000053, 0xfe00707f, riscv_rv32q_0010111dddddccccc000aaaaa1010011 },
	{ "fmax.q", /*           0010111dddddccccc001aaaaa1010011 */ 0x2e001053, 0xfe00707f, riscv_rv32q_0010111dddddccccc001aaaaa1010011 },
	{ "fle.q", /*            1010011dddddccccc000aaaaa1010011 */ 0xa6000053, 0xfe00707f, riscv_rv32q_1010011dddddccccc000aaaaa1010011 },
	{ "flt.q", /*            1010011dddddccccc001aaaaa1010011 */ 0xa6001053, 0xfe00707f, riscv_rv32q_1010011dddddccccc001aaaaa1010011 },
	{ "feq.q", /*            1010011dddddccccc010aaaaa1010011 */ 0xa6002053, 0xfe00707f, riscv_rv32q_1010011dddddccccc010aaaaa1010011 },
	{ "fsgnj.h", /*          0010010dddddccccc000aaaaa1010011 */ 0x24000053, 0xfe00707f, riscv_rv32zfh_0010010dddddccccc000aaaaa1010011 },
	{ "fsgnjn.h", /*         0010010dddddccccc001aaaaa1010011 */ 0x24001053, 0xfe00707f, riscv_rv32zfh_0010010dddddccccc001aaaaa1010011 },
	{ "fsgnjx.h", /*         0010010dddddccccc010aaaaa1010011 */ 0x24002053, 0xfe00707f, riscv_rv32zfh_0010010dddddccccc010aaaaa1010011 },
	{ "fmin.h", /*           0010110dddddccccc000aaaaa1010011 */ 0x2c000053, 0xfe00707f, riscv_rv32zfh_0010110dddddccccc000aaaaa1010011 },
	{ "fmax.h", /*           0010110dddddccccc001aaaaa1010011 */ 0x2c001053, 0xfe00707f, riscv_rv32zfh_0010110dddddccccc001aaaaa1010011 },
	{ "fle.h", /*            1010010dddddccccc000aaaaa1010011 */ 0xa4000053, 0xfe00707f, riscv_rv32zfh_1010010dddddccccc000aaaaa1010011 },
	{ "flt.h", /*            1010010dddddccccc001aaaaa1010011 */ 0xa4001053, 0xfe00707f, riscv_rv32zfh_1010010dddddccccc001aaaaa1010011 },
	{ "feq.h", /*            1010010dddddccccc010aaaaa1010011 */ 0xa4002053, 0xfe00707f, riscv_rv32zfh_1010010dddddccccc010aaaaa1010011 },
	{ "bmator", /*           0000100dddddccccc011aaaaa0110011 */ 0x08003033, 0xfe00707f, riscv_rv64b_0000100dddddccccc011aaaaa0110011 },
	{ "bmatxor", /*          0100100dddddccccc011aaaaa0110011 */ 0x48003033, 0xfe00707f, riscv_rv64b_0100100dddddccccc011aaaaa0110011 },
	{ "add.uw", /*           0000100dddddccccc000aaaaa0111011 */ 0x0800003b, 0xfe00707f, riscv_rv64b_0000100dddddccccc000aaaaa0111011 },
	{ "slow", /*             0010000dddddccccc001aaaaa0111011 */ 0x2000103b, 0xfe00707f, riscv_rv64b_0010000dddddccccc001aaaaa0111011 },
	{ "srow", /*             0010000dddddccccc101aaaaa0111011 */ 0x2000503b, 0xfe00707f, riscv_rv64b_0010000dddddccccc101aaaaa0111011 },
	{ "rolw", /*             0110000dddddccccc001aaaaa0111011 */ 0x6000103b, 0xfe00707f, riscv_rv64b_0110000dddddccccc001aaaaa0111011 },
	{ "rorw", /*             0110000dddddccccc101aaaaa0111011 */ 0x6000503b, 0xfe00707f, riscv_rv64b_0110000dddddccccc101aaaaa0111011 },
	{ "gorcw", /*            0010100dddddccccc101aaaaa0111011 */ 0x2800503b, 0xfe00707f, riscv_rv64b_0010100dddddccccc101aaaaa0111011 },
	{ "grevw", /*            0110100dddddccccc101aaaaa0111011 */ 0x6800503b, 0xfe00707f, riscv_rv64b_0110100dddddccccc101aaaaa0111011 },
	{ "sloiw", /*            0010000uuuuuccccc001aaaaa0011011 */ 0x2000101b, 0xfe00707f, riscv_rv64b_0010000uuuuuccccc001aaaaa0011011 },
	{ "sroiw", /*            0010000uuuuuccccc101aaaaa0011011 */ 0x2000501b, 0xfe00707f, riscv_rv64b_0010000uuuuuccccc101aaaaa0011011 },
	{ "roriw", /*            0110000uuuuuccccc101aaaaa0011011 */ 0x6000501b, 0xfe00707f, riscv_rv64b_0110000uuuuuccccc101aaaaa0011011 },
	{ "gorciw", /*           0010100uuuuuccccc101aaaaa0011011 */ 0x2800501b, 0xfe00707f, riscv_rv64b_0010100uuuuuccccc101aaaaa0011011 },
	{ "greviw", /*           0110100uuuuuccccc101aaaaa0011011 */ 0x6800501b, 0xfe00707f, riscv_rv64b_0110100uuuuuccccc101aaaaa0011011 },
	{ "sh1add.uw", /*        0010000dddddccccc010aaaaa0111011 */ 0x2000203b, 0xfe00707f, riscv_rv64b_0010000dddddccccc010aaaaa0111011 },
	{ "sh2add.uw", /*        0010000dddddccccc100aaaaa0111011 */ 0x2000403b, 0xfe00707f, riscv_rv64b_0010000dddddccccc100aaaaa0111011 },
	{ "sh3add.uw", /*        0010000dddddccccc110aaaaa0111011 */ 0x2000603b, 0xfe00707f, riscv_rv64b_0010000dddddccccc110aaaaa0111011 },
	{ "shflw", /*            0000100dddddccccc001aaaaa0111011 */ 0x0800103b, 0xfe00707f, riscv_rv64b_0000100dddddccccc001aaaaa0111011 },
	{ "unshflw", /*          0000100dddddccccc101aaaaa0111011 */ 0x0800503b, 0xfe00707f, riscv_rv64b_0000100dddddccccc101aaaaa0111011 },
	{ "bcompressw", /*       0000100dddddccccc110aaaaa0111011 */ 0x0800603b, 0xfe00707f, riscv_rv64b_0000100dddddccccc110aaaaa0111011 },
	{ "bdecompressw", /*     0100100dddddccccc110aaaaa0111011 */ 0x4800603b, 0xfe00707f, riscv_rv64b_0100100dddddccccc110aaaaa0111011 },
	{ "packw", /*            0000100dddddccccc100aaaaa0111011 */ 0x0800403b, 0xfe00707f, riscv_rv64b_0000100dddddccccc100aaaaa0111011 },
	{ "packuw", /*           0100100dddddccccc100aaaaa0111011 */ 0x4800403b, 0xfe00707f, riscv_rv64b_0100100dddddccccc100aaaaa0111011 },
	{ "bfpw", /*             0100100dddddccccc111aaaaa0111011 */ 0x4800703b, 0xfe00707f, riscv_rv64b_0100100dddddccccc111aaaaa0111011 },
	{ "xperm32", /*          0010100dddddccccc000aaaaa0110011 */ 0x28000033, 0xfe00707f, riscv_rv64b_0010100dddddccccc000aaaaa0110011 },
	{ "slliw", /*            0000000uuuuuccccc001aaaaa0011011 */ 0x0000101b, 0xfe00707f, riscv_rv64i_0000000uuuuuccccc001aaaaa0011011 },
	{ "srliw", /*            0000000uuuuuccccc101aaaaa0011011 */ 0x0000501b, 0xfe00707f, riscv_rv64i_0000000uuuuuccccc101aaaaa0011011 },
	{ "sraiw", /*            0100000uuuuuccccc101aaaaa0011011 */ 0x4000501b, 0xfe00707f, riscv_rv64i_0100000uuuuuccccc101aaaaa0011011 },
	{ "addw", /*             0000000dddddccccc000aaaaa0111011 */ 0x0000003b, 0xfe00707f, riscv_rv64i_0000000dddddccccc000aaaaa0111011 },
	{ "subw", /*             0100000dddddccccc000aaaaa0111011 */ 0x4000003b, 0xfe00707f, riscv_rv64i_0100000dddddccccc000aaaaa0111011 },
	{ "sllw", /*             0000000dddddccccc001aaaaa0111011 */ 0x0000103b, 0xfe00707f, riscv_rv64i_0000000dddddccccc001aaaaa0111011 },
	{ "srlw", /*             0000000dddddccccc101aaaaa0111011 */ 0x0000503b, 0xfe00707f, riscv_rv64i_0000000dddddccccc101aaaaa0111011 },
	{ "sraw", /*             0100000dddddccccc101aaaaa0111011 */ 0x4000503b, 0xfe00707f, riscv_rv64i_0100000dddddccccc101aaaaa0111011 },
	{ "aes64ks2", /*         0111111dddddccccc000aaaaa0110011 */ 0x7e000033, 0xfe00707f, riscv_rv64k_0111111dddddccccc000aaaaa0110011 },
	{ "aes64esm", /*         0011011dddddccccc000aaaaa0110011 */ 0x36000033, 0xfe00707f, riscv_rv64k_0011011dddddccccc000aaaaa0110011 },
	{ "aes64es", /*          0011001dddddccccc000aaaaa0110011 */ 0x32000033, 0xfe00707f, riscv_rv64k_0011001dddddccccc000aaaaa0110011 },
	{ "aes64dsm", /*         0011111dddddccccc000aaaaa0110011 */ 0x3e000033, 0xfe00707f, riscv_rv64k_0011111dddddccccc000aaaaa0110011 },
	{ "aes64ds", /*          0011101dddddccccc000aaaaa0110011 */ 0x3a000033, 0xfe00707f, riscv_rv64k_0011101dddddccccc000aaaaa0110011 },
	{ "mulw", /*             0000001dddddccccc000aaaaa0111011 */ 0x0200003b, 0xfe00707f, riscv_rv64m_0000001dddddccccc000aaaaa0111011 },
	{ "divw", /*             0000001dddddccccc100aaaaa0111011 */ 0x0200403b, 0xfe00707f, riscv_rv64m_0000001dddddccccc100aaaaa0111011 },
	{ "divuw", /*            0000001dddddccccc101aaaaa0111011 */ 0x0200503b, 0xfe00707f, riscv_rv64m_0000001dddddccccc101aaaaa0111011 },
	{ "remw", /*             0000001dddddccccc110aaaaa0111011 */ 0x0200603b, 0xfe00707f, riscv_rv64m_0000001dddddccccc110aaaaa0111011 },
	{ "remuw", /*            0000001dddddccccc111aaaaa0111011 */ 0x0200703b, 0xfe00707f, riscv_rv64m_0000001dddddccccc111aaaaa0111011 },
	{ "add8", /*             0100100dddddccccc000aaaaa1110111 */ 0x48000077, 0xfe00707f, riscv_rvp_0100100dddddccccc000aaaaa1110111 },
	{ "add16", /*            0100000dddddccccc000aaaaa1110111 */ 0x40000077, 0xfe00707f, riscv_rvp_0100000dddddccccc000aaaaa1110111 },
	{ "add64", /*            1100000dddddccccc001aaaaa1110111 */ 0xc0001077, 0xfe00707f, riscv_rvp_1100000dddddccccc001aaaaa1110111 },
	{ "ave", /*              1110000dddddccccc000aaaaa1110111 */ 0xe0000077, 0xfe00707f, riscv_rvp_1110000dddddccccc000aaaaa1110111 },
	{ "bitrev", /*           1110011dddddccccc000aaaaa1110111 */ 0xe6000077, 0xfe00707f, riscv_rvp_1110011dddddccccc000aaaaa1110111 },
	{ "cmpeq8", /*           0100111dddddccccc000aaaaa1110111 */ 0x4e000077, 0xfe00707f, riscv_rvp_0100111dddddccccc000aaaaa1110111 },
	{ "cmpeq16", /*          0100110dddddccccc000aaaaa1110111 */ 0x4c000077, 0xfe00707f, riscv_rvp_0100110dddddccccc000aaaaa1110111 },
	{ "cras16", /*           0100010dddddccccc000aaaaa1110111 */ 0x44000077, 0xfe00707f, riscv_rvp_0100010dddddccccc000aaaaa1110111 },
	{ "crsa16", /*           0100011dddddccccc000aaaaa1110111 */ 0x46000077, 0xfe00707f, riscv_rvp_0100011dddddccccc000aaaaa1110111 },
	{ "kadd8", /*            0001100dddddccccc000aaaaa1110111 */ 0x18000077, 0xfe00707f, riscv_rvp_0001100dddddccccc000aaaaa1110111 },
	{ "kadd16", /*           0001000dddddccccc000aaaaa1110111 */ 0x10000077, 0xfe00707f, riscv_rvp_0001000dddddccccc000aaaaa1110111 },
	{ "kadd64", /*           1001000dddddccccc001aaaaa1110111 */ 0x90001077, 0xfe00707f, riscv_rvp_1001000dddddccccc001aaaaa1110111 },
	{ "kaddh", /*            0000010dddddccccc001aaaaa1110111 */ 0x04001077, 0xfe00707f, riscv_rvp_0000010dddddccccc001aaaaa1110111 },
	{ "kaddw", /*            0000000dddddccccc001aaaaa1110111 */ 0x00001077, 0xfe00707f, riscv_rvp_0000000dddddccccc001aaaaa1110111 },
	{ "kcras16", /*          0001010dddddccccc000aaaaa1110111 */ 0x14000077, 0xfe00707f, riscv_rvp_0001010dddddccccc000aaaaa1110111 },
	{ "kcrsa16", /*          0001011dddddccccc000aaaaa1110111 */ 0x16000077, 0xfe00707f, riscv_rvp_0001011dddddccccc000aaaaa1110111 },
	{ "kdmbb", /*            0000101dddddccccc001aaaaa1110111 */ 0x0a001077, 0xfe00707f, riscv_rvp_0000101dddddccccc001aaaaa1110111 },
	{ "kdmbt", /*            0001101dddddccccc001aaaaa1110111 */ 0x1a001077, 0xfe00707f, riscv_rvp_0001101dddddccccc001aaaaa1110111 },
	{ "kdmtt", /*            0010101dddddccccc001aaaaa1110111 */ 0x2a001077, 0xfe00707f, riscv_rvp_0010101dddddccccc001aaaaa1110111 },
	{ "kdmabb", /*           1101001dddddccccc001aaaaa1110111 */ 0xd2001077, 0xfe00707f, riscv_rvp_1101001dddddccccc001aaaaa1110111 },
	{ "kdmabt", /*           1110001dddddccccc001aaaaa1110111 */ 0xe2001077, 0xfe00707f, riscv_rvp_1110001dddddccccc001aaaaa1110111 },
	{ "kdmatt", /*           1111001dddddccccc001aaaaa1110111 */ 0xf2001077, 0xfe00707f, riscv_rvp_1111001dddddccccc001aaaaa1110111 },
	{ "khm8", /*             1000111dddddccccc000aaaaa1110111 */ 0x8e000077, 0xfe00707f, riscv_rvp_1000111dddddccccc000aaaaa1110111 },
	{ "khmx8", /*            1001111dddddccccc000aaaaa1110111 */ 0x9e000077, 0xfe00707f, riscv_rvp_1001111dddddccccc000aaaaa1110111 },
	{ "khm16", /*            1000011dddddccccc000aaaaa1110111 */ 0x86000077, 0xfe00707f, riscv_rvp_1000011dddddccccc000aaaaa1110111 },
	{ "khmx16", /*           1001011dddddccccc000aaaaa1110111 */ 0x96000077, 0xfe00707f, riscv_rvp_1001011dddddccccc000aaaaa1110111 },
	{ "khmbb", /*            0000110dddddccccc001aaaaa1110111 */ 0x0c001077, 0xfe00707f, riscv_rvp_0000110dddddccccc001aaaaa1110111 },
	{ "khmbt", /*            0001110dddddccccc001aaaaa1110111 */ 0x1c001077, 0xfe00707f, riscv_rvp_0001110dddddccccc001aaaaa1110111 },
	{ "khmtt", /*            0010110dddddccccc001aaaaa1110111 */ 0x2c001077, 0xfe00707f, riscv_rvp_0010110dddddccccc001aaaaa1110111 },
	{ "kmabb", /*            0101101dddddccccc001aaaaa1110111 */ 0x5a001077, 0xfe00707f, riscv_rvp_0101101dddddccccc001aaaaa1110111 },
	{ "kmabt", /*            0110101dddddccccc001aaaaa1110111 */ 0x6a001077, 0xfe00707f, riscv_rvp_0110101dddddccccc001aaaaa1110111 },
	{ "kmatt", /*            0111101dddddccccc001aaaaa1110111 */ 0x7a001077, 0xfe00707f, riscv_rvp_0111101dddddccccc001aaaaa1110111 },
	{ "kmada", /*            0100100dddddccccc001aaaaa1110111 */ 0x48001077, 0xfe00707f, riscv_rvp_0100100dddddccccc001aaaaa1110111 },
	{ "kmaxda", /*           0100101dddddccccc001aaaaa1110111 */ 0x4a001077, 0xfe00707f, riscv_rvp_0100101dddddccccc001aaaaa1110111 },
	{ "kmads", /*            0101110dddddccccc001aaaaa1110111 */ 0x5c001077, 0xfe00707f, riscv_rvp_0101110dddddccccc001aaaaa1110111 },
	{ "kmadrs", /*           0110110dddddccccc001aaaaa1110111 */ 0x6c001077, 0xfe00707f, riscv_rvp_0110110dddddccccc001aaaaa1110111 },
	{ "kmaxds", /*           0111110dddddccccc001aaaaa1110111 */ 0x7c001077, 0xfe00707f, riscv_rvp_0111110dddddccccc001aaaaa1110111 },
	{ "kmar64", /*           1001010dddddccccc001aaaaa1110111 */ 0x94001077, 0xfe00707f, riscv_rvp_1001010dddddccccc001aaaaa1110111 },
	{ "kmda", /*             0011100dddddccccc001aaaaa1110111 */ 0x38001077, 0xfe00707f, riscv_rvp_0011100dddddccccc001aaaaa1110111 },
	{ "kmxda", /*            0011101dddddccccc001aaaaa1110111 */ 0x3a001077, 0xfe00707f, riscv_rvp_0011101dddddccccc001aaaaa1110111 },
	{ "kmmac", /*            0110000dddddccccc001aaaaa1110111 */ 0x60001077, 0xfe00707f, riscv_rvp_0110000dddddccccc001aaaaa1110111 },
	{ "kmmac.u", /*          0111000dddddccccc001aaaaa1110111 */ 0x70001077, 0xfe00707f, riscv_rvp_0111000dddddccccc001aaaaa1110111 },
	{ "kmmawb", /*           0100011dddddccccc001aaaaa1110111 */ 0x46001077, 0xfe00707f, riscv_rvp_0100011dddddccccc001aaaaa1110111 },
	{ "kmmawb.u", /*         0101011dddddccccc001aaaaa1110111 */ 0x56001077, 0xfe00707f, riscv_rvp_0101011dddddccccc001aaaaa1110111 },
	{ "kmmawb2", /*          1100111dddddccccc001aaaaa1110111 */ 0xce001077, 0xfe00707f, riscv_rvp_1100111dddddccccc001aaaaa1110111 },
	{ "kmmawb2.u", /*        1101111dddddccccc001aaaaa1110111 */ 0xde001077, 0xfe00707f, riscv_rvp_1101111dddddccccc001aaaaa1110111 },
	{ "kmmawt", /*           0110011dddddccccc001aaaaa1110111 */ 0x66001077, 0xfe00707f, riscv_rvp_0110011dddddccccc001aaaaa1110111 },
	{ "kmmawt.u", /*         0111011dddddccccc001aaaaa1110111 */ 0x76001077, 0xfe00707f, riscv_rvp_0111011dddddccccc001aaaaa1110111 },
	{ "kmmawt2", /*          1110111dddddccccc001aaaaa1110111 */ 0xee001077, 0xfe00707f, riscv_rvp_1110111dddddccccc001aaaaa1110111 },
	{ "kmmawt2.u", /*        1111111dddddccccc001aaaaa1110111 */ 0xfe001077, 0xfe00707f, riscv_rvp_1111111dddddccccc001aaaaa1110111 },
	{ "kmmsb", /*            0100001dddddccccc001aaaaa1110111 */ 0x42001077, 0xfe00707f, riscv_rvp_0100001dddddccccc001aaaaa1110111 },
	{ "kmmsb.u", /*          0101001dddddccccc001aaaaa1110111 */ 0x52001077, 0xfe00707f, riscv_rvp_0101001dddddccccc001aaaaa1110111 },
	{ "kmmwb2", /*           1000111dddddccccc001aaaaa1110111 */ 0x8e001077, 0xfe00707f, riscv_rvp_1000111dddddccccc001aaaaa1110111 },
	{ "kmmwb2.u", /*         1001111dddddccccc001aaaaa1110111 */ 0x9e001077, 0xfe00707f, riscv_rvp_1001111dddddccccc001aaaaa1110111 },
	{ "kmmwt2", /*           1010111dddddccccc001aaaaa1110111 */ 0xae001077, 0xfe00707f, riscv_rvp_1010111dddddccccc001aaaaa1110111 },
	{ "kmmwt2.u", /*         1011111dddddccccc001aaaaa1110111 */ 0xbe001077, 0xfe00707f, riscv_rvp_1011111dddddccccc001aaaaa1110111 },
	{ "kmsda", /*            0100110dddddccccc001aaaaa1110111 */ 0x4c001077, 0xfe00707f, riscv_rvp_0100110dddddccccc001aaaaa1110111 },
	{ "kmsxda", /*           0100111dddddccccc001aaaaa1110111 */ 0x4e001077, 0xfe00707f, riscv_rvp_0100111dddddccccc001aaaaa1110111 },
	{ "kmsr64", /*           1001011dddddccccc001aaaaa1110111 */ 0x96001077, 0xfe00707f, riscv_rvp_1001011dddddccccc001aaaaa1110111 },
	{ "ksllw", /*            0010011dddddccccc001aaaaa1110111 */ 0x26001077, 0xfe00707f, riscv_rvp_0010011dddddccccc001aaaaa1110111 },
	{ "kslliw", /*           0011011BBBBBccccc001aaaaa1110111 */ 0x36001077, 0xfe00707f, riscv_rvp_0011011BBBBBccccc001aaaaa1110111 },
	{ "ksll8", /*            0110110dddddccccc000aaaaa1110111 */ 0x6c000077, 0xfe00707f, riscv_rvp_0110110dddddccccc000aaaaa1110111 },
	{ "ksll16", /*           0110010dddddccccc000aaaaa1110111 */ 0x64000077, 0xfe00707f, riscv_rvp_0110010dddddccccc000aaaaa1110111 },
	{ "kslra8", /*           0101111dddddccccc000aaaaa1110111 */ 0x5e000077, 0xfe00707f, riscv_rvp_0101111dddddccccc000aaaaa1110111 },
	{ "kslra8.u", /*         0110111dddddccccc000aaaaa1110111 */ 0x6e000077, 0xfe00707f, riscv_rvp_0110111dddddccccc000aaaaa1110111 },
	{ "kslra16", /*          0101011dddddccccc000aaaaa1110111 */ 0x56000077, 0xfe00707f, riscv_rvp_0101011dddddccccc000aaaaa1110111 },
	{ "kslra16.u", /*        0110011dddddccccc000aaaaa1110111 */ 0x66000077, 0xfe00707f, riscv_rvp_0110011dddddccccc000aaaaa1110111 },
	{ "kslraw", /*           0110111dddddccccc001aaaaa1110111 */ 0x6e001077, 0xfe00707f, riscv_rvp_0110111dddddccccc001aaaaa1110111 },
	{ "kslraw.u", /*         0111111dddddccccc001aaaaa1110111 */ 0x7e001077, 0xfe00707f, riscv_rvp_0111111dddddccccc001aaaaa1110111 },
	{ "kstas16", /*          1100010dddddccccc010aaaaa1110111 */ 0xc4002077, 0xfe00707f, riscv_rvp_1100010dddddccccc010aaaaa1110111 },
	{ "kstsa16", /*          1100011dddddccccc010aaaaa1110111 */ 0xc6002077, 0xfe00707f, riscv_rvp_1100011dddddccccc010aaaaa1110111 },
	{ "ksub8", /*            0001101dddddccccc000aaaaa1110111 */ 0x1a000077, 0xfe00707f, riscv_rvp_0001101dddddccccc000aaaaa1110111 },
	{ "ksub16", /*           0001001dddddccccc000aaaaa1110111 */ 0x12000077, 0xfe00707f, riscv_rvp_0001001dddddccccc000aaaaa1110111 },
	{ "ksub64", /*           1001001dddddccccc001aaaaa1110111 */ 0x92001077, 0xfe00707f, riscv_rvp_1001001dddddccccc001aaaaa1110111 },
	{ "ksubh", /*            0000011dddddccccc001aaaaa1110111 */ 0x06001077, 0xfe00707f, riscv_rvp_0000011dddddccccc001aaaaa1110111 },
	{ "ksubw", /*            0000001dddddccccc001aaaaa1110111 */ 0x02001077, 0xfe00707f, riscv_rvp_0000001dddddccccc001aaaaa1110111 },
	{ "kwmmul", /*           0110001dddddccccc001aaaaa1110111 */ 0x62001077, 0xfe00707f, riscv_rvp_0110001dddddccccc001aaaaa1110111 },
	{ "kwmmul.u", /*         0111001dddddccccc001aaaaa1110111 */ 0x72001077, 0xfe00707f, riscv_rvp_0111001dddddccccc001aaaaa1110111 },
	{ "maddr32", /*          1100010dddddccccc001aaaaa1110111 */ 0xc4001077, 0xfe00707f, riscv_rvp_1100010dddddccccc001aaaaa1110111 },
	{ "maxw", /*             1111001dddddccccc000aaaaa1110111 */ 0xf2000077, 0xfe00707f, riscv_rvp_1111001dddddccccc000aaaaa1110111 },
	{ "minw", /*             1111000dddddccccc000aaaaa1110111 */ 0xf0000077, 0xfe00707f, riscv_rvp_1111000dddddccccc000aaaaa1110111 },
	{ "msubr32", /*          1100011dddddccccc001aaaaa1110111 */ 0xc6001077, 0xfe00707f, riscv_rvp_1100011dddddccccc001aaaaa1110111 },
	{ "mulr64", /*           1111000dddddccccc001aaaaa1110111 */ 0xf0001077, 0xfe00707f, riscv_rvp_1111000dddddccccc001aaaaa1110111 },
	{ "mulsr64", /*          1110000dddddccccc001aaaaa1110111 */ 0xe0001077, 0xfe00707f, riscv_rvp_1110000dddddccccc001aaaaa1110111 },
	{ "pbsad", /*            1111110dddddccccc000aaaaa1110111 */ 0xfc000077, 0xfe00707f, riscv_rvp_1111110dddddccccc000aaaaa1110111 },
	{ "pbsada", /*           1111111dddddccccc000aaaaa1110111 */ 0xfe000077, 0xfe00707f, riscv_rvp_1111111dddddccccc000aaaaa1110111 },
	{ "pkbb16", /*           0000111dddddccccc001aaaaa1110111 */ 0x0e001077, 0xfe00707f, riscv_rvp_0000111dddddccccc001aaaaa1110111 },
	{ "pkbt16", /*           0001111dddddccccc001aaaaa1110111 */ 0x1e001077, 0xfe00707f, riscv_rvp_0001111dddddccccc001aaaaa1110111 },
	{ "pktt16", /*           0010111dddddccccc001aaaaa1110111 */ 0x2e001077, 0xfe00707f, riscv_rvp_0010111dddddccccc001aaaaa1110111 },
	{ "pktb16", /*           0011111dddddccccc001aaaaa1110111 */ 0x3e001077, 0xfe00707f, riscv_rvp_0011111dddddccccc001aaaaa1110111 },
	{ "radd8", /*            0000100dddddccccc000aaaaa1110111 */ 0x08000077, 0xfe00707f, riscv_rvp_0000100dddddccccc000aaaaa1110111 },
	{ "radd16", /*           0000000dddddccccc000aaaaa1110111 */ 0x00000077, 0xfe00707f, riscv_rvp_0000000dddddccccc000aaaaa1110111 },
	{ "radd64", /*           1000000dddddccccc001aaaaa1110111 */ 0x80001077, 0xfe00707f, riscv_rvp_1000000dddddccccc001aaaaa1110111 },
	{ "raddw", /*            0010000dddddccccc001aaaaa1110111 */ 0x20001077, 0xfe00707f, riscv_rvp_0010000dddddccccc001aaaaa1110111 },
	{ "rcras16", /*          0000010dddddccccc000aaaaa1110111 */ 0x04000077, 0xfe00707f, riscv_rvp_0000010dddddccccc000aaaaa1110111 },
	{ "rcrsa16", /*          0000011dddddccccc000aaaaa1110111 */ 0x06000077, 0xfe00707f, riscv_rvp_0000011dddddccccc000aaaaa1110111 },
	{ "rstas16", /*          1011010dddddccccc010aaaaa1110111 */ 0xb4002077, 0xfe00707f, riscv_rvp_1011010dddddccccc010aaaaa1110111 },
	{ "rstsa16", /*          1011011dddddccccc010aaaaa1110111 */ 0xb6002077, 0xfe00707f, riscv_rvp_1011011dddddccccc010aaaaa1110111 },
	{ "rsub8", /*            0000101dddddccccc000aaaaa1110111 */ 0x0a000077, 0xfe00707f, riscv_rvp_0000101dddddccccc000aaaaa1110111 },
	{ "rsub16", /*           0000001dddddccccc000aaaaa1110111 */ 0x02000077, 0xfe00707f, riscv_rvp_0000001dddddccccc000aaaaa1110111 },
	{ "rsub64", /*           1000001dddddccccc001aaaaa1110111 */ 0x82001077, 0xfe00707f, riscv_rvp_1000001dddddccccc001aaaaa1110111 },
	{ "rsubw", /*            0010001dddddccccc001aaaaa1110111 */ 0x22001077, 0xfe00707f, riscv_rvp_0010001dddddccccc001aaaaa1110111 },
	{ "sclip32", /*          1110010BBBBBccccc000aaaaa1110111 */ 0xe4000077, 0xfe00707f, riscv_rvp_1110010BBBBBccccc000aaaaa1110111 },
	{ "scmple8", /*          0001111dddddccccc000aaaaa1110111 */ 0x1e000077, 0xfe00707f, riscv_rvp_0001111dddddccccc000aaaaa1110111 },
	{ "scmple16", /*         0001110dddddccccc000aaaaa1110111 */ 0x1c000077, 0xfe00707f, riscv_rvp_0001110dddddccccc000aaaaa1110111 },
	{ "scmplt8", /*          0000111dddddccccc000aaaaa1110111 */ 0x0e000077, 0xfe00707f, riscv_rvp_0000111dddddccccc000aaaaa1110111 },
	{ "scmplt16", /*         0000110dddddccccc000aaaaa1110111 */ 0x0c000077, 0xfe00707f, riscv_rvp_0000110dddddccccc000aaaaa1110111 },
	{ "sll8", /*             0101110dddddccccc000aaaaa1110111 */ 0x5c000077, 0xfe00707f, riscv_rvp_0101110dddddccccc000aaaaa1110111 },
	{ "sll16", /*            0101010dddddccccc000aaaaa1110111 */ 0x54000077, 0xfe00707f, riscv_rvp_0101010dddddccccc000aaaaa1110111 },
	{ "smal", /*             0101111dddddccccc001aaaaa1110111 */ 0x5e001077, 0xfe00707f, riscv_rvp_0101111dddddccccc001aaaaa1110111 },
	{ "smalbb", /*           1000100dddddccccc001aaaaa1110111 */ 0x88001077, 0xfe00707f, riscv_rvp_1000100dddddccccc001aaaaa1110111 },
	{ "smalbt", /*           1001100dddddccccc001aaaaa1110111 */ 0x98001077, 0xfe00707f, riscv_rvp_1001100dddddccccc001aaaaa1110111 },
	{ "smaltt", /*           1010100dddddccccc001aaaaa1110111 */ 0xa8001077, 0xfe00707f, riscv_rvp_1010100dddddccccc001aaaaa1110111 },
	{ "smalda", /*           1000110dddddccccc001aaaaa1110111 */ 0x8c001077, 0xfe00707f, riscv_rvp_1000110dddddccccc001aaaaa1110111 },
	{ "smalxda", /*          1001110dddddccccc001aaaaa1110111 */ 0x9c001077, 0xfe00707f, riscv_rvp_1001110dddddccccc001aaaaa1110111 },
	{ "smalds", /*           1000101dddddccccc001aaaaa1110111 */ 0x8a001077, 0xfe00707f, riscv_rvp_1000101dddddccccc001aaaaa1110111 },
	{ "smaldrs", /*          1001101dddddccccc001aaaaa1110111 */ 0x9a001077, 0xfe00707f, riscv_rvp_1001101dddddccccc001aaaaa1110111 },
	{ "smalxds", /*          1010101dddddccccc001aaaaa1110111 */ 0xaa001077, 0xfe00707f, riscv_rvp_1010101dddddccccc001aaaaa1110111 },
	{ "smar64", /*           1000010dddddccccc001aaaaa1110111 */ 0x84001077, 0xfe00707f, riscv_rvp_1000010dddddccccc001aaaaa1110111 },
	{ "smaqa", /*            1100100dddddccccc000aaaaa1110111 */ 0xc8000077, 0xfe00707f, riscv_rvp_1100100dddddccccc000aaaaa1110111 },
	{ "smaqa.su", /*         1100101dddddccccc000aaaaa1110111 */ 0xca000077, 0xfe00707f, riscv_rvp_1100101dddddccccc000aaaaa1110111 },
	{ "smax8", /*            1000101dddddccccc000aaaaa1110111 */ 0x8a000077, 0xfe00707f, riscv_rvp_1000101dddddccccc000aaaaa1110111 },
	{ "smax16", /*           1000001dddddccccc000aaaaa1110111 */ 0x82000077, 0xfe00707f, riscv_rvp_1000001dddddccccc000aaaaa1110111 },
	{ "smbb16", /*           0000100dddddccccc001aaaaa1110111 */ 0x08001077, 0xfe00707f, riscv_rvp_0000100dddddccccc001aaaaa1110111 },
	{ "smbt16", /*           0001100dddddccccc001aaaaa1110111 */ 0x18001077, 0xfe00707f, riscv_rvp_0001100dddddccccc001aaaaa1110111 },
	{ "smtt16", /*           0010100dddddccccc001aaaaa1110111 */ 0x28001077, 0xfe00707f, riscv_rvp_0010100dddddccccc001aaaaa1110111 },
	{ "smds", /*             0101100dddddccccc001aaaaa1110111 */ 0x58001077, 0xfe00707f, riscv_rvp_0101100dddddccccc001aaaaa1110111 },
	{ "smdrs", /*            0110100dddddccccc001aaaaa1110111 */ 0x68001077, 0xfe00707f, riscv_rvp_0110100dddddccccc001aaaaa1110111 },
	{ "smxds", /*            0111100dddddccccc001aaaaa1110111 */ 0x78001077, 0xfe00707f, riscv_rvp_0111100dddddccccc001aaaaa1110111 },
	{ "smin8", /*            1000100dddddccccc000aaaaa1110111 */ 0x88000077, 0xfe00707f, riscv_rvp_1000100dddddccccc000aaaaa1110111 },
	{ "smin16", /*           1000000dddddccccc000aaaaa1110111 */ 0x80000077, 0xfe00707f, riscv_rvp_1000000dddddccccc000aaaaa1110111 },
	{ "smmul", /*            0100000dddddccccc001aaaaa1110111 */ 0x40001077, 0xfe00707f, riscv_rvp_0100000dddddccccc001aaaaa1110111 },
	{ "smmul.u", /*          0101000dddddccccc001aaaaa1110111 */ 0x50001077, 0xfe00707f, riscv_rvp_0101000dddddccccc001aaaaa1110111 },
	{ "smmwb", /*            0100010dddddccccc001aaaaa1110111 */ 0x44001077, 0xfe00707f, riscv_rvp_0100010dddddccccc001aaaaa1110111 },
	{ "smmwb.u", /*          0101010dddddccccc001aaaaa1110111 */ 0x54001077, 0xfe00707f, riscv_rvp_0101010dddddccccc001aaaaa1110111 },
	{ "smmwt", /*            0110010dddddccccc001aaaaa1110111 */ 0x64001077, 0xfe00707f, riscv_rvp_0110010dddddccccc001aaaaa1110111 },
	{ "smmwt.u", /*          0111010dddddccccc001aaaaa1110111 */ 0x74001077, 0xfe00707f, riscv_rvp_0111010dddddccccc001aaaaa1110111 },
	{ "smslda", /*           1010110dddddccccc001aaaaa1110111 */ 0xac001077, 0xfe00707f, riscv_rvp_1010110dddddccccc001aaaaa1110111 },
	{ "smslxda", /*          1011110dddddccccc001aaaaa1110111 */ 0xbc001077, 0xfe00707f, riscv_rvp_1011110dddddccccc001aaaaa1110111 },
	{ "smsr64", /*           1000011dddddccccc001aaaaa1110111 */ 0x86001077, 0xfe00707f, riscv_rvp_1000011dddddccccc001aaaaa1110111 },
	{ "smul8", /*            1010100dddddccccc000aaaaa1110111 */ 0xa8000077, 0xfe00707f, riscv_rvp_1010100dddddccccc000aaaaa1110111 },
	{ "smulx8", /*           1010101dddddccccc000aaaaa1110111 */ 0xaa000077, 0xfe00707f, riscv_rvp_1010101dddddccccc000aaaaa1110111 },
	{ "smul16", /*           1010000dddddccccc000aaaaa1110111 */ 0xa0000077, 0xfe00707f, riscv_rvp_1010000dddddccccc000aaaaa1110111 },
	{ "smulx16", /*          1010001dddddccccc000aaaaa1110111 */ 0xa2000077, 0xfe00707f, riscv_rvp_1010001dddddccccc000aaaaa1110111 },
	{ "sra.u", /*            0010010dddddccccc001aaaaa1110111 */ 0x24001077, 0xfe00707f, riscv_rvp_0010010dddddccccc001aaaaa1110111 },
	{ "sra8", /*             0101100dddddccccc000aaaaa1110111 */ 0x58000077, 0xfe00707f, riscv_rvp_0101100dddddccccc000aaaaa1110111 },
	{ "sra8.u", /*           0110100dddddccccc000aaaaa1110111 */ 0x68000077, 0xfe00707f, riscv_rvp_0110100dddddccccc000aaaaa1110111 },
	{ "sra16", /*            0101000dddddccccc000aaaaa1110111 */ 0x50000077, 0xfe00707f, riscv_rvp_0101000dddddccccc000aaaaa1110111 },
	{ "sra16.u", /*          0110000dddddccccc000aaaaa1110111 */ 0x60000077, 0xfe00707f, riscv_rvp_0110000dddddccccc000aaaaa1110111 },
	{ "srl8", /*             0101101dddddccccc000aaaaa1110111 */ 0x5a000077, 0xfe00707f, riscv_rvp_0101101dddddccccc000aaaaa1110111 },
	{ "srl8.u", /*           0110101dddddccccc000aaaaa1110111 */ 0x6a000077, 0xfe00707f, riscv_rvp_0110101dddddccccc000aaaaa1110111 },
	{ "srl16", /*            0101001dddddccccc000aaaaa1110111 */ 0x52000077, 0xfe00707f, riscv_rvp_0101001dddddccccc000aaaaa1110111 },
	{ "srl16.u", /*          0110001dddddccccc000aaaaa1110111 */ 0x62000077, 0xfe00707f, riscv_rvp_0110001dddddccccc000aaaaa1110111 },
	{ "stas16", /*           1111010dddddccccc010aaaaa1110111 */ 0xf4002077, 0xfe00707f, riscv_rvp_1111010dddddccccc010aaaaa1110111 },
	{ "stsa16", /*           1111011dddddccccc010aaaaa1110111 */ 0xf6002077, 0xfe00707f, riscv_rvp_1111011dddddccccc010aaaaa1110111 },
	{ "sub8", /*             0100101dddddccccc000aaaaa1110111 */ 0x4a000077, 0xfe00707f, riscv_rvp_0100101dddddccccc000aaaaa1110111 },
	{ "sub16", /*            0100001dddddccccc000aaaaa1110111 */ 0x42000077, 0xfe00707f, riscv_rvp_0100001dddddccccc000aaaaa1110111 },
	{ "sub64", /*            1100001dddddccccc001aaaaa1110111 */ 0xc2001077, 0xfe00707f, riscv_rvp_1100001dddddccccc001aaaaa1110111 },
	{ "uclip32", /*          1111010BBBBBccccc000aaaaa1110111 */ 0xf4000077, 0xfe00707f, riscv_rvp_1111010BBBBBccccc000aaaaa1110111 },
	{ "ucmple8", /*          0011111dddddccccc000aaaaa1110111 */ 0x3e000077, 0xfe00707f, riscv_rvp_0011111dddddccccc000aaaaa1110111 },
	{ "ucmple16", /*         0011110dddddccccc000aaaaa1110111 */ 0x3c000077, 0xfe00707f, riscv_rvp_0011110dddddccccc000aaaaa1110111 },
	{ "ucmplt8", /*          0010111dddddccccc000aaaaa1110111 */ 0x2e000077, 0xfe00707f, riscv_rvp_0010111dddddccccc000aaaaa1110111 },
	{ "ucmplt16", /*         0010110dddddccccc000aaaaa1110111 */ 0x2c000077, 0xfe00707f, riscv_rvp_0010110dddddccccc000aaaaa1110111 },
	{ "ukadd8", /*           0011100dddddccccc000aaaaa1110111 */ 0x38000077, 0xfe00707f, riscv_rvp_0011100dddddccccc000aaaaa1110111 },
	{ "ukadd16", /*          0011000dddddccccc000aaaaa1110111 */ 0x30000077, 0xfe00707f, riscv_rvp_0011000dddddccccc000aaaaa1110111 },
	{ "ukadd64", /*          1011000dddddccccc001aaaaa1110111 */ 0xb0001077, 0xfe00707f, riscv_rvp_1011000dddddccccc001aaaaa1110111 },
	{ "ukaddh", /*           0001010dddddccccc001aaaaa1110111 */ 0x14001077, 0xfe00707f, riscv_rvp_0001010dddddccccc001aaaaa1110111 },
	{ "ukaddw", /*           0001000dddddccccc001aaaaa1110111 */ 0x10001077, 0xfe00707f, riscv_rvp_0001000dddddccccc001aaaaa1110111 },
	{ "ukcras16", /*         0011010dddddccccc000aaaaa1110111 */ 0x34000077, 0xfe00707f, riscv_rvp_0011010dddddccccc000aaaaa1110111 },
	{ "ukcrsa16", /*         0011011dddddccccc000aaaaa1110111 */ 0x36000077, 0xfe00707f, riscv_rvp_0011011dddddccccc000aaaaa1110111 },
	{ "ukmar64", /*          1011010dddddccccc001aaaaa1110111 */ 0xb4001077, 0xfe00707f, riscv_rvp_1011010dddddccccc001aaaaa1110111 },
	{ "ukmsr64", /*          1011011dddddccccc001aaaaa1110111 */ 0xb6001077, 0xfe00707f, riscv_rvp_1011011dddddccccc001aaaaa1110111 },
	{ "ukstas16", /*         1110010dddddccccc010aaaaa1110111 */ 0xe4002077, 0xfe00707f, riscv_rvp_1110010dddddccccc010aaaaa1110111 },
	{ "ukstsa16", /*         1110011dddddccccc010aaaaa1110111 */ 0xe6002077, 0xfe00707f, riscv_rvp_1110011dddddccccc010aaaaa1110111 },
	{ "uksub8", /*           0011101dddddccccc000aaaaa1110111 */ 0x3a000077, 0xfe00707f, riscv_rvp_0011101dddddccccc000aaaaa1110111 },
	{ "uksub16", /*          0011001dddddccccc000aaaaa1110111 */ 0x32000077, 0xfe00707f, riscv_rvp_0011001dddddccccc000aaaaa1110111 },
	{ "uksub64", /*          1011001dddddccccc001aaaaa1110111 */ 0xb2001077, 0xfe00707f, riscv_rvp_1011001dddddccccc001aaaaa1110111 },
	{ "uksubh", /*           0001011dddddccccc001aaaaa1110111 */ 0x16001077, 0xfe00707f, riscv_rvp_0001011dddddccccc001aaaaa1110111 },
	{ "uksubw", /*           0001001dddddccccc001aaaaa1110111 */ 0x12001077, 0xfe00707f, riscv_rvp_0001001dddddccccc001aaaaa1110111 },
	{ "umar64", /*           1010010dddddccccc001aaaaa1110111 */ 0xa4001077, 0xfe00707f, riscv_rvp_1010010dddddccccc001aaaaa1110111 },
	{ "umaqa", /*            1100110dddddccccc000aaaaa1110111 */ 0xcc000077, 0xfe00707f, riscv_rvp_1100110dddddccccc000aaaaa1110111 },
	{ "umax8", /*            1001101dddddccccc000aaaaa1110111 */ 0x9a000077, 0xfe00707f, riscv_rvp_1001101dddddccccc000aaaaa1110111 },
	{ "umax16", /*           1001001dddddccccc000aaaaa1110111 */ 0x92000077, 0xfe00707f, riscv_rvp_1001001dddddccccc000aaaaa1110111 },
	{ "umin8", /*            1001100dddddccccc000aaaaa1110111 */ 0x98000077, 0xfe00707f, riscv_rvp_1001100dddddccccc000aaaaa1110111 },
	{ "umin16", /*           1001000dddddccccc000aaaaa1110111 */ 0x90000077, 0xfe00707f, riscv_rvp_1001000dddddccccc000aaaaa1110111 },
	{ "umsr64", /*           1010011dddddccccc001aaaaa1110111 */ 0xa6001077, 0xfe00707f, riscv_rvp_1010011dddddccccc001aaaaa1110111 },
	{ "umul8", /*            1011100dddddccccc000aaaaa1110111 */ 0xb8000077, 0xfe00707f, riscv_rvp_1011100dddddccccc000aaaaa1110111 },
	{ "umulx8", /*           1011101dddddccccc000aaaaa1110111 */ 0xba000077, 0xfe00707f, riscv_rvp_1011101dddddccccc000aaaaa1110111 },
	{ "umul16", /*           1011000dddddccccc000aaaaa1110111 */ 0xb0000077, 0xfe00707f, riscv_rvp_1011000dddddccccc000aaaaa1110111 },
	{ "umulx16", /*          1011001dddddccccc000aaaaa1110111 */ 0xb2000077, 0xfe00707f, riscv_rvp_1011001dddddccccc000aaaaa1110111 },
	{ "uradd8", /*           0010100dddddccccc000aaaaa1110111 */ 0x28000077, 0xfe00707f, riscv_rvp_0010100dddddccccc000aaaaa1110111 },
	{ "uradd16", /*          0010000dddddccccc000aaaaa1110111 */ 0x20000077, 0xfe00707f, riscv_rvp_0010000dddddccccc000aaaaa1110111 },
	{ "uradd64", /*          1010000dddddccccc001aaaaa1110111 */ 0xa0001077, 0xfe00707f, riscv_rvp_1010000dddddccccc001aaaaa1110111 },
	{ "uraddw", /*           0011000dddddccccc001aaaaa1110111 */ 0x30001077, 0xfe00707f, riscv_rvp_0011000dddddccccc001aaaaa1110111 },
	{ "urcras16", /*         0010010dddddccccc000aaaaa1110111 */ 0x24000077, 0xfe00707f, riscv_rvp_0010010dddddccccc000aaaaa1110111 },
	{ "urcrsa16", /*         0010011dddddccccc000aaaaa1110111 */ 0x26000077, 0xfe00707f, riscv_rvp_0010011dddddccccc000aaaaa1110111 },
	{ "urstas16", /*         1101010dddddccccc010aaaaa1110111 */ 0xd4002077, 0xfe00707f, riscv_rvp_1101010dddddccccc010aaaaa1110111 },
	{ "urstsa16", /*         1101011dddddccccc010aaaaa1110111 */ 0xd6002077, 0xfe00707f, riscv_rvp_1101011dddddccccc010aaaaa1110111 },
	{ "ursub8", /*           0010101dddddccccc000aaaaa1110111 */ 0x2a000077, 0xfe00707f, riscv_rvp_0010101dddddccccc000aaaaa1110111 },
	{ "ursub16", /*          0010001dddddccccc000aaaaa1110111 */ 0x22000077, 0xfe00707f, riscv_rvp_0010001dddddccccc000aaaaa1110111 },
	{ "ursub64", /*          1010001dddddccccc001aaaaa1110111 */ 0xa2001077, 0xfe00707f, riscv_rvp_1010001dddddccccc001aaaaa1110111 },
	{ "ursubw", /*           0011001dddddccccc001aaaaa1110111 */ 0x32001077, 0xfe00707f, riscv_rvp_0011001dddddccccc001aaaaa1110111 },
	{ "wexti", /*            1101111BBBBBccccc000aaaaa1110111 */ 0xde000077, 0xfe00707f, riscv_rvp_1101111BBBBBccccc000aaaaa1110111 },
	{ "wext", /*             1100111dddddccccc000aaaaa1110111 */ 0xce000077, 0xfe00707f, riscv_rvp_1100111dddddccccc000aaaaa1110111 },
	{ "add32", /*            0100000dddddccccc010aaaaa1110111 */ 0x40002077, 0xfe00707f, riscv_rvp_0100000dddddccccc010aaaaa1110111 },
	{ "cras32", /*           0100010dddddccccc010aaaaa1110111 */ 0x44002077, 0xfe00707f, riscv_rvp_0100010dddddccccc010aaaaa1110111 },
	{ "crsa32", /*           0100011dddddccccc010aaaaa1110111 */ 0x46002077, 0xfe00707f, riscv_rvp_0100011dddddccccc010aaaaa1110111 },
	{ "kadd32", /*           0001000dddddccccc010aaaaa1110111 */ 0x10002077, 0xfe00707f, riscv_rvp_0001000dddddccccc010aaaaa1110111 },
	{ "kcras32", /*          0001010dddddccccc010aaaaa1110111 */ 0x14002077, 0xfe00707f, riscv_rvp_0001010dddddccccc010aaaaa1110111 },
	{ "kcrsa32", /*          0001011dddddccccc010aaaaa1110111 */ 0x16002077, 0xfe00707f, riscv_rvp_0001011dddddccccc010aaaaa1110111 },
	{ "kdmbb16", /*          1101101dddddccccc001aaaaa1110111 */ 0xda001077, 0xfe00707f, riscv_rvp_1101101dddddccccc001aaaaa1110111 },
	{ "kdmbt16", /*          1110101dddddccccc001aaaaa1110111 */ 0xea001077, 0xfe00707f, riscv_rvp_1110101dddddccccc001aaaaa1110111 },
	{ "kdmtt16", /*          1111101dddddccccc001aaaaa1110111 */ 0xfa001077, 0xfe00707f, riscv_rvp_1111101dddddccccc001aaaaa1110111 },
	{ "kdmabb16", /*         1101100dddddccccc001aaaaa1110111 */ 0xd8001077, 0xfe00707f, riscv_rvp_1101100dddddccccc001aaaaa1110111 },
	{ "kdmabt16", /*         1110100dddddccccc001aaaaa1110111 */ 0xe8001077, 0xfe00707f, riscv_rvp_1110100dddddccccc001aaaaa1110111 },
	{ "kdmatt16", /*         1111100dddddccccc001aaaaa1110111 */ 0xf8001077, 0xfe00707f, riscv_rvp_1111100dddddccccc001aaaaa1110111 },
	{ "khmbb16", /*          1101110dddddccccc001aaaaa1110111 */ 0xdc001077, 0xfe00707f, riscv_rvp_1101110dddddccccc001aaaaa1110111 },
	{ "khmbt16", /*          1110110dddddccccc001aaaaa1110111 */ 0xec001077, 0xfe00707f, riscv_rvp_1110110dddddccccc001aaaaa1110111 },
	{ "khmtt16", /*          1111110dddddccccc001aaaaa1110111 */ 0xfc001077, 0xfe00707f, riscv_rvp_1111110dddddccccc001aaaaa1110111 },
	{ "kmabb32", /*          0101101dddddccccc010aaaaa1110111 */ 0x5a002077, 0xfe00707f, riscv_rvp_0101101dddddccccc010aaaaa1110111 },
	{ "kmabt32", /*          0110101dddddccccc010aaaaa1110111 */ 0x6a002077, 0xfe00707f, riscv_rvp_0110101dddddccccc010aaaaa1110111 },
	{ "kmatt32", /*          0111101dddddccccc010aaaaa1110111 */ 0x7a002077, 0xfe00707f, riscv_rvp_0111101dddddccccc010aaaaa1110111 },
	{ "kmaxda32", /*         0100101dddddccccc010aaaaa1110111 */ 0x4a002077, 0xfe00707f, riscv_rvp_0100101dddddccccc010aaaaa1110111 },
	{ "kmda32", /*           0011100dddddccccc010aaaaa1110111 */ 0x38002077, 0xfe00707f, riscv_rvp_0011100dddddccccc010aaaaa1110111 },
	{ "kmxda32", /*          0011101dddddccccc010aaaaa1110111 */ 0x3a002077, 0xfe00707f, riscv_rvp_0011101dddddccccc010aaaaa1110111 },
	{ "kmads32", /*          0101110dddddccccc010aaaaa1110111 */ 0x5c002077, 0xfe00707f, riscv_rvp_0101110dddddccccc010aaaaa1110111 },
	{ "kmadrs32", /*         0110110dddddccccc010aaaaa1110111 */ 0x6c002077, 0xfe00707f, riscv_rvp_0110110dddddccccc010aaaaa1110111 },
	{ "kmaxds32", /*         0111110dddddccccc010aaaaa1110111 */ 0x7c002077, 0xfe00707f, riscv_rvp_0111110dddddccccc010aaaaa1110111 },
	{ "kmsda32", /*          0100110dddddccccc010aaaaa1110111 */ 0x4c002077, 0xfe00707f, riscv_rvp_0100110dddddccccc010aaaaa1110111 },
	{ "kmsxda32", /*         0100111dddddccccc010aaaaa1110111 */ 0x4e002077, 0xfe00707f, riscv_rvp_0100111dddddccccc010aaaaa1110111 },
	{ "ksll32", /*           0110010dddddccccc010aaaaa1110111 */ 0x64002077, 0xfe00707f, riscv_rvp_0110010dddddccccc010aaaaa1110111 },
	{ "kslli32", /*          1000010BBBBBccccc010aaaaa1110111 */ 0x84002077, 0xfe00707f, riscv_rvp_1000010BBBBBccccc010aaaaa1110111 },
	{ "kslra32", /*          0101011dddddccccc010aaaaa1110111 */ 0x56002077, 0xfe00707f, riscv_rvp_0101011dddddccccc010aaaaa1110111 },
	{ "kslra32.u", /*        0110011dddddccccc010aaaaa1110111 */ 0x66002077, 0xfe00707f, riscv_rvp_0110011dddddccccc010aaaaa1110111 },
	{ "kstas32", /*          1100000dddddccccc010aaaaa1110111 */ 0xc0002077, 0xfe00707f, riscv_rvp_1100000dddddccccc010aaaaa1110111 },
	{ "kstsa32", /*          1100001dddddccccc010aaaaa1110111 */ 0xc2002077, 0xfe00707f, riscv_rvp_1100001dddddccccc010aaaaa1110111 },
	{ "ksub32", /*           0001001dddddccccc010aaaaa1110111 */ 0x12002077, 0xfe00707f, riscv_rvp_0001001dddddccccc010aaaaa1110111 },
	{ "pkbb32", /*           0000111dddddccccc010aaaaa1110111 */ 0x0e002077, 0xfe00707f, riscv_rvp_0000111dddddccccc010aaaaa1110111 },
	{ "pkbt32", /*           0001111dddddccccc010aaaaa1110111 */ 0x1e002077, 0xfe00707f, riscv_rvp_0001111dddddccccc010aaaaa1110111 },
	{ "pktt32", /*           0010111dddddccccc010aaaaa1110111 */ 0x2e002077, 0xfe00707f, riscv_rvp_0010111dddddccccc010aaaaa1110111 },
	{ "pktb32", /*           0011111dddddccccc010aaaaa1110111 */ 0x3e002077, 0xfe00707f, riscv_rvp_0011111dddddccccc010aaaaa1110111 },
	{ "radd32", /*           0000000dddddccccc010aaaaa1110111 */ 0x00002077, 0xfe00707f, riscv_rvp_0000000dddddccccc010aaaaa1110111 },
	{ "rcras32", /*          0000010dddddccccc010aaaaa1110111 */ 0x04002077, 0xfe00707f, riscv_rvp_0000010dddddccccc010aaaaa1110111 },
	{ "rcrsa32", /*          0000011dddddccccc010aaaaa1110111 */ 0x06002077, 0xfe00707f, riscv_rvp_0000011dddddccccc010aaaaa1110111 },
	{ "rstas32", /*          1011000dddddccccc010aaaaa1110111 */ 0xb0002077, 0xfe00707f, riscv_rvp_1011000dddddccccc010aaaaa1110111 },
	{ "rstsa32", /*          1011001dddddccccc010aaaaa1110111 */ 0xb2002077, 0xfe00707f, riscv_rvp_1011001dddddccccc010aaaaa1110111 },
	{ "rsub32", /*           0000001dddddccccc010aaaaa1110111 */ 0x02002077, 0xfe00707f, riscv_rvp_0000001dddddccccc010aaaaa1110111 },
	{ "sll32", /*            0101010dddddccccc010aaaaa1110111 */ 0x54002077, 0xfe00707f, riscv_rvp_0101010dddddccccc010aaaaa1110111 },
	{ "slli32", /*           0111010BBBBBccccc010aaaaa1110111 */ 0x74002077, 0xfe00707f, riscv_rvp_0111010BBBBBccccc010aaaaa1110111 },
	{ "smax32", /*           1001001dddddccccc010aaaaa1110111 */ 0x92002077, 0xfe00707f, riscv_rvp_1001001dddddccccc010aaaaa1110111 },
	{ "smbt32", /*           0001100dddddccccc010aaaaa1110111 */ 0x18002077, 0xfe00707f, riscv_rvp_0001100dddddccccc010aaaaa1110111 },
	{ "smtt32", /*           0010100dddddccccc010aaaaa1110111 */ 0x28002077, 0xfe00707f, riscv_rvp_0010100dddddccccc010aaaaa1110111 },
	{ "smds32", /*           0101100dddddccccc010aaaaa1110111 */ 0x58002077, 0xfe00707f, riscv_rvp_0101100dddddccccc010aaaaa1110111 },
	{ "smdrs32", /*          0110100dddddccccc010aaaaa1110111 */ 0x68002077, 0xfe00707f, riscv_rvp_0110100dddddccccc010aaaaa1110111 },
	{ "smxds32", /*          0111100dddddccccc010aaaaa1110111 */ 0x78002077, 0xfe00707f, riscv_rvp_0111100dddddccccc010aaaaa1110111 },
	{ "smin32", /*           1001000dddddccccc010aaaaa1110111 */ 0x90002077, 0xfe00707f, riscv_rvp_1001000dddddccccc010aaaaa1110111 },
	{ "sra32", /*            0101000dddddccccc010aaaaa1110111 */ 0x50002077, 0xfe00707f, riscv_rvp_0101000dddddccccc010aaaaa1110111 },
	{ "sra32.u", /*          0110000dddddccccc010aaaaa1110111 */ 0x60002077, 0xfe00707f, riscv_rvp_0110000dddddccccc010aaaaa1110111 },
	{ "srai32", /*           0111000BBBBBccccc010aaaaa1110111 */ 0x70002077, 0xfe00707f, riscv_rvp_0111000BBBBBccccc010aaaaa1110111 },
	{ "srai32.u", /*         1000000BBBBBccccc010aaaaa1110111 */ 0x80002077, 0xfe00707f, riscv_rvp_1000000BBBBBccccc010aaaaa1110111 },
	{ "sraiw.u", /*          0011010BBBBBccccc001aaaaa1110111 */ 0x34001077, 0xfe00707f, riscv_rvp_0011010BBBBBccccc001aaaaa1110111 },
	{ "srl32", /*            0101001dddddccccc010aaaaa1110111 */ 0x52002077, 0xfe00707f, riscv_rvp_0101001dddddccccc010aaaaa1110111 },
	{ "srl32.u", /*          0110001dddddccccc010aaaaa1110111 */ 0x62002077, 0xfe00707f, riscv_rvp_0110001dddddccccc010aaaaa1110111 },
	{ "srli32", /*           0111001BBBBBccccc010aaaaa1110111 */ 0x72002077, 0xfe00707f, riscv_rvp_0111001BBBBBccccc010aaaaa1110111 },
	{ "srli32.u", /*         1000001BBBBBccccc010aaaaa1110111 */ 0x82002077, 0xfe00707f, riscv_rvp_1000001BBBBBccccc010aaaaa1110111 },
	{ "stas32", /*           1111000dddddccccc010aaaaa1110111 */ 0xf0002077, 0xfe00707f, riscv_rvp_1111000dddddccccc010aaaaa1110111 },
	{ "stsa32", /*           1111001dddddccccc010aaaaa1110111 */ 0xf2002077, 0xfe00707f, riscv_rvp_1111001dddddccccc010aaaaa1110111 },
	{ "sub32", /*            0100001dddddccccc010aaaaa1110111 */ 0x42002077, 0xfe00707f, riscv_rvp_0100001dddddccccc010aaaaa1110111 },
	{ "ukadd32", /*          0011000dddddccccc010aaaaa1110111 */ 0x30002077, 0xfe00707f, riscv_rvp_0011000dddddccccc010aaaaa1110111 },
	{ "ukcras32", /*         0011010dddddccccc010aaaaa1110111 */ 0x34002077, 0xfe00707f, riscv_rvp_0011010dddddccccc010aaaaa1110111 },
	{ "ukcrsa32", /*         0011011dddddccccc010aaaaa1110111 */ 0x36002077, 0xfe00707f, riscv_rvp_0011011dddddccccc010aaaaa1110111 },
	{ "ukstas32", /*         1110000dddddccccc010aaaaa1110111 */ 0xe0002077, 0xfe00707f, riscv_rvp_1110000dddddccccc010aaaaa1110111 },
	{ "ukstsa32", /*         1110001dddddccccc010aaaaa1110111 */ 0xe2002077, 0xfe00707f, riscv_rvp_1110001dddddccccc010aaaaa1110111 },
	{ "uksub32", /*          0011001dddddccccc010aaaaa1110111 */ 0x32002077, 0xfe00707f, riscv_rvp_0011001dddddccccc010aaaaa1110111 },
	{ "umax32", /*           1010001dddddccccc010aaaaa1110111 */ 0xa2002077, 0xfe00707f, riscv_rvp_1010001dddddccccc010aaaaa1110111 },
	{ "umin32", /*           1010000dddddccccc010aaaaa1110111 */ 0xa0002077, 0xfe00707f, riscv_rvp_1010000dddddccccc010aaaaa1110111 },
	{ "uradd32", /*          0010000dddddccccc010aaaaa1110111 */ 0x20002077, 0xfe00707f, riscv_rvp_0010000dddddccccc010aaaaa1110111 },
	{ "urcras32", /*         0010010dddddccccc010aaaaa1110111 */ 0x24002077, 0xfe00707f, riscv_rvp_0010010dddddccccc010aaaaa1110111 },
	{ "urcrsa32", /*         0010011dddddccccc010aaaaa1110111 */ 0x26002077, 0xfe00707f, riscv_rvp_0010011dddddccccc010aaaaa1110111 },
	{ "urstas32", /*         1101000dddddccccc010aaaaa1110111 */ 0xd0002077, 0xfe00707f, riscv_rvp_1101000dddddccccc010aaaaa1110111 },
	{ "urstsa32", /*         1101001dddddccccc010aaaaa1110111 */ 0xd2002077, 0xfe00707f, riscv_rvp_1101001dddddccccc010aaaaa1110111 },
	{ "ursub32", /*          0010001dddddccccc010aaaaa1110111 */ 0x22002077, 0xfe00707f, riscv_rvp_0010001dddddccccc010aaaaa1110111 },
	{ "vsetvl", /*           1000000dddddccccc111aaaaa1010111 */ 0x80007057, 0xfe00707f, riscv_rvv_1000000dddddccccc111aaaaa1010111 },
	{ "vfmerge.vfm", /*      0101110GGGGGccccc101DDDDD1010111 */ 0x5c005057, 0xfe00707f, riscv_rvv_0101110GGGGGccccc101DDDDD1010111 },
	{ "vadc.vxm", /*         0100000GGGGGccccc100DDDDD1010111 */ 0x40004057, 0xfe00707f, riscv_rvv_0100000GGGGGccccc100DDDDD1010111 },
	{ "vmadc.vxm", /*        0100010GGGGGccccc100DDDDD1010111 */ 0x44004057, 0xfe00707f, riscv_rvv_0100010GGGGGccccc100DDDDD1010111 },
	{ "vmadc.vx", /*         0100011GGGGGccccc100DDDDD1010111 */ 0x46004057, 0xfe00707f, riscv_rvv_0100011GGGGGccccc100DDDDD1010111 },
	{ "vsbc.vxm", /*         0100100GGGGGccccc100DDDDD1010111 */ 0x48004057, 0xfe00707f, riscv_rvv_0100100GGGGGccccc100DDDDD1010111 },
	{ "vmsbc.vxm", /*        0100110GGGGGccccc100DDDDD1010111 */ 0x4c004057, 0xfe00707f, riscv_rvv_0100110GGGGGccccc100DDDDD1010111 },
	{ "vmsbc.vx", /*         0100111GGGGGccccc100DDDDD1010111 */ 0x4e004057, 0xfe00707f, riscv_rvv_0100111GGGGGccccc100DDDDD1010111 },
	{ "vmerge.vxm", /*       0101110GGGGGccccc100DDDDD1010111 */ 0x5c004057, 0xfe00707f, riscv_rvv_0101110GGGGGccccc100DDDDD1010111 },
	{ "vadc.vvm", /*         0100000GGGGGFFFFF000DDDDD1010111 */ 0x40000057, 0xfe00707f, riscv_rvv_0100000GGGGGFFFFF000DDDDD1010111 },
	{ "vmadc.vvm", /*        0100010GGGGGFFFFF000DDDDD1010111 */ 0x44000057, 0xfe00707f, riscv_rvv_0100010GGGGGFFFFF000DDDDD1010111 },
	{ "vmadc.vv", /*         0100011GGGGGFFFFF000DDDDD1010111 */ 0x46000057, 0xfe00707f, riscv_rvv_0100011GGGGGFFFFF000DDDDD1010111 },
	{ "vsbc.vvm", /*         0100100GGGGGFFFFF000DDDDD1010111 */ 0x48000057, 0xfe00707f, riscv_rvv_0100100GGGGGFFFFF000DDDDD1010111 },
	{ "vmsbc.vvm", /*        0100110GGGGGFFFFF000DDDDD1010111 */ 0x4c000057, 0xfe00707f, riscv_rvv_0100110GGGGGFFFFF000DDDDD1010111 },
	{ "vmsbc.vv", /*         0100111GGGGGFFFFF000DDDDD1010111 */ 0x4e000057, 0xfe00707f, riscv_rvv_0100111GGGGGFFFFF000DDDDD1010111 },
	{ "vmerge.vvm", /*       0101110GGGGGFFFFF000DDDDD1010111 */ 0x5c000057, 0xfe00707f, riscv_rvv_0101110GGGGGFFFFF000DDDDD1010111 },
	{ "vadc.vim", /*         0100000GGGGGLLLLL011DDDDD1010111 */ 0x40003057, 0xfe00707f, riscv_rvv_0100000GGGGGLLLLL011DDDDD1010111 },
	{ "vmadc.vim", /*        0100010GGGGGLLLLL011DDDDD1010111 */ 0x44003057, 0xfe00707f, riscv_rvv_0100010GGGGGLLLLL011DDDDD1010111 },
	{ "vmadc.vi", /*         0100011GGGGGLLLLL011DDDDD1010111 */ 0x46003057, 0xfe00707f, riscv_rvv_0100011GGGGGLLLLL011DDDDD1010111 },
	{ "vmerge.vim", /*       0101110GGGGGLLLLL011DDDDD1010111 */ 0x5c003057, 0xfe00707f, riscv_rvv_0101110GGGGGLLLLL011DDDDD1010111 },
	{ "vcompress.vm", /*     0101111GGGGGFFFFF010DDDDD1010111 */ 0x5e002057, 0xfe00707f, riscv_rvv_0101111GGGGGFFFFF010DDDDD1010111 },
	{ "vmvnfr.v", /*         1001111GGGGGLLLLL011DDDDD1010111 */ 0x9e003057, 0xfe00707f, riscv_rvv_1001111GGGGGLLLLL011DDDDD1010111 },
	{ "hfence.vvma", /*      0010001dddddccccc000000001110011 */ 0x22000073, 0xfe007fff, riscv_rv32h_0010001dddddccccc000000001110011 },
	{ "hfence.gvma", /*      0110001dddddccccc000000001110011 */ 0x62000073, 0xfe007fff, riscv_rv32h_0110001dddddccccc000000001110011 },
	{ "hsv.b", /*            0110001dddddccccc100000001110011 */ 0x62004073, 0xfe007fff, riscv_rv32h_0110001dddddccccc100000001110011 },
	{ "hsv.h", /*            0110011dddddccccc100000001110011 */ 0x66004073, 0xfe007fff, riscv_rv32h_0110011dddddccccc100000001110011 },
	{ "hsv.w", /*            0110101dddddccccc100000001110011 */ 0x6a004073, 0xfe007fff, riscv_rv32h_0110101dddddccccc100000001110011 },
	{ "hsv.d", /*            0110111dddddccccc100000001110011 */ 0x6e004073, 0xfe007fff, riscv_rv64h_0110111dddddccccc100000001110011 },
	{ "sinval.vma", /*       0001011dddddccccc000000001110011 */ 0x16000073, 0xfe007fff, riscv_svinval_0001011dddddccccc000000001110011 },
	{ "hinval.vvma", /*      0010011dddddccccc000000001110011 */ 0x26000073, 0xfe007fff, riscv_svinval_0010011dddddccccc000000001110011 },
	{ "hinval.gvma", /*      0110011dddddccccc000000001110011 */ 0x66000073, 0xfe007fff, riscv_svinval_0110011dddddccccc000000001110011 },
	{ "sfence.vma", /*       0001001dddddccccc000000001110011 */ 0x12000073, 0xfe007fff, riscv_system_0001001dddddccccc000000001110011 },
	{ "vfmv.f.s", /*         0100001GGGGG00000001aaaaa1010111 */ 0x42001057, 0xfe0ff07f, riscv_rvv_0100001GGGGG00000001aaaaa1010111 },
	{ "vmv1r.v", /*          1001111GGGGG00000011DDDDD1010111 */ 0x9e003057, 0xfe0ff07f, riscv_rvv_1001111GGGGG00000011DDDDD1010111 },
	{ "vmv2r.v", /*          1001111GGGGG00001011DDDDD1010111 */ 0x9e00b057, 0xfe0ff07f, riscv_rvv_1001111GGGGG00001011DDDDD1010111 },
	{ "vmv4r.v", /*          1001111GGGGG00011011DDDDD1010111 */ 0x9e01b057, 0xfe0ff07f, riscv_rvv_1001111GGGGG00011011DDDDD1010111 },
	{ "vmv8r.v", /*          1001111GGGGG00111011DDDDD1010111 */ 0x9e03b057, 0xfe0ff07f, riscv_rvv_1001111GGGGG00111011DDDDD1010111 },
	{ "vmv.x.s", /*          0100001GGGGG00000010aaaaa1010111 */ 0x42002057, 0xfe0ff07f, riscv_rvv_0100001GGGGG00000010aaaaa1010111 },
	{ "aes64ks1i", /*        00110001wwwwccccc001aaaaa0010011 */ 0x31001013, 0xff00707f, riscv_rv64k_00110001wwwwccccc001aaaaa0010011 },
	{ "kslli16", /*          01110101AAAAccccc000aaaaa1110111 */ 0x75000077, 0xff00707f, riscv_rvp_01110101AAAAccccc000aaaaa1110111 },
	{ "sclip16", /*          10000100AAAAccccc000aaaaa1110111 */ 0x84000077, 0xff00707f, riscv_rvp_10000100AAAAccccc000aaaaa1110111 },
	{ "slli16", /*           01110100AAAAccccc000aaaaa1110111 */ 0x74000077, 0xff00707f, riscv_rvp_01110100AAAAccccc000aaaaa1110111 },
	{ "srai16", /*           01110000AAAAccccc000aaaaa1110111 */ 0x70000077, 0xff00707f, riscv_rvp_01110000AAAAccccc000aaaaa1110111 },
	{ "srai16.u", /*         01110001AAAAccccc000aaaaa1110111 */ 0x71000077, 0xff00707f, riscv_rvp_01110001AAAAccccc000aaaaa1110111 },
	{ "srli16", /*           01110010AAAAccccc000aaaaa1110111 */ 0x72000077, 0xff00707f, riscv_rvp_01110010AAAAccccc000aaaaa1110111 },
	{ "srli16.u", /*         01110011AAAAccccc000aaaaa1110111 */ 0x73000077, 0xff00707f, riscv_rvp_01110011AAAAccccc000aaaaa1110111 },
	{ "uclip16", /*          10000101AAAAccccc000aaaaa1110111 */ 0x85000077, 0xff00707f, riscv_rvp_10000101AAAAccccc000aaaaa1110111 },
	{ "insb", /*             101011000zzzccccc000aaaaa1110111 */ 0xac000077, 0xff80707f, riscv_rvp_101011000zzzccccc000aaaaa1110111 },
	{ "kslli8", /*           011111001zzzccccc000aaaaa1110111 */ 0x7c800077, 0xff80707f, riscv_rvp_011111001zzzccccc000aaaaa1110111 },
	{ "sclip8", /*           100011000zzzccccc000aaaaa1110111 */ 0x8c000077, 0xff80707f, riscv_rvp_100011000zzzccccc000aaaaa1110111 },
	{ "slli8", /*            011111000zzzccccc000aaaaa1110111 */ 0x7c000077, 0xff80707f, riscv_rvp_011111000zzzccccc000aaaaa1110111 },
	{ "srai8", /*            011110000zzzccccc000aaaaa1110111 */ 0x78000077, 0xff80707f, riscv_rvp_011110000zzzccccc000aaaaa1110111 },
	{ "srai8.u", /*          011110001zzzccccc000aaaaa1110111 */ 0x78800077, 0xff80707f, riscv_rvp_011110001zzzccccc000aaaaa1110111 },
	{ "srli8", /*            011110100zzzccccc000aaaaa1110111 */ 0x7a000077, 0xff80707f, riscv_rvp_011110100zzzccccc000aaaaa1110111 },
	{ "srli8.u", /*          011110101zzzccccc000aaaaa1110111 */ 0x7a800077, 0xff80707f, riscv_rvp_011110101zzzccccc000aaaaa1110111 },
	{ "uclip8", /*           100011010zzzccccc000aaaaa1110111 */ 0x8d000077, 0xff80707f, riscv_rvp_100011010zzzccccc000aaaaa1110111 },
	{ "fcvt.s.d", /*         010000000001cccccjjjaaaaa1010011 */ 0x40100053, 0xfff0007f, riscv_rv32d_010000000001cccccjjjaaaaa1010011 },
	{ "fcvt.d.s", /*         010000100000cccccjjjaaaaa1010011 */ 0x42000053, 0xfff0007f, riscv_rv32d_010000100000cccccjjjaaaaa1010011 },
	{ "fsqrt.d", /*          010110100000cccccjjjaaaaa1010011 */ 0x5a000053, 0xfff0007f, riscv_rv32d_010110100000cccccjjjaaaaa1010011 },
	{ "fcvt.w.d", /*         110000100000cccccjjjaaaaa1010011 */ 0xc2000053, 0xfff0007f, riscv_rv32d_110000100000cccccjjjaaaaa1010011 },
	{ "fcvt.wu.d", /*        110000100001cccccjjjaaaaa1010011 */ 0xc2100053, 0xfff0007f, riscv_rv32d_110000100001cccccjjjaaaaa1010011 },
	{ "fcvt.d.w", /*         110100100000cccccjjjaaaaa1010011 */ 0xd2000053, 0xfff0007f, riscv_rv32d_110100100000cccccjjjaaaaa1010011 },
	{ "fcvt.d.wu", /*        110100100001cccccjjjaaaaa1010011 */ 0xd2100053, 0xfff0007f, riscv_rv32d_110100100001cccccjjjaaaaa1010011 },
	{ "fcvt.h.d", /*         010001000001cccccjjjaaaaa1010011 */ 0x44100053, 0xfff0007f, riscv_rv32d-zfh_010001000001cccccjjjaaaaa1010011 },
	{ "fcvt.d.h", /*         010000100010cccccjjjaaaaa1010011 */ 0x42200053, 0xfff0007f, riscv_rv32d-zfh_010000100010cccccjjjaaaaa1010011 },
	{ "fsqrt.s", /*          010110000000cccccjjjaaaaa1010011 */ 0x58000053, 0xfff0007f, riscv_rv32f_010110000000cccccjjjaaaaa1010011 },
	{ "fcvt.w.s", /*         110000000000cccccjjjaaaaa1010011 */ 0xc0000053, 0xfff0007f, riscv_rv32f_110000000000cccccjjjaaaaa1010011 },
	{ "fcvt.wu.s", /*        110000000001cccccjjjaaaaa1010011 */ 0xc0100053, 0xfff0007f, riscv_rv32f_110000000001cccccjjjaaaaa1010011 },
	{ "fcvt.s.w", /*         110100000000cccccjjjaaaaa1010011 */ 0xd0000053, 0xfff0007f, riscv_rv32f_110100000000cccccjjjaaaaa1010011 },
	{ "fcvt.s.wu", /*        110100000001cccccjjjaaaaa1010011 */ 0xd0100053, 0xfff0007f, riscv_rv32f_110100000001cccccjjjaaaaa1010011 },
	{ "fcvt.s.q", /*         010000000011cccccjjjaaaaa1010011 */ 0x40300053, 0xfff0007f, riscv_rv32q_010000000011cccccjjjaaaaa1010011 },
	{ "fcvt.q.s", /*         010001100000cccccjjjaaaaa1010011 */ 0x46000053, 0xfff0007f, riscv_rv32q_010001100000cccccjjjaaaaa1010011 },
	{ "fcvt.d.q", /*         010000100011cccccjjjaaaaa1010011 */ 0x42300053, 0xfff0007f, riscv_rv32q_010000100011cccccjjjaaaaa1010011 },
	{ "fcvt.q.d", /*         010001100001cccccjjjaaaaa1010011 */ 0x46100053, 0xfff0007f, riscv_rv32q_010001100001cccccjjjaaaaa1010011 },
	{ "fsqrt.q", /*          010111100000cccccjjjaaaaa1010011 */ 0x5e000053, 0xfff0007f, riscv_rv32q_010111100000cccccjjjaaaaa1010011 },
	{ "fcvt.w.q", /*         110001100000cccccjjjaaaaa1010011 */ 0xc6000053, 0xfff0007f, riscv_rv32q_110001100000cccccjjjaaaaa1010011 },
	{ "fcvt.wu.q", /*        110001100001cccccjjjaaaaa1010011 */ 0xc6100053, 0xfff0007f, riscv_rv32q_110001100001cccccjjjaaaaa1010011 },
	{ "fcvt.q.w", /*         110101100000cccccjjjaaaaa1010011 */ 0xd6000053, 0xfff0007f, riscv_rv32q_110101100000cccccjjjaaaaa1010011 },
	{ "fcvt.q.wu", /*        110101100001cccccjjjaaaaa1010011 */ 0xd6100053, 0xfff0007f, riscv_rv32q_110101100001cccccjjjaaaaa1010011 },
	{ "fcvt.h.q", /*         010001000011cccccjjjaaaaa1010011 */ 0x44300053, 0xfff0007f, riscv_rv32q-zfh_010001000011cccccjjjaaaaa1010011 },
	{ "fcvt.q.h", /*         010001100010cccccjjjaaaaa1010011 */ 0x46200053, 0xfff0007f, riscv_rv32q-zfh_010001100010cccccjjjaaaaa1010011 },
	{ "fcvt.h.s", /*         010001000000cccccjjjaaaaa1010011 */ 0x44000053, 0xfff0007f, riscv_rv32zfh_010001000000cccccjjjaaaaa1010011 },
	{ "fcvt.s.h", /*         010000000010cccccjjjaaaaa1010011 */ 0x40200053, 0xfff0007f, riscv_rv32zfh_010000000010cccccjjjaaaaa1010011 },
	{ "fsqrt.h", /*          010111000000cccccjjjaaaaa1010011 */ 0x5c000053, 0xfff0007f, riscv_rv32zfh_010111000000cccccjjjaaaaa1010011 },
	{ "fcvt.w.h", /*         110001000000cccccjjjaaaaa1010011 */ 0xc4000053, 0xfff0007f, riscv_rv32zfh_110001000000cccccjjjaaaaa1010011 },
	{ "fcvt.wu.h", /*        110001000001cccccjjjaaaaa1010011 */ 0xc4100053, 0xfff0007f, riscv_rv32zfh_110001000001cccccjjjaaaaa1010011 },
	{ "fcvt.h.w", /*         110101000000cccccjjjaaaaa1010011 */ 0xd4000053, 0xfff0007f, riscv_rv32zfh_110101000000cccccjjjaaaaa1010011 },
	{ "fcvt.h.wu", /*        110101000001cccccjjjaaaaa1010011 */ 0xd4100053, 0xfff0007f, riscv_rv32zfh_110101000001cccccjjjaaaaa1010011 },
	{ "fcvt.l.d", /*         110000100010cccccjjjaaaaa1010011 */ 0xc2200053, 0xfff0007f, riscv_rv64d_110000100010cccccjjjaaaaa1010011 },
	{ "fcvt.lu.d", /*        110000100011cccccjjjaaaaa1010011 */ 0xc2300053, 0xfff0007f, riscv_rv64d_110000100011cccccjjjaaaaa1010011 },
	{ "fcvt.d.l", /*         110100100010cccccjjjaaaaa1010011 */ 0xd2200053, 0xfff0007f, riscv_rv64d_110100100010cccccjjjaaaaa1010011 },
	{ "fcvt.d.lu", /*        110100100011cccccjjjaaaaa1010011 */ 0xd2300053, 0xfff0007f, riscv_rv64d_110100100011cccccjjjaaaaa1010011 },
	{ "fcvt.l.s", /*         110000000010cccccjjjaaaaa1010011 */ 0xc0200053, 0xfff0007f, riscv_rv64f_110000000010cccccjjjaaaaa1010011 },
	{ "fcvt.lu.s", /*        110000000011cccccjjjaaaaa1010011 */ 0xc0300053, 0xfff0007f, riscv_rv64f_110000000011cccccjjjaaaaa1010011 },
	{ "fcvt.s.l", /*         110100000010cccccjjjaaaaa1010011 */ 0xd0200053, 0xfff0007f, riscv_rv64f_110100000010cccccjjjaaaaa1010011 },
	{ "fcvt.s.lu", /*        110100000011cccccjjjaaaaa1010011 */ 0xd0300053, 0xfff0007f, riscv_rv64f_110100000011cccccjjjaaaaa1010011 },
	{ "fcvt.l.q", /*         110001100010cccccjjjaaaaa1010011 */ 0xc6200053, 0xfff0007f, riscv_rv64q_110001100010cccccjjjaaaaa1010011 },
	{ "fcvt.lu.q", /*        110001100011cccccjjjaaaaa1010011 */ 0xc6300053, 0xfff0007f, riscv_rv64q_110001100011cccccjjjaaaaa1010011 },
	{ "fcvt.q.l", /*         110101100010cccccjjjaaaaa1010011 */ 0xd6200053, 0xfff0007f, riscv_rv64q_110101100010cccccjjjaaaaa1010011 },
	{ "fcvt.q.lu", /*        110101100011cccccjjjaaaaa1010011 */ 0xd6300053, 0xfff0007f, riscv_rv64q_110101100011cccccjjjaaaaa1010011 },
	{ "fcvt.l.h", /*         110001000010cccccjjjaaaaa1010011 */ 0xc4200053, 0xfff0007f, riscv_rv64zfh_110001000010cccccjjjaaaaa1010011 },
	{ "fcvt.lu.h", /*        110001000011cccccjjjaaaaa1010011 */ 0xc4300053, 0xfff0007f, riscv_rv64zfh_110001000011cccccjjjaaaaa1010011 },
	{ "fcvt.h.l", /*         110101000010cccccjjjaaaaa1010011 */ 0xd4200053, 0xfff0007f, riscv_rv64zfh_110101000010cccccjjjaaaaa1010011 },
	{ "fcvt.h.lu", /*        110101000011cccccjjjaaaaa1010011 */ 0xd4300053, 0xfff0007f, riscv_rv64zfh_110101000011cccccjjjaaaaa1010011 },
	{ "fsflags", /*          000000000001ccccc001aaaaa1110011 */ 0x00101073, 0xfff0707f, riscv_pseudo_000000000001ccccc001aaaaa1110011 },
	{ "fsflagsi", /*         000000000001sssss101aaaaa1110011 */ 0x00105073, 0xfff0707f, riscv_pseudo_000000000001sssss101aaaaa1110011 },
	{ "fsrm", /*             000000000010ccccc001aaaaa1110011 */ 0x00201073, 0xfff0707f, riscv_pseudo_000000000010ccccc001aaaaa1110011 },
	{ "fsrmi", /*            000000000010sssss101aaaaa1110011 */ 0x00205073, 0xfff0707f, riscv_pseudo_000000000010sssss101aaaaa1110011 },
	{ "fscsr", /*            000000000011ccccc001aaaaa1110011 */ 0x00301073, 0xfff0707f, riscv_pseudo_000000000011ccccc001aaaaa1110011 },
	{ "fmv.x.s", /*          111000000000ccccc000aaaaa1010011 */ 0xe0000053, 0xfff0707f, riscv_pseudo_111000000000ccccc000aaaaa1010011 },
	{ "fmv.s.x", /*          111100000000ccccc000aaaaa1010011 */ 0xf0000053, 0xfff0707f, riscv_pseudo_111100000000ccccc000aaaaa1010011 },
	{ "fence.tso", /*        10000011001100000000000000001111 */ 0x8330000f, 0xfff0707f, riscv_unique },
	{ "clz", /*              011000000000ccccc001aaaaa0010011 */ 0x60001013, 0xfff0707f, riscv_rv32b_011000000000ccccc001aaaaa0010011 },
	{ "ctz", /*              011000000001ccccc001aaaaa0010011 */ 0x60101013, 0xfff0707f, riscv_rv32b_011000000001ccccc001aaaaa0010011 },
	{ "cpop", /*             011000000010ccccc001aaaaa0010011 */ 0x60201013, 0xfff0707f, riscv_rv32b_011000000010ccccc001aaaaa0010011 },
	{ "sext.b", /*           011000000100ccccc001aaaaa0010011 */ 0x60401013, 0xfff0707f, riscv_rv32b_011000000100ccccc001aaaaa0010011 },
	{ "sext.h", /*           011000000101ccccc001aaaaa0010011 */ 0x60501013, 0xfff0707f, riscv_rv32b_011000000101ccccc001aaaaa0010011 },
	{ "crc32.b", /*          011000010000ccccc001aaaaa0010011 */ 0x61001013, 0xfff0707f, riscv_rv32b_011000010000ccccc001aaaaa0010011 },
	{ "crc32.h", /*          011000010001ccccc001aaaaa0010011 */ 0x61101013, 0xfff0707f, riscv_rv32b_011000010001ccccc001aaaaa0010011 },
	{ "crc32.w", /*          011000010010ccccc001aaaaa0010011 */ 0x61201013, 0xfff0707f, riscv_rv32b_011000010010ccccc001aaaaa0010011 },
	{ "crc32c.b", /*         011000011000ccccc001aaaaa0010011 */ 0x61801013, 0xfff0707f, riscv_rv32b_011000011000ccccc001aaaaa0010011 },
	{ "crc32c.h", /*         011000011001ccccc001aaaaa0010011 */ 0x61901013, 0xfff0707f, riscv_rv32b_011000011001ccccc001aaaaa0010011 },
	{ "crc32c.w", /*         011000011010ccccc001aaaaa0010011 */ 0x61a01013, 0xfff0707f, riscv_rv32b_011000011010ccccc001aaaaa0010011 },
	{ "fclass.d", /*         111000100000ccccc001aaaaa1010011 */ 0xe2001053, 0xfff0707f, riscv_rv32d_111000100000ccccc001aaaaa1010011 },
	{ "fmv.x.w", /*          111000000000ccccc000aaaaa1010011 */ 0xe0000053, 0xfff0707f, riscv_rv32f_111000000000ccccc000aaaaa1010011 },
	{ "fclass.s", /*         111000000000ccccc001aaaaa1010011 */ 0xe0001053, 0xfff0707f, riscv_rv32f_111000000000ccccc001aaaaa1010011 },
	{ "fmv.w.x", /*          111100000000ccccc000aaaaa1010011 */ 0xf0000053, 0xfff0707f, riscv_rv32f_111100000000ccccc000aaaaa1010011 },
	{ "hlv.b", /*            011000000000ccccc100aaaaa1110011 */ 0x60004073, 0xfff0707f, riscv_rv32h_011000000000ccccc100aaaaa1110011 },
	{ "hlv.bu", /*           011000000001ccccc100aaaaa1110011 */ 0x60104073, 0xfff0707f, riscv_rv32h_011000000001ccccc100aaaaa1110011 },
	{ "hlv.h", /*            011001000000ccccc100aaaaa1110011 */ 0x64004073, 0xfff0707f, riscv_rv32h_011001000000ccccc100aaaaa1110011 },
	{ "hlv.hu", /*           011001000001ccccc100aaaaa1110011 */ 0x64104073, 0xfff0707f, riscv_rv32h_011001000001ccccc100aaaaa1110011 },
	{ "hlvx.hu", /*          011001000011ccccc100aaaaa1110011 */ 0x64304073, 0xfff0707f, riscv_rv32h_011001000011ccccc100aaaaa1110011 },
	{ "hlv.w", /*            011010000000ccccc100aaaaa1110011 */ 0x68004073, 0xfff0707f, riscv_rv32h_011010000000ccccc100aaaaa1110011 },
	{ "hlvx.wu", /*          011010000011ccccc100aaaaa1110011 */ 0x68304073, 0xfff0707f, riscv_rv32h_011010000011ccccc100aaaaa1110011 },
	{ "fclass.q", /*         111001100000ccccc001aaaaa1010011 */ 0xe6001053, 0xfff0707f, riscv_rv32q_111001100000ccccc001aaaaa1010011 },
	{ "fmv.x.h", /*          111001000000ccccc000aaaaa1010011 */ 0xe4000053, 0xfff0707f, riscv_rv32zfh_111001000000ccccc000aaaaa1010011 },
	{ "fclass.h", /*         111001000000ccccc001aaaaa1010011 */ 0xe4001053, 0xfff0707f, riscv_rv32zfh_111001000000ccccc001aaaaa1010011 },
	{ "fmv.h.x", /*          111101000000ccccc000aaaaa1010011 */ 0xf4000053, 0xfff0707f, riscv_rv32zfh_111101000000ccccc000aaaaa1010011 },
	{ "bmatflip", /*         011000000011ccccc001aaaaa0010011 */ 0x60301013, 0xfff0707f, riscv_rv64b_011000000011ccccc001aaaaa0010011 },
	{ "crc32.d", /*          011000010011ccccc001aaaaa0010011 */ 0x61301013, 0xfff0707f, riscv_rv64b_011000010011ccccc001aaaaa0010011 },
	{ "crc32c.d", /*         011000011011ccccc001aaaaa0010011 */ 0x61b01013, 0xfff0707f, riscv_rv64b_011000011011ccccc001aaaaa0010011 },
	{ "clzw", /*             011000000000ccccc001aaaaa0011011 */ 0x6000101b, 0xfff0707f, riscv_rv64b_011000000000ccccc001aaaaa0011011 },
	{ "ctzw", /*             011000000001ccccc001aaaaa0011011 */ 0x6010101b, 0xfff0707f, riscv_rv64b_011000000001ccccc001aaaaa0011011 },
	{ "cpopw", /*            011000000010ccccc001aaaaa0011011 */ 0x6020101b, 0xfff0707f, riscv_rv64b_011000000010ccccc001aaaaa0011011 },
	{ "fmv.x.d", /*          111000100000ccccc000aaaaa1010011 */ 0xe2000053, 0xfff0707f, riscv_rv64d_111000100000ccccc000aaaaa1010011 },
	{ "fmv.d.x", /*          111100100000ccccc000aaaaa1010011 */ 0xf2000053, 0xfff0707f, riscv_rv64d_111100100000ccccc000aaaaa1010011 },
	{ "hlv.wu", /*           011010000001ccccc100aaaaa1110011 */ 0x68104073, 0xfff0707f, riscv_rv64h_011010000001ccccc100aaaaa1110011 },
	{ "hlv.d", /*            011011000000ccccc100aaaaa1110011 */ 0x6c004073, 0xfff0707f, riscv_rv64h_011011000000ccccc100aaaaa1110011 },
	{ "aes64im", /*          001100000000ccccc001aaaaa0010011 */ 0x30001013, 0xfff0707f, riscv_rv64k_001100000000ccccc001aaaaa0010011 },
	{ "sha512sum0", /*       000100000100ccccc001aaaaa0010011 */ 0x10401013, 0xfff0707f, riscv_rv64k_000100000100ccccc001aaaaa0010011 },
	{ "sha512sum1", /*       000100000101ccccc001aaaaa0010011 */ 0x10501013, 0xfff0707f, riscv_rv64k_000100000101ccccc001aaaaa0010011 },
	{ "sha512sig0", /*       000100000110ccccc001aaaaa0010011 */ 0x10601013, 0xfff0707f, riscv_rv64k_000100000110ccccc001aaaaa0010011 },
	{ "sha512sig1", /*       000100000111ccccc001aaaaa0010011 */ 0x10701013, 0xfff0707f, riscv_rv64k_000100000111ccccc001aaaaa0010011 },
	{ "sm3p0", /*            000100001000ccccc001aaaaa0010011 */ 0x10801013, 0xfff0707f, riscv_rvk_000100001000ccccc001aaaaa0010011 },
	{ "sm3p1", /*            000100001001ccccc001aaaaa0010011 */ 0x10901013, 0xfff0707f, riscv_rvk_000100001001ccccc001aaaaa0010011 },
	{ "sha256sum0", /*       000100000000ccccc001aaaaa0010011 */ 0x10001013, 0xfff0707f, riscv_rvk_000100000000ccccc001aaaaa0010011 },
	{ "sha256sum1", /*       000100000001ccccc001aaaaa0010011 */ 0x10101013, 0xfff0707f, riscv_rvk_000100000001ccccc001aaaaa0010011 },
	{ "sha256sig0", /*       000100000010ccccc001aaaaa0010011 */ 0x10201013, 0xfff0707f, riscv_rvk_000100000010ccccc001aaaaa0010011 },
	{ "sha256sig1", /*       000100000011ccccc001aaaaa0010011 */ 0x10301013, 0xfff0707f, riscv_rvk_000100000011ccccc001aaaaa0010011 },
	{ "clrs8", /*            101011100000ccccc000aaaaa1110111 */ 0xae000077, 0xfff0707f, riscv_rvp_101011100000ccccc000aaaaa1110111 },
	{ "clrs16", /*           101011101000ccccc000aaaaa1110111 */ 0xae800077, 0xfff0707f, riscv_rvp_101011101000ccccc000aaaaa1110111 },
	{ "clrs32", /*           101011111000ccccc000aaaaa1110111 */ 0xaf800077, 0xfff0707f, riscv_rvp_101011111000ccccc000aaaaa1110111 },
	{ "clo8", /*             101011100011ccccc000aaaaa1110111 */ 0xae300077, 0xfff0707f, riscv_rvp_101011100011ccccc000aaaaa1110111 },
	{ "clo16", /*            101011101011ccccc000aaaaa1110111 */ 0xaeb00077, 0xfff0707f, riscv_rvp_101011101011ccccc000aaaaa1110111 },
	{ "clo32", /*            101011111011ccccc000aaaaa1110111 */ 0xafb00077, 0xfff0707f, riscv_rvp_101011111011ccccc000aaaaa1110111 },
	{ "clz8", /*             101011100001ccccc000aaaaa1110111 */ 0xae100077, 0xfff0707f, riscv_rvp_101011100001ccccc000aaaaa1110111 },
	{ "clz16", /*            101011101001ccccc000aaaaa1110111 */ 0xae900077, 0xfff0707f, riscv_rvp_101011101001ccccc000aaaaa1110111 },
	{ "clz32", /*            101011111001ccccc000aaaaa1110111 */ 0xaf900077, 0xfff0707f, riscv_rvp_101011111001ccccc000aaaaa1110111 },
	{ "kabs8", /*            101011010000ccccc000aaaaa1110111 */ 0xad000077, 0xfff0707f, riscv_rvp_101011010000ccccc000aaaaa1110111 },
	{ "kabs16", /*           101011010001ccccc000aaaaa1110111 */ 0xad100077, 0xfff0707f, riscv_rvp_101011010001ccccc000aaaaa1110111 },
	{ "kabsw", /*            101011010100ccccc000aaaaa1110111 */ 0xad400077, 0xfff0707f, riscv_rvp_101011010100ccccc000aaaaa1110111 },
	{ "sunpkd810", /*        101011001000ccccc000aaaaa1110111 */ 0xac800077, 0xfff0707f, riscv_rvp_101011001000ccccc000aaaaa1110111 },
	{ "sunpkd820", /*        101011001001ccccc000aaaaa1110111 */ 0xac900077, 0xfff0707f, riscv_rvp_101011001001ccccc000aaaaa1110111 },
	{ "sunpkd830", /*        101011001010ccccc000aaaaa1110111 */ 0xaca00077, 0xfff0707f, riscv_rvp_101011001010ccccc000aaaaa1110111 },
	{ "sunpkd831", /*        101011001011ccccc000aaaaa1110111 */ 0xacb00077, 0xfff0707f, riscv_rvp_101011001011ccccc000aaaaa1110111 },
	{ "sunpkd832", /*        101011010011ccccc000aaaaa1110111 */ 0xad300077, 0xfff0707f, riscv_rvp_101011010011ccccc000aaaaa1110111 },
	{ "swap8", /*            101011011000ccccc000aaaaa1110111 */ 0xad800077, 0xfff0707f, riscv_rvp_101011011000ccccc000aaaaa1110111 },
	{ "zunpkd810", /*        101011001100ccccc000aaaaa1110111 */ 0xacc00077, 0xfff0707f, riscv_rvp_101011001100ccccc000aaaaa1110111 },
	{ "zunpkd820", /*        101011001101ccccc000aaaaa1110111 */ 0xacd00077, 0xfff0707f, riscv_rvp_101011001101ccccc000aaaaa1110111 },
	{ "zunpkd830", /*        101011001110ccccc000aaaaa1110111 */ 0xace00077, 0xfff0707f, riscv_rvp_101011001110ccccc000aaaaa1110111 },
	{ "zunpkd831", /*        101011001111ccccc000aaaaa1110111 */ 0xacf00077, 0xfff0707f, riscv_rvp_101011001111ccccc000aaaaa1110111 },
	{ "zunpkd832", /*        101011010111ccccc000aaaaa1110111 */ 0xad700077, 0xfff0707f, riscv_rvp_101011010111ccccc000aaaaa1110111 },
	{ "kabs32", /*           101011010010ccccc000aaaaa1110111 */ 0xad200077, 0xfff0707f, riscv_rvp_101011010010ccccc000aaaaa1110111 },
	{ "vlm.v", /*            000000101011ccccc000DDDDD0000111 */ 0x02b00007, 0xfff0707f, riscv_rvv_000000101011ccccc000DDDDD0000111 },
	{ "vsm.v", /*            000000101011ccccc000EEEEE0100111 */ 0x02b00027, 0xfff0707f, riscv_rvv_000000101011ccccc000EEEEE0100111 },
	{ "vl1re8.v", /*         000000101000ccccc000DDDDD0000111 */ 0x02800007, 0xfff0707f, riscv_rvv_000000101000ccccc000DDDDD0000111 },
	{ "vl1re16.v", /*        000000101000ccccc101DDDDD0000111 */ 0x02805007, 0xfff0707f, riscv_rvv_000000101000ccccc101DDDDD0000111 },
	{ "vl1re32.v", /*        000000101000ccccc110DDDDD0000111 */ 0x02806007, 0xfff0707f, riscv_rvv_000000101000ccccc110DDDDD0000111 },
	{ "vl1re64.v", /*        000000101000ccccc111DDDDD0000111 */ 0x02807007, 0xfff0707f, riscv_rvv_000000101000ccccc111DDDDD0000111 },
	{ "vl2re8.v", /*         001000101000ccccc000DDDDD0000111 */ 0x22800007, 0xfff0707f, riscv_rvv_001000101000ccccc000DDDDD0000111 },
	{ "vl2re16.v", /*        001000101000ccccc101DDDDD0000111 */ 0x22805007, 0xfff0707f, riscv_rvv_001000101000ccccc101DDDDD0000111 },
	{ "vl2re32.v", /*        001000101000ccccc110DDDDD0000111 */ 0x22806007, 0xfff0707f, riscv_rvv_001000101000ccccc110DDDDD0000111 },
	{ "vl2re64.v", /*        001000101000ccccc111DDDDD0000111 */ 0x22807007, 0xfff0707f, riscv_rvv_001000101000ccccc111DDDDD0000111 },
	{ "vl4re8.v", /*         011000101000ccccc000DDDDD0000111 */ 0x62800007, 0xfff0707f, riscv_rvv_011000101000ccccc000DDDDD0000111 },
	{ "vl4re16.v", /*        011000101000ccccc101DDDDD0000111 */ 0x62805007, 0xfff0707f, riscv_rvv_011000101000ccccc101DDDDD0000111 },
	{ "vl4re32.v", /*        011000101000ccccc110DDDDD0000111 */ 0x62806007, 0xfff0707f, riscv_rvv_011000101000ccccc110DDDDD0000111 },
	{ "vl4re64.v", /*        011000101000ccccc111DDDDD0000111 */ 0x62807007, 0xfff0707f, riscv_rvv_011000101000ccccc111DDDDD0000111 },
	{ "vl8re8.v", /*         111000101000ccccc000DDDDD0000111 */ 0xe2800007, 0xfff0707f, riscv_rvv_111000101000ccccc000DDDDD0000111 },
	{ "vl8re16.v", /*        111000101000ccccc101DDDDD0000111 */ 0xe2805007, 0xfff0707f, riscv_rvv_111000101000ccccc101DDDDD0000111 },
	{ "vl8re32.v", /*        111000101000ccccc110DDDDD0000111 */ 0xe2806007, 0xfff0707f, riscv_rvv_111000101000ccccc110DDDDD0000111 },
	{ "vl8re64.v", /*        111000101000ccccc111DDDDD0000111 */ 0xe2807007, 0xfff0707f, riscv_rvv_111000101000ccccc111DDDDD0000111 },
	{ "vs1r.v", /*           000000101000ccccc000EEEEE0100111 */ 0x02800027, 0xfff0707f, riscv_rvv_000000101000ccccc000EEEEE0100111 },
	{ "vs2r.v", /*           001000101000ccccc000EEEEE0100111 */ 0x22800027, 0xfff0707f, riscv_rvv_001000101000ccccc000EEEEE0100111 },
	{ "vs4r.v", /*           011000101000ccccc000EEEEE0100111 */ 0x62800027, 0xfff0707f, riscv_rvv_011000101000ccccc000EEEEE0100111 },
	{ "vs8r.v", /*           111000101000ccccc000EEEEE0100111 */ 0xe2800027, 0xfff0707f, riscv_rvv_111000101000ccccc000EEEEE0100111 },
	{ "vfmv.s.f", /*         010000100000ccccc101DDDDD1010111 */ 0x42005057, 0xfff0707f, riscv_rvv_010000100000ccccc101DDDDD1010111 },
	{ "vfmv.v.f", /*         010111100000ccccc101DDDDD1010111 */ 0x5e005057, 0xfff0707f, riscv_rvv_010111100000ccccc101DDDDD1010111 },
	{ "vmv.v.x", /*          010111100000ccccc100DDDDD1010111 */ 0x5e004057, 0xfff0707f, riscv_rvv_010111100000ccccc100DDDDD1010111 },
	{ "vmv.v.v", /*          010111100000FFFFF000DDDDD1010111 */ 0x5e000057, 0xfff0707f, riscv_rvv_010111100000FFFFF000DDDDD1010111 },
	{ "vmv.v.i", /*          010111100000LLLLL011DDDDD1010111 */ 0x5e003057, 0xfff0707f, riscv_rvv_010111100000LLLLL011DDDDD1010111 },
	{ "vmv.s.x", /*          010000100000ccccc110DDDDD1010111 */ 0x42006057, 0xfff0707f, riscv_rvv_010000100000ccccc110DDDDD1010111 },
	{ "vl1r.v", /*           000000101000ccccc000DDDDD0000111 */ 0x02800007, 0xfff0707f, riscv_rvv_000000101000ccccc000DDDDD0000111 },
	{ "vl2r.v", /*           000001101000ccccc101DDDDD0000111 */ 0x06805007, 0xfff0707f, riscv_rvv_000001101000ccccc101DDDDD0000111 },
	{ "vl4r.v", /*           000011101000ccccc110DDDDD0000111 */ 0x0e806007, 0xfff0707f, riscv_rvv_000011101000ccccc110DDDDD0000111 },
	{ "vl8r.v", /*           000111101000ccccc111DDDDD0000111 */ 0x1e807007, 0xfff0707f, riscv_rvv_000111101000ccccc111DDDDD0000111 },
	{ "vle1.v", /*           000000101011ccccc000DDDDD0000111 */ 0x02b00007, 0xfff0707f, riscv_rvv_000000101011ccccc000DDDDD0000111 },
	{ "vse1.v", /*           000000101011ccccc000EEEEE0100111 */ 0x02b00027, 0xfff0707f, riscv_rvv_000000101011ccccc000EEEEE0100111 },
	{ "cbo.clean", /*        000000000001ccccc010000000001111 */ 0x0010200f, 0xfff07fff, riscv_zicbo_000000000001ccccc010000000001111 },
	{ "cbo.flush", /*        000000000010ccccc010000000001111 */ 0x0020200f, 0xfff07fff, riscv_zicbo_000000000010ccccc010000000001111 },
	{ "cbo.inval", /*        000000000000ccccc010000000001111 */ 0x0000200f, 0xfff07fff, riscv_zicbo_000000000000ccccc010000000001111 },
	{ "cbo.zero", /*         000000000100ccccc010000000001111 */ 0x0040200f, 0xfff07fff, riscv_zicbo_000000000100ccccc010000000001111 },
	{ "frflags", /*          00000000000100000010aaaaa1110011 */ 0x00102073, 0xfffff07f, riscv_pseudo_00000000000100000010aaaaa1110011 },
	{ "frrm", /*             00000000001000000010aaaaa1110011 */ 0x00202073, 0xfffff07f, riscv_pseudo_00000000001000000010aaaaa1110011 },
	{ "frcsr", /*            00000000001100000010aaaaa1110011 */ 0x00302073, 0xfffff07f, riscv_pseudo_00000000001100000010aaaaa1110011 },
	{ "rdcycle", /*          11000000000000000010aaaaa1110011 */ 0xc0002073, 0xfffff07f, riscv_pseudo_11000000000000000010aaaaa1110011 },
	{ "rdtime", /*           11000000000100000010aaaaa1110011 */ 0xc0102073, 0xfffff07f, riscv_pseudo_11000000000100000010aaaaa1110011 },
	{ "rdinstret", /*        11000000001000000010aaaaa1110011 */ 0xc0202073, 0xfffff07f, riscv_pseudo_11000000001000000010aaaaa1110011 },
	{ "rdcycleh", /*         11001000000000000010aaaaa1110011 */ 0xc8002073, 0xfffff07f, riscv_pseudo_11001000000000000010aaaaa1110011 },
	{ "rdtimeh", /*          11001000000100000010aaaaa1110011 */ 0xc8102073, 0xfffff07f, riscv_pseudo_11001000000100000010aaaaa1110011 },
	{ "rdinstreth", /*       11001000001000000010aaaaa1110011 */ 0xc8202073, 0xfffff07f, riscv_pseudo_11001000001000000010aaaaa1110011 },
	{ "scall", /*            00000000000000000000000001110011 */ 0x00000073, 0xffffffff, riscv_unique },
	{ "sbreak", /*           00000000000100000000000001110011 */ 0x00100073, 0xffffffff, riscv_unique },
	{ "pause", /*            00000001000000000000000000001111 */ 0x0100000f, 0xffffffff, riscv_unique },
	{ "sfence.w.inval", /*   00011000000000000000000001110011 */ 0x18000073, 0xffffffff, riscv_unique },
	{ "sfence.inval.ir", /*  00011000000100000000000001110011 */ 0x18100073, 0xffffffff, riscv_unique },
	{ "ecall", /*            00000000000000000000000001110011 */ 0x00000073, 0xffffffff, riscv_unique },
	{ "ebreak", /*           00000000000100000000000001110011 */ 0x00100073, 0xffffffff, riscv_unique },
	{ "sret", /*             00010000001000000000000001110011 */ 0x10200073, 0xffffffff, riscv_unique },
	{ "mret", /*             00110000001000000000000001110011 */ 0x30200073, 0xffffffff, riscv_unique },
	{ "dret", /*             01111011001000000000000001110011 */ 0x7b200073, 0xffffffff, riscv_unique },
	{ "wfi", /*              00010000010100000000000001110011 */ 0x10500073, 0xffffffff, riscv_unique },
}

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


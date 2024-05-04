/** https://www.masswerk.at/6502/assembler.html */

#pragma once

#include <unordered_map>
#include <array>
#include <string>
#include <utility>

#include "types.hpp"

enum AddrMode {
		IM, ZP, ZPX,
		ZPY, AB, ABX,
		ABY, IN, INX, INY,
		A, REL, INVALID
};

static std::array<std::array<AddrMode, 8>, 3> groups = {
	std::array{ IM,		ZP,	INVALID,	AB,	INVALID,	ZPX,	INVALID,	ABX }, // group 3: cc=00
	std::array{ INX,	ZP,	IM,			AB,	INY,		ZPX,	ABY,		ABX }, // group 1: cc=01
	std::array{ IM,		ZP,	A,		AB,	INVALID,	ZPX,	INVALID,	ABX }, // group 2: cc=10
};

// consider moving the JMP instruction to a separate entry in the ungrouped table

static const std::unordered_map<const char*, std::pair<byte, byte>> base_opcodes = {
	// first group, cc = 01
	{ "ora", { 0b00000000, 0b01 } },
	{ "and", { 0b00100000, 0b01 } },
	{ "eor", { 0b01000000, 0b01 } },
	{ "adc", { 0b01100000, 0b01 } },
	{ "sta", { 0b10000000, 0b01 } },
	{ "lda", { 0b10100000, 0b01 } },
	{ "cmp", { 0b11000000, 0b01 } },
	{ "sbc", { 0b11100000, 0b01 } },
	// first group, cc = 10
	{ "asl", { 0b00000000, 0b10 } },
	{ "rol", { 0b00100000, 0b10 } },
	{ "lsr", { 0b01000000, 0b10 } },
	{ "ror", { 0b01100000, 0b10 } },
	{ "stx", { 0b10000000, 0b10 } },
	{ "ldx", { 0b10100000, 0b10 } },
	{ "dec", { 0b11000000, 0b10 } },
	{ "inc", { 0b11100000, 0b10 } },
	// first group, cc = 00
	{ "bit", { 0b00100000, 0b00 } },
	{ "jmp", { 0b01000000, 0b00 } },
	{ "JMP", { 0b01100000, 0b00 } }, // absolute
	{ "sty", { 0b10000000, 0b00 } },
	{ "ldy", { 0b10100000, 0b00 } },
	{ "cpy", { 0b11000000, 0b00 } },
	{ "cpx", { 0b11100000, 0b00 } },
};

static const std::unordered_map<const char*, byte> ungrouped_base_opcodes = {
	// conditional branching
	{ "bpl", 0x10 }, { "bmi", 0x30 }, { "bvc", 0x50 }, { "bvs", 0x70 }, { "bcc", 0x90 }, { "gcs", 0xB0 }, { "bne", 0xD0 }, { "beq", 0xF0 },
	// interrupts and subroutines
	{ "brk", 0x00 }, { "jsr", 0x20 }, { "rti", 0x40 }, { "rts", 0x60 },
	// other
	{ "php", 0x08 }, { "plp", 0x28 }, { "pha", 0x48 }, { "pla", 0x68 }, { "dey", 0x88 }, { "tay", 0xA8 }, { "iny", 0xC8 }, { "inx", 0xE8 },
	{ "cls", 0x18 }, { "sec", 0x38 }, { "cli", 0x58 }, { "sei", 0x78 }, { "tya", 0x98 }, { "clv", 0xB8 }, { "cld", 0xD8 }, { "sed", 0xF8 },
	{ "txa", 0x8A }, { "txs", 0x9A }, { "tax", 0xAA }, { "tsx", 0xBA }, { "dex", 0xCA }, { "nop", 0xEA },
};

// std::array<byte, 151> calculate_valid_opcodes() {
// 	std::array<byte,151> res{};
// 	int i{};
// 	for (auto entry : base_opcodes) {
// 		res[i] = entry.second.first;
// 		i++;
// 	}
// }

// static std::array<byte, 151> valid_opcodes = calculate_valid_opcodes();



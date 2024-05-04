#pragma once

#include <unordered_map>
#include <array>

enum AddrMode {
		IM, ZP, ZPX,
		ZPY, AB, ABX,
		ABY, IN, INX, INY,
		IMPL, REL, INVALID
};

static std::array<std::array<AddrMode, 8>, 3> groups = {
	std::array{ IM,		ZP,	INVALID,	AB,	INVALID,	ZPX,	INVALID,	ABX }, // group 3: cc=00
	std::array{ INX,	ZP,	IM,			AB,	INY,		ZPX,	ABY,		ABX }, // group 1: cc=01
	std::array{ IM,		ZP,	IMPL,		AB,	INVALID,	ZPX,	INVALID,	ABX }, // group 2: cc=10
};

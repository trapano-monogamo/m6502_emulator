#pragma once

#include "types.hpp"

struct Mem {
	static constexpr u32 MAX_MEM = 1024 * 64; // 64 KB
	byte memory[MAX_MEM];

	void initialize();

	byte operator[]( u16 address ) const;
	byte& operator[]( u16 address );

	void inspect( u16 address, u16 size = 8, u16 step = 8 );
};

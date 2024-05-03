#pragma once

#include "types.hpp"
#include "memory.hpp"

struct CPU {
	word PC;	// program counter
	byte SP;	// stack pointer
	
	byte A, X, Y; // registers

	union { // processor status
		byte flags;
		struct {
			byte __unused : 1; // first bit of corresponding 8-bit masks
			byte C : 1; // carry
			byte Z : 1; // zero
			byte I : 1; // interrupt
			byte D : 1; // decimal
			byte B : 1; // break
			byte V : 1; // overflow
			byte N : 1; // negative
		};
	};


	/** memory layout constants */
	static constexpr u16 RESET_VECTOR	= 0xFFFC;
	static constexpr u16 STACK			= 0x0100;

	/** processor status switches */
	static constexpr byte CARRY_MASK 				= 0b01000000;
	static constexpr byte ZERO_MASK 				= 0b00100000;
	static constexpr byte INTERRUPT_DISABLE_MASK	= 0b00010000;
	static constexpr byte DECIMAL_MODE_MASK 		= 0b00001000;
	static constexpr byte BREAK_COMMAND_MASK 		= 0b00000100;
	static constexpr byte OVERFLOW_MASK 			= 0b00000010;
	static constexpr byte NEGATIVE_MASK 			= 0b00000001;

	/** instruction set */
	static constexpr byte INS_JSR		= 0x20;

	static constexpr byte INS_LDA_IM	= 0xA9;
	static constexpr byte INS_LDA_ZP	= 0xA5;
	static constexpr byte INS_LDA_ZPX	= 0xB5;
	static constexpr byte INS_LDA_AB	= 0xAD;
	static constexpr byte INS_LDA_ABX	= 0xBD;

	static constexpr byte INS_STA_ZP	= 0x85;

	/** fetch oeprations */
	byte fetch_byte( u32& cycles, Mem& memory );
	word fetch_word( u32& cycles, Mem& memory );

	/** read operations */
	byte read_byte( u32& cycles, u16 addr, Mem& memory);

	/** write operations */
	void write_byte( u32& cycles, byte value, u16 addr, Mem& memory );
	void write_word( u32& cycles, word value, u16 addr, Mem& memory );

	/** execution */
	void reset( Mem& memory );
	void execute( Mem& memory, u32 cycles );

	/** utility functions */
	void set_flag(byte mask, byte value);
	void toggle_flag(byte mask);

	void LDA_set_status();

	void inspect();
	void inspect( Mem& memory, u16 stack_start_offset = 0x00, u16 stack_size = 0xFF, u16 step = 8 );
};

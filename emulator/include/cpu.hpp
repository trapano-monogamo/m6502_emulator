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
			// LEAST SIGNIFICANT BIT
			byte N : 1; // negative
			byte V : 1; // overflow
			byte B : 1; // break
			byte D : 1; // decimal
			byte I : 1; // interrupt
			byte Z : 1; // zero
			byte C : 1; // carry
			byte __unused : 1; // first bit of corresponding 8-bit masks
			// MOST SIGNIFICANT BIT
		};
	};


	/** memory layout constants */
	static constexpr u16 RESET_VECTOR	= 0xFFFC; // default reset position for PC
	static constexpr u16 STACK			= 0x0100;
	// the stack goes from 0x0100 to 0x01FF, but starts at 0x01FF and goes down

	/** processor status switches */
	static constexpr byte CARRY_MASK 				= 0b01000000;
	static constexpr byte ZERO_MASK 				= 0b00100000;
	static constexpr byte INTERRUPT_DISABLE_MASK	= 0b00010000;
	static constexpr byte DECIMAL_MODE_MASK 		= 0b00001000;
	static constexpr byte BREAK_COMMAND_MASK 		= 0b00000100;
	static constexpr byte OVERFLOW_MASK 			= 0b00000010;
	static constexpr byte NEGATIVE_MASK 			= 0b00000001;

	/** instruction set */

	// load instructions
	static constexpr byte INS_LDA_IM	= 0xA9;
	static constexpr byte INS_LDA_ZP	= 0xA5;
	static constexpr byte INS_LDA_ZPX	= 0xB5;
	static constexpr byte INS_LDA_AB	= 0xAD;
	static constexpr byte INS_LDA_ABX	= 0xBD;
	static constexpr byte INS_LDA_ABY	= 0xB9;
	static constexpr byte INS_LDA_INX	= 0xA1;
	static constexpr byte INS_LDA_INY	= 0xB1;

	static constexpr byte INS_LDX_IM	= 0xA2;
	static constexpr byte INS_LDX_ZP	= 0xA6;
	static constexpr byte INS_LDX_ZPY	= 0xB6;
	static constexpr byte INS_LDX_AB	= 0xAE;
	static constexpr byte INS_LDX_ABY	= 0xBE;

	static constexpr byte INS_LDY_IM	= 0xA0;
	static constexpr byte INS_LDY_ZP	= 0xA4;
	static constexpr byte INS_LDY_ZPX	= 0xB4;
	static constexpr byte INS_LDY_AB	= 0xAC;
	static constexpr byte INS_LDY_ABX	= 0xBC;

	// store instructions
	static constexpr byte INS_STA_ZP	= 0x85;
	static constexpr byte INS_STA_ZPX	= 0x95;
	static constexpr byte INS_STA_AB	= 0x8D;
	static constexpr byte INS_STA_ABX	= 0x9D;
	static constexpr byte INS_STA_ABY	= 0x99;
	static constexpr byte INS_STA_INX	= 0x81;
	static constexpr byte INS_STA_INY	= 0x91;

	static constexpr byte INS_STX_ZP	= 0x86;
	static constexpr byte INS_STX_ZPY	= 0x96;
	static constexpr byte INS_STX_AB	= 0x8E;

	static constexpr byte INS_STY_ZP	= 0x84;
	static constexpr byte INS_STY_ZPX	= 0x94;
	static constexpr byte INS_STY_AB	= 0x8C;

	// register transfer instructions
	static constexpr byte INS_TAX		= 0xAA;
	static constexpr byte INS_TAY		= 0xA8;
	static constexpr byte INS_TXA		= 0x8A;
	static constexpr byte INS_TYA		= 0x98;

	// stack operations instructions
	static constexpr byte INS_TSX		= 0xBA;
	static constexpr byte INS_TXS		= 0x9A;
	static constexpr byte INS_PHA		= 0x48;
	static constexpr byte INS_PHP		= 0x08;
	static constexpr byte INS_PLA		= 0x68;
	static constexpr byte INS_PLP		= 0x28;

	// logical operations instructions
	static constexpr byte INS_AND_IM	= 0x29;
	static constexpr byte INS_AND_ZP	= 0x25;
	static constexpr byte INS_AND_ZPX	= 0x35;
	static constexpr byte INS_AND_AB	= 0x2D;
	static constexpr byte INS_AND_ABX	= 0x3D;
	static constexpr byte INS_AND_ABY	= 0x39;
	static constexpr byte INS_AND_INX	= 0x21;
	static constexpr byte INS_AND_INY	= 0x31;

	static constexpr byte INS_EOR_IM	= 0x49;
	static constexpr byte INS_EOR_ZP	= 0x45;
	static constexpr byte INS_EOR_ZPX	= 0x55;
	static constexpr byte INS_EOR_AB	= 0x4D;
	static constexpr byte INS_EOR_ABX	= 0x5D;
	static constexpr byte INS_EOR_ABY	= 0x59;
	static constexpr byte INS_EOR_INX	= 0x41;
	static constexpr byte INS_EOR_INY	= 0x51;

	static constexpr byte INS_ORA_IM	= 0x09;
	static constexpr byte INS_ORA_ZP	= 0x05;
	static constexpr byte INS_ORA_ZPX	= 0x15;
	static constexpr byte INS_ORA_AB	= 0x0D;
	static constexpr byte INS_ORA_ABX	= 0x1D;
	static constexpr byte INS_ORA_ABY	= 0x19;
	static constexpr byte INS_ORA_INX	= 0x01;
	static constexpr byte INS_ORA_INY	= 0x11;

	static constexpr byte INS_BIT_ZP	= 0x24;
	static constexpr byte INS_BIT_AB	= 0x2C;

	// jumps and calls
	static constexpr byte INS_JMP_AB	= 0x4C;
	static constexpr byte INS_JMP_IN	= 0x6C;
	static constexpr byte INS_JSR_AB	= 0x20;
	static constexpr byte INS_RTS		= 0x60;


	/** fetch oeprations */
	byte fetch_byte( i32& cycles, Mem& memory );
	word fetch_word( i32& cycles, Mem& memory );

	/** read operations */
	byte read_byte( i32& cycles, u16 addr, Mem& memory);
	word read_word( i32& cycles, u16 addr, Mem& memory);

	/** write operations */
	void write_byte( i32& cycles, byte value, u16 addr, Mem& memory );
	void write_word( i32& cycles, word value, u16 addr, Mem& memory );

	/** stack push/pull operations */
	void push_byte( i32& cycles, byte data, Mem& memory );
	void push_word( i32& cycles, word data, Mem& memory );
	byte pull_byte( i32& cycles, Mem& memory );
	word pull_word( i32& cycles, Mem& memory );

	/** execution */
	void reset( Mem& memory, word pc = RESET_VECTOR );
	u32 execute( Mem& memory, i32 cycles );

	/** utility functions */
	bool test_bit(byte data, u16 position);
	void set_flag(byte mask, byte value);
	void toggle_flag(byte mask);

	void set_register_status(byte reg);

	void check_page_cross(i32& cycles, u16 addr, byte reg);

	void inspect();
	void inspect( Mem& memory, u16 stack_start_offset = 0x00, u16 stack_size = 0xFF, u16 step = 8 );
};

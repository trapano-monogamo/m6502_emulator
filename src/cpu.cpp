#include "cpu.hpp"
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>

void CPU::reset( Mem& memory ) {
	PC = 0xFFFC; // reset vector
	SP = 0x00;   // stack: 0x0100 - 0x01FF
	flags = 0x0;
	A = X = Y = 0x0;
	memory.initialize();
}

// fetch oeprations
byte CPU::fetch_byte( i32& cycles, Mem& memory ) {
	byte data = memory[PC];
	PC++; // fetch reads an instruction so the PC increments
	cycles--;
	return data;
}

word CPU::fetch_word( i32& cycles, Mem& memory ) {
	// m6502 is little endian
	word data = memory[PC];
	data |= (memory[PC + 1] << 8);
	// two read operations
	PC += 2;
	cycles -= 2;
	return data;
}

// read operations
byte CPU::read_byte( i32& cycles, u16 addr, Mem& memory) {
	byte data = memory[addr];
	cycles--;
	return data;
}

word CPU::read_word( i32& cycles, u16 addr, Mem& memory) {
	// m6502 is little endian
	word data = memory[addr];
	data |= (memory[addr + 1] << 8);
	// two read operations
	cycles -= 2;
	return data;
}

// write operations
void CPU::write_byte( i32& cycles, byte value, u16 addr, Mem& memory ) {
	memory[addr] = value;
	cycles--;
}

void CPU::write_word( i32& cycles, word value, u16 addr, Mem& memory ) {
	memory[addr] = (value << 8) >> 8;
	memory[addr+1] = value >> 8;
	// two write instructions
	cycles -= 2;
}

u32 CPU::execute( Mem& memory, i32 cycles ) {

	i32 initial_cycles = cycles;

	while (cycles > 0) {
		byte opcode = fetch_byte( cycles, memory );
		switch (opcode) {

			case INS_LDA_IM:
			{
				byte value = fetch_byte(cycles, memory);
				A = value;
				set_register_status(A);
			} break;

			case INS_LDA_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				byte value = read_byte(cycles, zero_page_addr, memory);
				A = value;
				set_register_status(A);
			} break;

			case INS_LDA_ZPX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				byte addr = zero_page_addr + X; // unsigned overflow wraps around
				cycles--; // addition takes one cycle
				byte value = read_byte(cycles, (u16)addr, memory);
				A = value;
				set_register_status(A);
			} break;

			case INS_LDA_AB:
			{
				word addr = fetch_word(cycles, memory);
				A = read_byte(cycles, addr, memory);
				set_register_status(A);
			} break;

			case INS_LDA_ABX:
			{
				word addr = fetch_word(cycles, memory);
				// overflow => page has been crossed
				//if (((word)(addr << 8) >> 8) + X > 0xFF) cycles--;
				check_page_cross(cycles, addr, X);
				addr += X;
				A = read_byte(cycles, addr, memory);
				set_register_status(A);
			} break;

			case INS_LDA_ABY:
			{
				word addr = fetch_word(cycles, memory);
				// overflow => page has been crossed
				//if (((word)(addr << 8) >> 8) + Y > 0xFF) cycles--;
				check_page_cross(cycles, addr, Y);
				addr += Y;
				A = read_byte(cycles, addr, memory);
				set_register_status(A);
			} break;

			case INS_LDA_INX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += X;
				cycles--;
				word addr = read_word(cycles, (u16)zero_page_addr, memory);
				byte value = read_byte(cycles, addr, memory);
				A = value;
				set_register_status(A);
			} break;

			case INS_LDA_INY:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				word addr = read_word(cycles, (u16)zero_page_addr, memory);
				//if (((word)(addr << 8) >> 8) + Y > 0xFF) cycles--;
				check_page_cross(cycles, addr, Y);
				addr += Y;
				byte value = read_byte(cycles, addr, memory);
				A = value;
				set_register_status(A);
			} break;

			case INS_LDX_IM:
			{
				byte value = fetch_byte(cycles, memory);
				X = value;
				set_register_status(X);
			} break;

			case INS_LDX_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				byte value = read_byte(cycles, zero_page_addr, memory);
				X = value;
				set_register_status(X);
			} break;

			case INS_LDX_ZPY:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += Y;
				cycles--;
				byte value = read_byte(cycles, zero_page_addr, memory);
				X = value;
				set_register_status(X);
			} break;

			case INS_LDX_AB:
			{
				word addr = fetch_word(cycles, memory);
				byte value = read_byte(cycles, addr, memory);
				X = value;
				set_register_status(X);
			} break;

			case INS_LDX_ABY:
			{
				word addr = fetch_word(cycles, memory);
				// overflow => page has been crossed
				//if (((word)(addr << 8) >> 8) + Y > 0xFF) cycles--;
				check_page_cross(cycles, addr, Y);
				addr += Y;
				X = read_byte(cycles, addr, memory);
				set_register_status(X);
			} break;

			case INS_LDY_IM:
			{
				byte value = fetch_byte(cycles, memory);
				Y = value;
				set_register_status(Y);
			} break;

			case INS_LDY_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				byte value = read_byte(cycles, zero_page_addr, memory);
				Y = value;
				set_register_status(Y);
			} break;

			case INS_LDY_ZPX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += X;
				cycles--;
				byte value = read_byte(cycles, zero_page_addr, memory);
				Y = value;
				set_register_status(Y);
			} break;

			case INS_LDY_AB:
			{
				word addr = fetch_word(cycles, memory);
				byte value = read_byte(cycles, addr, memory);
				Y = value;
				set_register_status(Y);
			} break;

			case INS_LDY_ABX:
			{
				word addr = fetch_word(cycles, memory);
				// overflow => page has been crossed
				//if (((word)(addr << 8) >> 8) + X > 0xFF) cycles--;
				check_page_cross(cycles, addr, X);
				addr += X;
				Y = read_byte(cycles, addr, memory);
				set_register_status(Y);
			} break;

			case INS_STA_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				write_byte(cycles, A, zero_page_addr, memory);
			} break;

			case INS_STA_ZPX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += X;
				cycles--;
				write_byte(cycles, A, zero_page_addr, memory);
			} break;

			case INS_STA_AB:
			{
				word addr = fetch_word(cycles, memory);
				write_byte(cycles, A, addr, memory);
			} break;

			case INS_STA_ABX:
			{
				word addr = fetch_word(cycles, memory);
				addr += X;
				cycles--;
				write_byte(cycles, A, addr, memory);
			} break;

			case INS_STA_ABY:
			{
				word addr = fetch_word(cycles, memory);
				addr += Y;
				cycles--;
				write_byte(cycles, A, addr, memory);
			} break;

			case INS_STA_INX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += X;
				cycles--;
				word addr = read_word(cycles, zero_page_addr, memory);
				write_byte(cycles, A, addr, memory);
			} break;

			case INS_STA_INY:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				word addr = read_word(cycles, zero_page_addr, memory);
				//if (((word)(addr << 8) >> 8) + Y > 0xFF) cycles--;
				check_page_cross(cycles, addr, Y);
				addr += Y;
				write_byte(cycles, A, addr, memory);
			} break;

			case INS_STX_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				write_byte(cycles, X, zero_page_addr, memory);
			} break;

			case INS_STX_ZPY:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += Y;
				cycles--;
				write_byte(cycles, X, zero_page_addr, memory);
			} break;

			case INS_STX_AB:
			{
				word addr = fetch_word(cycles, memory);
				write_byte(cycles, X, addr, memory);
			} break;

			case INS_STY_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				write_byte(cycles, Y, zero_page_addr, memory);
			} break;

			case INS_STY_ZPX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += X;
				cycles--;
				write_byte(cycles, Y, zero_page_addr, memory);
			} break;

			case INS_STY_AB:
			{
				word addr = fetch_word(cycles, memory);
				write_byte(cycles, Y, addr, memory);
			} break;

			case INS_TAX:
			{
				X = A;
				set_register_status(X);
			} break;

			case INS_TAY:
			{
				Y = A;
				set_register_status(Y);
			} break;

			case INS_TXA:
			{
				A = X;
				set_register_status(A);
			} break;

			case INS_TYA:
			{
				A = Y;
				set_register_status(A);
			} break;

			case INS_TSX:
			{
				X = SP;
				set_register_status(X);
			} break;

			case INS_TXS:
			{
				SP = X;
			} break;

			case INS_PHA:
			{
				write_byte(cycles, A, STACK + SP, memory);
				SP++;
				cycles--;
			} break;

			case INS_PLA:
			{
				SP--;
				A = read_byte(cycles, STACK + SP, memory);
				cycles -= 2; // idk where the 4th cycle comes from...
				set_register_status(A);
			} break;

			case INS_PHP:
			{
				write_byte(cycles, flags, STACK + SP, memory);
				SP++;
				cycles--;
			} break;

			case INS_PLP:
			{
				SP--;
				flags = read_byte(cycles, STACK + SP, memory);
				cycles -= 2; // idk where the 4th cycle comes from...
			} break;

			case INS_AND_IM:
			{
				byte mask = fetch_byte(cycles, memory);
				A &= mask;
				set_register_status(A);
			} break;

			case INS_AND_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				byte mask = read_byte(cycles, zero_page_addr, memory);
				A &= mask;
				set_register_status(A);
			} break;

			case INS_AND_ZPX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += X;
				cycles--;
				byte mask = read_byte(cycles, zero_page_addr, memory);
				A &= mask;
				set_register_status(A);
			} break;

			case INS_AND_AB:
			{
				word addr = fetch_word(cycles, memory);
				byte mask = read_byte(cycles, addr, memory);
				A &= mask;
				set_register_status(A);
			} break;

			case INS_AND_ABX:
			{
				word addr = fetch_word(cycles, memory);
				check_page_cross(cycles, addr, X);
				addr += X;
				byte mask = read_byte(cycles, addr, memory);
				A &= mask;
				set_register_status(A);
			} break;

			case INS_AND_ABY:
			{
				word addr = fetch_word(cycles, memory);
				check_page_cross(cycles, addr, Y);
				addr += Y;
				byte mask = read_byte(cycles, addr, memory);
				A &= mask;
				set_register_status(A);
			} break;

			case INS_AND_INX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += X;
				cycles--;
				word addr = read_word(cycles, zero_page_addr, memory);
				byte mask = read_byte(cycles, addr, memory);
				A &= mask;
				set_register_status(A);
			} break;

			case INS_AND_INY:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				word addr = read_word(cycles, zero_page_addr, memory);
				check_page_cross(cycles, addr, Y);
				addr += Y;
				byte mask = read_byte(cycles, addr, memory);
				A &= mask;
				set_register_status(A);
			} break;

			case INS_EOR_IM:
			{
				byte mask = fetch_byte(cycles, memory);
				A ^= mask;
				set_register_status(A);
			} break;

			case INS_EOR_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				byte mask = read_byte(cycles, zero_page_addr, memory);
				A ^= mask;
				set_register_status(A);
			} break;

			case INS_EOR_ZPX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += X;
				cycles--;
				byte mask = read_byte(cycles, zero_page_addr, memory);
				A ^= mask;
				set_register_status(A);
			} break;

			case INS_EOR_AB:
			{
				word addr = fetch_word(cycles, memory);
				byte mask = read_byte(cycles, addr, memory);
				A ^= mask;
				set_register_status(A);
			} break;

			case INS_EOR_ABX:
			{
				word addr = fetch_word(cycles, memory);
				check_page_cross(cycles, addr, X);
				addr += X;
				byte mask = read_byte(cycles, addr, memory);
				A ^= mask;
				set_register_status(A);
			} break;

			case INS_EOR_ABY:
			{
				word addr = fetch_word(cycles, memory);
				check_page_cross(cycles, addr, Y);
				addr += Y;
				byte mask = read_byte(cycles, addr, memory);
				A ^= mask;
				set_register_status(A);
			} break;

			case INS_EOR_INX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += X;
				cycles--;
				word addr = read_word(cycles, zero_page_addr, memory);
				byte mask = read_byte(cycles, addr, memory);
				A ^= mask;
				set_register_status(A);
			} break;

			case INS_EOR_INY:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				word addr = read_word(cycles, zero_page_addr, memory);
				check_page_cross(cycles, addr, Y);
				addr += Y;
				byte mask = read_byte(cycles, addr, memory);
				A ^= mask;
				set_register_status(A);
			} break;

			case INS_ORA_IM:
			{
				byte mask = fetch_byte(cycles, memory);
				A |= mask;
				set_register_status(A);
			} break;

			case INS_ORA_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				byte mask = read_byte(cycles, zero_page_addr, memory);
				A |= mask;
				set_register_status(A);
			} break;

			case INS_ORA_ZPX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += X;
				cycles--;
				byte mask = read_byte(cycles, zero_page_addr, memory);
				A |= mask;
				set_register_status(A);
			} break;

			case INS_ORA_AB:
			{
				word addr = fetch_word(cycles, memory);
				byte mask = read_byte(cycles, addr, memory);
				A |= mask;
				set_register_status(A);
			} break;

			case INS_ORA_ABX:
			{
				word addr = fetch_word(cycles, memory);
				check_page_cross(cycles, addr, X);
				addr += X;
				byte mask = read_byte(cycles, addr, memory);
				A |= mask;
				set_register_status(A);
			} break;

			case INS_ORA_ABY:
			{
				word addr = fetch_word(cycles, memory);
				check_page_cross(cycles, addr, Y);
				addr += Y;
				byte mask = read_byte(cycles, addr, memory);
				A |= mask;
				set_register_status(A);
			} break;

			case INS_ORA_INX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				zero_page_addr += X;
				cycles--;
				word addr = read_word(cycles, zero_page_addr, memory);
				byte mask = read_byte(cycles, addr, memory);
				A |= mask;
				set_register_status(A);
			} break;

			case INS_ORA_INY:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				word addr = read_word(cycles, zero_page_addr, memory);
				check_page_cross(cycles, addr, Y);
				addr += Y;
				byte mask = read_byte(cycles, addr, memory);
				A |= mask;
				set_register_status(A);
			} break;

			case INS_BIT_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				byte mask = read_byte(cycles, zero_page_addr, memory);
				byte res = (A & mask);
				Z = !!!res;
				V = test_bit(mask, 6);
				N = test_bit(mask, 7);
			} break;

			case INS_BIT_AB:
			{
				word addr = fetch_word(cycles, memory);
				byte mask = read_byte(cycles, addr, memory);
				byte res = (A & mask);
				Z = !!!res;
				V = test_bit(mask, 6);
				N = test_bit(mask, 7);
			} break;

			case INS_JSR:
			{
				/**
				 * when a JSR instruction is encountered, the sub_addr is pushed
				 * onto the stack (it's a stackframe containing the subrouting address),
				 * and the PC is set to the subroutine address so the CPU executes from there.
				 *
				 * PC-1 is because when the routine calls the RTS instruction
				 * (return from subroutine) it reads from the stack the value to set the PC to,
				 * but it increments it by one to avoid repeating the JSR. Overall this means
				 * that after the RTS instruction, the PC points to the instruction right after the JSR.
				 * */
				word sub_addr = fetch_word(cycles, memory);
				// push PC-1 to stack
				write_word(cycles, PC - 1, STACK + SP, memory);
				SP += 2;
				// jump
				PC = sub_addr;
				cycles--;
			} break;

			default:
			{
				// printf("Unknown instruction 0x%04x at 0x%04x\n", opcode, cpu->PC);
				std::cout << "Unknown instruction 0x"
					<< std::setfill('0') << std::setw(2) << std::hex << (u16)opcode << " at 0x"
					<< std::setfill('0') << std::setw(4) << std::hex << PC << "." << std::endl;
			} break;
		}
	}

	return initial_cycles - cycles;
}

/** utility functions */

bool CPU::test_bit(byte data, u16 position) { return !!(data & (0b1 << position)); }
void CPU::set_flag(byte mask, byte value) { this->flags ^= (mask * !!value); }
void CPU::toggle_flag(byte mask) { this->flags ^= mask; }

void CPU::set_register_status(byte reg) {
	Z = (reg == 0);
	N = !!(reg & 0b10000000);
}

void CPU::check_page_cross(i32& cycles, u16 addr, byte reg) {
	if (((word)(addr << 8) >> 8) + reg > 0xFF) cycles--;
}

void CPU::inspect() {
	std::cout
		<< "PC: 0x" << std::hex << std::setfill('0') << std::setw(4) << (u16)PC << "\t"
		<< "SP: 0x" << std::hex << std::setfill('0') << std::setw(4) << (u16)SP << "\n"
		<<  "A: 0x" << std::hex << std::setfill('0') << std::setw(2) << (u16)A << "\t"
		<<  "X: 0x" << std::hex << std::setfill('0') << std::setw(2) << (u16)X << "\t"
		<<  "Y: 0x" << std::hex << std::setfill('0') << std::setw(2) << (u16)Y << "\n"
		<<    "C: " << (u16)C
		<<  "\tZ: " << (u16)Z
		<<  "\tI: " << (u16)I
		<<  "\tD: " << (u16)D
		<<  "\tB: " << (u16)B
		<<  "\tV: " << (u16)V
		<<  "\tN: " << (u16)N << std::endl;
}

void CPU::inspect( Mem& memory, u16 stack_start_offset, u16 stack_size, u16 step ) {
	inspect();
	memory.inspect(0x0100 + stack_start_offset, stack_size, step);
}




/**
 * adds both the given constant and the carry bit to the accumulator
 * and then sets (overwrites) the carry flag as appropriate.
 * This makes it possible to perform 16bit (and higher) addition,
 * with the carry bit acting as a +1 or +0 for the hibytes.
 *
 *
 * lobyte unsigned ADC
 *          1 (carry flag)
 *   11101101 +
 *   01001001 =
 *   ---------
 *  100110111
 *  ^
 *  | carry
 *  => carry is set to one
 *
 * lobyte signed ADC:
 *          1
 *   10000001 +   -127 +
 *   00000001 =      1 =
 *   ----------   ------
 *  010000010     -126   (correct)
 *  ^
 *  | carry
 *  => carry is set to zero, negative is set to one
 *
 *          0
 *   10000000 +   -128 +
 *   11111111 =   -  1 =
 *   ----------   ------
 *  101111111      127   (wrong)
 *  ^
 *  | carry
 *  => carry is set to 1, negative is set to 0, overflow is set to 1
 *
 *          0
 *   11000000 +   - 64 +
 *   11111111 =   -  1 =
 *   ----------   ------
 *  110111111     - 65   (correct)
 *  ^
 *  | carry
 *  => carry is set to 1, negative is set to 0, overflow is set to 0
 *
 * */

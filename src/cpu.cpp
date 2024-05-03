#include "cpu.hpp"
#include <bitset>
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
byte CPU::fetch_byte( u32& cycles, Mem& memory ) {
	byte data = memory[PC];
	PC++; // fetch reads an instruction so the PC increments
	cycles--;
	return data;
}

word CPU::fetch_word( u32& cycles, Mem& memory ) {
	// m6502 is little endian
	word data = memory[PC];
	data |= (memory[PC + 1] << 8);
	// two read operations
	PC += 2;
	cycles -= 2;
	return data;
}

// read operations
byte CPU::read_byte( u32& cycles, u16 addr, Mem& memory) {
	byte data = memory[addr];
	cycles--;
	return data;
}

// write operations
void CPU::write_byte( u32& cycles, byte value, u16 addr, Mem& memory ) {
	memory[addr] = value;
	cycles--;
}

void CPU::write_word( u32& cycles, word value, u16 addr, Mem& memory ) {
	memory[addr] = (value << 8) >> 8;
	memory[addr+1] = value >> 8;
	// two write instructions
	cycles -= 2;
}

void CPU::execute( Mem& memory, u32 cycles ) {

	while (cycles > 0) {
		byte opcode = fetch_byte( cycles, memory );
		switch (opcode) {

			case INS_LDA_IM:
			{
				byte value = fetch_byte(cycles, memory);
				A = value;
				LDA_set_status();
			} break;

			case INS_LDA_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				byte value = read_byte(cycles, zero_page_addr, memory);
				A = value;
				LDA_set_status();
			} break;

			case INS_LDA_ZPX:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				u16 addr = zero_page_addr + X; // unsigned overflow wraps around
				cycles--; // addition takes one cycle
				byte value = read_byte(cycles, addr, memory);
				A = value;
				LDA_set_status();
			} break;

			case INS_LDA_AB:
			{
				word addr = fetch_word(cycles, memory);
				A = read_byte(cycles, addr, memory);
			} break;

			case INS_LDA_ABX:
			{
				word addr = fetch_word(cycles, memory);
				// overflow => page has been crossed
				if (((word)(addr << 8) >> 8) + X > 0xFF) cycles--;
				addr += X;
				A = read_byte(cycles, addr, memory);
			} break;

			case INS_STA_ZP:
			{
				byte zero_page_addr = fetch_byte(cycles, memory);
				write_byte(cycles, A, zero_page_addr, memory);
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

}

/** utility functions */

void CPU::set_flag(byte mask, byte value) { this->flags ^= (mask * !!value); }
void CPU::toggle_flag(byte mask) { this->flags ^= mask; }

void CPU::LDA_set_status() {
	Z = (A == 0);
	N = A & 0b10000000;
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

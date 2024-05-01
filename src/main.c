/** http://www.6502.org/users/obelisk/6502/index.html */

#include "stdio.h"
#include "stdlib.h"
#include <float.h>

typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char byte;
typedef unsigned short word;

// 64 KB maximum memory
#define MAX_MEM 65536

// processor status switches
#define CARRY_MASK 				0b01000000
#define ZERO_MASK 				0b00100000
#define INTERRUPT_DISABLE_MASK	0b00010000
#define DECIMAL_MODE_MASK 		0b00001000
#define BREAK_COMMAND_MASK 		0b00000100
#define OVERFLOW_MASK 			0b00000010
#define NEGATIVE_MASK 			0b00000001

// instruction set
#define INS_LDA_IM  0xA9
#define INS_LDA_ZP  0xA5
#define INS_JSR		0x20

typedef struct {
	word PC;	// program counter
	word SP;	// stack pointer
	
	byte A, X, Y; // registers

	byte flags; // processor status
}CPU;

void cpu_toggle_flag(CPU* cpu, byte mask) {
	cpu->flags ^= mask;
}

void cpu_set_flag(CPU* cpu, byte mask, byte value) {
	cpu->flags ^= (mask * !!value);
}


void initialize_memory(byte* memory) {
	for (u32 i=0; i<MAX_MEM; i++) memory[i] = 0x0;
}

void inspect_memory( byte* memory, u16 address, u16 size, u16 step ) {
	for (int addr=address; addr<address+size; addr++) {
		if ((addr-address) % step == 0)
			printf("\n 0x%04x - 0x%04x\t", addr, addr+step);
		printf("%02x ", (u16)memory[addr]);
	}
	printf("\n");
}

void inspect(CPU* cpu) {
	printf("PC: 0x%04x\tSP: 0x%04x\nA: %x\tX: %x\tY: %x\nC: %x, Z: %x, I: %x, D: %x, B: %x, V: %x, N: %x\n",
			cpu->PC, cpu->SP,
			cpu->A, cpu->X, cpu->Y,
			!!(cpu->flags & CARRY_MASK),
			!!(cpu->flags & ZERO_MASK),
			!!(cpu->flags & INTERRUPT_DISABLE_MASK),
			!!(cpu->flags & DECIMAL_MODE_MASK),
			!!(cpu->flags & BREAK_COMMAND_MASK),
			!!(cpu->flags & OVERFLOW_MASK),
			!!(cpu->flags & NEGATIVE_MASK));
}


void reset( CPU* cpu, byte* memory ) {
	cpu->PC = 0xFFFC;
	cpu->SP = 0x0100;

	cpu->flags = 0x0;
	cpu->A = cpu->X = cpu->Y = 0x0;

	initialize_memory(memory);
}

byte fetch_byte( CPU* cpu, u32* cycles, byte* memory ) {
	byte data = memory[cpu->PC];
	cpu->PC++; // fetch reads an instruction so the PC increments
	(*cycles)--;
	return data;
}

word fetch_word( CPU* cpu, u32* cycles, byte* memory ) {
	// m6502 is little endian
	word data = memory[cpu->PC];
	data |= (memory[cpu->PC + 1] << 8);
	// two read operations
	cpu->PC += 2;
	(*cycles) -= 2;
	return data;
}

byte read_byte( u32* cycles, u16 addr, byte* memory) {
	byte data = memory[addr];
	(*cycles)--;
	return data;
}

void write_word( CPU* cpu, u32* cycles, word value, u32 addr, byte* memory ) {
	memory[addr] = (value << 8) >> 8;
	memory[addr+1] = value >> 8;
	// two write instructions
	(*cycles) -= 2;
}

void LDA_set_status(CPU* cpu) {
	cpu_set_flag(cpu, ZERO_MASK, cpu->A == 0);
	cpu_set_flag(cpu, NEGATIVE_MASK, cpu->A & 0b10000000);
}

void execute( CPU* cpu, byte* memory, u32 cycles ) {

	while (cycles > 0) {
		byte opcode = fetch_byte( cpu, &cycles, memory );
		switch (opcode) {

			case INS_LDA_IM:
			{
				byte value = fetch_byte(cpu, &cycles, memory);
				cpu->A = value;
				LDA_set_status(cpu);
			} break;

			case INS_LDA_ZP:
			{
				byte zero_page_addr = fetch_byte(cpu, &cycles, memory);
				byte value = read_byte(&cycles, zero_page_addr, memory);
				cpu->A = value;
				LDA_set_status(cpu);
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
				word sub_addr = fetch_word(cpu, &cycles, memory);
				// push PC-1 to stack
				write_word(cpu, &cycles, cpu->PC - 1, cpu->SP, memory);
				cpu->SP += 2;
				// jump
				cpu->PC = sub_addr;
				cycles--;
			} break;

			default:
			{
				printf("Unknown instruction 0x%04x at 0x%04x\n", opcode, cpu->PC);
			} break;
		}
	}

}

int main() {
	byte* memory = malloc(MAX_MEM);
	CPU* cpu = malloc(sizeof(CPU));

	reset(cpu, memory);

	// start - inline program
	memory[0xFFFC] = INS_JSR;
	memory[0xFFFD] = 0xF0;
	memory[0xFFFE] = 0x00;
	memory[0x00F0] = INS_LDA_ZP;
	memory[0x00F1] = 0xFA;
	memory[0x00FA] = 0x3;
	// end - inline program
	execute(cpu, memory, 9);

	inspect(cpu);
	inspect_memory(memory, 0xFFFC, 4, 8);
	inspect_memory(memory, 0xF0, 16, 8);

    return 0;
}

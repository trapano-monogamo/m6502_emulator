#include "memory.hpp"
#include <iostream>
#include <iomanip>

void Mem::initialize() { for (u32 i=0; i<MAX_MEM; i++) memory[i] = 0x0; }

void Mem::inspect( u16 address, u16 size, u16 step ) {
	for (int addr=address; addr<address+size && addr < (int)MAX_MEM; addr++) {
		if ((addr-address) % step == 0) {
			u32 hi_addr = (u32)(addr+step);
			if (hi_addr >= MAX_MEM) hi_addr = MAX_MEM - 1;
			std::cout << std::endl << " 0x"
				<< std::hex << std::setfill('0') << std::setw(4) << (u16)addr << " - 0x"
				<< std::hex << std::setfill('0') << std::setw(4) << (u16)hi_addr << "\t";
		}
		std::cout << std::hex << std::setfill('0') << std::setw(2) << (u16)memory[addr] << " ";
	}
	std::cout << std::endl;
}

byte Mem::operator[]( u16 address ) const { return memory[address]; }
byte& Mem::operator[]( u16 address ) { return memory[address]; }

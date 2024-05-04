/** http://www.6502.org/users/obelisk/6502/index.html */

#include <iostream>
#include "types.hpp"
#include "cpu.hpp"
#include "memory.hpp"

#ifdef TEST_MODE
#include "tests.hpp"
#endif

/**
 * ~ TODO ~
 * 1. make a separate makefile rule for testing
 * 2. have a tests directory with tester.hpp and tester.cpp files which contain the macros and
 *    functions needed to create a test, and a tests.cpp file that actually contains the tests
 *    (the makefile rule for testing should execute the the tests.cpp file, not the main.cpp file
 * */


enum TestEnum {
	First = 0,
	Second = 1,
	Third = 2,
};


int main() {

#ifdef TEST_MODE

	test_load_instructions();
	test_store_instructions();
	test_transfer_instructions();
	test_stack_instructions();
	test_logic_instructions();
	test_jump_instructions();

#else

	CPU cpu;
	Mem memory;
	cpu.reset(memory, 0xFF00);

	memory[0xAA00] = 0xFF;
	memory[0xAA01] = 0xFF;

	cpu.inspect();
	memory.inspect(0xAA00);

	// clear memory 16 bit
	memory[0xFF00] = CPU::INS_LDA_IM;
	memory[0xFF01] = 0x00;
	memory[0xFF02] = CPU::INS_STA_AB;
	memory[0xFF03] = 0x00;
	memory[0xFF04] = 0xAA;
	memory[0xFF05] = CPU::INS_STA_AB;
	memory[0xFF06] = 0x01;
	memory[0xFF07] = 0xAA;

	cpu.execute(memory, 10);

	cpu.inspect();
	memory.inspect(0xAA00);

#endif

    return 0;
}

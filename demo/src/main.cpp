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

#else

	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_IM;
	memory[0xFFFD] = 0x03;
	cpu.execute(memory, 2);

	cpu.inspect();

	// assemble("test.asm");

#endif

    return 0;
}

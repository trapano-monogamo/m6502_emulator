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


int main() {

#ifdef TEST_MODE

	// test_load_instructions();
	// test_store_instructions();
	// test_transfer_instructions();
	// test_stack_instructions();
	test_logic_instructions();

#else

	CPU cpu;

	byte reg =  0b10010110;
	byte mask = 0b10001000;
	for (int i=0; i<8; i++) {
		std::cout
			<< cpu.test_bit(reg, i) << ", "
			<< cpu.test_bit(mask, i) << ", "
			<< cpu.test_bit(reg & mask, i) << std::endl;;
	}

#endif

    return 0;
}

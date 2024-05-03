/** http://www.6502.org/users/obelisk/6502/index.html */

#include <bitset>
#include <iomanip>
#include <iostream>
#include "types.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "tests.hpp"

struct AddResult {
	short res;
	byte carry : 1;
	byte overflow : 1;
};

void bitwise_add(short x, short y) {
	// std::cout << "before: " << std::bitset<16>(res.res) << ", C: " << (u16)res.carry << ", V: " << (u16)res.overflow << std::endl;
	// res.res = (x ^ y) ^ ((x & y) << 1);
	// std::cout << "after:  " << std::bitset<16>(res.res) << ", C: " << (u16)res.carry << ", V: " << (u16)res.overflow << std::endl;

	std::cout
		<< std::bitset<16>(x) << " +\n"
		<< std::bitset<16>(y) << " =\n"
		<< "----------------" << "\n"
		<< std::bitset<16>(x+y) << "\n"
		<< "----------------" << "\n"
		<< std::bitset<16>((x&y) >> 7) << "\n"
		<< std::endl;
}


/**
 * ~ TODO ~
 * 1. make a separate makefile rule for testing
 * 2. have a tests directory with tester.hpp and tester.cpp files which contain the macros and
 *    functions needed to create a test, and a tests.cpp file that actually contains the tests
 *    (the makefile rule for testing should execute the the tests.cpp file, not the main.cpp file
 * */


int main() {
	Mem memory;
	CPU cpu;

	test_LDA(cpu, memory);
	test_STA(cpu, memory);

	RUN_TEST(LDA_IM_works);
	RUN_TEST(LDA_ZP_works);

    return 0;
}

#pragma once
#include "cpu.hpp"
#include "memory.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>


typedef std::unordered_map<std::string, bool> TestResults;


class TestBase {};

// static std::unordered_map<std::string, TestBase> TESTS;


#define CFG_TEST(NAME) \
	class NAME : public TestBase {\
		bool success = true;\
		std::stringstream failures;\
		void run_test();\
		public:\
		NAME() : TestBase() {};\
		void run() {\
			run_test();\
			std::cout << "[" << #NAME << "]: ";\
			if (!success) {\
				std::cout << "\033[31mFAILED\033[37m" << std::endl << failures.str();\
			} else {\
				std::cout << "\033[32mPASSED\033[37m" << std::endl;\
			}\
		}\
	};\
	void NAME::run_test()

#define EXPECT_EQ(A, B) \
	if (A != B) {\
		failures << "\t" << #A << " is not equal to " << #B << ". Got "\
			<< ((typeid(A).name() == std::string("byte")) ? A : (u16)(A)) << " and "\
			<< ((typeid(B).name() == std::string("byte")) ? B : (u16)(B)) << std::endl;\
		success = false;\
	}\

#define RUN_TEST(NAME) NAME().run()

// #define MAKE_TEST(NAME)

CFG_TEST(LDA_IM_works) {
	CPU cpu;
	Mem memory;

	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_IM; // 2 bytes, 2 cycles
	memory[0xFFFD] = 0x03;
	cpu.execute(memory, 2);

	// if (cpu.A == 0x03 && cpu.PC == 0xFFFE) results["INS_LDA_IM"] = true;
	EXPECT_EQ(cpu.A, 0x03);
}

CFG_TEST(LDA_ZP_works) {
	CPU cpu;
	Mem memory;

	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_ZP; // 2 bytes, 3 cycles
	memory[0xFFFD] = 0x30;
	memory[0x0030] = 0xAA; // putting AA into address 0x0030
	
	cpu.execute(memory, 3);
	
	// if (cpu.A == 0xAA && cpu.PC == 0xFFFE) results["INS_LDA_ZP"] = true;
	EXPECT_EQ(cpu.A, 0xAA);
	EXPECT_EQ(cpu.PC, 0xFFFE);
}


void output_results(const std::string& instruction, const TestResults& results) {
	std::cout << "[*] " << instruction << " results:" << std::endl;
	for (auto& entry : results) {
		std::cout << "\t[" << entry.first << "]: "
			<< ((entry.second) ? "\033[32mPASSED\033[37m" : "\033[31mFAILED\033[37m") << std::endl;
	}
}


void test_LDA( CPU& cpu, Mem& memory ) {
	TestResults results = {
		std::make_pair("INS_LDA_IM", false),
		std::make_pair("INS_LDA_ZP", false),
		std::make_pair("INS_LDA_ZPX", false),
		std::make_pair("INS_LDA_AB", false),
		std::make_pair("INS_LDA_ABX", false),
		std::make_pair("INS_LDA_ABY", false),
		std::make_pair("INS_LDA_INX", false),
		std::make_pair("INS_LDA_INY", false),
	};

	

	/** INS_LDA_IM */
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_IM; // 2 bytes, 2 cycles
	memory[0xFFFD] = 0x03;
	cpu.execute(memory, 2);

	if (cpu.A == 0x03 && cpu.PC == 0xFFFE) results["INS_LDA_IM"] = true;



	/** INS_LDA_ZP */
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_ZP; // 2 bytes, 3 cycles
	memory[0xFFFD] = 0x30;
	memory[0x0030] = 0xAA; // putting AA into address 0x0030
	
	cpu.execute(memory, 3);
	
	if (cpu.A == 0xAA && cpu.PC == 0xFFFE) results["INS_LDA_ZP"] = true;


	/** INS_LDA_ZPX */
	cpu.reset(memory);

	cpu.X = 0x0030; // manually setting the X register
	memory[0xFFFC] = CPU::INS_LDA_ZPX; // 2 bytes, 4 cycles
	memory[0xFFFD] = 0x0A;
	memory[0x003A] = 0xAA; // putting AA into addres 0x003A (0x0030 + 0x0A)
	
	cpu.execute(memory, 4);

	if (cpu.A == 0xAA && cpu.PC == 0xFFFE) results["INS_LDA_ZPX"] = true;


	/** INS_LDA_AB */
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_AB; // 3 bytes, 4 cycles
	memory[0xFFFD] = 0xAA; // lobyte
	memory[0xFFFE] = 0xBB; // hibyte
	memory[0xBBAA] = 0x30; // put 0x30 at 0xBBAA
	
	cpu.execute(memory, 4);

	if (cpu.A == 0x30 && cpu.PC == 0xFFFF) results["INS_LDA_AB"] = true;


	/** INS_LDA_ABX */
	cpu.reset(memory);

	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_LDA_ABX; // 3 bytes, 5 cycles give the value in X
	memory[0xFFFD] = 0xFF; // lobyte
	memory[0xFFFE] = 0xBB; // hibyte
	memory[0xBC00] = 0x30; // put 0x30 at 0xBBAA
	
	cpu.execute(memory, 5);

	if (cpu.A == 0x30 && cpu.PC == 0xFFFF) results["INS_LDA_ABX"] = true;


	output_results("INS_LDA", results);
}


void test_STA( CPU& cpu, Mem& memory ) {
	TestResults results = {
		std::make_pair("INS_STA_ZP", false),
		std::make_pair("INS_STA_ZPX", false),
		std::make_pair("INS_STA_AB", false),
		std::make_pair("INS_STA_ABX", false),
		std::make_pair("INS_STA_ABY", false),
		std::make_pair("INS_STA_INX", false),
		std::make_pair("INS_STA_INY", false),
	};


	/** INS_STA_ZP */
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_IM; // 2 bytes, 2 cycles
	memory[0xFFFD] = 0x30;
	memory[0xFFFE] = CPU::INS_STA_ZP; // 2 bytes, 3 cycles
	memory[0xFFFF] = 0xAA;

	cpu.execute(memory, 5);

	if (cpu.A == 0x30 && memory[0xAA] == 0x30 && cpu.PC == 0x0000 /*PC overflows*/) results["INS_STA_ZP"] = true;

	output_results("INS_STA", results);
}

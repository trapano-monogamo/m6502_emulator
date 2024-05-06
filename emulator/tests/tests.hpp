#pragma once
#include "cpu.hpp"
#include "memory.hpp"

#include <iostream>
#include <sstream>


#define CFG_TEST(NAME) \
	class NAME {\
		bool success = true;\
		std::stringstream failures;\
		void run_test();\
		public:\
		NAME() = default;\
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
	if ((A) != (B)) {\
		failures << "\t" << #A << " is not equal to " << #B << ". Got "\
			<< ((typeid(A).name() == std::string("byte")) ? (A) : (u16)(A)) << " and "\
			<< ((typeid(B).name() == std::string("byte")) ? (B) : (u16)(B)) << std::endl;\
		success = false;\
	}

#define EXPECT_FALSE(A) \
	if (A) {\
		failures << "\t" << #A << " is not false " << std::endl;\
		success = false;\
	}

#define EXPECT_TRUE(A) \
	if (!(A)) {\
		failures << "\t" << #A << " is not true " << std::endl;\
		success = false;\
	}

#define TODO() success = false

#define RUN_TEST(NAME) NAME().run()

CFG_TEST(cpu_can_execute_more_cycles_if_needed) {
	CPU cpu;
	Mem memory;

	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_IM;
	memory[0xFFFD] = 0x03;
	cpu.execute(memory, 1);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(cpu_returns_correct_number_of_cycles_used) {
	CPU cpu;
	Mem memory;

	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_IM;
	memory[0xFFFD] = 0x03;
	u32 used_cycles = cpu.execute(memory, 2);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(used_cycles, 2);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(cpu_does_nothing_if_given_zero_cycles) {
	CPU cpu;
	Mem memory;

	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_IM;
	memory[0xFFFD] = 0x03;
	u32 used_cycles = cpu.execute(memory, 0);

	EXPECT_EQ(cpu.A, 0x00);
	EXPECT_EQ(cpu.PC, 0xFFFC);
	EXPECT_EQ(used_cycles, 0);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_set_status_negative_works) {
	CPU cpu;
	Mem memory;

	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_IM; // 2 bytes, 2 cycles
	memory[0xFFFD] = 0xFF;
	cpu.execute(memory, 2);

	EXPECT_EQ(cpu.A, 0xFF);
	EXPECT_TRUE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_set_status_zero_works) {
	CPU cpu;
	Mem memory;

	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_IM; // 2 bytes, 2 cycles
	memory[0xFFFD] = 0x00;
	cpu.execute(memory, 2);

	EXPECT_EQ(cpu.A, 0x00);
	EXPECT_FALSE(cpu.N);
	EXPECT_TRUE(cpu.Z);
}

CFG_TEST(LDA_IM_works) {
	CPU cpu;
	Mem memory;

	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_IM; // 2 bytes, 2 cycles
	memory[0xFFFD] = 0x03;
	cpu.execute(memory, 2);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_ZP_works) {
	CPU cpu;
	Mem memory;

	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_ZP; // 2 bytes, 3 cycles
	memory[0xFFFD] = 0x30;
	memory[0x0030] = 0x03; // putting AA into address 0x0030
	
	cpu.execute(memory, 3);
	
	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_ZPX_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x0A;
	memory[0xFFFC] = CPU::INS_LDA_ZPX;
	memory[0xFFFD] = 0xA0;
	memory[0x00AA] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_ZPX_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_LDA_ZPX;
	memory[0xFFFD] = 0xFF;
	memory[0x0000] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_AB_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDA_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_ABX_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_LDA_ABX;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAB] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_ABX_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_LDA_ABX;
	memory[0xFFFD] = 0xFF;
	memory[0xFFFE] = 0xBB;
	memory[0xBC00] = 0x03;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_ABY_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_LDA_ABY;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAB] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_ABY_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_LDA_ABY;
	memory[0xFFFD] = 0xFF;
	memory[0xFFFE] = 0xBB;
	memory[0xBC00] = 0x03;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_INX_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_LDA_INX;
	memory[0xFFFD] = 0xA0;
	memory[0x00A1] = 0xAA;
	memory[0x00A2] = 0xBB;
	memory[0xBBAA] = 0x03;

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_INX_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_LDA_INX;
	memory[0xFFFD] = 0xFF;
	memory[0x0000] = 0xAA;
	memory[0x0001] = 0xBB;
	memory[0xBBAA] = 0x03; // wraps around

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_INY_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_LDA_INY;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0xBB;
	memory[0x00A1] = 0xCC;
	memory[0xCCBC] = 0x03;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDA_INY_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_LDA_INY;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0xFF;
	memory[0x00A1] = 0xCC;
	memory[0xCD00] = 0x03;

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDX_IM_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDX_IM;
	memory[0xFFFD] = 0x03;

	u32 expected_used_cycles = 2;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDX_set_status_negative_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDX_IM;
	memory[0xFFFD] = 0xFF;

	u32 expected_used_cycles = 2;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, 0xFF);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_TRUE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDX_set_status_zero_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDX_IM;
	memory[0xFFFD] = 0x00;

	u32 expected_used_cycles = 2;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, 0x00);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_TRUE(cpu.Z);
}

CFG_TEST(LDX_ZP_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDX_ZP;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0x03;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDX_ZPY_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_LDX_ZPY;
	memory[0xFFFD] = 0xA0;
	memory[0x00A1] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDX_ZPY_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_LDX_ZPY;
	memory[0xFFFD] = 0xFF;
	memory[0x0000] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDX_AB_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDX_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDX_ABY_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_LDX_ABY;
	memory[0xFFFD] = 0xA0;
	memory[0xFFFE] = 0xAA;
	memory[0xAAA1] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDX_ABY_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_LDX_ABY;
	memory[0xFFFD] = 0xFF;
	memory[0xFFFE] = 0xAA;
	memory[0xAB00] = 0x03;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDY_IM_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDY_IM;
	memory[0xFFFD] = 0x03;

	u32 expected_used_cycles = 2;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.Y, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDY_set_status_negative_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDY_IM;
	memory[0xFFFD] = 0xFF;

	u32 expected_used_cycles = 2;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.Y, 0xFF);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_TRUE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDY_set_status_zero_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDY_IM;
	memory[0xFFFD] = 0x00;

	u32 expected_used_cycles = 2;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.Y, 0x00);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_TRUE(cpu.Z);
}

CFG_TEST(LDY_ZP_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDY_ZP;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0x03;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.Y, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDY_ZPX_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_LDY_ZPX;
	memory[0xFFFD] = 0xA0;
	memory[0x00A1] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.Y, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDY_ZPX_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_LDY_ZPX;
	memory[0xFFFD] = 0xFF;
	memory[0x0000] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.Y, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDY_AB_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_LDY_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.Y, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDY_ABX_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_LDY_ABX;
	memory[0xFFFD] = 0xA0;
	memory[0xFFFE] = 0xAA;
	memory[0xAAA1] = 0x03;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.Y, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(LDY_ABX_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_LDY_ABX;
	memory[0xFFFD] = 0xFF;
	memory[0xFFFE] = 0xAA;
	memory[0xAB00] = 0x03;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.Y, 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(STA_ZP_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	memory[0xFFFC] = CPU::INS_STA_ZP;
	memory[0xFFFD] = 0xAA;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xAA], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STA_ZPX_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_STA_ZPX;
	memory[0xFFFD] = 0xAA;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xAB], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STA_ZPX_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_STA_ZPX;
	memory[0xFFFD] = 0xFF;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0x00], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STA_AB_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	memory[0xFFFC] = CPU::INS_STA_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xBBAA], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STA_ABX_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_STA_ABX;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xBBAB], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STA_ABX_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_STA_ABX;
	memory[0xFFFD] = 0xFF;
	memory[0xFFFE] = 0xBB;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xBC00], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STA_ABY_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_STA_ABY;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xBBAB], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STA_ABY_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_STA_ABY;
	memory[0xFFFD] = 0xFF;
	memory[0xFFFE] = 0xBB;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xBC00], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STA_INX_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_STA_INX;
	memory[0xFFFD] = 0xA0;
	memory[0x00A1] = 0xAA;
	memory[0x00A2] = 0xBB;

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xBBAA], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STA_INX_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_STA_INX;
	memory[0xFFFD] = 0xFF;
	memory[0x0000] = 0xAA;
	memory[0x0001] = 0xBB;

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xBBAA], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STA_INY_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_STA_INY;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0xAA;
	memory[0x00A1] = 0xBB;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xBBAB], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STA_INY_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_STA_INY;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0xFF;
	memory[0x00A1] = 0xBB;

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xBC00], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STX_ZP_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x03;
	memory[0xFFFC] = CPU::INS_STX_ZP;
	memory[0xFFFD] = 0xAA;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0x00AA], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STX_ZPY_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x03;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_STX_ZPY;
	memory[0xFFFD] = 0xAA;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0x00AB], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STX_ZPY_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x03;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_STX_ZPY;
	memory[0xFFFD] = 0xFF;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0x0000], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STX_AB_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x03;
	memory[0xFFFC] = CPU::INS_STX_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xBBAA], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STY_ZP_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x03;
	memory[0xFFFC] = CPU::INS_STY_ZP;
	memory[0xFFFD] = 0xAA;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0x00AA], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STY_ZPX_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x03;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_STY_ZPX;
	memory[0xFFFD] = 0xAA;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0x00AB], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STY_ZPX_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x03;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_STY_ZPX;
	memory[0xFFFD] = 0xFF;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0x0000], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(STY_AB_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x03;
	memory[0xFFFC] = CPU::INS_STY_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(memory[0xBBAA], 0x03);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(TAX_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	memory[0xFFFC] = CPU::INS_TAX;

	u32 expected_used_cycles = 1;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(cpu.PC, 0xFFFD);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(TAY_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0x03;
	memory[0xFFFC] = CPU::INS_TAY;

	u32 expected_used_cycles = 1;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(cpu.PC, 0xFFFD);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(TXA_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0x03;
	memory[0xFFFC] = CPU::INS_TXA;

	u32 expected_used_cycles = 1;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(cpu.PC, 0xFFFD);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(TYA_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.Y = 0x03;
	memory[0xFFFC] = CPU::INS_TYA;

	u32 expected_used_cycles = 1;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(cpu.PC, 0xFFFD);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(TSX_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.SP = 0xFA;
	memory[0xFFFC] = CPU::INS_TSX;

	u32 expected_used_cycles = 1;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.X, cpu.SP);
	EXPECT_EQ(cpu.PC, 0xFFFD);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_TRUE(cpu.N);
	EXPECT_FALSE(cpu.Z);
}

CFG_TEST(TXS_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.X = 0xFA;
	memory[0xFFFC] = CPU::INS_TXS;

	u32 expected_used_cycles = 1;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.SP, cpu.X);
	EXPECT_EQ(cpu.PC, 0xFFFD);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(PHA_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.A = 0xFA;
	memory[0xFFFC] = CPU::INS_PHA;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.SP, 0xFE);
	EXPECT_EQ(memory[cpu.STACK + cpu.SP + 0x01], cpu.A);
	EXPECT_EQ(cpu.PC, 0xFFFD);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(PLA_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[cpu.STACK + cpu.SP] = 0x03;
	cpu.SP--;
	memory[0xFFFC] = CPU::INS_PLA;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03);
	EXPECT_EQ(cpu.SP, 0xFF);
	// EXPECT_EQ(memory[cpu.STACK + cpu.SP], 0x03); // the stack location is not explicitly overwritten
	EXPECT_EQ(cpu.PC, 0xFFFD);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(PHP_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	cpu.flags = 0b01001101;
	memory[0xFFFC] = CPU::INS_PHP;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.SP, 0xFE);
	EXPECT_EQ(memory[cpu.STACK + cpu.SP + 0x01], cpu.flags);
	EXPECT_EQ(cpu.PC, 0xFFFD);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.__unused);
	EXPECT_TRUE(cpu.C);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.I);
	EXPECT_TRUE(cpu.D);
	EXPECT_TRUE(cpu.B);
	EXPECT_FALSE(cpu.V);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(PLP_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[cpu.STACK + cpu.SP] = 0b01001101;
	cpu.SP = 0xFE;
	memory[0xFFFC] = CPU::INS_PLP;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.SP, 0xFF);
	EXPECT_EQ(cpu.PC, 0xFFFD);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.__unused);
	EXPECT_TRUE(cpu.C);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.I);
	EXPECT_TRUE(cpu.D);
	EXPECT_TRUE(cpu.B);
	EXPECT_FALSE(cpu.V);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(AND_IM_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_AND_IM;
	memory[0xFFFD] = mask;

	u32 expected_used_cycles = 2;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg & mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(AND_ZP_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_AND_ZP;
	memory[0xFFFD] = 0xAA;
	memory[0x00AA] = mask;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg & mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(AND_ZPX_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_AND_ZPX;
	memory[0xFFFD] = 0xAA;
	memory[0x00AB] = mask;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg & mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(AND_AB_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_AND_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = mask;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg & mask);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(AND_ABX_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_AND_ABX;
	memory[0xFFFD] = 0xFF;
	memory[0xFFFE] = 0xBB;
	memory[0xBC00] = mask;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg & mask);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(AND_ABY_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_AND_ABY;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAB] = mask;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg & mask);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(AND_INX_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_AND_INX;
	memory[0xFFFD] = 0xA0;
	memory[0x00A1] = 0xAA;
	memory[0x00A2] = 0xBB;
	memory[0xBBAA] = mask;

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg & mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(AND_INY_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_AND_INY;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0xAA;
	memory[0x00A1] = 0xBB;
	memory[0xBBAB] = mask;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg & mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(AND_INY_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_AND_INY;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0xFF;
	memory[0x00A1] = 0xBB;
	memory[0xBC00] = mask;

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg & mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(EOR_IM_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_EOR_IM;
	memory[0xFFFD] = mask;

	u32 expected_used_cycles = 2;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg ^ mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(EOR_ZP_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_EOR_ZP;
	memory[0xFFFD] = 0xAA;
	memory[0x00AA] = mask;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg ^ mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(EOR_ZPX_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_EOR_ZPX;
	memory[0xFFFD] = 0xAA;
	memory[0x00AB] = mask;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg ^ mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(EOR_AB_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_EOR_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = mask;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg ^ mask);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(EOR_ABX_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_EOR_ABX;
	memory[0xFFFD] = 0xFF;
	memory[0xFFFE] = 0xBB;
	memory[0xBC00] = mask;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg ^ mask);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(EOR_ABY_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_EOR_ABY;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAB] = mask;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg ^ mask);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(EOR_INX_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_EOR_INX;
	memory[0xFFFD] = 0xA0;
	memory[0x00A1] = 0xAA;
	memory[0x00A2] = 0xBB;
	memory[0xBBAA] = mask;

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg ^ mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(EOR_INY_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_EOR_INY;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0xAA;
	memory[0x00A1] = 0xBB;
	memory[0xBBAB] = mask;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg ^ mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(EOR_INY_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_EOR_INY;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0xFF;
	memory[0x00A1] = 0xBB;
	memory[0xBC00] = mask;

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg ^ mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(ORA_IM_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_ORA_IM;
	memory[0xFFFD] = mask;

	u32 expected_used_cycles = 2;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg | mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(ORA_ZP_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_ORA_ZP;
	memory[0xFFFD] = 0xAA;
	memory[0x00AA] = mask;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg | mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(ORA_ZPX_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_ORA_ZPX;
	memory[0xFFFD] = 0xAA;
	memory[0x00AB] = mask;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg | mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(ORA_AB_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_ORA_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = mask;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg | mask);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(ORA_ABX_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_ORA_ABX;
	memory[0xFFFD] = 0xFF;
	memory[0xFFFE] = 0xBB;
	memory[0xBC00] = mask;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg | mask);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(ORA_ABY_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_ORA_ABY;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAB] = mask;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg | mask);
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(ORA_INX_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.X = 0x01;
	memory[0xFFFC] = CPU::INS_ORA_INX;
	memory[0xFFFD] = 0xA0;
	memory[0x00A1] = 0xAA;
	memory[0x00A2] = 0xBB;
	memory[0xBBAA] = mask;

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg | mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(ORA_INY_without_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_ORA_INY;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0xAA;
	memory[0x00A1] = 0xBB;
	memory[0xBBAB] = mask;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg | mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(ORA_INY_with_page_cross_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg = 0b10100101;
	byte mask = 0b11111000;

	cpu.A = reg;
	cpu.Y = 0x01;
	memory[0xFFFC] = CPU::INS_ORA_INY;
	memory[0xFFFD] = 0xA0;
	memory[0x00A0] = 0xFF;
	memory[0x00A1] = 0xBB;
	memory[0xBC00] = mask;

	u32 expected_used_cycles = 6;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg | mask);
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(BIT_ZP_positive_test_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg =  0b10100101;
	byte mask = 0b01000100;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_BIT_ZP;
	memory[0xFFFD] = 0xAA;
	memory[0x00AA] = mask;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg); // unaltered A register
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.V);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(BIT_AB_positive_test_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg =  0b10100101;
	byte mask = 0b01000100;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_BIT_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = mask;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg); // unaltered A register
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.V);
	EXPECT_FALSE(cpu.N);
}

CFG_TEST(BIT_ZP_negative_test_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg =  0b10100101;
	byte mask = 0b10001000;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_BIT_ZP;
	memory[0xFFFD] = 0xAA;
	memory[0x00AA] = mask;

	u32 expected_used_cycles = 3;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg); // unaltered A register
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.V);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(BIT_AB_negative_test_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	byte reg =  0b10100101;
	byte mask = 0b10001000;

	cpu.A = reg;
	memory[0xFFFC] = CPU::INS_BIT_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = mask;

	u32 expected_used_cycles = 4;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, reg); // unaltered A register
	EXPECT_EQ(cpu.PC, 0xFFFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.V);
	EXPECT_TRUE(cpu.N);
}

CFG_TEST(JMP_AB_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_JMP_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = CPU::INS_LDA_IM;
	memory[0xBBAB] = 0x03;

	u32 expected_used_cycles = 5;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03); // unaltered A register
	EXPECT_EQ(cpu.PC, 0xBBAC);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(JMP_IN_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_JMP_IN;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = 0x42;
	memory[0xBBAB] = 0x42;
	memory[0x4242] = CPU::INS_LDA_IM;
	memory[0x4243] = 0x03;

	u32 expected_used_cycles = 7;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03); // unaltered A register
	EXPECT_EQ(cpu.PC, 0x4244);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(JSR_AB_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_JSR_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = CPU::INS_LDA_IM;
	memory[0xBBAB] = 0x03;

	u32 expected_used_cycles = 8;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03); // unaltered A register
	EXPECT_EQ(cpu.PC, 0xBBAC);
	EXPECT_EQ(cpu.SP, 0xFD);
	EXPECT_EQ(memory[cpu.STACK + cpu.SP + 0x01], 0xFE);
	EXPECT_EQ(memory[cpu.STACK + cpu.SP + 0x02], 0xFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

CFG_TEST(RTS_works) {
	CPU cpu;
	Mem memory;
	cpu.reset(memory);

	memory[0xFFFC] = CPU::INS_JSR_AB;
	memory[0xFFFD] = 0xAA;
	memory[0xFFFE] = 0xBB;
	memory[0xBBAA] = CPU::INS_LDA_IM;
	memory[0xBBAB] = 0x03;
	memory[0xBBAC] = CPU::INS_RTS;

	u32 expected_used_cycles = 14;
	u32 used_cycles = cpu.execute(memory, expected_used_cycles);

	EXPECT_EQ(cpu.A, 0x03); // unaltered A register
	EXPECT_EQ(cpu.PC, 0xFFFE);
	EXPECT_EQ(cpu.SP, 0xFF);
	EXPECT_EQ(memory[cpu.STACK + cpu.SP - 0x01], 0xFE);
	EXPECT_EQ(memory[cpu.STACK + cpu.SP], 0xFF);
	EXPECT_EQ(used_cycles, expected_used_cycles);
}

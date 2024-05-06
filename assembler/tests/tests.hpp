#pragma once

#include "assembler.hpp"
#include "lexer.hpp"

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


CFG_TEST(first_test) {
	assemble("../../../assembler/tests/programs/tokens.asm");
}

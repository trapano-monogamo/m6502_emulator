#pragma once

#include "assembler.hpp"

#include <fstream>
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
		failures << "\t" << #A << " is not equal to " << #B << std::endl; \
		/* \
			<< ". Got "\
			<< ((typeid(A).name() == std::string("byte")) ? (A) : (u16)(A)) << " and "\
			<< ((typeid(B).name() == std::string("byte")) ? (B) : (u16)(B)) << std::endl;\
		*/ \
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

#define EXPECT_MATCH(VECA, VECB) \
	{\
		if (VECA.size() != VECB.size()) { \
			failures << "\tsize of " << #VECA << " and " << #VECB << " do not match" << std::endl; \
			success = false; \
		} else { \
			for (int i=0; i<VECA.size(); i++) { \
				if (VECA[i] != VECB[i]) {\
					failures << "elements at index " << i << " do not match" << std::endl; \
					success = false; \
				}\
			} \
		} \
	}

#define TODO() success = false

#define RUN_TEST(NAME) NAME().run()


CFG_TEST(lexer_test) {
	std::ifstream src("../../../assembler/tests/programs/tokens.asm");
	std::string line;
	std::stringstream src_str;
	while (getline(src, line)) {
		src_str << line << std::endl;
	}

	std::vector<Token> generated_tokens = lex(src_str.str()).tokens;

	std::vector<Token> expected_tokens = {
		{ Token::Type::DIRECTIVE,	".org" },
		{ Token::Type::OPERAND,		"$8000" },
		{ Token::Type::SYMBOL,		"VALUE" },
		{ Token::Type::EQUAL_SIGN,	"=" },
		{ Token::Type::OPERAND,		"$AA" },
		{ Token::Type::SYMBOL,		"CONST" },
		{ Token::Type::EQUAL_SIGN,	"=" },
		{ Token::Type::OPERAND,		"$00" },
		{ Token::Type::SYMBOL,		"VAR" },
		{ Token::Type::EQUAL_SIGN,	"=" },
		{ Token::Type::OPERAND,		"$0000" },
		{ Token::Type::SYMBOL,		"symbol" },
		{ Token::Type::EQUAL_SIGN,	"=" },
		{ Token::Type::OPERAND,		"gj53i0e[}{\\" },
		{ Token::Type::INSTRUCTION,	"jsr" },
		{ Token::Type::OPERAND,		"main" },
		{ Token::Type::INSTRUCTION,	"bvc" },
		{ Token::Type::OPERAND,		"end" },
		{ Token::Type::LABEL,		"main" },
		{ Token::Type::INSTRUCTION,	"lda" },
		{ Token::Type::OPERAND,		"VALUE" },
		{ Token::Type::LABEL,		"loop" },
		{ Token::Type::INSTRUCTION,	"sta" },
		{ Token::Type::OPERAND,		"$00,x" },
		{ Token::Type::INSTRUCTION,	"inx" },
		{ Token::Type::INSTRUCTION,	"bvc" },
		{ Token::Type::OPERAND,		"loop" },
		{ Token::Type::INSTRUCTION,	"rts" },
		{ Token::Type::LABEL,		"sub" },
		{ Token::Type::DIRECTIVE,	".org" },
		{ Token::Type::OPERAND,		"VAR" },
		{ Token::Type::LABEL,		"end" },
	};

	EXPECT_MATCH(generated_tokens, expected_tokens);
}

CFG_TEST(AST_is_built_correctly) {
	std::ifstream src("../../../assembler/tests/programs/tokens.asm");
	std::string line;
	std::stringstream src_str;
	while (getline(src, line)) {
		src_str << line << std::endl;
	}
}

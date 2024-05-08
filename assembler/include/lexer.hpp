#pragma once
#include <string>
#include <vector>

struct Token {
	enum Type {
		LABEL,			// subroutines and jump locations
		INSTRUCTION,	// instructions
		OPERAND,		// arguments
		SYMBOL,			// for constants
		DIRECTIVE,
		EQUAL_SIGN,
		NONE
	} type;
	std::string value;
};
std::ostream& operator<<(std::ostream& out, Token t);
bool operator==(Token&, Token&);

std::vector<Token> lex(const std::string& input);

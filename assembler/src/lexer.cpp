#include "lexer.hpp"
#include "instructions.hpp"

#include <array>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

struct Token {
	// enum Type {
	// 	LABEL,			// subroutines and jump locations
	// 	INSTRUCTION,	// instructions
	// 	OPERAND,		// arguments
	// 	SYMBOL,			// for constants
	// 	OPERATOR, 		// they act on an rvalue and an lvalue (=,+,-,*,/)
	// 	SPECIFIER,		// '.' for directives, ':' for labels, ',' for addressing modes, '$' and '#' and ''' for literals
	// 	NONE
	// } type;
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

char delimiters[] = { ' ', '\t', '\n', ':', '=' };
// char operators[] = { '=', '+', '*', '-', '/' };
// char specifiers[] = { '.', ',', ':', '\'', '#', '$' };

std::ostream& operator<<(std::ostream& out, Token t) {
	std::string s_type{};
	switch (t.type) {
		case Token::Type::LABEL:
			s_type = "LABEL"; break;
		case Token::Type::INSTRUCTION:
			s_type = "INSTRUCTION"; break;
		case Token::Type::OPERAND:
			s_type = "OPERAND"; break;
		case Token::Type::SYMBOL:
			s_type = "SYMBOL"; break;
		case Token::Type::DIRECTIVE:
			s_type = "SYMBOL"; break;
		default:
			s_type = "NONE"; break;
	}

	out << "{ " << s_type << ", " << t.value << " }";
	return out;
}

bool is_delimiter(char c) {
	for (unsigned int i=0; i<sizeof(delimiters); i++)
		if (c == delimiters[i]) return true;
	return false;
}

void lex(const std::string& input) {
	std::vector<Token> tokens{};
	std::string current_token{};
	Token::Type current_type = Token::Type::NONE;

	int line_number = 0;
	bool comment = false;

	// WHAT THE FUCK IS HAPPENING? THIS SHOULDN'T BE SO HARD... i'm tired
	for (char c : input) {
		if (c == ';') comment = true;

		if (!is_delimiter(c)) {
			if (comment) continue;
			else current_token += c;
		} else {
			// check which delimiter has been encountered
			if (c == '\n') {
				line_number++;
				comment = false; // finish the possible comment
			}

			// if the token is empty, skip to the next
			if (current_token.empty()) continue;

			// infer token type
			if (current_token == "=") {
				tokens.at(tokens.size() - 1).type = Token::Type::SYMBOL;
			} else if (c == ':') {
				current_type = Token::Type::LABEL;
			} else if (current_token[0] == '.') {
				current_type = Token::Type::DIRECTIVE;
			} else if (std::find(instruction_mnemonics.begin(), instruction_mnemonics.end(), current_token) != nullptr) {
				current_type = Token::Type::INSTRUCTION;
			} else {
				current_type = Token::Type::OPERAND;
			}

			tokens.push_back(Token{ current_type, current_token });
			current_token.clear();
			current_type = Token::Type::NONE;
		}
	}

	for (auto& t : tokens) {
		std::cout << " " << t << " " << std::endl;
	}
}

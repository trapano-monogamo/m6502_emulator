#include "lexer.hpp"
#include "instructions.hpp"

#include <array>
#include <string>
#include <iostream>
#include <algorithm>

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
			s_type = "DIRECTIVE"; break;

		case Token::Type::EQUAL_SIGN:
			s_type = "EQUAL_SIGN"; break;

		default:
			s_type = "NONE"; break;
	}

	out << "{ " << s_type << ", '" << t.value << "' }";
	return out;
}

bool operator==(Token& a, Token&b) {
	return a.type == b.type && a.value == b.value;
}

bool is_delimiter(char c) {
	for (unsigned int i=0; i<sizeof(delimiters); i++)
		if (c == delimiters[i]) return true;
	return false;
}

std::vector<Token> lex(const std::string& input) {
	std::vector<Token> tokens{};
	std::string current_token{};
	Token::Type current_type = Token::Type::NONE;

	int line_number = 0;
	bool comment = false;
	bool expects_operand = false;

	// WHAT THE HELL IS HAPPENING? THIS SHOULDN'T BE SO HARD... i'm tired
	for (char c : input) {
		if (c == ';') comment = true;

		if (!is_delimiter(c)) {
			if (comment) continue;
			else current_token += c;
		} else {

			if (c == '\n') {
				line_number++;
				comment = false; // finish the possible comment
			}

			if (expects_operand) {
				current_type = Token::Type::OPERAND;
			}

			if (c == '=') {
				if (!current_token.empty()) {
					tokens.push_back(Token{ Token::Type::SYMBOL, current_token });
					current_token.clear();
				}
				current_type = Token::Type::EQUAL_SIGN;
				current_token = "=";
			}
			if (c == ':') {
				current_type = Token::Type::LABEL;
			}
			if (current_token.find('.') != std::string::npos) {
				current_type = Token::Type::DIRECTIVE;
				expects_operand = true;
			}
			if (std::find(instruction_mnemonics.begin(), instruction_mnemonics.end(), current_token) != instruction_mnemonics.end()) {
				current_type = Token::Type::INSTRUCTION;
				expects_operand = true;
			}

			if (current_type == Token::Type::NONE && expects_operand) {
				current_type = Token::Type::OPERAND;
				expects_operand = false;
			}

			if (!current_token.empty()) {
				tokens.push_back(Token{ current_type, current_token });
				current_token.clear();
			}

			current_type = Token::Type::NONE;
		}
	}

	// second pass because I can't be bothered to fix the previous loop for now...
	Token* prev = nullptr;
	for (auto& t : tokens) {
		if (t.value == "=" && prev != nullptr) {
			prev->type = Token::Type::SYMBOL;
			t.type = Token::Type::EQUAL_SIGN;
		}
		prev = &t;
	}

	// for (auto& t : tokens)
	// 	std::cout << " " << t << " " << std::endl;

	return tokens;
}

#include "assembler.hpp"
#include "instructions.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


/* useful implementations */


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


/* ASSEMBLER */


void assemble(const char* filepath) {
	std::ifstream code(filepath);

	if (!code.good()) {
		std::cout << "Couldn't open file '" << filepath << "'." << std::endl;
		return;
	}

	std::stringstream src_string{};
	std::string line{};
	while (std::getline(code, line)) {
		src_string << line << std::endl;
	}

	lex(src_string.str());
}


/* LEXER */


char delimiters[] = { ' ', '\t', '\n', ':', '=' };

bool is_delimiter(char c) {
	for (unsigned int i=0; i<sizeof(delimiters); i++)
		if (c == delimiters[i]) return true;
	return false;
}

lexer_results lex(const std::string& input) {
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

	return { tokens, build_ast(tokens) };
}


/* PARSER */


ASTNode build_ast(std::vector<Token>& tokens) {
	// default location of the program in memory. The .org directive overrites this default
	word zero_addr = 0x0000;
	std::unordered_map<std::string, word> subroutines{};

	/* !!! TODO: consider removing the EQUAL_SIGN tokens from the results of the
	 *           lexer. They are necessary to recognize assignments, but once the
	 *           token type is deduced, a symbol already expects an operand, so the
	 *           equal sign doesn't add any information
	 */

	ASTNode root{};
	ASTNode* current = &root;

	using ty = Token::Type;
	for (auto& t : tokens) {
		switch (t.type) {

			// mmmh... look up more about directives
			case ty::DIRECTIVE:
			{
				if (t.value == ".org") {
					// here the assembler should act as an interpreter
					// and directly read the operand to set the zero address
				}
			} break;

			case ty::LABEL:
			{
				current->children.push_back(ASTNode{current, t, 0, {}});
				current = &current->children[0];
				// subroutines look up table
				// -- code --
			} break;

			case ty::SYMBOL:
			{} break;

			case ty::EQUAL_SIGN:
			{} break;

			case ty::OPERAND:
			{} break;

			case ty::INSTRUCTION:
			{} break;

			default: // ty::NONE
			{
				std::cout << "Unknown token in AST build process" << std::endl;
			} break;
		}
	}
}

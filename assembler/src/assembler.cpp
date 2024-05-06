/* ~ TODO ~
 * Consider adding a byte size for the operand in the tables containing the opcodes.
 *
 * In the AST, a label could have the contained instructions as children, and an instruction
 * could have its operand and addressing mode as children.
 * The root of the tree is an abstract entry point, the first children will be the first instruction/label
 * encountered, and it will be the first byte of the compiled program.
 *
 *
 * To compile the program, traverse the AST to order the instructions, and then compose the program
 * by substituting the opcodes, label addresses and operand numerical values.
 *
 * In the first pass, determine the amount of bytes that each node occupies:
 * - an instruction occupies one byte for the opcode plus the bytes the operand takes
 *   - the number of bytes occupied by the operand is determined by the addressing mode
 * - a label occupies the total number of bytes occupied by each instruction and its operand
 *
 * So an ASTNode should hold a token, a size attribute, a vector of children and maybe a pointer to its parent
 *
 * 		struct ASTNode {
 * 			ASTNode* parent;
 * 			Token token;
 * 			size_t byte_size;
 * 			std::vector<ASTNode> children;
 * 		};
 *
 * In the second pass, put the program together aligning each node and check for compilation errors.
 * Having determined the size of each node, traverse the AST top to bottom and set each byte of the program to the correct :
 * - assign an address to each label encountered
 *   - this should involve knowing where the program will be put in memory
 *     to apply the correct offset to each label's address
 *   - subroutines should be put one after the other just as encountered during the traversal of the AST
 *     so the actual address of the label should be the index of the byte reached in the traversal plus
 *     the offset of the program
 *   - to substitute the correct address when calling a subroutine, build an unordered_map that assigns
 *     this calculated address to the label, and when the same label is encountered as an operand, substitute
 *     the address
 * - when an instruction is encountered, build the opcode from the tables and put it in the program followed by its operand
 *   - the operand must be stripped of its addressing mode identifiers and parsed to a byte or a sequence of two bytes
 * (!) if an implied addressing mode instruction has children, there's an error
 * (!) if an instruction that doesn't support implied addressing mode doesn't have children, there's an error
 * (!) if the size of an instruction's operand doesn't match the size expected by the addressing mode, there's an error
 * (!) if the operand of a JMP/JSR instruction isn't present in the table of subroutines there's an error
 * (!) if an instruction's mnemonic isn't present in any of the tables (so the opcode couldn't be constructed), there's an error
 *
 * */

#include "assembler.hpp"
#include "lexer.hpp"
#include "parser.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// #include <cctype>
// #include <ostream>
// #include <string>
// #include <vector>
// #include <algorithm>
// #include <iterator>
// #include <cstddef>

/* ---------------------------------------------- */


/*

struct ASTNode {
	ASTNode* parent;
	Token token;
	size_t byte_size;
	std::vector<ASTNode> children;
};

void trim_comment(std::string& line) {
	std::size_t pos = line.find(';');
	if (pos != std::string::npos) {
		line.erase(pos);
	}
}

void replace_spaces(std::string& line) {
	std::replace(line.begin(), line.end(), ' ', '\t');
}

void refine_tokens(std::vector<Token>& line) {
	bool has_label = false;

	int i = 0;
	for (auto& token : line) {

		if (i == 0 && token.value.find(':') != std::string::npos) {
			has_label = true;
			token.type = Token::Type::LABEL;
		} else if ((i == 0 && !has_label) || (i == 1 && has_label)) {
			token.type = Token::Type::INSTRUCTION;
		} else if (i != 0) {
			token.type = Token::Type::OPERAND;
		}

		i++;
	}
}

std::vector<Token> tokenize_line(std::string line) {
	std::vector<Token> tokens;

	std::string delimiter = "\t";
	line += delimiter;

	size_t pos = 0;
	std::string token;
	while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);
		if (!token.empty()) {
			std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c){ return std::tolower(c); });
			tokens.push_back(Token{ .type = Token::Type::NONE, .value = token, .addr_mode = AddrMode::INVALID });
		}
		line.erase(0, pos + delimiter.length());
	}

	refine_tokens(tokens);

	return tokens;
}

void compile(const std::vector<Token>& tokens) {
	std::unordered_map<std::string, word> subroutines;

	ASTNode root = ASTNode{
		.parent = nullptr,
		.token = Token{},
		.byte_size = 0,
		.children = std::vector<ASTNode>(),
	};

	ASTNode* current = &root;

	for (const Token& token : tokens) {
		switch (token.type) {
			case Token::Type::LABEL:
			{
				if (current->parent != nullptr) {
					if (current->parent->token.type == Token::Type::LABEL) {
						std::cout << "Label in a label..." << std::endl;
						exit(-1);
					}
				}
				subroutines.insert({token.value, 0x0000});
				current->children.push_back(ASTNode{
							.parent = current,
							.token = token,
							.byte_size = 0,
							.children = std::vector<ASTNode>()
						});
				current = &(current->children[current->children.size() - 1]);
			} break;
			case Token::Type::INSTRUCTION:
			{
				current->children.push_back(ASTNode{
							.parent = current,
							.token = token,
							.byte_size = 0,
							.children = std::vector<ASTNode>()
						});
				if (token.value == "rts" || token.value == "rti") {
					if (current->parent != nullptr && current->parent->parent != nullptr) {
						current = current->parent->parent;
					}
				} else {
					current = &(current->children[current->children.size() - 1]);
				}
			} break;
			case Token::Type::OPERAND:
			{
				// set addressing mode
				current->children.push_back(ASTNode{
							.parent = current,
							.token = token,
							.byte_size = 0,
							.children = std::vector<ASTNode>()
						});
			} break;
			default:
			{
				std::cout << "Fell through default..." << std::endl;
				exit(-1);
			} break;
		}
	}
}

*/

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

	// std::vector<Token> tokens{};

	// std::string line;
	// while (std::getline(code, line)) {
	// 	trim_comment(line);
	// 	replace_spaces(line);
	// 	auto tokenized = tokenize_line(line);

	// 	tokens.insert(
	// 		tokens.end(),
	// 		std::make_move_iterator(tokenized.begin()),
	// 		std::make_move_iterator(tokenized.end())
	// 	);
	// }

	// for (auto& t : tokens) {
	// 	std::cout << t << std::endl;
	// }

	// compile(tokens);
}

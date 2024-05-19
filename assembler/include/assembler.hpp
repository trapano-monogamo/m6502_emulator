/** https://www.masswerk.at/6502/assembler.html */

#pragma once

#include "types.hpp"

std::vector<Token> lex(const std::string& input);
ASTNode build_ast(std::vector<Token>& tokens);
void assemble(const char* filepath);

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

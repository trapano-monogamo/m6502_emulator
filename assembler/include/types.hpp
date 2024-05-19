#pragma once

#include <string>
#include <vector>

typedef unsigned char 	byte;
typedef unsigned short 	word;
typedef unsigned short	u16;
typedef unsigned int 	u32;
typedef int 			i32;

struct Token {
	enum Type {
		LABEL,
		INSTRUCTION,
		OPERAND,
		SYMBOL,
		DIRECTIVE,
		NONE
	} type;
	std::string value;
};
std::ostream& operator<<(std::ostream& out, Token t);
bool operator==(Token&, Token&);

struct ASTNode {
	ASTNode* parent;
	Token token;
	size_t byte_size;
	std::vector<ASTNode> children;
};

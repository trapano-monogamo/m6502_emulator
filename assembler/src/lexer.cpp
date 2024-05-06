#include "assembler/lexer.hpp"
#include "assembler/instructions.hpp"

#include <string>
#include <iostream>

struct Token {
	enum Type {
		LABEL,
		INSTRUCTION,
		OPERAND,
		NONE
	} type;
	std::string value;
};

std::ostream& operator<<(std::ostream& out, Token t) {
	std::string s_addr_mode{};
	// switch (t.addr_mode) {
	// 	case AddrMode::A: s_addr_mode = "A"; break;
	// 	case AddrMode::AB: s_addr_mode = "ab"; break;
	// 	case AddrMode::ABX: s_addr_mode = "ab,x"; break;
	// 	case AddrMode::ABY: s_addr_mode = "ab,y"; break;
	// 	case AddrMode::IM: s_addr_mode = "#"; break;
	// 	case AddrMode::IN: s_addr_mode = "in"; break;
	// 	case AddrMode::INX: s_addr_mode = "(in,X)"; break;
	// 	case AddrMode::INY: s_addr_mode = "(in),Y"; break;
	// 	case AddrMode::REL: s_addr_mode = "rel"; break;
	// 	case AddrMode::ZP: s_addr_mode = "zp"; break;
	// 	case AddrMode::ZPX: s_addr_mode = "zp,X"; break;
	// 	case AddrMode::ZPY: s_addr_mode = "zp,Y"; break;
	// 	case AddrMode::IMPL: s_addr_mode = "impl"; break;
	// 	default: s_addr_mode = "INVALID"; break;
	// }

	std::string s_type{};
	switch (t.type) {
		case Token::Type::LABEL:
			s_type = "LABEL"; break;
		case Token::Type::INSTRUCTION:
			s_type = "INSTRUCTION"; break;
		case Token::Type::OPERAND:
			s_type = "OPERAND"; break;
		default:
			s_type = "NONE"; break;
	}

	out << "{ " << s_type << ", '" << t.value << "', " << s_addr_mode << " }";
	return out;
}

void lex(const std::string &input) {
	for (char c : input) {
		std::cout << c << std::endl;
	}
}

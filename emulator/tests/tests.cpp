#include "tests.hpp"

void test_cyles() {
	RUN_TEST(cpu_can_execute_more_cycles_if_needed);
	RUN_TEST(cpu_returns_correct_number_of_cycles_used);
	RUN_TEST(cpu_does_nothing_if_given_zero_cycles);
}

void test_load_instructions() {
	RUN_TEST(LDA_IM_works);
	RUN_TEST(LDA_set_status_negative_works);
	RUN_TEST(LDA_set_status_zero_works);
	RUN_TEST(LDA_ZP_works);
	RUN_TEST(LDA_ZPX_without_page_cross_works);
	RUN_TEST(LDA_ZPX_with_page_cross_works);
	RUN_TEST(LDA_AB_works);
	RUN_TEST(LDA_ABX_without_page_cross_works);
	RUN_TEST(LDA_ABX_with_page_cross_works);
	RUN_TEST(LDA_ABY_without_page_cross_works);
	RUN_TEST(LDA_ABY_with_page_cross_works);
	RUN_TEST(LDA_INX_without_page_cross_works);
	RUN_TEST(LDA_INX_with_page_cross_works);
	RUN_TEST(LDA_INY_without_page_cross_works);
	RUN_TEST(LDA_INY_with_page_cross_works);

	RUN_TEST(LDX_IM_works);
	RUN_TEST(LDX_set_status_negative_works);
	RUN_TEST(LDX_set_status_zero_works);
	RUN_TEST(LDX_ZP_works);
	RUN_TEST(LDX_ZPY_without_page_cross_works);
	RUN_TEST(LDX_ZPY_with_page_cross_works);
	RUN_TEST(LDX_AB_works);
	RUN_TEST(LDX_ABY_without_page_cross_works);
	RUN_TEST(LDX_ABY_with_page_cross_works);

	RUN_TEST(LDY_IM_works);
	RUN_TEST(LDY_set_status_negative_works);
	RUN_TEST(LDY_set_status_zero_works);
	RUN_TEST(LDY_ZP_works);
	RUN_TEST(LDY_ZPX_without_page_cross_works);
	RUN_TEST(LDY_ZPX_with_page_cross_works);
	RUN_TEST(LDY_AB_works);
	RUN_TEST(LDY_ABX_without_page_cross_works);
	RUN_TEST(LDY_ABX_with_page_cross_works);
}

void test_store_instructions() {
	RUN_TEST(STA_ZP_works);
	RUN_TEST(STA_ZPX_without_page_cross_works);
	RUN_TEST(STA_ZPX_with_page_cross_works);
	RUN_TEST(STA_AB_works);
	RUN_TEST(STA_ABX_without_page_cross_works);
	RUN_TEST(STA_ABX_with_page_cross_works);
	RUN_TEST(STA_ABY_without_page_cross_works);
	RUN_TEST(STA_ABY_with_page_cross_works);
	RUN_TEST(STA_INX_without_page_cross_works);
	RUN_TEST(STA_INX_with_page_cross_works);
	RUN_TEST(STA_INY_without_page_cross_works);
	RUN_TEST(STA_INY_with_page_cross_works);

	RUN_TEST(STX_ZP_works);
	RUN_TEST(STX_ZPY_without_page_cross_works);
	RUN_TEST(STX_ZPY_with_page_cross_works);
	RUN_TEST(STX_AB_works);

	RUN_TEST(STY_ZP_works);
	RUN_TEST(STY_ZPX_without_page_cross_works);
	RUN_TEST(STY_ZPX_with_page_cross_works);
	RUN_TEST(STY_AB_works);
}

void test_transfer_instructions() {
	// i don't have the patience to test for zero and negative cases, especially
	// considering the LD*_set_status() functions have already been tested...

	RUN_TEST(TAX_works);
	RUN_TEST(TAY_works);

	RUN_TEST(TXA_works);
	RUN_TEST(TYA_works);
}

void test_stack_instructions() {
	RUN_TEST(TSX_works);
	RUN_TEST(TXS_works);

	RUN_TEST(PHA_works);
	RUN_TEST(PLA_works);
	RUN_TEST(PHP_works);
	RUN_TEST(PLP_works);
}

void test_logic_instructions() {
	RUN_TEST(AND_IM_works);
	RUN_TEST(AND_ZP_works);
	RUN_TEST(AND_ZPX_works);
	RUN_TEST(AND_AB_works);
	RUN_TEST(AND_ABX_works);
	RUN_TEST(AND_ABY_works);
	RUN_TEST(AND_INX_works);
	RUN_TEST(AND_INY_without_page_cross_works);
	RUN_TEST(AND_INY_with_page_cross_works);

	RUN_TEST(EOR_IM_works);
	RUN_TEST(EOR_ZP_works);
	RUN_TEST(EOR_ZPX_works);
	RUN_TEST(EOR_AB_works);
	RUN_TEST(EOR_ABX_works);
	RUN_TEST(EOR_ABY_works);
	RUN_TEST(EOR_INX_works);
	RUN_TEST(EOR_INY_without_page_cross_works);
	RUN_TEST(EOR_INY_with_page_cross_works);

	RUN_TEST(ORA_IM_works);
	RUN_TEST(ORA_ZP_works);
	RUN_TEST(ORA_ZPX_works);
	RUN_TEST(ORA_AB_works);
	RUN_TEST(ORA_ABX_works);
	RUN_TEST(ORA_ABY_works);
	RUN_TEST(ORA_INX_works);
	RUN_TEST(ORA_INY_without_page_cross_works);
	RUN_TEST(ORA_INY_with_page_cross_works);

	RUN_TEST(BIT_ZP_positive_test_works);
	RUN_TEST(BIT_AB_positive_test_works);
	RUN_TEST(BIT_ZP_negative_test_works);
	RUN_TEST(BIT_AB_negative_test_works);
}

void test_jump_instructions() {
	RUN_TEST(JMP_AB_works);
	RUN_TEST(JMP_IN_works);
	RUN_TEST(JSR_AB_works);
	RUN_TEST(RTS_works);
}

int main() {
	test_load_instructions();
	test_store_instructions();
	test_transfer_instructions();
	test_stack_instructions();
	test_logic_instructions();
	test_jump_instructions();

	return 0;
}

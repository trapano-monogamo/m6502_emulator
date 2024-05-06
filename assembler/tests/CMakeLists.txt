set(CMAKE_CXX_STANDARD 20)

add_executable(assembler_tests tests.cpp)

target_link_libraries(assembler_tests PUBLIC emulator assembler)
target_include_directories(assembler_tests PUBLIC ../include)

add_custom_target(test_assembler
    COMMAND ./assembler_tests
    DEPENDS assembler_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Running Assembler Tests"
)

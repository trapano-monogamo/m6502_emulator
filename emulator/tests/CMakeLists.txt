set(CMAKE_CXX_STANDARD 20)

add_executable(emulator_tests tests.cpp)

target_link_libraries(emulator_tests PUBLIC emulator)
target_include_directories(emulator_tests PUBLIC ../include)

add_custom_target(test_emulator
    COMMAND ./emulator_tests
    DEPENDS emulator_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Running Emulator Tests"
)

set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE SRC src/*.cpp)

add_library(assembler ${SRC})

target_include_directories(assembler PUBLIC include)
# target_link_libraries(assembler PRIVATE emulator)

add_subdirectory(tests)
add_test(
    NAME assembler_test 
    COMMAND test_assembler
)

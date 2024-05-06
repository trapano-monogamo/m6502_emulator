set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE SRC src/*.cpp)

add_library(emulator ${SRC})

target_include_directories(emulator PUBLIC include)

add_subdirectory(tests)
add_test(
    NAME emulator_test 
    COMMAND test_emulator
)

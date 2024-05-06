set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC src/*.cpp)

add_library(emulator ${SRC})

target_include_directories(emulator PUBLIC include)

add_subdirectory(tests)

# TODO: add tests
add_test(
    NAME emulator_test 
    COMMAND test_emulator
)

# target_link_directories(emulator PUBLIC lib/)
# target_link_libraries(emulator -l...)

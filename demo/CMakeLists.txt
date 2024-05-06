file(GLOB_RECURSE SRC src/*.cpp)

add_executable(demo ${SRC})

target_link_libraries(demo PRIVATE emulator)
target_include_directories(demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../emulator/include)

add_custom_target(run
    COMMAND ./demo
    DEPENDS demo
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Running Demo"
)

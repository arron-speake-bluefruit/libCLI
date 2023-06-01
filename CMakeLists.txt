cmake_minimum_required(VERSION 3.10)
project(CLI VERSION 0.1.0 LANGUAGES C)

option(UNIT_TESTS "Enable the compilation of unit tests" Off)

set(SOURCES
	"source/cli.c"
	"source/parse.c"
)

set(ADDITIONAL_CFLAGS "-Wall" "-Wextra" "-Wpedantic")

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_options(${PROJECT_NAME} PRIVATE ${ADDITIONAL_CFLAGS})

if(${UNIT_TESTS})
	enable_testing()

	add_executable(tests "tests/tests.c")
	target_link_libraries(tests PRIVATE ${PROJECT_NAME})
	target_compile_options(tests PRIVATE ${ADDITIONAL_CFLAGS})

	add_test(NAME tests COMMAND tests)
endif()

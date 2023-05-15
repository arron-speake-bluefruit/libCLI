cmake_minimum_required(VERSION 3.10)
project(CLI VERSION 0.1.0 LANGUAGES C)

option(TESTING "Enable the compilation of unit tests" Off)

set(sources
	"source/cli_internal.c"
)

add_library(${PROJECT_NAME} STATIC ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")

add_subdirectory(example)

if(${TESTING})
	enable_testing()

	add_test(NAME tests COMMAND python3 tests.py WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

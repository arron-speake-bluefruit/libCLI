cmake_minimum_required(VERSION 3.10)
project(CLI VERSION 0.1.0 LANGUAGES C)

option(TESTING "Enable the compilation of unit tests" Off)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE "${PROJECT_SOURCE_DIR}/include")

add_subdirectory(example)

if(${TESTING})
	enable_testing()

	add_test(NAME tests COMMAND python3 tests.py)
endif()

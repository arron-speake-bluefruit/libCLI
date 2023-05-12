cmake_minimum_required(VERSION 3.10)
project(CLI VERSION 0.1.0 LANGUAGES C)

set(SOURCES
	"src/cli.c"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/inc")
target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic")

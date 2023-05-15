project(example)

set(sources "main.c" "cli.c")

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} PRIVATE CLI)

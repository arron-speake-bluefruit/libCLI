#include <assert.h>
#include <stdio.h>
#include "cli.h"

static size_t first_command_call_count = 0;
static size_t second_command_call_count = 0;
static size_t third_command_call_count = 0;
static size_t fourth_command_call_count = 0;

static void first_command(void) {
    first_command_call_count += 1;
}

static void second_command(void) {
    second_command_call_count += 1;
}

static void third_command(void) {
    third_command_call_count += 1;
}

static void fourth_command(void) {
    fourth_command_call_count += 1;
}

static void can_init_and_register_single_command(void) {
    // Given
    enum { capacity = 32 };
    CliCommand commands[capacity];
    CliHeader header = libcli_new(capacity, commands);

    libcli_add(&header, "first", first_command);

    // When, Then
    libcli_run(&header, "first");
    assert(first_command_call_count == 1);

    // When, Then
    libcli_run(&header, "wszhe3bbs32");
    assert(first_command_call_count == 1);
}

static void can_register_multiple_commands(void) {
    // Given
    enum { capacity = 4 };
    CliCommand commands[capacity];
    CliHeader header = libcli_new(capacity, commands);

    libcli_add(&header, "first", first_command);
    libcli_add(&header, "second", second_command);
    libcli_add(&header, "third", third_command);
    libcli_add(&header, "fourth", fourth_command);

    // When, Then
    libcli_run(&header, "first");
    assert(first_command_call_count == 1);

    // When, Then
    libcli_run(&header, "second");
    assert(second_command_call_count == 1);

    // When, Then
    libcli_run(&header, "third");
    assert(third_command_call_count == 1);

    // When, Then
    libcli_run(&header, "fourth");
    assert(fourth_command_call_count == 1);
}

static void cleanup(void) {
    first_command_call_count = 0;
    second_command_call_count = 0;
    third_command_call_count = 0;
    fourth_command_call_count = 0;
}

int main() {
    typedef void (*Test)(void);

    const Test tests[] = {
        can_init_and_register_single_command,
        can_register_multiple_commands,
    };

    const size_t test_count = sizeof(tests) / sizeof(Test);
    for (size_t i = 0; i < test_count; i++) {
        Test test = tests[i];
        test();
        cleanup();
    }

    printf("All tests (%zu) passed.\n", test_count);
}

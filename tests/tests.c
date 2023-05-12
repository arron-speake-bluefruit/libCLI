#include <assert.h>
#include <stdio.h>
#include "cli.h"

static size_t first_command_call_count = 0;
static size_t second_command_call_count = 0;
static size_t third_command_call_count = 0;
static size_t fourth_command_call_count = 0;
static void* first_command_last_userdata = NULL;
static void* second_command_last_userdata = NULL;
static void* third_command_last_userdata = NULL;
static void* fourth_command_last_userdata = NULL;

static void first_command(void* userdata) {
    first_command_call_count += 1;
    first_command_last_userdata = userdata;
}

static void second_command(void* userdata) {
    second_command_call_count += 1;
    second_command_last_userdata = userdata;
}

static void third_command(void* userdata) {
    third_command_call_count += 1;
    third_command_last_userdata = userdata;
}

static void fourth_command(void* userdata) {
    fourth_command_call_count += 1;
    fourth_command_last_userdata = userdata;
}

static void can_init_and_register_single_command(void) {
    // Given
    enum { capacity = 32 };
    CliCommand commands[capacity];
    CliHeader header = libcli_new(capacity, commands);

    bool added = libcli_add(&header, "first", first_command);
    assert(added);

    // When, Then
    int data = 3;
    CliRunResult result = libcli_run(&header, "first", &data);
    assert(result == cli_run_result_ok);
    assert(first_command_call_count == 1);
    assert(first_command_last_userdata == (void*)&data);

    // When, Then
    result = libcli_run(&header, "wszhe3bbs32", NULL);
    assert(result == cli_run_result_unknown);
    assert(first_command_call_count == 1);
}

static void can_register_multiple_commands(void) {
    // Given
    enum { capacity = 4 };
    CliCommand commands[capacity];
    CliHeader header = libcli_new(capacity, commands);

    bool added_all_commands = libcli_add(&header, "first", first_command)
        && libcli_add(&header, "second", second_command)
        && libcli_add(&header, "third", third_command)
        && libcli_add(&header, "fourth", fourth_command);
    assert(added_all_commands);

    int userdata = 12354;

    // When, Then
    CliRunResult result = libcli_run(&header, "first", &userdata);
    assert(result == cli_run_result_ok);
    assert(first_command_call_count == 1);
    assert(first_command_last_userdata == &userdata);

    // When, Then
    result = libcli_run(&header, "second", &userdata);
    assert(result == cli_run_result_ok);
    assert(second_command_call_count == 1);
    assert(second_command_last_userdata == &userdata);

    // When, Then
    result = libcli_run(&header, "third", NULL);
    assert(result == cli_run_result_ok);
    assert(third_command_call_count == 1);
    assert(third_command_last_userdata == NULL);

    // When, Then
    libcli_run(&header, "fourth", NULL);
    assert(result == cli_run_result_ok);
    assert(fourth_command_call_count == 1);
    assert(fourth_command_last_userdata == NULL);
}

static void cant_exceed_capacity(void) {
    // Given
    enum { capacity = 1 };
    CliCommand commands[capacity];
    CliHeader header = libcli_new(capacity, commands);

    bool added = libcli_add(&header, "first", first_command);
    assert(added);
    added = libcli_add(&header, "second", second_command);
    assert(!added);

    // When, Then
    CliRunResult result = libcli_run(&header, "first", NULL);
    assert(result == cli_run_result_ok);
    assert(first_command_call_count == 1);

    // When, Then
    result = libcli_run(&header, "second", NULL);
    assert(result == cli_run_result_unknown);
    assert(second_command_call_count == 0);
}

static void cleanup(void) {
    first_command_call_count = 0;
    second_command_call_count = 0;
    third_command_call_count = 0;
    fourth_command_call_count = 0;
    first_command_last_userdata = NULL;
    second_command_last_userdata = NULL;
    third_command_last_userdata = NULL;
    fourth_command_last_userdata = NULL;
}

int main(void) {
    typedef void (*Test)(void);

    const Test tests[] = {
        can_init_and_register_single_command,
        can_register_multiple_commands,
        cant_exceed_capacity,
    };

    const size_t test_count = sizeof(tests) / sizeof(Test);
    for (size_t i = 0; i < test_count; i++) {
        Test test = tests[i];
        test();
        cleanup();
    }

    printf("All tests (%zu) passed.\n", test_count);
}

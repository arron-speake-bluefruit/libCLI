#include "cli.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// Mocks & utility

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

static size_t writeback_size = 0;
static char writeback_buffer[512] = {0};

static void write_to_buffer(const char* string, void* userdata) {
    (void)userdata;
    size_t length = strlen(string);

    assert((length + writeback_size) < (sizeof(writeback_buffer) - 1));

    memcpy(&writeback_buffer[writeback_size], string, length);
    writeback_size += length;
    writeback_buffer[writeback_size] = '\0';
}

static void clear_writeback_buffer(void) {
    writeback_size = 0;
    memset(writeback_buffer, 0x00, sizeof(writeback_buffer));
}

static void printf_writeback(const char* string, void* userdata) {
    (void)userdata;
    printf("%s", string);
}

static void* writeback_userdata_pointer = NULL;
static void writeback_userdata(const char* string, void* userdata) {
    (void)string;
    writeback_userdata_pointer = userdata;
}

// Tests

static void can_init_and_register_single_command(void) {
    // Given
    enum { capacity = 32 };
    CliCommand commands[capacity];
    CliNewInfo info = { commands, capacity, printf_writeback, NULL };
    CliHeader header = libcli_new(&info);

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
    enum { capacity = 5 };
    CliCommand commands[capacity];
    CliNewInfo info = { commands, capacity, printf_writeback, NULL };
    CliHeader header = libcli_new(&info);

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
    enum { capacity = 2 };
    CliCommand commands[capacity];
    CliNewInfo info = { commands, capacity, printf_writeback, NULL };
    CliHeader header = libcli_new(&info);

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

static void automatic_help_command(void) {
    // Given
    enum { capacity = 4 };
    CliCommand commands[capacity];
    CliNewInfo info = { commands, capacity, write_to_buffer, NULL };
    CliHeader header = libcli_new(&info);

    // When, Then
    CliRunResult result = libcli_run(&header, "help", NULL);
    assert(result == cli_run_result_ok);
    const char* expected = "list of commands:\n"
        "    help\n";
    assert(strcmp(expected, writeback_buffer) == 0);

    // Given
    clear_writeback_buffer();
    libcli_add(&header, "build", first_command);

    // When, Then
    result = libcli_run(&header, "help", NULL);
    assert(result == cli_run_result_ok);
    expected = "list of commands:\n"
        "    build\n"
        "    help\n";
    assert(strcmp(expected, writeback_buffer) == 0);

    // Given
    clear_writeback_buffer();
    libcli_add(&header, "aaaa", second_command);
    libcli_add(&header, "zzzzzz", third_command);

    // When, Then
    result = libcli_run(&header, "help", NULL);
    assert(result == cli_run_result_ok);
    expected = "list of commands:\n"
        "    aaaa\n"
        "    build\n"
        "    help\n"
        "    zzzzzz\n";
    assert(strcmp(expected, writeback_buffer) == 0);
}

static void writeback_data_is_passed_to_writeback(void) {
    // Given
    int userdata = 987654321;

    enum { capacity = 1 };
    CliCommand commands[capacity];
    CliNewInfo info = { commands, capacity, writeback_userdata, &userdata };
    CliHeader header = libcli_new(&info);

    // When
    libcli_run(&header, "help", NULL);

    // Then
    assert(writeback_userdata_pointer == &userdata);
}

// Test runner

static void cleanup(void) {
    first_command_call_count = 0;
    second_command_call_count = 0;
    third_command_call_count = 0;
    fourth_command_call_count = 0;
    first_command_last_userdata = NULL;
    second_command_last_userdata = NULL;
    third_command_last_userdata = NULL;
    fourth_command_last_userdata = NULL;
    writeback_userdata_pointer = NULL;
    clear_writeback_buffer();
}

int main(void) {
    typedef void (*Test)(void);

    const Test tests[] = {
        can_init_and_register_single_command,
        can_register_multiple_commands,
        cant_exceed_capacity,
        automatic_help_command,
        writeback_data_is_passed_to_writeback,
    };

    const size_t test_count = sizeof(tests) / sizeof(Test);
    for (size_t i = 0; i < test_count; i++) {
        Test test = tests[i];
        test();
        cleanup();
    }

    printf("All tests (%zu) passed.\n", test_count);
}

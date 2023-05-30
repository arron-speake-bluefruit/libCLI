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

static void first_command(size_t argc, const CliArgument* argv, void* userdata) {
    (void)argc;
    (void)argv;
    first_command_call_count += 1;
    first_command_last_userdata = userdata;
}

static void second_command(size_t argc, const CliArgument* argv, void* userdata) {
    (void)argc;
    (void)argv;
    second_command_call_count += 1;
    second_command_last_userdata = userdata;
}

static void third_command(size_t argc, const CliArgument* argv, void* userdata) {
    (void)argc;
    (void)argv;
    third_command_call_count += 1;
    third_command_last_userdata = userdata;
}

static void fourth_command(size_t argc, const CliArgument* argv, void* userdata) {
    (void)argc;
    (void)argv;
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

    bool added = libcli_add(&header, "first", "", first_command);
    assert(added);

    // When, Then
    int data = 3;
    char first[] = "first";
    CliRunResult result = libcli_run(&header, first, &data);
    assert(result == cli_run_result_ok);
    assert(first_command_call_count == 1);
    assert(first_command_last_userdata == (void*)&data);

    // When, Then
    char wszhe3bbs32[] = "wszhe3bbs32";
    result = libcli_run(&header, wszhe3bbs32, NULL);
    assert(result == cli_run_result_unknown);
    assert(first_command_call_count == 1);
}

static void can_register_multiple_commands(void) {
    // Given
    enum { capacity = 5 };
    CliCommand commands[capacity];
    CliNewInfo info = { commands, capacity, printf_writeback, NULL };
    CliHeader header = libcli_new(&info);

    bool added_all_commands = libcli_add(&header, "first", "", first_command)
        && libcli_add(&header, "second", "", second_command)
        && libcli_add(&header, "third", "", third_command)
        && libcli_add(&header, "fourth", "", fourth_command);
    assert(added_all_commands);

    int userdata = 12354;

    // When, Then
    char first[] = "first";
    CliRunResult result = libcli_run(&header, first, &userdata);
    assert(result == cli_run_result_ok);
    assert(first_command_call_count == 1);
    assert(first_command_last_userdata == &userdata);

    // When, Then
    char second[] = "second";
    result = libcli_run(&header, second, &userdata);
    assert(result == cli_run_result_ok);
    assert(second_command_call_count == 1);
    assert(second_command_last_userdata == &userdata);

    // When, Then
    char third[] = "third";
    result = libcli_run(&header, third, NULL);
    assert(result == cli_run_result_ok);
    assert(third_command_call_count == 1);
    assert(third_command_last_userdata == NULL);

    // When, Then
    char fourth[] = "fourth";
    libcli_run(&header, fourth, NULL);
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

    bool added = libcli_add(&header, "first", "", first_command);
    assert(added);
    added = libcli_add(&header, "second", "", second_command);
    assert(!added);

    // When, Then
    char first[] = "first";
    CliRunResult result = libcli_run(&header, first, NULL);
    assert(result == cli_run_result_ok);
    assert(first_command_call_count == 1);

    // When, Then
    char second[] = "second";
    result = libcli_run(&header, second, NULL);
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
    char help[] = "help";
    CliRunResult result = libcli_run(&header, help, NULL);
    assert(result == cli_run_result_ok);
    const char* expected = "list of commands:\n"
        "    help    displays information about commands\n";
    assert(strcmp(expected, writeback_buffer) == 0);

    // Given
    clear_writeback_buffer();
    libcli_add(&header, "build", "does something or whatever", first_command);

    // When, Then
    char help2[] = "help";
    result = libcli_run(&header, help2, NULL);
    assert(result == cli_run_result_ok);
    expected = "list of commands:\n"
        "    build    does something or whatever\n"
        "    help     displays information about commands\n";
    assert(strcmp(expected, writeback_buffer) == 0);

    // Given
    clear_writeback_buffer();
    libcli_add(&header, "aaaa", "some stuff", second_command);
    libcli_add(&header, "zzzzzz", "who knows what this command does", third_command);

    // When, Then
    char help3[] = "help";
    result = libcli_run(&header, help3, NULL);
    assert(result == cli_run_result_ok);
    expected = "list of commands:\n"
        "    aaaa      some stuff\n"
        "    build     does something or whatever\n"
        "    help      displays information about commands\n"
        "    zzzzzz    who knows what this command does\n";
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
    char help[] = "help";
    libcli_run(&header, help, NULL);

    // Then
    assert(writeback_userdata_pointer == &userdata);
}

enum { test_max_argc = 16 };
static size_t command_last_argc;
static CliArgument command_last_argv[test_max_argc];

static void command_remember_args(size_t argc, const CliArgument* argv, void* userdata) {
    (void)userdata;

    command_last_argc = argc;
    assert(argc < test_max_argc);
    memcpy(command_last_argv, argv, sizeof(CliArgument) * argc);
}

static void can_parse_space_separated_arguments(void) {
    // Given
    enum { capacity = 2 };
    CliCommand commands[capacity];
    CliNewInfo info = { commands, capacity, printf_writeback, NULL };
    CliHeader header = libcli_new(&info);

    // And
    libcli_add(&header, "example", "", command_remember_args);

    // When
    char input[] = "     example a bc defg hijklmnop      ";
    libcli_run(&header, input, NULL);

    // Then
    assert(command_last_argc == 4);
    assert(command_last_argv[0].type == cli_argument_type_string);
    assert(strcmp(command_last_argv[0].string, "a") == 0);
    assert(command_last_argv[1].type == cli_argument_type_string);
    assert(strcmp(command_last_argv[1].string, "bc") == 0);
    assert(command_last_argv[2].type == cli_argument_type_string);
    assert(strcmp(command_last_argv[2].string, "defg") == 0);
    assert(command_last_argv[3].type == cli_argument_type_string);
    assert(strcmp(command_last_argv[3].string, "hijklmnop") == 0);
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
    command_last_argc = 0;
}

int main(void) {
    typedef void (*Test)(void);

    const Test tests[] = {
        can_init_and_register_single_command,
        can_register_multiple_commands,
        cant_exceed_capacity,
        automatic_help_command,
        writeback_data_is_passed_to_writeback,
        can_parse_space_separated_arguments,
    };

    const size_t test_count = sizeof(tests) / sizeof(Test);
    for (size_t i = 0; i < test_count; i++) {
        Test test = tests[i];
        test();
        cleanup();
    }

    printf("All tests (%zu) passed.\n", test_count);
}

#include "cli.h"
#include "internal/parse.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

enum {
    // Maximum number of arguments the parser can handle
    input_parser_argument_capacity = 16,
};

typedef struct {
    bool found;
    size_t index;
} SearchResult;

static void dummy_help_command(size_t argc, const CliArgument* argv, void* userdata) {
    (void)argc;
    (void)argv;
    (void)userdata;
    // Unused. If the help command is selected, a help function is executed instead of this.
}

static void writeback(const CliHeader* header, const char* string) {
    header->writeback(string, header->writeback_data);
}

static void run_help_command(const CliHeader* header) {
    writeback(header, "list of commands:");

    for (size_t i = 0; i < header->count; i++) {
        CliCommand command = header->commands[i];

        writeback(header, "\n    ");
        writeback(header, command.name);

        size_t name_length = strlen(command.name);
        for (size_t j = name_length; j < header->longest_command_name_length; j++) {
            writeback(header, " ");
        }

        writeback(header, "    ");
        writeback(header, command.summary);
    }

    writeback(header, "\n");
}

// Perform a binary search for a command called `name`.
static SearchResult find_command_by_name(const CliHeader* header, const char* name) {
    size_t size = header->count;
    size_t left = 0;
    size_t right = size;

    while (left < right) {
        size_t mid = left + size / 2;

        const CliCommand* command = &header->commands[mid];
        int cmp = strcmp(command->name, name);

        if (cmp < 0) {
            left = mid + 1;
        } else if (cmp > 0) {
            right = mid;
        } else { // cmp == 0
            return (SearchResult){ true, mid };
        }

        size = right - left;
    }

    return (SearchResult) { false, left };
}

static void update_longest_name_length(CliHeader* header, const char* name) {
    size_t length = strlen(name);

    if (length > header->longest_command_name_length) {
        header->longest_command_name_length = length;
    }
}

static void insert_command(
    CliHeader* header,
    size_t index,
    CliCommand command
) {
    size_t move_size = (header->capacity - index - 1) * sizeof(CliCommand);
    memmove(&header->commands[index + 1], &header->commands[index], move_size);

    header->count += 1;
    header->commands[index] = command;
}

static bool add_command(
    CliHeader* header,
    const char* name,
    const char* summary,
    CliCommandFunction function
) {
    SearchResult result = find_command_by_name(header, name);

    if (result.found) {
        return false;
    } else {
        CliCommand command = {
            .name = name,
            .summary = summary,
            .function = function,
        };
        insert_command(header, result.index, command);

        update_longest_name_length(header, name);

        return true;
    }
}

CliHeader libcli_new(const CliNewInfo* info) {
    CliHeader header = (CliHeader){
        .capacity = info->commands_size,
        .count = 0,
        .commands = info->commands,
        .writeback = info->writeback,
        .writeback_data = info->writeback_data,
    };

    libcli_add(&header, "help", "displays information about commands", 0, NULL, dummy_help_command);

    return header;
}

bool libcli_add(
    CliHeader* header,
    const char* name,
    const char* summary,
    size_t argument_count,
    const CliArgumentType* arguments,
    CliCommandFunction function
) {
    (void)argument_count;
    (void)arguments;

    if (header->count < header->capacity) {
        return add_command(header, name, summary, function);
    } else {
        return false;
    }
}

static CliRunResult run_command(
    const CliHeader* header,
    CliCommand command,
    size_t argc,
    const CliArgument* argv,
    void* userdata
) {
    if (command.function == dummy_help_command) {
        run_help_command(header);
        return cli_run_result_ok;
    } else {
        command.function(argc, argv, userdata);
        return cli_run_result_ok;
    }
}

static CliRunResult run_arguments(
    const CliHeader* header,
    const char* command_name,
    size_t argc,
    const CliArgument* argv,
    void* userdata
) {
    SearchResult result = find_command_by_name(header, command_name);

    if (result.found) {
        CliCommand command = header->commands[result.index];

        return run_command(header, command, argc, argv, userdata);
    } else {
        return cli_run_result_unknown;
    }
}

CliRunResult libcli_run(const CliHeader* header, char* input, void* userdata) {
    const char* argument_strings[input_parser_argument_capacity];
    ParseResult result = libcli_parse(input, argument_strings, input_parser_argument_capacity);

    switch (result.status) {
        case parse_status_success:
            break;
        case parse_status_eof_after_slash:
            return cli_run_result_eof_after_slash;
        case parse_status_unterminated_double_quote:
            return cli_run_result_unterminated_double_quote;
        case parse_status_unterminated_single_quote:
            return cli_run_result_unterminated_single_quote;
    }

    size_t argument_count = result.argument_count;

    if (argument_count > 0) {
        const char* command_name = argument_strings[0];
        CliArgument argv[input_parser_argument_capacity - 1];
        size_t argc = argument_count - 1;

        for (size_t i = 0; i < (argument_count - 1); i++) {
            argv[i] = (CliArgument){
                .type = cli_argument_type_string,
                .string = argument_strings[i + 1],
            };
        }

        return run_arguments(header, command_name, argc, argv, userdata);
    } else {
        return cli_run_result_ok;
    }
}

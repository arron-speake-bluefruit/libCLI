#include "cli.h"

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

static void dummy_help_command(size_t argc, const char* const* argv, void* userdata) {
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

    libcli_add(&header, "help", "displays information about commands", dummy_help_command);

    return header;
}

bool libcli_add(
    CliHeader* header,
    const char* name,
    const char* summary,
    CliCommandFunction function
) {
    if (header->count < header->capacity) {
        return add_command(header, name, summary, function);
    } else {
        return false;
    }
}

static size_t parse_input(char* input, const char** arguments) {
    const size_t input_length = strlen(input);

    size_t argument_count = 0;
    bool is_in_argument = false;

    for (size_t i = 0; i < input_length; i++) {
        char c = input[i];
        bool is_space = isspace(c);
        bool can_fit_more_args = (argument_count < input_parser_argument_capacity);

        if (is_space && is_in_argument && can_fit_more_args) {
            // end of an argument
            input[i] = '\0';
            is_in_argument = false;
        } else if (!is_space && !is_in_argument) {
            // starting new argument
            arguments[argument_count] = &input[i];
            argument_count += 1;
            is_in_argument = true;
        } else {
            // nothing to do
        }
    }

    return argument_count;
}

static CliRunResult run_command(
    const CliHeader* header,
    CliCommand command,
    size_t argc,
    const char* const* argv,
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
    const char* const* argv,
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
    const char* arguments[input_parser_argument_capacity];
    size_t argument_count = parse_input(input, arguments);

    if (argument_count > 0) {
        const char* command_name = arguments[0];
        size_t argc = argument_count - 1;
        const char* const* argv = &arguments[1];

        return run_arguments(header, command_name, argc, argv, userdata);
    } else {
        return cli_run_result_ok;
    }
}

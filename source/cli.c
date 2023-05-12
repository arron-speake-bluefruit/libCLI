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
    }

    writeback(header, "\n");
}

// Perform a binary search for a command called `name`.
static SearchResult search(const CliHeader* header, const char* name) {
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

static bool add_command(CliHeader* header, const char* name, CliCommandFunction function) {
    SearchResult result = search(header, name);

    if (result.found) {
        return false;
    } else {
        CliCommand command = {
            .name = name,
            .function = function,
        };

        size_t index = result.index;
        size_t move_size = (header->capacity - index - 1) * sizeof(CliCommand);
        memmove(&header->commands[index + 1], &header->commands[index], move_size);

        header->count += 1;
        header->commands[index] = command;

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

    libcli_add(&header, "help", dummy_help_command);

    return header;
}

bool libcli_add(CliHeader* header, const char* name, CliCommandFunction function) {
    if (header->count < header->capacity) {
        return add_command(header, name, function);
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

CliRunResult libcli_run(const CliHeader* header, char* input, void* userdata) {
    const char* arguments[input_parser_argument_capacity];
    size_t argument_count = parse_input(input, arguments);

    if (argument_count == 0) {
        return cli_run_result_ok;
    }

    SearchResult result = search(header, arguments[0]);

    if (!result.found) {
        return cli_run_result_unknown;
    }

    CliCommand command = header->commands[result.index];

    if (command.function == dummy_help_command) {
        run_help_command(header);
        return cli_run_result_ok;
    }

    command.function(argument_count - 1, &arguments[1], userdata);
    return cli_run_result_ok;
}

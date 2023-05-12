#include "cli.h"

#include <stdio.h>
#include <string.h>

typedef struct {
    bool found;
    size_t index;
} SearchResult;

static void dummy_help_command(void* userdata) {
    (void)userdata;
    // Unused. If the help command is selected, a help function is executed instead of this.
}

static void run_help_command(const CliHeader* header) {
    header->writeback("list of commands:");

    for (size_t i = 0; i < header->count; i++) {
        CliCommand command = header->commands[i];
        header->writeback("\n    ");
        header->writeback(command.name);
    }

    header->writeback("\n");
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

CliRunResult libcli_run(const CliHeader* header, const char* input, void* userdata) {
    SearchResult result = search(header, input);

    if (!result.found) {
        return cli_run_result_unknown;
    } else {
        CliCommand command = header->commands[result.index];

        if (command.function == dummy_help_command) {
            run_help_command(header);
        } else {
            command.function(userdata);
        }

        return cli_run_result_ok;
    }
}

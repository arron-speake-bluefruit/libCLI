#include "cli.h"

#include <string.h>

static const CliCommand* find_command(const CliHeader* header, const char* name) {
    for (size_t i = 0; i < header->count; i++) {
        const CliCommand* command = &header->commands[i];

        if (strcmp(name, command->name) == 0) {
            return command;
        }
    }

    return NULL;
}

CliHeader libcli_new(size_t commands_size, CliCommand* commands) {
    return (CliHeader){
        .capacity = commands_size,
        .count = 0,
        .commands = commands,
    };
}

void libcli_add(CliHeader* header, const char* name, CliCommandFunction function) {
    CliCommand command = {
        .name = name,
        .function = function,
    };

    size_t index = header->count;
    header->count += 1;
    header->commands[index] = command;
}

void libcli_run(const CliHeader* header, const char* input) {
    const CliCommand* command = find_command(header, input);

    if (command != NULL) {
        command->function();
    }
}

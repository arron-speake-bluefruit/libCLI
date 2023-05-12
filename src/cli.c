#include "cli.h"

#include <stdio.h>
#include <string.h>

typedef struct {
    bool found;
    size_t index;
} SearchResult;

// Perform a binary search for a command called `name`.
static SearchResult search(const CliHeader* header, const char* name) {
    size_t size = header->count;
    size_t left = 0;
    size_t right = size;

    while (left < right) {
        size_t mid = left + size / 2;

        const CliCommand* command = &header->commands[mid];
        int cmp = strcmp(name, command->name);

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

CliHeader libcli_new(size_t commands_size, CliCommand* commands) {
    return (CliHeader){
        .capacity = commands_size,
        .count = 0,
        .commands = commands,
    };
}

bool libcli_add(CliHeader* header, const char* name, CliCommandFunction function) {
    if (header->count >= header->capacity) {
        return false;
    } else {
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
}

void libcli_run(const CliHeader* header, const char* input, void* userdata) {
    SearchResult result = search(header, input);

    if (result.found) {
        CliCommand command = header->commands[result.index];
        command.function(userdata);
    }
}

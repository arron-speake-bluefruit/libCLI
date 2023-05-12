#ifndef LIBCLI_CLI_H
#define LIBCLI_CLI_H

#include <stddef.h>
#include <stdbool.h>

// A function belonging to a command.
typedef void (*CliCommandFunction)(void);

// A command registered by the CLI. All fields are private and must not be modified manually.
typedef struct CliCommand {
    const char* name;
    CliCommandFunction function;
} CliCommand;

// Contains all information used by the CLI. All fields are private and must not be modified
// manually.
typedef struct CliHeader {
    size_t capacity;
    size_t count;
    CliCommand* commands;
} CliHeader;

// Initialize a new `CliHeader` with the given command buffer and capacity.
CliHeader libcli_new(size_t commands_size, CliCommand* commands);

// Add a new command `name` (calling `function`) to the header. Returns true if the command was
// added. Returns false if the command was not added (if the command buffer is full).
bool libcli_add(CliHeader* header, const char* name, CliCommandFunction function);

// Parse and execute `input` using the given CLI (`header`).
void libcli_run(const CliHeader* header, const char* input);

#endif // LIBCLI_CLI_H

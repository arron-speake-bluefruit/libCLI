#ifndef LIBCLI_CLI_H
#define LIBCLI_CLI_H

#include <stddef.h>
#include <stdbool.h>

typedef enum CliArgumentType {
    cli_argument_type_string,
} CliArgumentType;

typedef struct CliArgument {
    CliArgumentType type;
    union {
        const char* string;
    };
} CliArgument;

// A function belonging to a command.
typedef void (*CliCommandFunction)(size_t, const CliArgument*, void*);

// Type of a function called when the CLI needs to write a string back to the user.
typedef void (*CliWritebackFunction)(const char*, void*);

// A command registered by the CLI. All fields are private and must not be modified manually.
typedef struct CliCommand {
    const char* name;
    const char* summary;
    CliCommandFunction function;
} CliCommand;

// Contains all information used by the CLI. All fields are private and must not be modified
// manually.
typedef struct CliHeader {
    size_t capacity;
    size_t count;
    CliCommand* commands;
    CliWritebackFunction writeback;
    void* writeback_data;
    size_t longest_command_name_length;
} CliHeader;

// The result of a `libcli_run` call.
typedef enum CliRunResult {
    // The input was parsed and a command was executed
    cli_run_result_ok,

    // The command name given in the input did not correspond to an existing command
    cli_run_result_unknown,
} CliRunResult;

// Information required to create a new CLI. All field are public and must be written to before
// calling `libcli_new`.
typedef struct CliNewInfo {
    // The buffer which CLI commands are stored.
    CliCommand* commands;

    // The maximum number of elements in `commands`.
    size_t commands_size;

    // The function used to write strings back to the user.
    CliWritebackFunction writeback;

    // Additional data passed to `writeback` calls. 
    void* writeback_data;
} CliNewInfo;

// Initialize a new `CliHeader` with the given command buffer and capacity.
CliHeader libcli_new(const CliNewInfo* info);

// Add a new command `name` (calling `function`) to the header. Returns true if the command was
// added. Returns false if the command was not added (if the command buffer is full).
bool libcli_add(
    CliHeader* header,
    const char* name,
    const char* summary,
    CliCommandFunction function
);

// Parse and execute `input` using the given CLI (`header`). This parses in-place and destroys the
// original string pointed to by `input`. Do _not_ pass immutable data or re-use `input` after this
// call.
CliRunResult libcli_run(const CliHeader* header, char* input, void* userdata);

#endif // LIBCLI_CLI_H

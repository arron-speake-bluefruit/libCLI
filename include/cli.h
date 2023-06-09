#ifndef LIBCLI_CLI_H
#define LIBCLI_CLI_H

#include <stddef.h>
#include <stdbool.h>

enum {
    cli_max_argument_count = 4,
};

typedef enum CliArgumentType {
    cli_argument_type_string,
    cli_argument_type_int,
    cli_argument_type_float,
} CliArgumentType;

typedef struct CliArgument {
    CliArgumentType type;
    union {
        const char* string;
        int integer;
        float float_;
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
    size_t argument_count;
    CliArgumentType arguments[cli_max_argument_count];
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

    // The incorrect number of arguments were provided
    cli_run_result_bad_argc,

    // Unexpected end of input after '\'
    cli_run_result_eof_after_slash,

    // Unexpected end of input inside double-quotes
    cli_run_result_unterminated_double_quote,

    // Unexpected end of input inside single-quotes
    cli_run_result_unterminated_single_quote,

    // The command name given in the input did not correspond to an existing command
    cli_run_result_unknown,

    // One or more arguments were invalid or not formatted correctly
    cli_run_result_bad_argument,
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
    size_t argument_count,
    const CliArgumentType* arguments,
    CliCommandFunction function
);

// Parse and execute `input` using the given CLI (`header`). This parses in-place and destroys the
// original string pointed to by `input`. Do _not_ pass immutable data or re-use `input` after this
// call.
CliRunResult libcli_run(const CliHeader* header, char* input, void* userdata);

#endif // LIBCLI_CLI_H

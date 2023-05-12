#ifndef LIBCLI_CLI_H
#define LIBCLI_CLI_H

#include <stddef.h>

typedef void (*CliCommandFunction)(void);

typedef struct CliCommand {
    const char* name;
    CliCommandFunction function;
} CliCommand;

typedef struct CliHeader {
    size_t capacity;
    size_t count;
    CliCommand* commands;
} CliHeader;

CliHeader libcli_new(size_t commands_size, CliCommand* commands);

void libcli_add(CliHeader* header, const char* name, CliCommandFunction function);

void libcli_run(const CliHeader* header, const char* input);

#endif // LIBCLI_CLI_H

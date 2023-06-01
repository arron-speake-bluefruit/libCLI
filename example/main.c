#include "cli.h"
#include <stdio.h>

static bool is_newline(char c) {
    return (c == '\n') || (c == '\r');
}

// read the stdin into `buffer` until a newline is reached (or `size` bytes are read).
static void read_line(char* buffer, size_t size) {
    size_t index = 0;

    while (true) {
        char c = getchar();

        if ((index == (size - 1)) || is_newline(c)) {
            buffer[index] = '\0';
            return;
        } else {
            buffer[index] = c;
            index += 1;
        }
    }
}

static void cli_write(const char* string, void* userdata) {
    (void)userdata;
    printf("%s", string);
}

int main(void) {
    enum { command_capacity = 8 };
    CliCommand commands[command_capacity];
    CliNewInfo cli_info = {
        .commands = commands,
        .commands_size = command_capacity,
        .writeback = cli_write,
        .writeback_data = NULL,
    };
    CliHeader cli = libcli_new(&cli_info);

    bool last_run_was_success = true;
    while (true) {
        // Print an input prompt
        if (last_run_was_success) {
            printf("> ");
        } else {
            printf("(err)> ");
        }
        fflush(stdout);

        // Get user input from somewhere
        char input[128];
        read_line(input, sizeof(input));

        // Pass input to CLI
        CliRunResult result = libcli_run(&cli, input, NULL);
        last_run_was_success = (result == cli_run_result_ok);
    }
}

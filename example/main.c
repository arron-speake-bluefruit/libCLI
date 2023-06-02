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

static void hello_command(size_t argc, const CliArgument* argv, void* userdata) {
    (void)argc;
    (void)userdata;
    (void)userdata;

    const char* name = argv[0].string;
    printf("Hello, %s\n", name);
}

static void add_command(size_t argc, const CliArgument* argv, void* userdata) {
    (void)argc;
    (void)userdata;
    (void)userdata;

    float a = argv[0].float_;
    float b = argv[1].float_;
    printf("%f\n", a + b);
}

static CliHeader setup_cli(CliCommand* commands, size_t commands_size) {
    CliNewInfo cli_info = {
        .commands = commands,
        .commands_size = commands_size,
        .writeback = cli_write,
        .writeback_data = NULL,
    };
    CliHeader cli = libcli_new(&cli_info);

    CliArgumentType hello_args[] = { cli_argument_type_string };
    libcli_add(&cli, "hello", "Say hello to something", 1, hello_args, hello_command);

    CliArgumentType add_args[] = { cli_argument_type_float, cli_argument_type_float };
    libcli_add(&cli, "add", "Add two numbers together", 2, add_args, add_command);

    return cli;
}

int main(void) {
    enum { command_capacity = 8 };
    CliCommand commands[command_capacity];
    CliHeader cli = setup_cli(commands, command_capacity);

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

        switch (result) {
        case cli_run_result_bad_argc:
            printf("error: incorrect number of arguments\n");
            break;
        case cli_run_result_eof_after_slash:
            printf("error: unexpected EOF after '\\'\n");
            break;
        case cli_run_result_unterminated_double_quote:
            printf("error: unexpected EOF inside \"-quotes\n");
            break;
        case cli_run_result_unterminated_single_quote:
            printf("error: unexpected EOF inside '-quotes");
            break;
        case cli_run_result_unknown:
            printf("error: unknown command\n");
            break;
        default:
            break;
        }

        last_run_was_success = (result == cli_run_result_ok);
    }
}

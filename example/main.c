#include "macrocli/macrocli.h"
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

// Printf wrapper passed into macrocli.
static int my_print(void* userdata, const char* restrict format, ...) {
    (void)userdata;

    va_list args;
    va_start(args, format);
    int value = vprintf(format, args);
    va_end(args);
    return value;
}

int main(void) {
    char input_buffer[256];

    while (true) {
        printf("> ");
        fflush(stdout);

        read_line(input_buffer, sizeof(input_buffer));

        macrocli_run(my_print, NULL, input_buffer);
    }
}

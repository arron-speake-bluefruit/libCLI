#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static bool parse_string(const char* input, const char** output) {
    *output = input;
    return true;
}

static bool parse_int(const char* input, int* output) {
    char* end = NULL;
    long long value = strtoll(input, &end, 10);
    *output = value;

    size_t length = strlen(input);
    size_t parsed = (size_t)(end - input);

    return length == parsed;
}

static bool parse_float(const char* input, float* output) {
    char* end = NULL;
    *output = strtof(input, &end);

    size_t length = strlen(input);
    size_t parsed = (size_t)(end - input);

    return length == parsed;
}

static bool parse_fail(const char* input, void* output) {
    (void)input;
    (void)output;
    return false;
}

#define PARSE_ARGUMENT(output) _Generic((output), \
        int: parse_int, \
        float: parse_float, \
        const char*: parse_string, \
        default: parse_fail \
    ) \

#define COMMAND_0(name, function) \
    if (strcmp(name, args[0]) == 0) { \
        if (count != 1) { \
            printf("too many args for " name "\n"); \
        } else { \
            function(userdata); \
        } \
        return; \
    } \

#define COMMAND_1(name, function, arg0_type, arg0_name) \
    if (strcmp(name, args[0]) == 0) { \
        if (count < 2) { \
            printf("missing arg " #arg0_name " for " name "\n"); \
        } else if (count > 2) { \
            printf("too many args for " name "\n"); \
        } else { \
            arg0_type arg0_name; \
            if (!PARSE_ARGUMENT(arg0_name)(args[1], &arg0_name)) { \
                printf("bad argument " #arg0_name " for " name "\n"); \
            } else { \
                function(arg0_name, userdata); \
            } \
        } \
        return; \
    } \


#define COMMAND_2(name, function, arg0_type, arg0_name, arg1_type, arg1_name) \
    if (strcmp(name, args[0]) == 0) { \
        if (count < 2) { \
            printf("missing arg " #arg0_name " for " name "\n"); \
        } else if (count == 2) { \
            printf("missing arg " #arg1_name " for " name "\n"); \
        } else if (count > 3) { \
            printf("too many args for " name "\n"); \
        } else { \
            arg0_type arg0_name; \
            arg1_type arg1_name; \
            if (!PARSE_ARGUMENT(arg0_name)(args[1], &arg0_name)) { \
                printf("bad argument " #arg0_name " for " name "\n"); \
            } else if (!PARSE_ARGUMENT(arg1_name)(args[2], &arg1_name)) { \
                printf("bad argument " #arg1_name " for " name "\n"); \
            } else { \
                function(arg0_name, arg1_name, userdata); \
            } \
        } \
        return; \
    } \

enum { input_parser_argument_capacity = 16 };

static size_t parse_input(char* input, const char** arguments) {
    const size_t input_length = strlen(input);

    size_t argument_count = 0;
    bool is_in_argument = false;

    for (size_t i = 0; i < input_length; i++) {
        char c = input[i];
        bool is_space = isspace(c);
        bool can_fit_more_args = (argument_count < input_parser_argument_capacity);

        if (is_space && is_in_argument && can_fit_more_args) {
            // end of an argument
            input[i] = '\0';
            is_in_argument = false;
        } else if (!is_space && !is_in_argument) {
            // starting new argument
            arguments[argument_count] = &input[i];
            argument_count += 1;
            is_in_argument = true;
        } else {
            // nothing to do
        }
    }

    return argument_count;
}

void libcli_run(char* input, void* userdata) {
    const char* args[input_parser_argument_capacity];
    size_t count = parse_input(input, args);

    if (count == 0) {
        return;
    }

    COMMANDS

    printf("command not found for %s\n", args[0]);
}

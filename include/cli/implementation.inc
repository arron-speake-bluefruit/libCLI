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

size_t libcli_parse(char* input, const char** arguments, size_t max_arguments);

void libcli_run(char* input, void* userdata) {
    enum { capacity = 16 };
    const char* args[capacity];
    size_t count = libcli_parse(input, args, capacity);

    if (count == 0) {
        return;
    }

    COMMANDS

    printf("command not found for %s\n", args[0]);
}

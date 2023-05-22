#include "internal/parse.h"
#include "macrocli.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#ifndef COMMANDS
#error COMMANDS must be defined to generate the CLI implementation.
#endif

static bool parse_float(const char* in, float* out) {
    char* end = NULL;
    *out = strtof(in, &end);
    return *end == '\0';
}

static bool parse_double(const char* in, double* out) {
    char* end = NULL;
    *out = strtod(in, &end);
    return *end == '\0';
}

static bool parse_long_double(const char* in, long double* out) {
    char* end = NULL;
    *out = strtold(in, &end);
    return *end == '\0';
}

static bool parse_char(const char* in, char* out) {
    // Accept any length-1 input.
    *out = in[0];
    return (in[0] != '\0') && (in[1] == '\0');
}

static bool parse_short(const char* in, short* out) {
    char* end = NULL;
    long value = strtol(in, &end, 0);
    *out = value;
    return (*end == '\0') && (value >= SHRT_MIN) && (value <= SHRT_MAX);
}

static bool parse_int(const char* in, int* out) {
    char* end = NULL;
    long value = strtol(in, &end, 0);
    *out = value;
    return (*end == '\0') && (value >= INT_MIN) && (value <= INT_MAX);
}

static bool parse_long(const char* in, long* out) {
    char* end = NULL;
    *out = strtol(in, &end, 0);
    return *end == '\0';
}

static bool parse_long_long(const char* in, long long* out) {
    char* end = NULL;
    *out = strtoll(in, &end, 0);
    return *end == '\0';
}

static bool parse_unsigned_char(const char* in, unsigned char* out) {
    char* end = NULL;
    unsigned long value = strtoul(in, &end, 0);
    *out = value;
    return (*end == '\0') && (value < UCHAR_MAX);
}

static bool parse_unsigned_short(const char* in, unsigned short* out) {
    char* end = NULL;
    unsigned long value = strtoul(in, &end, 0);
    *out = value;
    return (*end == '\0') && (value <= USHRT_MAX);
}

static bool parse_unsigned_int(const char* in, unsigned int* out) {
    char* end = NULL;
    unsigned long value = strtoul(in, &end, 0);
    *out = value;
    return (*end == '\0') && (value <= UINT_MAX);
}

static bool parse_unsigned_long(const char* in, unsigned long* out) {
    char* end = NULL;
    *out = strtoul(in, &end, 0);
    return *end == '\0';
}

static bool parse_unsigned_long_long(const char* in, unsigned long long* out) {
    char* end = NULL;
    *out = strtoull(in, &end, 0);
    return *end == '\0';
}

static bool parse_string(const char* in, const char** out) {
    *out = in;
    return true;
}

static bool parse_bool(const char* in, bool* out) {
    if (strcmp(in, "true") == 0) {
        *out = true;
        return true;
    } else if (strcmp(in, "false") == 0) {
        *out = false;
        return true;
    } else {
        return false;
    }
}

#define parse_arg(x) _Generic((x), \
        char: parse_char, \
        short: parse_short, \
        int: parse_int, \
        long: parse_long, \
        long long: parse_long_long, \
        unsigned char: parse_unsigned_char, \
        unsigned short: parse_unsigned_short, \
        unsigned int: parse_unsigned_int, \
        unsigned long: parse_unsigned_long, \
        unsigned long long: parse_unsigned_long_long, \
        float: parse_float, \
        double: parse_double, \
        long double: parse_long_double, \
        bool: parse_bool, \
        const char*: parse_string \
    )

#define arg_type_name(x) _Generic((x), \
        char: "char", \
        short: "short", \
        int: "int", \
        long: "long", \
        long long: "long long", \
        unsigned char: "unsigned char", \
        unsigned short: "unsigned short", \
        unsigned int: "unsigned int", \
        unsigned long: "unsigned long", \
        unsigned long long: "unsigned long long", \
        float: "float", \
        double: "double", \
        long double: "long double", \
        bool: "bool", \
        const char*: "string" \
    )

bool macrocli_run(MacroCliPrintf print, void* userdata, char* input) {
    enum { max_arguments = 3 };

    // Convert `input` into an array of strings.
    const char* args[max_arguments];
    ParseResult parse_result = macrocli_parse(input, args, max_arguments);
    unsigned arg_count = parse_result.argument_count;

    // Handle any errors encountered during parsing.
    switch (parse_result.status) {
    case parse_status_eof_after_slash:
        print(userdata, "error: unexpected EOF after `\\`\n");
        return false;
    case parse_status_unterminated_double_quote:
        print(userdata, "error: unexpected EOF inside `\"` quotes\n");
        return false;
    case parse_status_unterminated_single_quote:
        print(userdata, "error: unexpected EOF inside `'` quotes\n");
        return false;
    case parse_status_success:
        break;
    }

    // Silently succeed for empty inputs.
    if (arg_count == 0) {
        return true;
    }

    // Generated command lookup code. Will either return on success, fallthrough for an unknown
    // command or jump into error handling.
#include "internal/command_lookup.inc"

    // Generated builtin `help` command. Will either fall through or return.
#include "internal/builtin_help.inc"

    // Fell through all defined commands, args[0] is an unknown command.
    print(userdata, "error: unknown command `%s`\n", args[0]);
    return false;

    // Generated error handling code each with jump labels. All paths return.
#include "internal/error_handling.inc"
}

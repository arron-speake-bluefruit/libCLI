#include <stdio.h>
#include <stdbool.h>

void simple_command(void* userdata) {
    (void)userdata;
    printf("Hello, world.\n");
}

void add_command(float a, float b, void* userdata) {
    (void)userdata;
    printf("%f + %f = %f\n", a, b, a + b);
}

void overloaded_ull(unsigned long long value, void* userdata) {
    (void)userdata;
    printf("%llu is an unsigned integer\n", value);
}

void overloaded_ll(long long value, void* userdata) {
    (void)userdata;
    printf("%lli is an integer\n", value);
}

void overloaded_ld(long double value, void* userdata) {
    (void)userdata;
    printf("%Lf is a decimal\n", value);
}

void overloaded_b(bool value, void* userdata) {
    (void)userdata;
    printf("%s is a bool\n", value ? "true" : "false");
}

void overloaded_s(const char* value, void* userdata) {
    (void)userdata;
    printf("%s is a string\n", value);
}

void with_optional(const char* first, void* userdata) {
    (void)userdata;
    printf("%s\n", first);
}

void with_optional2(const char* first, float second, void* userdata) {
    (void)userdata;
    printf("%s and %f\n", first, second);
}

#define COMMANDS \
    COMMAND(simple, "most basic command", ARGS_0(simple_command, "print a message")) \
    COMMAND(add, "add two numbers together", ARGS_2(add_command, "takes the sum of both arguments", float, float)) \
    COMMAND(overloaded, "print the type of the argument", \
        ARGS_1(overloaded_ull, "unsigned integer argument", unsigned long long) \
        ARGS_1(overloaded_ll, "integer argument", long long) \
        ARGS_1(overloaded_ld, "decimal argument", long double) \
        ARGS_1(overloaded_b, "boolean argument", bool) \
        ARGS_1(overloaded_s, "string argument", const char*) \
    ) \
    COMMAND(with_optional, "command with an optional argument", \
        ARGS_1(with_optional, "unsigned integer argument", const char*) \
        ARGS_2(with_optional2, "unsigned integer argument", const char*, float) \
    ) \

#include "macrocli/implementation.inc"

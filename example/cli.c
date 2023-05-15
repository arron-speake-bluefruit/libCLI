#include "cli/cli.h"
#include <stdio.h>

static void enable_all(void* userdata) {
    (void)userdata;
    printf("enabled all items\n");
}

static void disable_all(void* userdata) {
    (void)userdata;
    printf("disabled all items\n");
}

static void enable(const char* item, int value, void* userdata) {
    (void)userdata;
    printf("enabled %s with value %i\n", item, value);
}

static void disable(const char* item, void* userdata) {
    (void)userdata;
    printf("disabled %s\n", item);
}

#define COMMANDS \
    COMMAND_0("enable-all", enable_all) \
    COMMAND_0("disable-all", disable_all) \
    COMMAND_2("enable", enable, const char*, item, int, value) \
    COMMAND_1("disable", disable, const char*, item) \

#include "cli/implementation.inc"

#include <assert.h>
#include <stdio.h>
#include "cli.h"

static void can_init_libcli(void) {
    libcli_new();
}

int main() {
    can_init_libcli();
    printf("All tests passed.");
}
